#ifndef TRAINING_DATA_ENCODING_V2_H
#define TRAINING_DATA_ENCODING_V2_H

#include <vector>
#include "Table.h"

namespace_mahjong

namespace TrainingDataEncoding {
	namespace v2
	{
		constexpr size_t n_tile_types = 9 + 9 + 9 + 7;

		enum class EnumSelfInformation
		{
			pos_hand_1,
			pos_hand_2,
			pos_hand_3,
			pos_hand_4,
			pos_dora_1,
			pos_dora_indicator_1,
			pos_aka_dora,
			pos_game_wind,
			pos_self_wind,
			pos_tsumo_tile,
			pos_discarded_by_player_1,
			pos_discarded_by_player_2,
			pos_discarded_by_player_3,
			pos_discarded_by_player_4,
			pos_discarded_number_1,
			pos_discarded_number_2,
			pos_discarded_number_3,
			pos_discarded_number_4,
			n_self_information
		};
		constexpr size_t n_row_self_info = (size_t)EnumSelfInformation::n_self_information;
		constexpr size_t n_col_self_info = n_tile_types;

		using self_info_t = std::array<int8_t, n_row_self_info* n_col_self_info>;

		constexpr size_t locate_attribute(size_t attribute_row, size_t tile_type)
		{
			if (tile_type >= n_col_self_info)
			{
				throw std::runtime_error(fmt::format("Bad access to [{},{}]", attribute_row, tile_type));
			}
			return n_tile_types * attribute_row + tile_type;
		}

		constexpr size_t n_tile_types_include_aka = n_tile_types + 3;
		enum class EnumGameRecordAction
		{
			DrawNormal,
			DrawRinshan,
			DiscardFromHand,
			DiscardFromTsumo,
			ChiLeft,
			ChiMiddle,
			ChiRight,
			Pon,
			Kan,
			Ankan,
			Kakan,
			RiichiFromHand,
			RiichiFromTsumo,
			RiichiSuccess,
			n_actions,
		};
		constexpr size_t n_actions = (size_t)EnumGameRecordAction::n_actions;
		constexpr size_t n_players = 4;
		constexpr size_t n_step_actions = n_tile_types_include_aka + n_actions + n_players;
		constexpr size_t offset_tile = 0;
		constexpr size_t offset_action = offset_tile + n_tile_types_include_aka;
		constexpr size_t offset_player = offset_action + n_actions;

		// a record structure for encoding
		using game_record_t = std::array<int8_t, n_step_actions>;

		enum class EnumGlobalInformation
		{
			pos_game_number,
			pos_game_size,
			pos_honba,
			pos_kyoutaku,
			pos_self_wind,
			pos_game_wind,
			pos_player_0_point, // self
			pos_player_1_point, // next
			pos_player_2_point, // opposite
			pos_player_3_point, // previous
			pos_player_0_ippatsu, // self
			pos_player_1_ippatsu, // next
			pos_player_2_ippatsu, // opposite
			pos_player_3_ippatsu, // previous
			pos_remaining_tiles,
			n_global_information
		};
		
		using global_info_t = std::array<int8_t, (size_t)EnumGlobalInformation::n_global_information>;

		struct TableEncoder
		{
			std::array<uint8_t, 4 * n_col_self_info> visible_tiles = { 0 };
			std::array<uint8_t, n_col_self_info> visible_tiles_count = { 0 };
			Table* table = nullptr;
			int record_count = 0;

		public:
			std::array<self_info_t, 4> self_infos;
			std::array<std::vector<game_record_t>, 4> records;
			std::array<global_info_t, 4> global_infos;

			inline TableEncoder(Table* t) : table(t) {
				self_infos.fill(self_info_t{ 0 });
				records.fill(std::vector<game_record_t>());
				global_infos.fill(global_info_t{ 0 });
			}

			inline const auto& get_self_info(size_t player) const
			{
				return self_infos[player];
			}

			inline const auto& get_play_record(size_t player) const
			{
				return records[player];
			}

			inline const auto& get_global_info(size_t player) const
			{
				return global_infos[player];
			}

			bool _require_update();
			void _update_from_ankan(const BaseGameLog& log);
			void _update_from_call(const BaseGameLog& log);
			void _update_from_kakan(const BaseGameLog& log);
			void _update_from_discard(const BaseGameLog& log, bool fromhand);
			void _update_from_riichi(const BaseGameLog& log, bool fromhand);
			void _update_from_riichi_success(const BaseGameLog& log);
			void _update_from_draw(const BaseGameLog& log, bool from_rinshan);
			void _update_from_dora_reveal(const BaseGameLog& log);

			void _update_hand(int player);
			void _update_visible_tiles();
			void _update_record(const BaseGameLog& log);
			void _update_ippatsu();

			void init();
			void update();
		};

	}
}

namespace_mahjong_end
#endif