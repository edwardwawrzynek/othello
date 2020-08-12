#pragma once

#include "bitboard.hpp"
#include <cstdint>

#define EVAL_INF    1000000
#define MINIMAX_INF 1000000000

/**
 * Evaluate a board.
 * Result is positive if board is good for player 0, negative if it is good for
 * player 1 */
int32_t evaluate_board(board_t *board, bitboard_t player0_moves,
                       bitboard_t player1_moves);

/**
 * If the board is a terminal (end) board, return its score, 0 otherwise
 */
int32_t evaluate_is_terminal(board_t *board, bitboard_t player0_moves,
                             bitboard_t player1_moves);