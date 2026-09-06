
#ifndef ASCIIRACER_ASCIISPRITE_H
#define ASCIIRACER_ASCIISPRITE_H
#include <string>
#include <vector>
#include "cars.h"

class AsciiSprite
{
  // UI and screen
public:
  static void print_title(int row, int col);
  static void print_game(const Cars* main_car);
  static void print_score(char c);
};


#endif //ASCIIRACER_ASCIISPRITE_H
