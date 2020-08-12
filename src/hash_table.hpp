#pragma once
#include "bitboard.hpp"

/**
 * Transposition Table
 * The table maps hashes of already visited board positions to their values
 * calculated on the previous visit. The hash table is open addressed and has a
 * fixed size. Entries are replaced as it fills up.
 */

/* Hash table key size mask */
#define HASH_KEY_MASK 0xffffff
// hash table size (2^HASH_KEY_SIZE)
#define HASH_TABLE_SIZE 16777216

#define HASH_TABLE_FLAGS_USED 1
#define BOUND_TYPE_EXACT 2
#define BOUND_TYPE_LOWERBOUND 4
#define BOUND_TYPE_UPERBOUND 8

/* TODO: aging replacement of nodes */
typedef struct {
  /* exact bound value */
  board_t board;
  /* minimax value (considering bound_type) */
  int32_t value;
  /* depth of search performed from this node */
  uint8_t depth;
  /* best move from this position */
  uint8_t best_move;
  /* bound characteristic of the move */
  uint8_t flags;
  /* age of the entry */
  uint8_t age;
} hash_entry_t;

typedef struct {
  hash_entry_t *hash_table;
} hash_table_t;

uint32_t hash_board(board_t *board);

/* allocate a hash table */
void hash_table_alloc(hash_table_t *hash_table);

/* clear a hash table (set all entries to unused) */
void hash_table_clear(hash_table_t hash_table);

/* age all entries in the hash table */
void hash_table_age(hash_table_t hash_table);

/* hash table precalculations */
void hash_table_precalc();

/* lookup entry in hash table */
hash_entry_t *hash_table_lookup(hash_table_t hash_table, board_t *board);
/* add entry to hash table */
void hash_table_insert(hash_table_t hash_table, hash_entry_t *entry);