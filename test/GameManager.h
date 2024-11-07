#pragma once
#include "../Mahjong/Table.h"
#include "../Mahjong/Action.h"
#include <vector>

using namespace mahjong;

class GameManager {
public:
    static GameManager& getInstance();
    void start_game();
    void set_game_length(int wind, int oya);
    
private:
    GameManager() = default;
    int max_wind = 2;
    int max_oya = 4;
    Result result;
}; 