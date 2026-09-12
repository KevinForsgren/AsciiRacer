
#ifndef ASCIIRACER_ASCIISPRITE_H
#define ASCIIRACER_ASCIISPRITE_H
#include <string>
#include <vector>
#include "cars.h"



class AsciiSprite
{
  // UI and screen
public:
  static void print_main_menu(int row, int col);
  static void print_pause_menu(int row, int col);
  static int print_game(const Cars* main_car, int screen_row, int screen_col, std::string* game_over_message);
  static void print_score(int high_score, int score, int row, int col, const std::string& game_over_message);
};


#endif //ASCIIRACER_ASCIISPRITE_H
