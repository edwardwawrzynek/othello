#include "stats.hpp"
#include "hash_table.hpp"
#include <cstdio>

int64_t boards_visited;
int64_t boards_direct_table_hits;
int64_t boards_bounds_table_hits;
int64_t boards_best_move_hits;
int64_t table_set_entries;
int64_t minimax_depth;

void stats_reset() {
#ifdef COUNT_STATS
  boards_visited = 0;
  boards_direct_table_hits = 0;
  boards_bounds_table_hits = 0;
  boards_best_move_hits = 0;
  minimax_depth = 0;
  // don't reset table_set_entries, as table entries last between turns
#endif
}

/* print a number with a unit prefix (k / M) */
void pprint_num(double num) {
  if (num < 1000.0) {
    printf("%.2lf", num);
  } else if (num < 1000000.0) {
    printf("%.2lf k", num / 1000.0);
  } else {
    printf("%.2lf M", num / 1000000.0);
  }
}

void stats_print(double search_time) {
#ifdef COUNT_STATS
  printf("Depth Visited:        %li\n", minimax_depth);
  printf("Boards Visited:       ");
  pprint_num((double)boards_visited);
  printf("\nBoards/Second:        ");
  pprint_num(((double)boards_visited) / search_time);
  printf("/s\nTransposition Table:\n");
  printf("  Load Factor:        %.2lf %%\n",
         ((double)table_set_entries) / ((double)HASH_TABLE_SIZE) * 100.0);
  printf("  Exact Board Hits:   %.2lf %%\n",
         ((double)boards_direct_table_hits) / ((double)boards_visited) * 100.0);
  printf("  Bounds Hits:        %.2lf %%\n",
         ((double)boards_bounds_table_hits) / ((double)boards_visited) * 100.0);
  printf("  Best Move Hits:     %.2lf %%\n",
         ((double)boards_best_move_hits) / ((double)boards_visited) * 100.0);
#endif
}