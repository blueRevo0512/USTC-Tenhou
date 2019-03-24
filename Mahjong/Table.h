﻿#ifndef TABLE_H
#define TABLE_H

#include "Tile.h"

constexpr auto N_TILES = (34*4);
constexpr auto INIT_SCORE = 25000;

enum class Action : uint8_t {
	吃, 碰,
	杠,
	暗杠,
	手切,
	摸切,
	手切立直,
	摸切立直,
	摸牌,
	自摸,
	荣和,
	九种九牌,
};

struct SelfAction {
	Action action;
	std::vector<Tile*> correspond_tiles;
	SelfAction(Action, std::vector<Tile*>);
};

struct ResponseAction {
	Action action;
	std::vector<Tile*> correspond_tiles;
};

struct BaseGameLog {
	int player; // 主叫
	int player2;// 被叫
	Action action;
	Tile* 牌;
	std::vector<Tile*> 副露;	

	BaseGameLog(int, int, Action, Tile*, std::vector<Tile*>);

	std::string to_string();
};

class GameLog {
public:
	std::vector<BaseGameLog> logs;
	std::string to_string();
	void _log(int player1, int player2, Action, Tile*, std::vector<Tile*>);
	void log摸牌(int player, Tile*);
	void log摸切(int player, Tile*);
	void log手切(int player, Tile*);
};

class Player {
public:
	Player();
	bool riichi;
	Wind wind;
	bool 亲家;
	int score;
	std::vector<Tile*> hand;
	std::vector<Tile*> river;
	std::unordered_map<Tile*, std::vector<Tile*>> 副露s;
	std::string hand_to_string();
	std::string river_to_string();

	std::string to_string();

	void play_tile();

	void sort_hand();
	void test_show_hand();	
};

enum ResultType {
	荣和终局,
	自摸终局,
	无人听牌流局,
	一人听牌流局,
	两人听牌流局,
	三人听牌流局,
	四人听牌流局,
	两家荣和,
	三家荣和,

	流局满贯,

	九种九牌流局,
	四风连打,
	四杠流局,
	四家立直,
};

struct Result {
	ResultType result_type;

	// 自摸，一家荣和的情况
	int score;
	int fan;
	int fu;
	int 役满倍数;

	// 两家荣和的情况
	int score2;
	int fan2;
	int fu2;
	int 役满倍数2;

	// 三家荣和的情况
	int score3;
	int fan3;
	int fu3;
	int 役满倍数3;

	std::vector<int> winner;
	std::vector<int> loser;
};

class Table
{
private:
	Tile tiles[N_TILES];
public:
	int dora_spec;
	Wind 场风;
	int 庄家;
	void init_tiles();
	void init_red_dora_3();
	void shuffle_tiles();
	void init_yama();

	void init_wind();
	
	void _deal(int i_player);
	void _deal(int i_player, int n_tiles);

	void 发牌(int i_player);

	inline void next_turn() { turn++; turn = turn % 4; }

	GameLog openGameLog;
	GameLog fullGameLog;

	int turn;

	Table(int 庄家);

	// 因为一定是turn所在的player行动，所以不需要输入playerID
	std::vector<SelfAction> GetValidActions();
	std::vector<ResponseAction> GetValidResponse(int player);

	std::vector<Tile*> 牌山;
	Player player[4];
	~Table();

	void test_show_yama_with_王牌();
	void test_show_yama();
	void test_show_player_hand(int i_player);
	void test_show_all_player_hand();
	void test_show_player_info(int i_player);
	void test_show_all_player_info();
	void test_show_open_gamelog();
	void test_show_full_gamelog();

	inline void test_show_all() {
		test_show_yama_with_王牌();
		test_show_all_player_info();
		test_show_open_gamelog();
		test_show_full_gamelog();
		std::cout << "轮到Player" << turn << std::endl;
	}

	Result GameProcess(bool);

};

#endif