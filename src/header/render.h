
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
  /** Render the main menu, centered for the supplied terminal dimensions. */
  static std::string render_main_menu(int row, int col);
  /** Render the difficulty-selection screen. */
  static std::string render_pause_menu(Screen game_screen);
  /**
   * Render one gameplay frame and update the player's resource state.
   * @param player_car player vehicle to draw and update
   * @param game_screen current terminal dimensions
   * @param race_track track geometry; start and end positions are recalculated
   * @param game_state elapsed-time and tick information
   * @param gameplay_settings difficulty-dependent degradation values
   * @return terminal escape sequences and artwork for the frame
   */
  static std::string render_game(
    Cars* player_car,
    Screen game_screen,
    Track* race_track,
    const GameState* game_state,
    GameplaySettings gameplay_settings
    );
  /** Render a car model at its current screen position. */
  static std::string print_race_car(const Cars* car);
  /**
   * Render the score or game-over screen.
   * @return terminal escape sequences and score-screen content
   */
  static std::string render_score(int high_score, int score, Screen game_screen, const std::string& game_over_message);
};


#endif //ASCIIRACER_ASCIISPRITE_H
