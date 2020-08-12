#include "evaluator.hpp"
#include <cassert>

// evaluate board position based purely on material
static inline int32_t evaluate_material(board_t *board) {
  return bits_popcount(board->players[0]) - bits_popcount(board->players[1]);
}

// evaluate the board based on mobility (number of moves available)
// also evaluates based on win / loss
static inline int32_t evaluate_mobility(board_t *board,
                                        bitboard_t player0_moves,
                                        bitboard_t player1_moves) {
  auto num_moves0 = bits_popcount(player0_moves);
  auto num_moves1 = bits_popcount(player1_moves);

  auto material = evaluate_material(board);

  // check for end condition
  if (num_moves0 == 0 && num_moves1 == 0) {
    return material * EVAL_INF;
  }

  return num_moves0 - num_moves1;
}

// evaluate the board based on frontier disks (less frontier -> higher score)
static inline int32_t evaluate_frontier(board_t *board) {
  return -bits_popcount(board_gen_frontiers(board, 0)) + bits_popcount(board_gen_frontiers(board, 1));
}

int32_t evaluate_is_terminal(board_t *board, bitboard_t player0_moves,
                             bitboard_t player1_moves) {

  auto num_moves0 = bits_popcount(player0_moves);
  auto num_moves1 = bits_popcount(player1_moves);

  // check for end condition
  if (num_moves0 == 0 && num_moves1 == 0) {
    return evaluate_material(board) * EVAL_INF;
  }

  return 0;
}

#define CORNERS 0x8100000000000081
#define AVOID_CORNERS_1 0x0042000000004200
#define AVOID_CORNERS_2 0x4281000000008142

int32_t evaluate_corners(board_t *board) {

  return +(10 * bits_popcount(board->players[0] & CORNERS)) -
         (10 * bits_popcount(board->players[1] & CORNERS)) +
         (-2 * bits_popcount(board->players[0] & AVOID_CORNERS_1)) -
         (-2 * bits_popcount(board->players[1] & AVOID_CORNERS_1)) +
         (-1 * bits_popcount(board->players[0] & AVOID_CORNERS_2)) -
         (-1 * bits_popcount(board->players[1] & AVOID_CORNERS_2));
}

int32_t evaluate_board(board_t *board, bitboard_t player0_moves,
                       bitboard_t player1_moves) {
  int32_t value = 4 * evaluate_mobility(board, player0_moves, player1_moves) +
                  4 * evaluate_corners(board);

  // during the midgame, minimize frontiers
  if(bits_popcount(board->players[0] | board->players[1]) < 40) {
    value += evaluate_frontier(board);
  }

  assert(value < MINIMAX_INF && value > -MINIMAX_INF);

  return value;
}
