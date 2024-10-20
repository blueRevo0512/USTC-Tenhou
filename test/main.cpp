#include <fstream>
#include <chrono>
#include <random>
#include <iostream>
#include "Table.h"
#include "Rule.h"
#include "macro.h"
#include "GamePlay.h"
#include "Encoding/TrainingDataEncodingV1.h"
#include "Encoding/TrainingDataEncodingV2.h"
#include <boost/asio.hpp>
#include <thread>
#include <set>

using boost::asio::ip::tcp;
using namespace std;
using_mahjong;

random_device rd;
mt19937 gen(rd());
int max_wind = 2, max_oya = 4;
map<int, shared_ptr<tcp::socket>> clients,clients_selection;

void server(boost::asio::io_service& io_service, int port)
{
	tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));
	fmt::print("Waiting for players to join.\n");
	clients.clear();
	clients_selection.clear();
	int client_count = 0;
	vector<int> client_id={0, 1, 2, 3};
	shuffle(client_id.begin(), client_id.end(), gen);
	while(client_count < 4)
	{
		auto socket = make_shared<tcp::socket>(io_service);
		acceptor.accept(*socket);
		int id=client_id[client_count++];
		clients[id]=socket;
		boost::asio::write(*socket, boost::asio::buffer(to_string(id)));
		fmt::print("Player {} connected.\n",id);
		auto selection_socket = make_shared<tcp::socket>(io_service);
		acceptor.accept(*selection_socket);
		clients_selection[id]=selection_socket;
		fmt::print("Selection channel for player {} established.\n",id);
	}
	fmt::print("All players connected.\n");
}

void client(boost::asio::io_service& io_service, const string& host, const string& port, bool is_server)
{
	tcp::resolver resolver(io_service);
	auto endpoints = resolver.resolve(tcp::resolver::query(host, port));
	auto socket = make_shared<tcp::socket>(io_service);
	boost::asio::connect(*socket, endpoints);
	fmt::print("Connected to server.\n");
	auto selection_socket = make_shared<tcp::socket>(io_service);
	boost::asio::connect(*selection_socket, endpoints);
	fmt::print("Selection channel established.\n");
	int player_id=0;
	{
		char data[1024];
		boost::system::error_code error;
		size_t length = socket->read_some(boost::asio::buffer(data), error);
		if(error == boost::asio::error::eof)
			return;
		else if(error)
			throw boost::system::system_error(error);
		fmt::print("You are player {}.\n",string(data,length));
		player_id=string(data,length)[0] - '0';
	}
	if(is_server)
	{
		thread([socket, selection_socket, player_id]() {
			try{
				while(true)
				{
					char data[1024];
					boost::system::error_code error;
					size_t length = socket->read_some(boost::asio::buffer(data), error);
					if(!length)
						continue;
					else if(error == boost::asio::error::eof)
						break;
					else if(error)
						throw boost::system::system_error(error);
					system("cls");
					fmt::print("{}\n",string(data, length));
					if(string(data, length) == "Game Ends.")
						break;
					if(string(data, length).find("Action") != string::npos)
					{
						fmt::print("Enter action number:\n");
						string selection;
						cin >> selection;
						boost::asio::write(*selection_socket, boost::asio::buffer(selection));
					}
					if(string(data, length).find("anything") != string::npos)
					{
						string anything;
						cin >> anything;
						boost::asio::write(*selection_socket, boost::asio::buffer(anything));
					}
				}
			} catch(const exception& e) {
				fmt::print("Exception in client receiver: {}\n",e.what());
			}
		}).detach();
	}
	else
	{
		thread([socket, selection_socket, player_id]() {
			try{
				while(true)
				{
					char data[1024];
					boost::system::error_code error;
					size_t length = socket->read_some(boost::asio::buffer(data), error);
					if(!length)
						continue;
					else if(error == boost::asio::error::eof)
						break;
					else if(error)
						throw boost::system::system_error(error);
					system("cls");
					fmt::print("{}\n",string(data, length));
					if(string(data, length) == "Game Ends.")
						break;
					if(string(data, length).find("Action") != string::npos)
					{
						fmt::print("Enter action number:\n");
						string selection;
						cin >> selection;
						// fmt::print("Gonna write selection.\n");
						boost::asio::write(*selection_socket, boost::asio::buffer(selection));
						// fmt::print("Selection written.\n");
					}
					if(string(data, length).find("anything") != string::npos)
					{
						string anything;
						cin >> anything;
						boost::asio::write(*selection_socket, boost::asio::buffer(anything));
					}
				}
			} catch(const exception& e) {
				fmt::print("Exception in client receiver: {}\n",e.what());
			}
		}).join();
	}
}

void start_game()
{
	Result result;
	bool is_first = true;
	for (int wind = 0; wind < max_wind + (is_first ? 0 : *max_element(result.score.begin(), result.score.end()) < 30000); wind++)
		for(int oya = 0; oya < max_oya + (max_oya == 1 && *max_element(result.score.begin(), result.score.end()) < 30000 ? 4 : 0); oya++)
		{
			while((wind < max_wind && oya < max_oya) || *max_element(result.score.begin(),result.score.end()) < 30000)
			{
				Table t;
				if (is_first)
				{
					t.game_init();
					is_first = false;
				}
				else
					t.game_init_with_config({}, vector<int>(result.score.begin(),result.score.end()), result.n_riichibo, result.n_honba, wind, oya);
				try
				{
					string message[4];
					while(!t.is_over())
					{
						for(int player = 0; player < 4; player++)
							message[player] = t.to_string(player) + "\n" + to_string(t.who_make_selection()) + " is making selection.\n";
						if (t.get_phase() < 4)
						{
							message[t.who_make_selection()] += "SelfAction phase.\n";
							auto actions = t.get_self_actions();
							int index = 0, real_index = 0;
							set<string> action_set;
							map<int, int> index_map;
							for (auto &action : actions)
							{
								string action_str = action.to_string() + (t.check_from_hand(action) ? "" : "*");
								index_map[index += (!action_set.count(action_str))] = real_index++;
								if (!action_set.count(action_str))
								{
									action_set.insert(action_str);
									message[t.who_make_selection()] += to_string(index) + ". " + action_str + "\n";
								}
							}
							for(int player = 0; player < 4; player++)
								boost::asio::write(*clients[player], boost::asio::buffer(message[player]));
							int selection = 1;
							try{
								while(true)
								{
									char data[1024];
									boost::system::error_code error;
									size_t length = clients_selection[t.who_make_selection()]->read_some(boost::asio::buffer(data), error);
									if(!length)
										continue;
									else if(error == boost::asio::error::eof)
										break;
									else if(error)
										throw boost::system::system_error(error);
									selection = 0;
									for(int i = 0; i < length; i++)
										selection = selection * 10 + data[i] - '0';
									break;
								}
							} catch(const exception& e) {
								fmt::print("Exception in selection receiver: {}\n",e.what());
							}
							t.make_selection(index_map[selection]);
						}
						else
						{
							message[t.who_make_selection()] += "Response Action phase.\n";
							auto actions = t.get_response_actions();
							if(actions.size() == 1)
							{
								t.make_selection(0);
								continue;
							}
							int index = 0, real_index = 0;
							set<string> action_set;
							map<int, int> index_map;
							for (auto &action : actions)
							{
								string action_str = action.to_string();
								index_map[index += (!action_set.count(action_str))] = real_index++;
								if(!action_set.count(action_str))
								{
									action_set.insert(action_str);
									message[t.who_make_selection()] += to_string(index) + ". " + action_str + "\n";
								}
							}
							for(int player = 0; player < 4; player++)
								boost::asio::write(*clients[player], boost::asio::buffer(message[player]));
							int selection = 1;
							try{
								while(true)
								{
									char data[1024];
									boost::system::error_code error;
									size_t length = clients_selection[t.who_make_selection()]->read_some(boost::asio::buffer(data), error);
									if(!length)
										continue;
									else if(error == boost::asio::error::eof)
										break;
									else if(error)
										throw boost::system::system_error(error);
									selection = 0;
									for(int i = 0; i < length; i++)
										selection = selection * 10 + data[i] - '0';
									break;
								}
							} catch(const exception& e) {
								fmt::print("Exception in selection receiver: {}\n",e.what());
							}
							t.make_selection(index_map[selection]);
						}
					}
					for(int player = 0; player < 4; player++)
						message[player] = "Result: \n" + t.get_result().to_string() + "\n" + "Type anything to continue.\n";
					for(int player = 0; player < 4; player++)
						boost::asio::write(*clients[player], boost::asio::buffer(message[player]));
					for(int player = 0; player < 4; player++)
						try{
							while(true)
							{
								char data[1024];
								boost::system::error_code error;
								size_t length = clients_selection[player]->read_some(boost::asio::buffer(data), error);
								if(!length)
									continue;
								else if(error == boost::asio::error::eof)
									break;
								else if(error)
									throw boost::system::system_error(error);
								break;
							}
						} catch(const exception& e) {
							fmt::print("Exception in selection receiver: {}\n",e.what());
						}
					result = t.get_result();
				} catch(const exception& e) {
					fmt::print("Exception in client receiver: {}\n",e.what());
				}
				if(!result.renchan) break;
			}
		}
}

void create_room()
{
	fmt::print("Please input the game length.\n");
	fmt::print("1. Topparai.\n");
	fmt::print("2. Tompuuso.\n");
	fmt::print("3. Hanchan.\n");
	fmt::print("4. Iichan.\n\n");
	string game_length;
	cin >> game_length;
	while (game_length != "1" && game_length != "2" && game_length != "3" && game_length != "4")
	{
		fmt::print("Invalid input! Please try again.\n");
		cin >> game_length;
	}
	if (game_length == "1")
		max_wind = 1, max_oya = 1;
	if (game_length == "2")
		max_wind = 1, max_oya = 4;
	if (game_length == "3")
		max_wind = 2, max_oya = 4;
	if (game_length == "4")
		max_wind = 4, max_oya = 4;
	boost::asio::io_service io_service;
	uniform_int_distribution<> dist(10000, 60000);
	int port;
	bool port_available = false;
	while (!port_available)
	{
		port=dist(gen);
		fmt::print("Trying to create room on port {}.\n",port);
		try
		{
			thread server_thread([&io_service, port]() {
				server(io_service, port);
			});
			fmt::print("Successfully created room on port {}.\n",port);
			client(io_service, "127.0.0.1", to_string(port), true);
			port_available = true;
			server_thread.join();
		}
		catch(const exception& e)
		{
			fmt::print("Port {} is unavailable: {}. Trying another port.\n",port,e.what());
		}
	}
	start_game();
}

void join_room()
{
	boost::asio::io_service io_service;
	string host, port;
	fmt::print("Enter host:\n");
	cin >> host;
	fmt::print("Enter port:\n");
	cin >> port;
	client(io_service, host, port, false);
}

void start_ui()
{
	fmt::print("Welcome to USTC Tenhou!\n");
	while (true)
	{
		fmt::print("Type 'h' or 'help' to see the rules and instructions of the game.\n");
		fmt::print("Type 'i' or 'info' to see the information about the project.\n");
		fmt::print("Type '1' or 'c' or 'create' to create a room for playing.\n");
		fmt::print("Type '2' or 'j' or 'join' to join a room for playing.\n");
		fmt::print("Type '3' or 'q' or 'quit' to quit the game.\n\n");
		string input;
		cin >> input;
		while(input != "h" && input != "help" && input != "i" && input != "info" && input != "1"
		&& input != "c" && input != "create" && input != "2" && input != "j" && input != "join"
		&& input != "3" && input != "q" && input != "quit")
		{
			fmt::print("Invalid input! Please try again.\n");
			cin >> input;
		}
		if(input == "h" || input == "help")
		{
			fmt::print("This mahjong game is a simulation of four-player Tenhou.\n");
			fmt::print("You can check out the game rules of Tenhou at https://zh.wikipedia.org/wiki/日本麻雀 and https://zh.wikipedia.org/wiki/天鳳_(麻將).\n");
			fmt::print("In this USTC version of Tenhou, three new yakumans are added.\n");
			fmt::print("1. 红专并进 (Daredbrick) Double Yakuman: Agari with four koutsu or kantsu of 5m, 5p, 5s, 7z. For 5m, 5p, 5s, you also need to have the red dora.\n");
			fmt::print("2. 又红又专 (Xiaoredbrick) Yakuman: Agari with a koutsu or kantsu in Daredbrick substituted by toitsu. For 5m, 5p, 5s, you still need to have the red dora.\n");
			fmt::print("3. 理实交融 (Basicalgebra) Yakuman: Agari with four koutsu or kantsu of kazupai containing 1, 9, 5, 8. For example, 1s koutsu, 9m kantsu, 5p koutsu, 8p koutsu.\n");
			fmt::print("In order to read the Tile River, you need to know the following numbers and symbols after the tile:\n");
			fmt::print("The number is the order number of this tile in the river.\n");
			fmt::print("1. h It means the tile is discarded from hand. If not shown, the tile is discarded from tsumo.\n");
			fmt::print("2. r It means the tile is the riichi tile.\n");
			fmt::print("3. - It means someone Chi, Pon, or Kan the tile. The tile is actually not in the river.\n");
			fmt::print("In order to discard tiles more precisely, you need to know that the '*' after the tile means the tile is from tsumo.\n");
			fmt::print("That is all you need to know. Enjoy the game!\n\n");
		}
		else if(input == "i" || input == "info")
		{
			fmt::print("This project is based on https://github.com/Agony5757/mahjong.\n");
			fmt::print("This project also uses fmt, boost/asio, CMake, git, and other tools.\n");
			fmt::print("Thanks for the help from Dongqi Han (MSRA, OIST, USTC), Agony (USTC), and other contributors of https://github.com/Agony5757/mahjong.\n");
			fmt::print("This project is developed by Tianhao Jin and Hongxi Zou from USTC.\n");
			fmt::print("If you meet any problems or want to contribute to this project, contact us through QQ: 2328036454 or Email: jintianhao@ustc.edu.cn.\n");
			fmt::print("Thank you for playing this game!\n\n");
		}
		else if(input == "3" || input == "q" || input == "quit")
		{
			fmt::print("See you next time.\n");
			return;
		}
		else if(input == "1" || input == "c" || input == "create")
		{
			create_room();
		}
		else if(input == "2" || input == "j" || input == "join")
		{
			join_room();
		}
	}
}

int main() {
	start_ui();
	return 0;
}
