#include "bitboard.hpp"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/** Bitboard Representation
 *    a  b  c  d  e  f  g  h
 *   ------------------------
 * 8| 56 57 58 59 60 61 62 63
 * 7| 48 49 50 51 52 53 54 55
 * 6| 40 41 42 43 44 45 46 47
 * 5| 32 33 34 35 36 37 38 39
 * 4| 24 25 26 27 28 29 30 31
 * 3| 16 17 18 19 20 21 22 23
 * 2| 08 09 10 11 12 13 14 15
 * 1| 00 01 02 03 04 05 06 07
 */

/* --- bitboard shifting methods --- */
static inline bitboard_t bitboard_shift_n(bitboard_t board) {
  return board << 8;
}

static inline bitboard_t bitboard_shift_s(bitboard_t board) {
  return board >> 8;
}

// for east and west shifting, mask needed to prevent edges pieces from moving
// up or down a row
#define shift_w_mask 0xfefefefefefefefe
#define shift_e_mask 0x7f7f7f7f7f7f7f7f

static inline bitboard_t bitboard_shift_w(bitboard_t board) {
  return (board << 1) & shift_w_mask;
}

static inline bitboard_t bitboard_shift_e(bitboard_t board) {
  return (board >> 1) & shift_e_mask;
}

static inline bitboard_t bitboard_shift_nw(bitboard_t board) {
  return bitboard_shift_w(bitboard_shift_n(board));
}

static inline bitboard_t bitboard_shift_ne(bitboard_t board) {
  return bitboard_shift_e(bitboard_shift_n(board));
}

static inline bitboard_t bitboard_shift_sw(bitboard_t board) {
  return bitboard_shift_w(bitboard_shift_s(board));
}

static inline bitboard_t bitboard_shift_se(bitboard_t board) {
  return bitboard_shift_e(bitboard_shift_s(board));
}

/* --- move generation --- */

// consider a row (where U is us, T them)
// _UTTT__T
// we can >> us, & it with them, and |= it into moves, repeat 6 times, resulting
// in: 00111000 which we >> once more, & with empty squares, finding the move:
// 00000100
#define board_gen_moves_case(shift_func)                                       \
  tmp = shift_func(us) & them;                                                 \
  for (int i = 0; i < 5; i++) {                                                \
    tmp |= shift_func(tmp) & them;                                             \
  }                                                                            \
  moves |= shift_func(tmp) & empty

bitboard_t board_gen_moves(board_t *board, color_t color) {
  auto us = board->players[color];
  auto them = board->players[!color];
  auto empty = ~(us | them);
  // make sure us and them don't share any pieces
  assert(!(us & them));

  bitboard_t moves = 0;
  bitboard_t tmp = 0;

  board_gen_moves_case(bitboard_shift_n);
  board_gen_moves_case(bitboard_shift_s);
  board_gen_moves_case(bitboard_shift_w);
  board_gen_moves_case(bitboard_shift_e);
  board_gen_moves_case(bitboard_shift_nw);
  board_gen_moves_case(bitboard_shift_ne);
  board_gen_moves_case(bitboard_shift_sw);
  board_gen_moves_case(bitboard_shift_se);

  assert(!(moves & ~empty));
  return moves;
}

// making a move follows the same principle as generating, except that we test
// that the shift after captures is the move
#define board_make_move_case(shift_func)                                       \
  captures = shift_func(move) & them;                                          \
  for (int i = 0; i < 5; i++) {                                                \
    captures |= shift_func(captures) & them;                                   \
  }                                                                            \
  if (shift_func(captures) & us) {                                             \
    *us_ptr |= captures;                                                       \
    *them_ptr &= ~captures;                                                    \
  }

void board_make_move(board_t *board, move_t move_index, color_t color) {
  // decode move
  bitboard_t move = 1ULL << move_index;

  auto us_ptr = &board->players[color];
  auto them_ptr = &board->players[!color];
  auto us = *us_ptr;
  auto them = *them_ptr;

  bitboard_t captures = 0;

  assert(!(us & them));
  assert((~(us | them)) & move);
#ifndef NDEBUG
  auto total_pieces = bits_popcount(us | them);
#endif

  *us_ptr |= move;
  board_make_move_case(bitboard_shift_n);
  board_make_move_case(bitboard_shift_s);
  board_make_move_case(bitboard_shift_w);
  board_make_move_case(bitboard_shift_e);
  board_make_move_case(bitboard_shift_nw);
  board_make_move_case(bitboard_shift_ne);
  board_make_move_case(bitboard_shift_sw);
  board_make_move_case(bitboard_shift_se);

  assert(bits_popcount(*us_ptr | *them_ptr) - total_pieces == 1);
}

#define board_frontier_case(func, inverse_func) \
  tmp = func(us) & empty;                       \
  frontier |= inverse_func(tmp);

bitboard_t board_gen_frontiers(board_t *board, color_t color) {
  auto us = board->players[color];
  auto empty = ~(board->players[0] | board->players[1]);
  bitboard_t frontier = 0ULL;
  bitboard_t tmp = 0ULL;

  board_frontier_case(bitboard_shift_n, bitboard_shift_s);
  board_frontier_case(bitboard_shift_s, bitboard_shift_n);

  board_frontier_case(bitboard_shift_w, bitboard_shift_e);
  board_frontier_case(bitboard_shift_e, bitboard_shift_w);

  board_frontier_case(bitboard_shift_nw, bitboard_shift_se);
  board_frontier_case(bitboard_shift_se, bitboard_shift_nw);

  board_frontier_case(bitboard_shift_ne, bitboard_shift_sw);
  board_frontier_case(bitboard_shift_sw, bitboard_shift_ne);

  return frontier;
}

move_t bitboard_get_and_clear_first_move(bitboard_t *board) {
#ifndef NDEBUG
  int pbits = bits_popcount(*board);
#endif
  assert(!!*board);
  move_t move = bits_index_of_first_set(*board);
  *board &= ~(1ULL << move);

  assert(pbits - bits_popcount(*board) == 1);

  return move;
}

void board_set_cell(board_t *board, move_t location, color_t color) {
  if (color == 255) {
    board->players[0] &= ~(1ULL << location);
    board->players[1] &= ~(1ULL << location);
  } else if (color == 0) {
    board->players[0] |= (1ULL << location);
    board->players[1] &= ~(1ULL << location);
  } else if (color == 1) {
    board->players[0] &= ~(1ULL << location);
    board->players[1] |= (1ULL << location);
  }
}

void move_to_xy(move_t move, int *x, int *y) {
  *x = move & 0x7;
  *y = (move >> 3) & 0x7;
}

move_t xy_to_move(int x, int y) {
  assert(x < 8 && y < 8 && x >= 0 && y >= 0);
  return x + y * 8;
}

void move_to_string(char *buf, move_t move) {
  int x, y;
  move_to_xy(move, &x, &y);
  assert(x < 8 && y < 8 && x >= 0 && y >= 0);

  buf[0] = "abcdefgh"[x];
  buf[1] = "12345678"[y];
  buf[2] = '\0';
}

void board_create_random(board_t *board) {
  // clear board
  memset(board, 0, sizeof(board_t));
  for (size_t i = 0; i < 64; i++) {
    auto type = rand() % 3;
    if (type == 0) {
      board->players[0] |= 1ULL << i;
    } else if (type == 1) {
      board->players[1] |= 1ULL << i;
    }
  }
}

void bitboard_print(bitboard_t bitboard) {
  for (size_t y = 8; y--;) {
    for (size_t x = 0; x < 8; x++) {
      size_t i = y * 8 + x;

      printf(" %lu", (bitboard >> i) & 1);
    }
    printf("\n");
  }
}

void board_print_short(board_t *board) {
  printf("%li-%li\n", board->players[0], board->players[1]);
}

void board_print(board_t *board) {
  for (size_t y = 8; y--;) {
    for (size_t x = 0; x < 8; x++) {
      size_t i = y * 8 + x;

      if ((board->players[0] >> i) & 1) {
        printf(" W");
      } else if ((board->players[1] >> i) & 1) {
        printf(" B");
      } else {
        printf(" .");
      }
    }
    printf("\n");
  }
}

void board_pretty_print(board_t *board) {
  for (size_t y = 8; y--;) {
    if (y == 7)
      printf("\x1b[38;5;3m   a b c d e f g h\n");

    printf(" \x1b[38;5;3m%lu\x1b[1m", y + 1);
    for (size_t x = 0; x < 8; x++) {
      size_t i = y * 8 + x;
      if ((board->players[0] >> i) & 1) {
        printf(" \x1b[38;5;15m#");
      } else if ((board->players[1] >> i) & 1) {
        printf(" \x1b[38;5;9m#");
      } else {
        printf(" \x1b[m.\x1b[1m");
      }
    }
    printf("\x1b[m\x1b[38;5;3m %lu\n", y + 1);
    if (y == 0)
      printf("\x1b[38;5;3m   a b c d e f g h\x1b[m\n");
  }

  printf("Distribution: %i-%i\n", bits_popcount(board->players[0]),
         bits_popcount(board->players[1]));
}
