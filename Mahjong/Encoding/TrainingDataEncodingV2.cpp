#include "TrainingDataEncodingV2.h"
#include "fmt/core.h"

namespace_mahjong
namespace TrainingDataEncoding {
	namespace v2 {

		bool TableEncoder::_require_update()
		{
			return record_count < table->gamelog.logsize();
		}

		void TableEncoder::init()
		{
			// After initializing Table, oya is ready to play (with 14 tiles in hand)

			// Set visible_tiles
			//   Initial dora is visible and will always be updated when "DoraReveal" is logged
			Tile* dora_indicator = table->dora_indicator[0];
			if (dora_indicator->red_dora)
			{
				visible_tiles[locate_attribute(3, dora_indicator->tile)] = 1;
			}
			else
			{
				visible_tiles[locate_attribute(0, dora_indicator->tile)] = 1;
				visible_tiles_count[dora_indicator->tile]++;
			}

			size_t pos;
			// Init static self information
			
			pos = (size_t)EnumSelfInformation::pos_dora_indicator_1;			
			self_infos[0][locate_attribute(pos, dora_indicator->tile)]++;
			self_infos[1][locate_attribute(pos, dora_indicator->tile)]++;
			self_infos[2][locate_attribute(pos, dora_indicator->tile)]++;
			self_infos[3][locate_attribute(pos, dora_indicator->tile)]++;
			
			pos = (size_t)EnumSelfInformation::pos_dora_1;
			self_infos[0][locate_attribute(pos, get_dora_next(dora_indicator->tile))]++;
			self_infos[1][locate_attribute(pos, get_dora_next(dora_indicator->tile))]++;
			self_infos[2][locate_attribute(pos, get_dora_next(dora_indicator->tile))]++;
			self_infos[3][locate_attribute(pos, get_dora_next(dora_indicator->tile))]++;

			pos = (size_t)EnumSelfInformation::pos_self_wind;
			for (int i = 0; i < 4; ++i)
			{
				auto wind_type = table->players[i].wind - Wind::East + BaseTile::_1z;
				self_infos[i][locate_attribute(pos, wind_type)] = 1;
			}
			
			pos = (size_t)EnumSelfInformation::pos_game_wind;
			for (auto& self_info : self_infos)
			{
				auto wind_type = table->game_wind - Wind::East + BaseTile::_1z;
				self_info[locate_attribute(pos, wind_type)] = 1;
			}
			
			// Init static global information
			pos = (size_t)EnumGlobalInformation::pos_game_number;
			global_infos[0][pos] =
			global_infos[1][pos] =
			global_infos[2][pos] =
			global_infos[3][pos] = (table->game_wind - Wind::East) * 4 + (table->oya);

			pos = (size_t)EnumGlobalInformation::pos_game_size;
			global_infos[0][pos] =
			global_infos[1][pos] =
			global_infos[2][pos] =
			global_infos[3][pos] = 7;

			pos = (size_t)EnumGlobalInformation::pos_honba;
			global_infos[0][pos] =
			global_infos[1][pos] =
			global_infos[2][pos] =
			global_infos[3][pos] = table->honba;

			pos = (size_t)EnumGlobalInformation::pos_kyoutaku;
			global_infos[0][pos] =
			global_infos[1][pos] =
			global_infos[2][pos] =
			global_infos[3][pos] = table->kyoutaku;

			pos = (size_t)EnumGlobalInformation::pos_self_wind;
			global_infos[0][pos] = table->players[0].wind - Wind::East;
			global_infos[1][pos] = table->players[1].wind - Wind::East;
			global_infos[2][pos] = table->players[2].wind - Wind::East;
			global_infos[3][pos] = table->players[3].wind - Wind::East;

			pos = (size_t)EnumGlobalInformation::pos_game_wind;
			global_infos[0][pos] =
			global_infos[1][pos] =
			global_infos[2][pos] =
			global_infos[3][pos] = table->game_wind - Wind::East;

			/* pos_player_{x}_point has a reverse-positional implementation.
			*  For example,
			*  pos_player_{x}_point for player0
			*  x = 0 (player0 point)
			*  x = 1 (player3 point) Previous player
			*  x = 2 (player2 point) Opposite player
			*  x = 3 (player1 point) Next player
			*/
			pos = (size_t)EnumGlobalInformation::pos_player_0_point;
			global_infos[0][pos] = table->players[0].score / 100;
			global_infos[1][pos] = table->players[1].score / 100;
			global_infos[2][pos] = table->players[2].score / 100;
			global_infos[3][pos] = table->players[3].score / 100;

			pos = (size_t)EnumGlobalInformation::pos_player_1_point;
			global_infos[0][pos] = table->players[3].score / 100;
			global_infos[1][pos] = table->players[0].score / 100;
			global_infos[2][pos] = table->players[1].score / 100;
			global_infos[3][pos] = table->players[2].score / 100;

			pos = (size_t)EnumGlobalInformation::pos_player_2_point;
			global_infos[0][pos] = table->players[2].score / 100;
			global_infos[1][pos] = table->players[3].score / 100;
			global_infos[2][pos] = table->players[0].score / 100;
			global_infos[3][pos] = table->players[1].score / 100;

			pos = (size_t)EnumGlobalInformation::pos_player_3_point;
			global_infos[0][pos] = table->players[1].score / 100;
			global_infos[1][pos] = table->players[2].score / 100;
			global_infos[2][pos] = table->players[3].score / 100;
			global_infos[3][pos] = table->players[0].score / 100;

			pos = (size_t)EnumGlobalInformation::pos_remaining_tiles;
			global_infos[0][pos] = 
			global_infos[1][pos] = 
			global_infos[2][pos] = 
			global_infos[3][pos] = table->get_remain_tile();
			

			_update_hand(0);
			_update_hand(1);
			_update_hand(2);
			_update_hand(3);
			_update_visible_tiles();

			// The first record shouldn't be taken into account.
			record_count = table->gamelog.logsize();
		}

		void TableEncoder::_update_from_ankan(const BaseGameLog& log)
		{
			// update visible tiles
			auto tile = log.call_tiles[0]->tile;
			visible_tiles[locate_attribute(0, tile)] = 1;
			visible_tiles[locate_attribute(1, tile)] = 1;
			visible_tiles[locate_attribute(2, tile)] = 1;
			visible_tiles[locate_attribute(3, tile)] = 1; 
			visible_tiles_count[tile] = 4;

			// update the corresponding self_info
			int player = log.player;
			_update_hand(player);
			_update_visible_tiles();
		}

		void TableEncoder::_update_from_call(const BaseGameLog& log)
		{
			// update visible tiles
			for (auto tile : log.call_tiles)
			{
				auto basetile = tile->tile;
				if (tile->red_dora)
					visible_tiles[locate_attribute(3, basetile)] = 1;
				else
				{
					visible_tiles[locate_attribute(visible_tiles_count[basetile]++, basetile)] = 1;
				}
			}

			// update the corresponding self_info
			int player = log.player;
			_update_hand(player);
			_update_visible_tiles();
		}

		void TableEncoder::_update_from_kakan(const BaseGameLog& log)
		{
			// update visible tiles
			auto tile = log.tile->tile;
			visible_tiles[locate_attribute(0, tile)] = 1;
			visible_tiles[locate_attribute(1, tile)] = 1;
			visible_tiles[locate_attribute(2, tile)] = 1;
			visible_tiles[locate_attribute(3, tile)] = 1;
			visible_tiles_count[tile] = 4;

			// update the corresponding self_info
			int player = log.player;
			_update_hand(player);
			_update_visible_tiles();
		}

		void TableEncoder::_update_from_discard(const BaseGameLog& log, bool fromhand)
		{
			auto tile = log.tile;
			auto basetile = tile->tile;
			int player = log.player;
			if (tile->red_dora)
				visible_tiles[locate_attribute(3, basetile)] = 1;
			else
			{
				visible_tiles[locate_attribute(visible_tiles_count[basetile]++, basetile)] = 1;
			}

			// set furiten area
			// 对i来说，player是i的第p家，p = 0自家，p = 1下家，etc..
			// e.g. player = 2, i = 3, 则为上家，因此 player - i == -1 == 3
			for (int i = 0; i < 4; ++i)
			{
				int p = log.player - i;
				p = (p < 0) ? (p + 4) : p;
				size_t pos_discarded_by = p + (size_t)EnumSelfInformation::pos_discarded_by_player_1;
				self_infos[i][locate_attribute(pos_discarded_by, basetile)] = 1;
			}

			_update_hand(player);
			_update_visible_tiles();
		}

		void TableEncoder::_update_from_riichi(const BaseGameLog& log, bool fromhand)
		{
			auto tile = log.tile;
			auto basetile = tile->tile;
			int player = log.player;
			if (tile->red_dora)
				visible_tiles[locate_attribute(3, basetile)] = 1;
			else
			{
				visible_tiles[locate_attribute(visible_tiles_count[basetile]++, basetile)] = 1;
			}

			// set furiten area
			// 对i来说，player是i的第p家，p = 0自家，p = 1下家，etc..
			// e.g. player = 2, i = 3, 则为上家，因此 player - i == -1 == 3
			for (int i = 0; i < 4; ++i)
			{
				int p = log.player - i;
				p = (p < 0) ? (p + 4) : p;
				size_t pos_discarded_by = p + (size_t)EnumSelfInformation::pos_discarded_by_player_1;
				self_infos[i][locate_attribute(pos_discarded_by, basetile)] = 1;
			}

			_update_hand(player);
			_update_visible_tiles();
		}

		void TableEncoder::_update_from_riichi_success(const BaseGameLog& log)
		{
			size_t pos_kyoutaku = (size_t)EnumGlobalInformation::pos_kyoutaku;

			// 对i来说，player是i的第p家，p = 0自家，p = 1下家，etc..
			// e.g. player = 2, i = 3, 则为上家，因此 player - i == -1 == 3
			for (int i = 0; i < 4; ++i)
			{
				int p = log.player - i;
				p = (p < 0) ? (p + 4) : p;
				size_t pos_player = (size_t)EnumGlobalInformation::pos_player_0_point + p;
				global_infos[i][pos_player] -= 10;
			}
			global_infos[0][pos_kyoutaku] += 1;
			global_infos[1][pos_kyoutaku] += 1;
			global_infos[2][pos_kyoutaku] += 1;
			global_infos[3][pos_kyoutaku] += 1;
		}

		void TableEncoder::_update_hand(int player)
		{
			std::array<int, n_tile_types> ntiles = { 0 };
			std::array<dtype, 4 * n_col_self_info> hand_cols = { 0 };
			std::array<dtype, n_col_self_info> hand_cols_aka = { 0 };
			std::array<dtype, n_col_self_info> tsumo_tile = { 0 };
			auto& hand = table->players[player].hand;
			constexpr size_t offset_tile = (size_t)EnumSelfInformation::pos_hand_1;
			constexpr size_t offset_akadora = (size_t)EnumSelfInformation::pos_aka_dora;

			for (size_t i = 0; i < hand.size(); ++i) {
				int tile_type = (hand[i]->tile);
				hand_cols[locate_attribute(ntiles[tile_type] + offset_tile, tile_type)] = 1;
				++ntiles[tile_type];

				if (hand[i]->red_dora)
				{
					hand_cols_aka[tile_type] = 1;
				}
			}

			auto& self_info = self_infos[player];
			// overwrite self_info with hand_cols
			constexpr size_t szbytes_hand = 4 * n_col_self_info * sizeof(decltype(hand_cols[0]));
			memcpy(self_info.data(), hand_cols.data(), szbytes_hand);

			// overwrite self_info with hand_cols_aka
			constexpr size_t szbytes_aka = n_col_self_info * sizeof(decltype(hand_cols[0]));
			memcpy(self_info.data() + offset_akadora * n_col_self_info, hand_cols_aka.data(), szbytes_aka);

			constexpr size_t offset_tsumo = (size_t)EnumSelfInformation::pos_tsumo_tile;
			if (hand.size() % 3 == 2 && table->is_self_acting())
			{
				int tile_type = hand.back()->tile;
				tsumo_tile[tile_type] = 1;
			}

			// overwrite self_info with tsumo_tile
			constexpr size_t szbytes_tsumo = n_col_self_info * sizeof(decltype(tsumo_tile[0]));
			memcpy(self_info.data() + offset_tsumo * n_col_self_info, tsumo_tile.data(), szbytes_tsumo);
		}

		void TableEncoder::_update_from_draw(const BaseGameLog& log, bool from_rinshan)
		{
			int player = log.player;
			auto& self_info = self_infos[player];
			_update_hand(player);

			global_infos[0].back()--;
			global_infos[1].back()--;
			global_infos[2].back()--;
			global_infos[3].back()--;
		}

		void TableEncoder::_update_from_dora_reveal(const BaseGameLog& log)
		{
			auto tile = log.tile;
			auto dora_indicator = tile->tile;
			if (tile->red_dora)
				visible_tiles[locate_attribute(3, dora_indicator)] = 1;
			else
			{
				visible_tiles[locate_attribute(visible_tiles_count[dora_indicator]++, dora_indicator)] = 1;
			}
			_update_visible_tiles();

			constexpr size_t offset_dora_indicator = (size_t)EnumSelfInformation::pos_dora_indicator_1;
			constexpr size_t offset_dora = (size_t)EnumSelfInformation::pos_dora_indicator_1;
			auto dora = get_dora_next(dora_indicator);
			for (auto& self_info : self_infos)
			{
				self_info[locate_attribute(offset_dora_indicator, dora_indicator)]++;
				self_info[locate_attribute(offset_dora, dora)]++;
			}
		}

		void TableEncoder::_update_visible_tiles()
		{			
			constexpr size_t offset = (size_t)EnumSelfInformation::pos_discarded_number_1 * n_col_self_info;
			constexpr size_t szbytes = 4 * n_col_self_info * sizeof(decltype(visible_tiles[0]));

			memcpy(self_infos[0].data() + offset, visible_tiles.data(), szbytes);
			memcpy(self_infos[1].data() + offset, visible_tiles.data(), szbytes);
			memcpy(self_infos[2].data() + offset, visible_tiles.data(), szbytes);
			memcpy(self_infos[3].data() + offset, visible_tiles.data(), szbytes);
		}

		void TableEncoder::_update_record(const BaseGameLog& log)
		{
			game_record_t record = { 0 };
			
			static auto tile2idx = [](Tile* tile) -> size_t
			{
				if (tile->red_dora)
				{
					switch (tile->tile)
					{
					case _5m:
						return n_tile_types;
					case _5p:
						return n_tile_types + 1;
					case _5s:
						return n_tile_types + 2;
					default:
						throw std::runtime_error("Bad tile.");
					}
				}
				else
					return (size_t)(tile->tile);
			};

			int player = log.player;

			/* Update the call tiles for all fuuro, kakan */
			if (log.call_tiles.size() != 0)
			{
				for (auto tile : log.call_tiles)
				{
					record[tile2idx(tile)] = 1;
				}
			}

			if (log.action != LogAction::Chi &&
				log.action != LogAction::DrawNormal &&
				log.action != LogAction::DrawRinshan &&
				log.tile)
			{
				// For chi, the tiles correspond only to the 2 chi-tiles
				// For draw/drawrinshan, update in the last part. 
				// (Here only involves global update)
				record[tile2idx(log.tile)] = 1;
			}

			switch (log.action)
			{
			case LogAction::DrawNormal:
				record[offset_action + (size_t)EnumGameRecordAction::DrawNormal] = 1;
				break;
			case LogAction::DrawRinshan:
				record[offset_action + (size_t)EnumGameRecordAction::DrawRinshan] = 1;
				break;
			case LogAction::DiscardFromHand:
				record[offset_action + (size_t)EnumGameRecordAction::DiscardFromHand] = 1;
				break;
			case LogAction::DiscardFromTsumo:
				record[offset_action + (size_t)EnumGameRecordAction::DiscardFromTsumo] = 1;
				break;
			case LogAction::Chi: {
					int chitile = log.tile->tile;
					int handtile1 = log.call_tiles[0]->tile;
					int handtile2 = log.call_tiles[1]->tile;

					if (handtile2 < handtile1)
					{
						std::swap(handtile1, handtile2); 
						throw std::runtime_error("An abnormal LogAction object.");
					}

					if (chitile < handtile1)
						record[offset_action + (size_t)EnumGameRecordAction::ChiLeft] = 1;
					else if (chitile > handtile2)
						record[offset_action + (size_t)EnumGameRecordAction::ChiRight] = 1;
					else
						record[offset_action + (size_t)EnumGameRecordAction::ChiMiddle] = 1;
					break;
			    }
			case LogAction::Pon:
				record[offset_action + (size_t)EnumGameRecordAction::Pon] = 1;
				break;
			case LogAction::Kan:
				record[offset_action + (size_t)EnumGameRecordAction::Kan] = 1;
				break;
			case LogAction::KaKan:
				record[offset_action + (size_t)EnumGameRecordAction::Kakan] = 1;
				break;
			case LogAction::AnKan:
				record[offset_action + (size_t)EnumGameRecordAction::Ankan] = 1;
				break;
			case LogAction::RiichiDiscardFromHand:
				record[offset_action + (size_t)EnumGameRecordAction::RiichiFromHand] = 1;
				break;
			case LogAction::RiichiDiscardFromTsumo:
				record[offset_action + (size_t)EnumGameRecordAction::RiichiFromTsumo] = 1;
				break;
			case LogAction::RiichiSuccess:
				record[offset_action + (size_t)EnumGameRecordAction::RiichiSuccess] = 1;
				break;
			case LogAction::DoraReveal:
				// this does not involve record update.
				break;
			default:
				throw std::runtime_error("Bad LogAction (not handled in the _update_record).");
			}

			records[0].push_back(record);
			records[1].push_back(record);
			records[2].push_back(record);
			records[3].push_back(record);

			// 对i来说，player是i的第p家，p = 0自家，p = 1下家，etc..
			// e.g. player = 2, i = 3, 则为上家，因此 player - i == -1 == 3
			for (int i = 0; i < 4; ++i)
			{
				int p = player - i;
				p = (p < 0) ? (p + 4) : p;
				records[i].back()[offset_player + p] = 1;
			}

			/* Only player record the tile, other is empty */
			if (log.action == LogAction::DrawNormal ||
				log.action == LogAction::DrawRinshan)
				records[player].back()[tile2idx(log.tile)] = 1;
		}

		void TableEncoder::_update_ippatsu()
		{
			for (int player = 0; player < 4; ++player) {
				// 对i来说，player是i的第p家，p = 0自家，p = 1下家，etc..
				// e.g. player = 2, i = 3, 则为上家，因此 player - i == -1 == 3
				for (int i = 0; i < 4; ++i)
				{
					int p = player - i;
					p = (p < 0) ? (p + 4) : p;
					size_t pos_ippatsu = p + (size_t)EnumGlobalInformation::pos_player_0_ippatsu;
					global_infos[i][pos_ippatsu] = table->players[player].ippatsu ? 1 : 0;
				}
			}

		}

		void TableEncoder::update()
		{
			while (_require_update())
			{
				auto& log = table->gamelog[record_count];
				switch (log.action)
				{
				case LogAction::AnKan:
					_update_from_ankan(log);
					break;
				case LogAction::Pon:
				case LogAction::Chi:
				case LogAction::Kan:
					_update_from_call(log);
					break;
				case LogAction::KaKan:
					_update_from_kakan(log);
					break;
				case LogAction::DiscardFromHand:
					_update_from_discard(log, DiscardFromHand);
					break;
				case LogAction::DiscardFromTsumo:
					_update_from_discard(log, DiscardFromTsumo);
					break;
				case LogAction::RiichiDiscardFromHand:
					_update_from_riichi(log, DiscardFromHand);
					break;
				case LogAction::RiichiDiscardFromTsumo:
					_update_from_riichi(log, DiscardFromTsumo);
					break;
				case LogAction::RiichiSuccess:
					_update_from_riichi_success(log);
					break;
				case LogAction::DrawNormal:
					_update_from_draw(log, DrawNormally);
					break;
				case LogAction::DrawRinshan:
					_update_from_draw(log, DrawFromRinshan);
					break;
				case LogAction::DoraReveal:
					_update_from_dora_reveal(log);
					break;
				case LogAction::Kyushukyuhai:
				case LogAction::Ron:
				case LogAction::Tsumo:
					++record_count;
					return;
				default:
					throw std::runtime_error("Bad LogAction.");					
				}
				
				_update_record(log);
				_update_ippatsu();

				++record_count;
			}
		}

		void PassiveTableEncoder::encode_game_basic(int game_number,
			int game_size,
			int honba,
			int kyoutaku,
			int self_wind,
			int game_wind)
		{
			// Init static global information
			size_t pos;
			pos = (size_t)EnumGlobalInformation::pos_game_number;
			global_info[pos] = game_number;

			pos = (size_t)EnumGlobalInformation::pos_game_size;
			global_info[pos] = game_size;

			pos = (size_t)EnumGlobalInformation::pos_honba;
			global_info[pos] = honba;

			pos = (size_t)EnumGlobalInformation::pos_kyoutaku;
			global_info[pos] = kyoutaku;

			pos = (size_t)EnumGlobalInformation::pos_self_wind;
			global_info[pos] = self_wind - Wind::East;

			pos = (size_t)EnumGlobalInformation::pos_game_wind;
			global_info[pos]  = game_wind - Wind::East;

			pos = (size_t)EnumSelfInformation::pos_self_wind;
			self_info[locate_attribute(pos, game_wind - Wind::East + BaseTile::_1z)] = 1;

			pos = (size_t)EnumSelfInformation::pos_game_wind;
			self_info[locate_attribute(pos, game_wind - Wind::East + BaseTile::_1z)] = 1;
		}
		
		void PassiveTableEncoder::encode_points(const std::array<int, 4>& points)
		{
			/* pos_player_{x}_point has a reverse-positional implementation.
			*  For example,
			*  pos_player_{x}_point for player0
			*  x = 0 (player0 point)
			*  x = 1 (player3 point) Previous player
			*  x = 2 (player2 point) Opposite player
			*  x = 3 (player1 point) Next player
			*/
			size_t pos;
			pos = (size_t)EnumGlobalInformation::pos_player_0_point;
			global_info[pos] = points[0];

			pos = (size_t)EnumGlobalInformation::pos_player_1_point;
			global_info[pos] = points[3];

			pos = (size_t)EnumGlobalInformation::pos_player_2_point;
			global_info[pos] = points[2];

			pos = (size_t)EnumGlobalInformation::pos_player_3_point;
			global_info[pos] = points[1];
		}

		void PassiveTableEncoder::encode_remaining_tiles(int remain_tiles)
		{
			constexpr size_t pos = (size_t)EnumGlobalInformation::pos_remaining_tiles;
			global_info[pos] = remain_tiles;
		}

		void PassiveTableEncoder::encode_hand(const std::vector<Tile>& hand, bool after_chipon)
		{
			std::array<int, n_tile_types> ntiles = { 0 };
			std::array<dtype, 4 * n_col_self_info> hand_cols = { 0 };
			std::array<dtype, n_col_self_info> hand_cols_aka = { 0 };
			std::array<dtype, n_col_self_info> tsumo_tile = { 0 };
			// auto& hand = table->players[player].hand;
			constexpr size_t offset_tile = (size_t)EnumSelfInformation::pos_hand_1;
			constexpr size_t offset_akadora = (size_t)EnumSelfInformation::pos_aka_dora;

			for (size_t i = 0; i < hand.size(); ++i) {
				int tile_type = hand[i].tile;
				if (hand[i].red_dora)
					hand_cols_aka[tile_type] = 1;
				hand_cols[locate_attribute(ntiles[tile_type] + offset_tile, tile_type)] = 1;
				++ntiles[tile_type];
			}

			// overwrite self_info with hand_cols
			constexpr size_t szbytes_hand = 4 * n_col_self_info * sizeof(decltype(hand_cols[0]));
			memcpy(self_info.data(), hand_cols.data(), szbytes_hand);

			// overwrite self_info with hand_cols_aka
			constexpr size_t szbytes_aka = n_col_self_info * sizeof(decltype(hand_cols[0]));
			memcpy(self_info.data() + offset_akadora * n_col_self_info, hand_cols_aka.data(), szbytes_aka);

			constexpr size_t offset_tsumo = (size_t)EnumSelfInformation::pos_tsumo_tile;
			if (!after_chipon && hand.size() % 3 == 2)
			{
				int tile_type = hand.back().tile;
				tsumo_tile[tile_type] = 1;
			}

			// overwrite self_info with tsumo_tile
			constexpr size_t szbytes_tsumo = n_col_self_info * sizeof(decltype(tsumo_tile[0]));
			memcpy(self_info.data() + offset_tsumo * n_col_self_info, tsumo_tile.data(), szbytes_tsumo);
		}

		void PassiveTableEncoder::encode_river(const std::vector<BaseTile>& river, int relative_position)
		{
			for (auto tile : river)
			{
				// record visible_tiles
				visible_tiles[locate_attribute(visible_tiles_count[tile], tile)] += 1;
				visible_tiles_count[tile]++;

				// update furiten area
				size_t pos_discarded_by = (size_t)EnumSelfInformation::pos_discarded_by_player_1 + relative_position;
				self_info[locate_attribute(pos_discarded_by, tile)] = 1;
			}
			// update visible_tiles
			_update_visible_tiles();
		}

		void PassiveTableEncoder::encode_self_river(const std::vector<BaseTile>& river)
		{
			encode_river(river, 0);
		}

		void PassiveTableEncoder::encode_next_river(const std::vector<BaseTile>& river)
		{
			encode_river(river, 1);
		}

		void PassiveTableEncoder::encode_opposite_river(const std::vector<BaseTile>& river)
		{
			encode_river(river, 2);
		}

		void PassiveTableEncoder::encode_previous_river(const std::vector<BaseTile>& river)
		{
			encode_river(river, 3);
		}

		void PassiveTableEncoder::_update_visible_tiles()
		{
			constexpr size_t offset = (size_t)EnumSelfInformation::pos_discarded_number_1 * n_col_self_info;
			constexpr size_t szbytes = 4 * n_col_self_info * sizeof(decltype(visible_tiles[0]));

			memcpy(self_info.data() + offset, visible_tiles.data(), szbytes);
		}

		void PassiveTableEncoder::encode_fuuro(const std::vector<CallGroup>& callgroups, int relative_position)
		{
			for (auto& group : callgroups)
			{
				for (auto tile_ : group.tiles)
				{
					// record visible_tiles
					auto tile = tile_->tile;
					visible_tiles[locate_attribute(visible_tiles_count[tile], tile)] += 1;
					visible_tiles_count[tile]++;
				}
			}
			_update_visible_tiles();
		}

		void PassiveTableEncoder::encode_self_fuuro(const std::vector<CallGroup>& callgroups)
		{
			encode_fuuro(callgroups, 0);
		}

		void PassiveTableEncoder::encode_next_fuuro(const std::vector<CallGroup>& callgroups)
		{
			encode_fuuro(callgroups, 1);
		}

		void PassiveTableEncoder::encode_opposite_fuuro(const std::vector<CallGroup>& callgroups)
		{
			encode_fuuro(callgroups, 2);
		}

		void PassiveTableEncoder::encode_previous_fuuro(const std::vector<CallGroup>& callgroups)
		{
			encode_fuuro(callgroups, 3);
		}

		void PassiveTableEncoder::encode_dora(const std::vector<BaseTile> revealed_doras)
		{
			for (auto tile : revealed_doras)
			{
				// record visible_tiles
				visible_tiles[locate_attribute(visible_tiles_count[tile], tile)] += 1;
				visible_tiles_count[tile]++;
			}
			_update_visible_tiles();

			for (auto tile : revealed_doras)
			{
				self_info[locate_attribute((size_t)EnumSelfInformation::pos_dora_indicator_1, tile)] += 1;
				self_info[locate_attribute((size_t)EnumSelfInformation::pos_dora_1, get_dora_next(tile))] += 1;
			}
		}

		void PassiveTableEncoder::encode_riichi_states(const std::array<int, 4>& riichi_states)
		{
			// no information
		}

		void PassiveTableEncoder::encode_ippatsu_states(const std::array<int, 4>& ippatsu_states)
		{
			for (int i = 0; i < 4; ++i)
			{
				size_t pos = (size_t)EnumGlobalInformation::pos_player_0_ippatsu + i;
				global_info[pos] = ippatsu_states[i];
			}
		}
	}
}
namespace_mahjong_end
