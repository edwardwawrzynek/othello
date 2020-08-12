#include "minimax.hpp"
#include "stats.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>

// in order to facilitate limited search timing, minimax checks the time every
// ~half a million boards
static int board_i;
#define TIME_CHECK_BOARDS 2000000

class OthelloTimeUp {};

static inline int32_t minimax(move_t *dst_best_move, board_t *old_board,
                              move_t move_to_make, int depth, int32_t alpha,
                              int32_t beta, color_t player,
                              hash_table_t hash_table, time_t *start_time,
                              double search_time) {
  board_i++;
  if (board_i > TIME_CHECK_BOARDS) {
    board_i = 0;
    time_t curTime = time(nullptr);
    auto diff = difftime(curTime, *start_time);
    printf("Searching... %.0lf s    \r", search_time - diff);
    fflush(stdout);
    if (diff >= search_time) {
      throw OthelloTimeUp();
    }
  }
#ifdef COUNT_STATS
  boards_visited++;
#endif
  // preserve original alpha value
  int32_t orig_alpha = alpha;

  // copy board and make move
  board_t board = *old_board;
  if (move_to_make != 255) {
    board_make_move(&board, move_to_make, player == 1 ? 0 : 1);
  }

  auto color = player == 1 ? -1 : 1;
  // calculate moves for each player
  auto player0_moves = board_gen_moves(&board, 0);
  auto player1_moves = board_gen_moves(&board, 1);
  // if node is a win/loss, stop
  auto is_terminal = evaluate_is_terminal(&board, player0_moves, player1_moves);
  if (is_terminal != 0) {
    return color * is_terminal;
  }
  // if max depth was hit, stop
  if (depth == 0) {
    return color * evaluate_board(&board, player0_moves, player1_moves);
  }

  // first move to try
  move_t first_move = 255;
  move_t first_move_ignore_normal = 255;
  // lookup board in hash table
  hash_entry_t *hash_entry = hash_table_lookup(hash_table, &board);
  if (hash_entry != nullptr && hash_entry->depth >= depth) {
    // entry is valid
    if (hash_entry->flags & BOUND_TYPE_EXACT) {
#ifdef COUNT_STATS
      boards_direct_table_hits++;
#endif
      if (dst_best_move != nullptr) {
        *dst_best_move = hash_entry->best_move;
      }
      return hash_entry->value;
    } else if (hash_entry->flags & BOUND_TYPE_LOWERBOUND) {
#ifdef COUNT_STATS
      boards_bounds_table_hits++;
#endif
      alpha = std::max(alpha, hash_entry->value);
    } else if (hash_entry->flags & BOUND_TYPE_UPERBOUND) {
#ifdef COUNT_STATS
      boards_bounds_table_hits++;
#endif
      beta = std::min(beta, hash_entry->value);
    }

    // check for cutoff
    if (alpha >= beta) {
      if (dst_best_move != nullptr) {
        *dst_best_move = hash_entry->best_move;
      }
      return hash_entry->value;
    }
  } else if (hash_entry != nullptr && hash_entry->depth < depth) {
#ifdef COUNT_STATS
    boards_best_move_hits++;
#endif
    // search was to lower depth, but we can use it to order moves
    first_move = hash_entry->best_move;
    first_move_ignore_normal = first_move;
  }

  // can't use entry in hash table, so run minimax

  int32_t value = -MINIMAX_INF;
  // pick moves for us
  auto moves = player == 1 ? player1_moves : player0_moves;
  // if there are no legal moves for this player, skip to next player
  if (!moves) {
    value = -minimax(nullptr, &board, 255, depth - 1, -beta, -alpha,
                     player == 1 ? 0 : 1, hash_table, start_time, search_time);
  }
  // visit each move
  move_t best_move = 255;
  while (moves) {
    move_t move;
    // if we found a first move, use it first
    if (first_move != 255) {
      move = first_move;
      // clear first move
      first_move = 255;
    } else {
      move = bitboard_get_and_clear_first_move(&moves);
      if (move == first_move_ignore_normal)
        continue;
    }
    // run minimax on move
    auto child_score =
        -minimax(nullptr, &board, move, depth - 1, -beta, -alpha,
                 player == 1 ? 0 : 1, hash_table, start_time, search_time);
    if (child_score > value) {
      value = child_score;
      best_move = move;
    }
    // adjust alpha and cutoff
    alpha = std::max(alpha, value);
    if (alpha >= beta) {
      break;
    }
  }
  // create entry in hash table
  hash_entry_t new_entry;
  memset(&new_entry, 0, sizeof(hash_entry_t));
  new_entry.age = 0;
  new_entry.depth = depth;
  new_entry.value = value;
  new_entry.flags = HASH_TABLE_FLAGS_USED;
  if (value <= orig_alpha) {
    new_entry.flags |= BOUND_TYPE_UPERBOUND;
  } else if (value >= beta) {
    new_entry.flags |= BOUND_TYPE_LOWERBOUND;
  } else {
    new_entry.flags |= BOUND_TYPE_EXACT;
  }
  memcpy(&new_entry.board.players, &board.players, sizeof(board.players));
  new_entry.best_move = best_move;
  // insert entry into hash table
  hash_table_insert(hash_table, &new_entry);

  if (dst_best_move != nullptr) {
    *dst_best_move = best_move;
  }

  return value;
}

int32_t get_move(move_t *dst_res_move, board_t *board, color_t player,
                 hash_table_t hash_table, double search_time) {
  int32_t final_score;
  // get start time
  time_t start = time(nullptr);
  // run iterative deepening
  for (int cur_depth = 1; true; cur_depth++) {
#ifdef COUNT_STATS
    minimax_depth = cur_depth;
#endif
    try {
      final_score = minimax(dst_res_move, board, 255, cur_depth, -MINIMAX_INF,
                            +MINIMAX_INF, 0, hash_table, &start, search_time);
      if (final_score > EVAL_INF || final_score < -EVAL_INF)
        break;
    } catch (const OthelloTimeUp &e) {
      printf("Time Up                    \n");
      break;
    }
  }

  return final_score;
}
