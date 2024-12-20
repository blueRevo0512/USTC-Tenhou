﻿#include "GameResult.h"
#include "Table.h"
#include "ScoreCounter.h"
#include "macro.h"
#include "Rule.h"

using namespace std;
namespace_mahjong

static Result _ryukyouku_interval(Table *table, ResultType type) {
	Result result;
	result.result_type = type;
	for (int i = 0; i < 4; ++i)
		result.score[i] = table->players[i].score;

	result.renchan = true;
	result.n_honba = table->honba + 1;
	result.n_riichibo = table->kyoutaku;

	return result;
}

Result generate_result_9hai(Table *table)
{
	return _ryukyouku_interval(table, ResultType::Ryukyouku_Interval_9Hai);
}

Result generate_result_4wind(Table *table)
{
	return _ryukyouku_interval(table, ResultType::Ryukyouku_Interval_4Wind);
}

Result generate_result_4riichi(Table *table)
{
	return _ryukyouku_interval(table, ResultType::Ryukyouku_Interval_4Riichi);
}

Result generate_result_4kan(Table *table)
{
	return _ryukyouku_interval(table, ResultType::Ryukyouku_Interval_4Kan);
}

Result generate_result_3ron(Table* table)
{
	return _ryukyouku_interval(table, ResultType::Ryukyouku_Interval_3Ron);
}

bool is_nagashi_mangan(River r) {
	return all_of(r.river.begin(), r.river.end(), [](RiverTile& tile){
		if (is_yaochuhai(tile.tile->tile) && tile.remain) {
			// remain 表示还在河里
			// 然后全是幺九牌
			return true;
		}
		return false;
	});
}

void nagashimangan_handler(Result& result, bool nagashi_mangan_status[4], 
	int n_nagashimangan, bool tenpai_status[4], Table* table)
{
	result.result_type = ResultType::NagashiMangan;
	if (tenpai_status[table->oya])
		result.renchan = true;
	else
		result.renchan = false;

	result.n_honba = table->honba + 1;
	result.n_riichibo = table->kyoutaku;
	if (n_nagashimangan == 4) {
		// 不涉及到分数支付
	}
	else if (n_nagashimangan == 3) {
		if (nagashi_mangan_status[table->oya])
		{
			result.score[table->oya] += 2000;
			for (int i = 0; i < 4; ++i) {
				if (nagashi_mangan_status[i])
					result.score[i] += 2000;
				else
					result.score[i] -= 8000;
			}
		}
		else
		{
			for (int i = 0; i < 4; ++i) {
				if (nagashi_mangan_status[i])
					result.score[i] += 4000;
				else
					result.score[i] -= 8000;
			}
		}
	}
	else if (n_nagashimangan == 2) {
		if (nagashi_mangan_status[table->oya])
		{
			result.score[table->oya] += 4000;
			for (int i = 0; i < 4; ++i) {
				if (nagashi_mangan_status[i])
					result.score[i] += 4000;
				else
					result.score[i] -= 6000;
			}
		}
		else
		{
			result.score[table->oya] -= 4000;
			for (int i = 0; i < 4; ++i) {
				if (nagashi_mangan_status[i])
					result.score[i] += 6000;
				else
					result.score[i] -= 4000;
			}
		}
	}
	else if (n_nagashimangan == 1) {
		if (nagashi_mangan_status[table->oya])
		{
			for (int i = 0; i < 4; ++i) {
				if (nagashi_mangan_status[i])
					result.score[i] += 12000;
				else
					result.score[i] -= 4000;
			}
		}
		else
		{
			result.score[table->oya] -= 2000;
			for (int i = 0; i < 4; ++i) {
				if (nagashi_mangan_status[i])
					result.score[i] += 8000;
				else
					result.score[i] -= 2000;
			}
		}
	}

}

Result generate_result_notile(Table *table)
{
	Result result;
	
	for (int i = 0; i < 4; ++i)
		result.score[i] = table->players[i].score;

	int n_nagashimangan = 0;
	bool nagashi_mangan_status[4] = { false,false,false,false };
	// 统计流局满贯的人数
	for (int i = 0; i < 4; ++i) {
		if (is_nagashi_mangan(table->players[i].river)) {
			nagashi_mangan_status[i] = true;
			n_nagashimangan++;
		}
	}

	// the number of tenpai players
	int n_tenpai = 0;
	// the atari status
	bool tenpai_status[4] = { false, false, false, false };

	for (int i = 0; i < 4; ++i) {
		auto atari_hai = get_atari_hai(convert_tiles_to_basetiles(table->players[i].hand),
			table->players[i].get_false_atari_hai());

		if (atari_hai.size() > 0) {
			tenpai_status[i] = true;
			n_tenpai++;
		}
		else {
			tenpai_status[i] = false;
		}
	}
	
	/* Nagashi mangan special handler */
	if (n_nagashimangan > 0) {
		nagashimangan_handler(result, nagashi_mangan_status, n_nagashimangan, tenpai_status, table);
		return result;
	}

	/* Normal ryukyouku */

	result.result_type = ResultType::Ryukyouku_Notile;
	// 统计罚符
	// 开始统计四人听牌的状态	

	result.n_honba = table->honba + 1;
	result.n_riichibo = table->kyoutaku;
	if (tenpai_status[table->oya])
		result.renchan = true;
	else
		result.renchan = false;

	if (n_tenpai == 4) {	}
	else if (n_tenpai == 0) { }
	else if (n_tenpai == 1) {
		for (int i = 0; i < 4; ++i) {
			if (tenpai_status[i])
				result.score[i] += 3000;
			else
				result.score[i] -= 1000;
		}
	}
	else if (n_tenpai == 2) {
		for (int i = 0; i < 4; ++i) {
			if (tenpai_status[i])
				result.score[i] += 1500;
			else
				result.score[i] -= 1500;
		}
	}
	else if (n_tenpai == 3) {
		for (int i = 0; i < 4; ++i) {
			if (tenpai_status[i])
				result.score[i] += 1000;
			else
				result.score[i] -= 3000;
		}
	}		
	
	return result;
}

Result generate_result_tsumo(Table * table)
{
	Result result;
	result.result_type = ResultType::TsumoAgari;
	int winner = table->turn;
	// 一人赢
	result.winner.push_back(winner);

	// 三人输
	for (int i = 0; i < 4; ++i) if (i != winner) result.loser.push_back(i);
	ScoreCounter sc(table, &table->players[winner], nullptr, false, false);
	auto yakus = sc.yaku_counter();
	bool is_oya = false;
	if (table->turn == table->oya)
		is_oya = true;
	yakus.calculate_score(is_oya, true);

	for (int i = 0; i < 4; ++i) {
		result.score[i] = table->players[i].score;
	}

	if (is_oya) {
		for (int i = 0; i < 4; ++i) {
			if (i == winner) {
				result.score[i] += (yakus.score1 * 3);
				result.score[i] += (table->kyoutaku * 1000);
				result.score[i] += (table->honba * 300);
			}
			else {
				result.score[i] -= (yakus.score1);
				result.score[i] -= (table->honba * 100);
			}
		}
	}
	else {
		for (int i = 0; i < 4; ++i) {
			if (i == winner) {
				result.score[i] += yakus.score1;
				result.score[i] += yakus.score2;
				result.score[i] += yakus.score2;
				result.score[i] += (table->kyoutaku * 1000);
				result.score[i] += (table->honba * 300);
			}
			else if (i == table->oya) {
				result.score[i] -= (yakus.score1);
				result.score[i] -= (table->honba * 100);
			}
			else {
				result.score[i] -= (yakus.score2);
				result.score[i] -= (table->honba * 100);
			}
		}
	}
	result.result_type = ResultType::TsumoAgari;
	
	if (winner == table->oya)
	{
		result.n_honba = table->honba + 1;
		result.renchan = true;
	}
	else
	{
		result.renchan = false;
		result.n_honba = 0;
	}
	result.n_riichibo = 0;
	result.results.insert({ winner, yakus });
	return result;
}

Result generate_result_ron(Table *table, Tile *agari_tile, const std::vector<int> &response_player, bool chankan, bool chanankan)
{
	Result result;
	result.result_type = ResultType::RonAgari;

	result.loser.push_back(table->turn); // 当回合的玩家是loser
	result.winner.assign(response_player.begin(), response_player.end());

	for (int i = 0; i < 4; ++i) {
		result.score[i] = table->players[i].score;
	}

	for (auto winner : response_player) {
		ScoreCounter sc(table, &table->players[winner], agari_tile, chankan, chanankan);
		auto yaku = sc.yaku_counter();
		yaku.calculate_score(winner == table->oya, false);

		result.results.insert({ winner, yaku });

		result.score[winner] += yaku.score1;
		result.score[table->turn] -= yaku.score1;			
	}

	// riichi 结算
	// response_player中离loser最近的那个
	int loser = table->turn;
	auto iter = min_element(response_player.begin(), response_player.end(), 
		[loser](int x1, int x2) {
		return get_player_distance(loser, x1) < get_player_distance(loser, x2);
	});
	result.score[*iter] += (table->kyoutaku * 1000);
	result.score[*iter] += table->honba * 300;
	result.score[table->turn] -= table->honba * 300;

	if (is_in(response_player, table->oya))
	{
		result.n_honba = table->honba + 1;
		result.renchan = true;
	}
	else
	{
		result.renchan = false;
		result.n_honba = 0;
	}
	result.n_riichibo = 0;
	return result;
}

Result generate_result_chanankan(Table * table, Tile* agari_tile, const std::vector<int> &response_player)
{
	return generate_result_ron(table, agari_tile, response_player, false, true);
}

Result generate_result_chankan(Table * table, Tile* agari_tile, const std::vector<int> &response_player)
{
	return generate_result_ron(table, agari_tile, response_player, true, false);
}

namespace_mahjong_end
