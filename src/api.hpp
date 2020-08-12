#pragma once

#include "bitboard.hpp"
#include <stdint.h>

typedef struct {
  const char *key;
  const char *url;
} api_config_t;

/*
 * Query the /api/move_needed route
 * return true if move is needed */
bool api_move_needed(const api_config_t *config);

/*
 * Query the /api/board route and save the result in board
 * return nonzero if board could not be read */
int api_board(const api_config_t *config, board_t *board);

/*
 * Post to /api/move */
void api_do_move(const api_config_t *config, move_t move);

/*
 * Set the player's name */
void api_set_name(const api_config_t *config, const char *name);