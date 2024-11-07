#include "GameManager.h"
#include "Network.h"
#include "pages/ConsolePage.h"
#include <boost/asio.hpp>
#include <set>

using namespace mahjong;

void GameManager::set_game_length(int wind, int oya) {
    max_wind = wind;
    max_oya = oya;
}

GameManager& GameManager::getInstance() {
    static GameManager instance;
    return instance;
}

void GameManager::start_game() {
    auto& network = NetworkManager::getInstance();
    bool is_first = true;
    
    for (int wind = 0; wind < max_wind + (is_first || max_wind == 4 ? 0 : *std::max_element(result.score.begin(), result.score.end()) < 30000); wind++) {
        for(int oya = 0; oya < max_oya + (max_oya == 1 && *std::max_element(result.score.begin(), result.score.end()) < 30000 ? 3 : 0); oya++) {
            while((wind < max_wind && oya < max_oya) || *std::max_element(result.score.begin(), result.score.end()) < 30000) {
                mahjong::Table t;
                if (is_first) {
                    t.game_init();
                    is_first = false;
                }
                else {
                    t.game_init_with_config({}, std::vector<int>(result.score.begin(), result.score.end()), 
                                          result.n_riichibo, result.n_honba, wind, oya);
                }

                try {
                    std::string message[4];
                    int last_player;
                    mahjong::SelfAction self_action;
                    mahjong::ResponseAction response_action;
                    bool is_self_action = false;

                    while(!t.is_over()) {
                        for(int player = 0; player < 4; player++) {
                            message[player] = (t.get_phase() < 4 ? 
                                t.to_string(player) + "\n" + std::to_string(t.who_make_selection()) + " is making selection.\n" : 
                                t.to_string(player) + "\n" + (t.who_make_selection() == player ? "You are making selection.\n" : ""));
                        }

                        if((is_self_action && self_action.action != mahjong::BaseAction::Discard) || 
                           (!is_self_action && response_action.action != mahjong::BaseAction::Pass)) {
                            for(int player = 0; player < 4; player++) {
                                message[player] = message[player] + "Player " + std::to_string(last_player) + " just " + 
                                    (is_self_action ? self_action.to_string() : response_action.to_string()) + "!\n";
                            }
                        }

                        if (t.get_phase() < 4) {
                            message[t.who_make_selection()] += "SelfAction phase.\n";
                            auto actions = t.get_self_actions();
                            int index = 0, real_index = 0;
                            std::set<std::string> action_set;
                            std::map<int, int> index_map;

                            for (auto &action : actions) {
                                std::string action_str = action.to_string() + (t.check_from_hand(action) ? "" : "*");
                                index_map[index += (!action_set.count(action_str))] = real_index++;
                                if (!action_set.count(action_str)) {
                                    action_set.insert(action_str);
                                    message[t.who_make_selection()] += std::to_string(index) + ". " + action_str + "\n";
                                }
                            }

                            for(int player = 0; player < 4; player++) {
                                boost::asio::write(*network.clients[player], boost::asio::buffer(message[player]));
                            }

                            int selection = 1;
                            try {
                                while(true) {
                                    char data[1024];
                                    boost::system::error_code error;
                                    size_t length = network.clients_selection[t.who_make_selection()]->read_some(
                                        boost::asio::buffer(data), error);
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
                            } catch(const std::exception& e) {
                                ConsolePage::print("Exception in selection receiver: " + std::string(e.what()));
                            }

                            last_player = t.who_make_selection();
                            self_action = actions[index_map[selection]];
                            is_self_action = true;
                            t.make_selection(index_map[selection]);
                        }
                        else {
                            message[t.who_make_selection()] += "ResponseAction phase.\n";
                            auto actions = t.get_response_actions();
                            if(actions.size() == 1) {
                                t.make_selection(0);
                                continue;
                            }
                            int index = 0, real_index = 0;
                            std::set<std::string> action_set;
                            std::map<int, int> index_map;
                            for (auto &action : actions) {
                                std::string action_str = action.to_string();
                                index_map[index += (!action_set.count(action_str))] = real_index++;
                                if(!action_set.count(action_str)) {
                                    action_set.insert(action_str);
                                    message[t.who_make_selection()] += std::to_string(index) + ". " + action_str + "\n";
                                }
                            }
                            for(int player = 0; player < 4; player++) {
                                boost::asio::write(*network.clients[player], boost::asio::buffer(message[player]));
                            }
                            int selection = 1;
                            try {
                                while(true) {
                                    char data[1024];
                                    boost::system::error_code error;
                                    size_t length = network.clients_selection[t.who_make_selection()]->read_some(
                                        boost::asio::buffer(data), error);
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
                            } catch(const std::exception& e) {
                                ConsolePage::print("Exception in selection receiver: " + std::string(e.what()));
                            }
                            last_player = t.who_make_selection();
                            response_action = actions[index_map[selection]];
                            is_self_action = false;
                            t.make_selection(index_map[selection]);
                        }
                    }

                    bool is_riichi = false;
                    for(auto result: t.get_result().results) {
                        is_riichi |= std::any_of(result.second.yakus.begin(), result.second.yakus.end(),
                            [](const mahjong::Yaku& yaku) { return yaku == mahjong::Yaku::Riichi || yaku == mahjong::Yaku::Dabururiichi; });
                    }

                    for(int player = 0; player < 4; player++) {
                        message[player] = t.show_dora(is_riichi) + "Result: \n" + t.get_result().to_string() + 
                            "\n" + "Type anything to continue.\n";
                    }

                    for(int player = 0; player < 4; player++) {
                        boost::asio::write(*network.clients[player], boost::asio::buffer(message[player]));
                    }

                    for(int player = 0; player < 4; player++) {
                        try {
                            while(true) {
                                char data[1024];
                                boost::system::error_code error;
                                size_t length = network.clients_selection[player]->read_some(
                                    boost::asio::buffer(data), error);
                                if(!length)
                                    continue;
                                else if(error == boost::asio::error::eof)
                                    break;
                                else if(error)
                                    throw boost::system::system_error(error);
                                break;
                            }
                        } catch(const std::exception& e) {
                            ConsolePage::print("Exception in selection receiver: " + std::string(e.what()));
                        }
                    }
                    result = t.get_result();
                } catch(const std::exception& e) {
                    ConsolePage::print("Exception in client receiver: " + std::string(e.what()));
                }
                if(!result.renchan) break;
            }
        }
    }
} 