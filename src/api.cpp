#include "api.hpp"

#include <cpr/cpr.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool api_move_needed(const api_config_t *config) {
  std::string comp_url = config->url;
  comp_url.append("/move_needed/");
  comp_url.append(config->key);

  cpr::Response r = cpr::Get(cpr::Url{comp_url});
  json data = json::parse(r.text);
  if (data.contains("needed") && data.value("needed", false)) {
    return true;
  } else {
    return false;
  }
}

int api_board(const api_config_t *config, board_t *board) {
  std::string comp_url = config->url;
  comp_url.append("/boards/");
  comp_url.append(config->key);

  cpr::Response r = cpr::Get(cpr::Url{comp_url});
  json data = json::parse(r.text);

  if (!data.contains("boards") || data.at("boards").is_null() ||
      !data.at("boards").is_array())
    return 1;

  auto &data_board = data.at("boards").at(0);
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      auto cell = data_board.at(x).at(y).get<int>();
      board_set_cell(board, xy_to_move(x, y),
                     cell == 0 ? -1 : (cell == -1 ? 1 : 0));
    }
  }

  return 0;
}

void api_do_move(const api_config_t *config, move_t move) {
  int x, y;
  move_to_xy(move, &x, &y);

  std::string comp_url = config->url;
  comp_url.append("/move/");
  comp_url.append(config->key);
  comp_url.push_back('/');
  comp_url.append(std::to_string(y));
  comp_url.push_back('/');
  comp_url.append(std::to_string(x));

  cpr::Post(cpr::Url{comp_url});
}

void api_set_name(const api_config_t *config, const char *name) {
  std::string comp_url = config->url;
  comp_url.append("/set_name/");
  comp_url.append(config->key);
  comp_url.push_back('/');
  std::string escaped_name = "";
  for(size_t i = 0; i < strlen(name); i++) {
    if(name[i] == ' ') 
      escaped_name.append("%20");
    else  
      escaped_name.push_back(name[i]);
  }
  comp_url.append(escaped_name);

  cpr::Post(cpr::Url{comp_url});
}
