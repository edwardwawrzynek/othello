#pragma once

#include <cstdint>

#define COUNT_STATS

// number of total boards visited by minimax
extern int64_t boards_visited;
// number of boards visited by minimax that used transposition table directly
// for value
extern int64_t boards_direct_table_hits;
// number of boards visited that used bounds from table
extern int64_t boards_bounds_table_hits;
// number of boards visited that used previous best move calculations
extern int64_t boards_best_move_hits;

// number of set entries in the transposition table
extern int64_t table_set_entries;
// depth to which minimax went
extern int64_t minimax_depth;

void stats_reset();
void stats_print(double search_time);