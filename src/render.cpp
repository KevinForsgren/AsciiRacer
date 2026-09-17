#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

#include "header/AsciiArt.h"
#include "header/cars.h"
#include "header/gameSettings.h"
#include "header/render.h"
#include "header/terminal.h"

static std::string print_race_car(const Cars* car);
static std::string print_infotainment_screen(int screen_row, int screen_col, int track_end, int second, const Cars* player_car);
static std::string print_meter(int current_capacity, int max_capacity);
static int random_int(int min, int max);

// Aliases
using TC = TerminalControl;

constexpr  int Lane_size = 12;

static std::vector<Car_Designs> car_designs = {
    {.body = TC::tc_color(220, 40, 55), .bumper = TC::tc_color(245, 245, 245), .tyre = TC::tc_color(190, 190, 190)},
    {.body = TC::tc_color(45, 105, 225), .bumper = TC::tc_color(235, 235, 235), .tyre = TC::tc_color(180, 180, 180)},
    {.body = TC::tc_color(30, 175, 95), .bumper = TC::tc_color(245, 245, 245), .tyre = TC::tc_color(185, 185, 185)},
    {.body = TC::tc_color(235, 115, 25), .bumper = TC::tc_color(250, 240, 220), .tyre = TC::tc_color(195, 195, 195)},
    {.body = TC::tc_color(145, 65, 210), .bumper = TC::tc_color(240, 235, 250), .tyre = TC::tc_color(185, 185, 190)},
    {.body = TC::tc_color(230, 185, 25), .bumper = TC::tc_color(255, 250, 220), .tyre = TC::tc_color(220, 220, 225)},
};


/**
 * Prints the game's menu screen
 * @param row total no of terminal screen row
 * @param col total no of terminal screen col
 */
void render::render_main_menu(const int row, const int col)
{
    std::stringstream frame_buffer;
    int i = 2;

    // only printing logo if screen is big enough
    if (col > 140 && row > 40)
    {
        for (const auto& str : game_title_big)
        {
            frame_buffer << TC::move_cursor(i, ( col - game_title_big_rows )/ 2) << str << std::endl;
            i++;
        }
    }
    else if (col > 70 && row > 22)
    {
        for (const auto& str : game_title_small)
        {
            frame_buffer << TC::move_cursor(i, (col - game_title_small_rows) / 2)
                << str << std::endl;
            i++;
        }
    }

    // printing options
    i += 5;
    for (const auto& str : game_menu)
    {
        frame_buffer << TC::move_cursor(i, (col - game_menu_cols) / 2) << str << std::endl;
        i++;
    }
    i += 2;

    const std::string message = "Press the appropriate highlighted key";

    frame_buffer << TC::move_cursor(i, (col - static_cast<int>(message.length())) / 2) << message <<
        std::endl;

    std::cout << frame_buffer.str() << std::flush;
}


/**
 *
 *
 */
void render::render_pause_menu(const Screen game_screen)
{
    std::stringstream frame_buffer;

    constexpr int pause_menu_height = game_difficulty_rows + 6;

    int i = (game_screen.Row - pause_menu_height) / 2;

    const std::string message_1 = "Select Difficulty";
    frame_buffer << TC::move_cursor(i, (game_screen.Col - static_cast<int>(message_1.length())) / 2) << message_1;
    i += 2;

    for (const auto& str: game_difficulty)
    {
        frame_buffer << TC::move_cursor(i, (game_screen.Col - game_difficulty_cols) /  2) << str;
        i++;
    }

    i += 2;

    const std::string message_2 = "Press the appropriate highlighted key";

    frame_buffer << TC::move_cursor(i, (game_screen.Col - static_cast<int>(message_2.length())) / 2) << message_2 <<
        std::endl;

    std::cout << frame_buffer.str() << std::flush;
}


/**
 * Prints the main racing track, cars and other ui to the terminal
 * @param player_car pointer to a class Car's object
 * @param game_screen pointer to game screen properties
 * @param game_over_message pointer to game over message
 * @param game_state
 * @param gameplay_settings
 */
int render::render_game(Cars* player_car, const Screen game_screen, std::string* game_over_message, GameState* game_state, const GameplaySettings gameplay_settings)
{


    std::stringstream frame_buffer;

    // Drawing track
    constexpr int track_size = (5 * Lane_size) + 10;
    const int track_start = (game_screen.Col - track_size) / 2;

    for (int i = 0; i <= game_screen.Row; i++)
    {
        frame_buffer << TC::move_cursor(i, track_start) << "║║";
        const int left_side_ground = track_start + Lane_size + 5;
        frame_buffer << TC::move_cursor(i, left_side_ground) << "┃";
        const int first_lane = left_side_ground + Lane_size;
        frame_buffer << TC::move_cursor(i, first_lane) << "।";
        const int middle_lane = first_lane + Lane_size;
        frame_buffer << TC::move_cursor(i, middle_lane) << "।" ;
        const int last_lane = middle_lane + Lane_size;
        frame_buffer << TC::move_cursor(i, last_lane) << "┃";
        const int right_side_ground = last_lane + Lane_size + 5;
        frame_buffer << TC::move_cursor(i, right_side_ground) << "║║";
    }
    const int track_end = track_start + (5 * Lane_size) + 10;


    // Printing player guides
    const std::string guide_message_left = "Press [A] for moving Left   Press [D] for moving Right";
    const std::string guide_message_right = "Avoid Grass and Collect fuel/tyre";

    frame_buffer << TC::move_cursor(game_screen.Row - 1, ( game_screen.Col - (game_screen.Col - track_start) - static_cast<int>(guide_message_left.length())) / 2 ) << guide_message_left;
    frame_buffer << TC::move_cursor(game_screen.Row - 1, track_end + (game_screen.Col - track_end - static_cast<int>(guide_message_right.length())) / 2 ) << guide_message_right;

    //Drawing Infotainment screen
    frame_buffer << print_infotainment_screen(game_screen.Row, game_screen.Col, track_end, static_cast<int>(game_state->gameTime), player_car);

    // Drawing player car
    frame_buffer << print_race_car(player_car);

    // Detecting track collision and car status
    if (player_car->x_position <= (track_start + 1) || (player_car->x_position + player_car->width) >= (track_end))
    {
        *game_over_message = "Car Collide to track";
        if (player_car->score > player_car->high_score)
        {
            player_car->high_score = player_car->score;
        }
        return 1;
    }

    if (player_car->tyre_health <= 0 || player_car->fuel <= 0 || player_car->chassis_health <= 0)
    {
        if (player_car->tyre_health <= 0)
        {
            *game_over_message = "Tyre Punctured";
        }
        else if (player_car->chassis_health == 0)
        {
            *game_over_message = "Car Chassis Destroyed";
        }
        else
        {
            *game_over_message = "Empty Fuel Tank";
        }

        if (player_car->score > player_car->high_score)
        {
            player_car->high_score = player_car->score;
        }
        return 1;
    }

    // Decrement player car fuel and tyre health
    if (static_cast<int>(game_state->gameTick) % 60 == 0)
    {
        player_car->tyre_health -=  gameplay_settings.tyre_degradation;
        player_car->fuel -= gameplay_settings.fuel_degradation;

    }

    // Increment player score
    // need to implement reward collecting score increment
    player_car->score = static_cast<int>(game_state->gameTick * 0.25);

    std::cout << frame_buffer.str() << std::flush;

    game_state->gameTick++;

    return 0;
}



void render::render_score(const int high_score, const int score, const Screen game_screen, const std::string& game_over_message)
{
    std::stringstream score_buffer;

    constexpr int score_page_height = 13;

    int i = (game_screen.Row - score_page_height)/2;
    if (!game_over_message.empty())
    {
        score_buffer << TC::move_cursor(i, (game_screen.Col - 9)/ 2) << "GAME OVER";
        i += 2;
        score_buffer << TC::move_cursor(i, (game_screen.Col - static_cast<int>(game_over_message.length())) / 2) << game_over_message;
        i += 2;
    } else
    {
        for (const auto& str: score_title)
        {
            score_buffer << TC::move_cursor(i, (game_screen.Col - score_title_cols) / 2) << str;
            i++;
        }
        i += 2;
    }

    score_buffer << TC::move_cursor(i, (game_screen.Col - 15) / 2) << "HIGH SCORE: " << high_score;
    i += 2;
    score_buffer << TC::move_cursor(i, (game_screen.Col - 15) / 2) << "YOUR SCORE: " << score;
    i += 2;

    const std::string message = "Press [H] for Main Menu";
    score_buffer << TC::move_cursor(i, (game_screen.Col - static_cast<int>(message.length())) / 2) << message <<
         std::endl;

    std::cout << score_buffer.str() << std::flush;
}


/**
 * Prints car on the racing track
 * @param car pointer to an object of Class car
 * @return
 */
static std::string print_race_car(const Cars* car)
{
    std::stringstream car_buffer;

    int a = 0;
    for (const auto& str : car->car_model)
    {
        car_buffer << TC::move_cursor(car->y_position + a, car->x_position);

        car_buffer << str;
        car_buffer << std::endl;
        a++;
    }

    car_buffer << TC::tc_color(255, 255, 255);

    return car_buffer.str();
}


static std::string print_infotainment_screen(const int screen_row, const int screen_col, const int track_end, const int second, const Cars* player_car)
{
    std::stringstream screen_buffer;

    int screen_start_row = ( screen_row - ( upper_infotainment_screen_rows + lower_infotainment_screen_rows + 3 ) - 2) / 2;

    const std::string heading = "CAR CONTROL";
    screen_buffer << TC::move_cursor(screen_start_row - 3, track_end + (screen_col - track_end - static_cast<int>(heading.length())) / 2) << heading;

    const int screen_start_col = track_end + (( screen_col - track_end - infotainment_screen_cols) / 2);

    // Upper Half
    for (int i = 0; i < upper_infotainment_screen_rows; i++)
    {
        // handling part with variable content size
        if (i == 4 || i == 6 || i == 8)
        {
            int current_capacity;

            if (i == 4)
            {
                screen_buffer << TC::move_cursor(screen_start_row + i, screen_start_col) << "│   Chassis   ";
                screen_buffer << print_meter(player_car->chassis_health, 1000);
            }
            else if (i == 6)
            {
                screen_buffer << TC::move_cursor(screen_start_row + i, screen_start_col) << "│   Tyre      ";
                screen_buffer << print_meter(player_car->tyre_health, 1000);
            }
            else
            {
                screen_buffer << TC::move_cursor(screen_start_row + i, screen_start_col) << "│   Fuel      ";
                screen_buffer << print_meter(player_car->fuel, 1000);
            }


            screen_buffer << "    │";
            continue;
        }



        screen_buffer << TC::move_cursor(screen_start_row + i, screen_start_col) << upper_infotainment_screen[i];
    }


    // Lower half
    screen_start_row += upper_infotainment_screen_rows + 3;

    for (int i = 0; i < lower_infotainment_screen_rows; i++)
    {
        // handling part with variable content size
        if (i == 5)
        {
            screen_buffer << TC::move_cursor(screen_start_row + i,screen_start_col) << "│   " << second << "s";

            for (int j = 0; j < infotainment_screen_cols - ( std::to_string(second).length() + 6); j++)
            {
                screen_buffer << " ";
            }

            screen_buffer << "│";
            continue;
        }

        if (i == 8 || i == 11)
        {
            int variable_digit;
            if (i == 8)
            {
                variable_digit = player_car->score;
            } else
            {
                variable_digit = player_car->high_score;
            }

            screen_buffer << TC::move_cursor(screen_start_row + i,screen_start_col) << "│   " << variable_digit;

            for (int j = 0; j < infotainment_screen_cols - ( std::to_string(variable_digit).length() + 5); j++)
            {
                screen_buffer << " ";
            }

            screen_buffer << "│";
            continue;
        }

        screen_buffer << TC::move_cursor(screen_start_row + i, screen_start_col) << lower_infotainment_screen[i];
    }


    return screen_buffer.str();
}


static std::string print_meter(const int current_capacity, const int max_capacity)
{
    std::stringstream meter_buffer;

    meter_buffer << "[";

    const float filled_part_percentage = std::round(( static_cast<float>(current_capacity) / max_capacity ) * 10);

    const int filled_part = static_cast<int>(filled_part_percentage);

    for (int i = 0; i < filled_part; i++)
    {
        meter_buffer << block_full;
    }

    const int empty_part = 10 - filled_part;
    for (int i = 0; i < empty_part; i++)
    {
        meter_buffer << non_block_full;
    }

    meter_buffer << "]";

    return meter_buffer.str();
}

/**
 * Generates a random number from the given range
 * @param min Range starts from
 * @param max Range ends
 * @return the random integer
 */
static int random_int(const int min, const int max)
{
    std::random_device device;

    std::mt19937 gen(device());
    std::uniform_int_distribution<> distribution(min, max);

    return distribution(gen);

}