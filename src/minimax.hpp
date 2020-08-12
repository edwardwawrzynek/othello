#pragma once

#include "bitboard.hpp"
#include "evaluator.hpp"
#include "hash_table.hpp"
#include <ctime>

/**
 * Get a move from the given board
 * search to the given depth */
int32_t get_move(move_t *dst_res_move, board_t *board, color_t player,
                 hash_table_t hash_table, double search_time);
