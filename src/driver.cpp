#include "api.hpp"
#include "bitboard.hpp"
#include "hash_table.hpp"
#include "minimax.hpp"
#include "stats.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>

hash_table_t hash_table;
api_config_t api_config;

void init_hash_table() {
  srand(0);
  hash_table_precalc();
  hash_table_alloc(&hash_table);
}

// number of pieces present on last board
static int pieces_on_last_board = 4;
void clear_hash_table_if_new_board(board_t *board, hash_table_t hash_table) {
  int num_pieces = bits_popcount(board->players[0] | board->players[1]);

  if (abs(pieces_on_last_board - num_pieces) > 5)
    hash_table_clear(hash_table);

  pieces_on_last_board = num_pieces;
}

move_t process_move(board_t *board, double search_time) {
  stats_reset();
  printf("-------------------------------\n:: ");
  board_print_short(board);
  board_pretty_print(board);
  hash_table_age(hash_table);
  clear_hash_table_if_new_board(board, hash_table);
  // run minimax
  move_t move;
  int32_t score = get_move(&move, board, 0, hash_table, search_time);

  char move_name[3];
  move_to_string(move_name, move);
  printf("Selected Move: \x1b[1m%s\x1b[m, Score: %i\n", move_name, score);
  if (score > EVAL_INF || score < -EVAL_INF) {
    int32_t piece_diff = (score / EVAL_INF)/2;
    printf("Expected Distribution: %i-%i\n", 32 + piece_diff, 32 - piece_diff);
  }
  printf("\n");
  stats_print(search_time);

  return move;
}

int main(int argc, char **argv) {
  if (argc != 5) {
    printf("Usage: %s URL KEY NAME SEARCH_TIME(s)\n", argv[0]);
    exit(1);
  }
  api_config.url = argv[1];
  api_config.key = argv[2];

  init_hash_table();
  hash_table_clear(hash_table);

  api_set_name(&api_config, argv[3]);
  double search_time = (double)strtol(argv[4], nullptr, 10);

  while (true) {
    if (!api_move_needed(&api_config)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      continue;
    }

    board_t board;
    api_board(&api_config, &board);
    move_t move = process_move(&board, search_time);
    api_do_move(&api_config, move);
  }
}
