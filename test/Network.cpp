#include "Network.h"
#include "pages/ConsolePage.h"
#include <fmt/format.h>
#include <random>
#include <thread>
#include <iostream>

extern std::mt19937 gen;

NetworkManager& NetworkManager::getInstance() {
    static NetworkManager instance;
    return instance;
}

void NetworkManager::server(boost::asio::io_service& io_service, int port) {
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));
    ConsolePage::print("Waiting for players to join.");
    clients.clear();
    clients_selection.clear();
    int client_count = 0;
    std::vector<int> client_id={0, 1, 2, 3};
    shuffle(client_id.begin(), client_id.end(), gen);
    
    while(client_count < 4) {
        auto socket = std::make_shared<tcp::socket>(io_service);
        acceptor.accept(*socket);
        int id = client_id[client_count++];
        clients[id] = socket;
        boost::asio::write(*socket, boost::asio::buffer(std::to_string(id)));
        ConsolePage::print("Player " + std::to_string(id) + " connected.");
        
        auto selection_socket = std::make_shared<tcp::socket>(io_service);
        acceptor.accept(*selection_socket);
        clients_selection[id] = selection_socket;
        ConsolePage::print("Selection channel for player " + std::to_string(id) + " established.");
    }
    ConsolePage::print("All players connected.");
}

void NetworkManager::client(boost::asio::io_service& io_service, const std::string& host, const std::string& port, bool is_server) {
    tcp::resolver resolver(io_service);
    auto endpoints = resolver.resolve(tcp::resolver::query(host, port));
    auto socket = std::make_shared<tcp::socket>(io_service);
    boost::asio::connect(*socket, endpoints);
    ConsolePage::print("Connected to server.");
    
    auto selection_socket = std::make_shared<tcp::socket>(io_service);
    boost::asio::connect(*selection_socket, endpoints);
    ConsolePage::print("Selection channel established.");
    
    int player_id = 0;
    {
        char data[1024];
        boost::system::error_code error;
        size_t length = socket->read_some(boost::asio::buffer(data), error);
        if(error == boost::asio::error::eof)
            return;
        else if(error)
            throw boost::system::system_error(error);
        ConsolePage::print("You are player " + std::string(data, length) + ".");
        player_id = std::string(data, length)[0] - '0';
    }

    if(is_server) {
        std::thread([socket, selection_socket, player_id]() {
            try {
                while(true) {
                    char data[1024];
                    boost::system::error_code error;
                    size_t length = socket->read_some(boost::asio::buffer(data), error);
                    if(!length)
                        continue;
                    else if(error == boost::asio::error::eof)
                        break;
                    else if(error)
                        throw boost::system::system_error(error);
                    ConsolePage::clear();
                    ConsolePage::print(std::string(data, length));
                    if(std::string(data, length) == "Game Ends.")
                        break;
                    if(std::string(data, length).find("Action") != std::string::npos) {
                        ConsolePage::print("Enter action number:");
                        std::string selection = ConsolePage::getInput();
                        boost::asio::write(*selection_socket, boost::asio::buffer(selection));
                    }
                    if(std::string(data, length).find("anything") != std::string::npos) {
                        std::string anything = ConsolePage::getInput();
                        boost::asio::write(*selection_socket, boost::asio::buffer(anything));
                    }
                }
            } catch(const std::exception& e) {
                ConsolePage::print("Exception in client receiver: " + std::string(e.what()));
            }
        }).detach();
    }
    else {
        std::thread([socket, selection_socket, player_id]() {
            try {
                while(true) {
                    char data[1024];
                    boost::system::error_code error;
                    size_t length = socket->read_some(boost::asio::buffer(data), error);
                    if(!length)
                        continue;
                    else if(error == boost::asio::error::eof)
                        break;
                    else if(error)
                        throw boost::system::system_error(error);
                    ConsolePage::clear();
                    ConsolePage::print(std::string(data, length));
                    if(std::string(data, length) == "Game Ends.")
                        break;
                    if(std::string(data, length).find("Action") != std::string::npos) {
                        ConsolePage::print("Enter action number:");
                        std::string selection = ConsolePage::getInput();
                        boost::asio::write(*selection_socket, boost::asio::buffer(selection));
                    }
                    if(std::string(data, length).find("anything") != std::string::npos) {
                        std::string anything = ConsolePage::getInput();
                        boost::asio::write(*selection_socket, boost::asio::buffer(anything));
                    }
                }
            } catch(const std::exception& e) {
                ConsolePage::print("Exception in client receiver: " + std::string(e.what()));
            }
        }).join();
    }
} 