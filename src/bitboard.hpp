#pragma once

#include <cstdint>

/* count the number of set bits in a (x86 popcnt) */
#define bits_popcount(a) __builtin_popcountll(a)
/* get the index of the first set bit in a (starting at lsb) (x86 bsf) */
#define bits_index_of_first_set(a) __builtin_ctzll(a)

/**
 * An Othello bitboard.
 * A bitboard has one bit for each square on the board.
 * The lower left square (a1) is bit 0 (lsb).
 * The upper right square (h8) is bit 63 (msb).
 */
typedef uint64_t bitboard_t;

/**
 * A move on a board
 * represented as the index into a bitboard */
typedef uint8_t move_t;

/**
 * A player's color on the board
 * 0 (white) or 1 (black) */
typedef uint8_t color_t;

/**
 * A full othello board, with a bitboard for each player */
typedef struct {
  bitboard_t players[2];
} board_t;

/**
 * Set a cell on a board to the given color, or empty if color == 255 */
void board_set_cell(board_t *board, move_t location, color_t color);

/**
 * Generate all legal moves for the given color
 * Moves are returned as a bitboard, where a set bit indicates a legal move */
bitboard_t board_gen_moves(board_t *board, color_t color);

/**
 * Find all frontier pieces (pieces next to empty squares) for the given color
 * Returns a bitboard, where set bits indicate a frontier pieces */
bitboard_t board_gen_frontiers(board_t *board, color_t color);

/**
 * Make a move on a full board. Places a piece at the move location, and flips
 * appropriate enemy pieces */
void board_make_move(board_t *board, move_t move_index, color_t color);

/**
 * Given a bitboard produced by full_board_gen_moves, convert the first set bit
 * into a move_t (returned), and clear it from the bitboard */
move_t bitboard_get_and_clear_first_move(bitboard_t *board);

/**
 * Convert a move_t to x and y coordinates on the board */
void move_to_xy(move_t move, int *x, int *y);

/**
 * Convert x and y to a move */
move_t xy_to_move(int x, int y);

/**
 * Convert a move into a labelled board position
 * buf has to be three characters long */
void move_to_string(char *buf, move_t move);

/**
 * Set the board to a random configuration */
void board_create_random(board_t *board);

/* print short output of board */
void board_print_short(board_t *board);

/* print the contents of a bitboard */
void bitboard_print(bitboard_t bitboard);

/* print a board (simple styling) */
void board_print(board_t *board);

/* print a board (ansi colored styling) */
void board_pretty_print(board_t *board);
