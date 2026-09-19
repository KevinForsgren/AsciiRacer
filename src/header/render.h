
#ifndef ASCIIRACER_ASCIISPRITE_H
#define ASCIIRACER_ASCIISPRITE_H
#include <string>
#include <vector>
#include "cars.h"
#include "gameSettings.h"




class render
{
  // UI and screen
public:
  static void render_main_menu(int row, int col);
  static void render_pause_menu(Screen game_screen);
  static std::string render_game(
    Cars* player_car,
    Screen game_screen,
    Track* race_track,
    const GameState* game_state,
    GameplaySettings gameplay_settings
    );
  static std::string print_race_car(const Cars* car);
  static void render_score(int high_score, int score, Screen game_screen, const std::string& game_over_message);
};


#endif //ASCIIRACER_ASCIISPRITE_H
