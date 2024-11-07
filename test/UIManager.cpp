#include "UIManager.h"
#include "GameManager.h"
#include "Network.h"
#include "pages/CreateRoomPage.h"
#include "pages/JoinRoomPage.h"
#include "pages/ConsolePage.h"
#include <fmt/format.h>
#include <iostream>
#include <random>
#include <thread>

extern std::mt19937 gen;

UIManager& UIManager::getInstance() {
    static UIManager instance;
    return instance;
}

void UIManager::show_help() {
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

void UIManager::show_info() {
    fmt::print("This project is based on https://github.com/Agony5757/mahjong.\n");
    fmt::print("This project also uses fmt, boost/asio, CMake, git, and other tools.\n");
    fmt::print("Thanks for the help from Dongqi Han (MSRA, OIST, USTC), Agony (USTC), and other contributors of https://github.com/Agony5757/mahjong.\n");
    fmt::print("This project is developed by Tianhao Jin and Hongxi Zou from USTC.\n");
    fmt::print("If you meet any problems or want to contribute to this project, contact us through QQ: 2328036454 or Email: jintianhao@ustc.edu.cn.\n");
    fmt::print("Thank you for playing this game!\n\n");
}

void UIManager::create_room() {
    // 使用 CreateRoomPage 获取游戏长度和端口选择
    int game_length = CreateRoomPage::getGameLength();
    std::string port_choice = CreateRoomPage::getPortChoice();

    auto& game_manager = GameManager::getInstance();
    if (game_length == 1)
        game_manager.set_game_length(1, 1);
    if (game_length == 2)
        game_manager.set_game_length(1, 4);
    if (game_length == 3)
        game_manager.set_game_length(2, 4);
    if (game_length == 4)
        game_manager.set_game_length(4, 4);

    boost::asio::io_service io_service;
    std::uniform_int_distribution<> dist(10000, 60000);
    int port;
    bool port_available = false;
    
    auto& network = NetworkManager::getInstance();
    
    if (port_choice == "1") {  // 指定端口
        port = std::stoi(CreateRoomPage::getPort());
        ConsolePage::print("创建服务器，使用端口: " + std::to_string(port));
        std::thread server_thread([&io_service, port, &network]() {
            network.server(io_service, port);
        });
        network.client(io_service, "127.0.0.1", std::to_string(port), true);
        port_available = true;
        server_thread.join();
    }
    else {  // 随机端口
        while (!port_available) {
            port = dist(gen);
            try {
                ConsolePage::print("尝试创建服务器，使用端口: " + std::to_string(port));
                std::thread server_thread([&io_service, port, &network]() {
                    network.server(io_service, port);
                });
                network.client(io_service, "127.0.0.1", std::to_string(port), true);
                port_available = true;
                server_thread.join();
            }
            catch(const std::exception& e) {
                ConsolePage::print("端口 " + std::to_string(port) + " 不可用: " + e.what() + "，尝试其他端口。");
            }
        }
    }
    game_manager.start_game();
}

void UIManager::join_room() {
    boost::asio::io_service io_service;
    
    // 使用 JoinRoomPage 获取主机和端口
    std::string host = JoinRoomPage::getIpAddress();
    std::string port = JoinRoomPage::getPort();
    
    NetworkManager::getInstance().client(io_service, host, port, false);
}

void UIManager::start_ui() {
    while (true) {
        fmt::print("Welcome to USTC Tenhou!\n");
        fmt::print("Type 'h' or 'help' to see the rules and instructions of the game.\n");
        fmt::print("Type 'i' or 'info' to see the information about the project.\n");
        fmt::print("Type '1' or 'c' or 'create' to create a room for playing.\n");
        fmt::print("Type '2' or 'j' or 'join' to join a room for playing.\n");
        fmt::print("Type '3' or 'q' or 'quit' to quit the game.\n\n");
        
        std::string input;
        std::cin >> input;
        while(input != "h" && input != "help" && input != "i" && input != "info" && input != "1"
            && input != "c" && input != "create" && input != "2" && input != "j" && input != "join"
            && input != "3" && input != "q" && input != "quit") {
            fmt::print("Invalid input! Please try again.\n");
            std::cin >> input;
        }
        
        if(input == "h" || input == "help") {
            show_help();
        }
        else if(input == "i" || input == "info") {
            show_info();
        }
        else if(input == "3" || input == "q" || input == "quit") {
            fmt::print("See you next time.\n");
            return;
        }
        else if(input == "1" || input == "c" || input == "create") {
            create_room();
        }
        else if(input == "2" || input == "j" || input == "join") {
            join_room();
        }
        system("cls");
    }
}