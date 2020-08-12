#include "hash_table.hpp"
#include "stats.hpp"
#include <cassert>
#include <cstdlib>
#include <cstring>

/**
 * The hash table uses zobrist hashing
 * Each position and color combination has a random bitstring
 * The xor of all of the bitstrings produces the hash for the board
 */

// TODO: precalculate these at compile time

// random bitstrings for each piece
// indexed as [location][color]
static uint32_t piece_bitstrings[64][2];

// hash results are precalculated for each row
// indexed as [y][row combination][color]
static uint32_t row_hash_precalc[8][256][2];

void hash_table_precalc() {
  // precalc bitstrings
  for (int i = 0; i < 64; i++) {
    for (int p = 0; p < 2; p++) {
      piece_bitstrings[i][p] = rand();
    }
  }

  // precalculate rows
  for (int y = 0; y < 8; y++) {
    for (unsigned int row = 0; row < 256; row++) {
      for (int p = 0; p < 2; p++) {
        uint32_t hash = 0;
        unsigned int row_tmp = row;
        while (row_tmp) {
          int set_index = bits_index_of_first_set(row_tmp);
          hash ^= piece_bitstrings[y * 8 + set_index][p];

          row_tmp &= ~(1ULL << set_index);
        }

        row_hash_precalc[y][row][p] = hash;
      }
    }
  }
}

void hash_table_alloc(hash_table_t *hash_table) {
  hash_table->hash_table = static_cast<hash_entry_t *>(
      calloc(HASH_TABLE_SIZE, sizeof(hash_entry_t)));
  memset(hash_table->hash_table, 0, HASH_TABLE_SIZE * sizeof(hash_entry_t));

  assert(hash_table->hash_table != nullptr);
}

void hash_table_clear(hash_table_t hash_table) {
#ifdef COUNT_STATS
  table_set_entries = 0;
#endif
  for (int i = 0; i < HASH_TABLE_SIZE; i++) {
    hash_table.hash_table[i].flags &= ~(HASH_TABLE_FLAGS_USED);
  }
}

void hash_table_age(hash_table_t hash_table) {
  for (int i = 0; i < HASH_TABLE_SIZE; i++) {
    if(hash_table.hash_table[i].flags & HASH_TABLE_FLAGS_USED)
      hash_table.hash_table[i].age++;
  }
}

uint32_t hash_board(board_t *board) {
  uint32_t hash = 0;
  for (int y = 0; y < 8; y++) {
    for (int p = 0; p < 2; p++) {
      hash ^= row_hash_precalc[y][(board->players[p] >> (y * 8)) & 0xff][p];
    }
  }

  return hash;
}

hash_entry_t *hash_table_lookup(hash_table_t hash_table, board_t *board) {
  uint32_t hash = hash_board(board) & HASH_KEY_MASK;
  hash_entry_t *entry = &hash_table.hash_table[hash];

  if ((entry->flags & HASH_TABLE_FLAGS_USED) &&
      board->players[0] == entry->board.players[0] &&
      board->players[1] == entry->board.players[1]) {
    entry->age = 0;
    return entry;
  } else {
    return nullptr;
  }
}

void hash_table_insert(hash_table_t hash_table, hash_entry_t *entry) {
  // hash entry
  uint32_t hash = hash_board(&entry->board) & HASH_KEY_MASK;
  // existing entry
  hash_entry_t *slot = &hash_table.hash_table[hash];

  /* if slot is unused, replace
   * if the new entry has a higher depth, replace */
  if (!(slot->flags & HASH_TABLE_FLAGS_USED) || entry->depth > slot->depth  || entry->age >= 2) {
#ifdef COUNT_STATS
    if (!(slot->flags & HASH_TABLE_FLAGS_USED))
      table_set_entries++;
#endif
    memcpy(slot, entry, sizeof(hash_entry_t));
  }
}