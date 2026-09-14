
#ifndef ASCIIRACER_ASCIISPRITE_H
#define ASCIIRACER_ASCIISPRITE_H
#include <string>
#include <vector>
#include "cars.h"



class render
{
  // UI and screen
public:
  static void render_main_menu(int row, int col);
  static void render_pause_menu(int row, int col);
  static int render_game(const Cars* player_car, int screen_row, int screen_col, std::string* game_over_message);
  static void render_score(int high_score, int score, int row, int col, const std::string& game_over_message);
};


#endif //ASCIIRACER_ASCIISPRITE_H
