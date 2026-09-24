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

static std::string print_infotainment_screen(int screen_row, int screen_col, int track_end, int second, const Cars* player_car);
static std::string print_meter(int current_capacity, int max_capacity = 1'000);

// Aliases
using TC = TerminalControl;


/**
 * Prints the game's menu screen
 * @param row total no of terminal screen row
 * @param col total no of terminal screen col
 */
std::string render::render_main_menu(const int row, const int col)
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

    // Menu options
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

    //std::cout << frame_buffer.str() << std::flush;
    return frame_buffer.str();
}


/**
 *
 *
 */
std::string render::render_pause_menu(const Screen game_screen)
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

    return frame_buffer.str();
}


/**
 * Prints the main racing track, cars and other ui to the terminal
 * @param player_car pointer to a class Car's object
 * @param game_screen pointer to game screen properties
 * @param game_state
 * @param race_track
 * @param gameplay_settings
 */
std::string render::render_game(Cars* player_car,
    const Screen game_screen,
    Track* race_track,
    const GameState* game_state,
    const GameplaySettings gameplay_settings)
{


    std::stringstream frame_buffer;

    // Drawing track
    const int track_size = (5 * race_track->LaneSize) + 10;
    race_track->TrackStart = (game_screen.Col - track_size) / 2;

    for (int i = 0; i <= game_screen.Row; i++)
    {
        frame_buffer << TC::move_cursor(i, race_track->TrackStart) << "║║";
        const int left_side_ground = race_track->TrackStart + race_track->GroundSize;
        frame_buffer << TC::move_cursor(i, left_side_ground) << "┃";
        const int first_lane = left_side_ground + race_track->LaneSize;
        frame_buffer << TC::move_cursor(i, first_lane) << "।";
        const int middle_lane = first_lane + race_track->LaneSize;
        frame_buffer << TC::move_cursor(i, middle_lane) << "।" ;
        const int last_lane = middle_lane + race_track->LaneSize;
        frame_buffer << TC::move_cursor(i, last_lane) << "┃";
        const int right_side_ground = last_lane + race_track->GroundSize;
        frame_buffer << TC::move_cursor(i, right_side_ground) << "║║";
    }
    race_track->TrackEnd = race_track->TrackStart + track_size;


    // Printing player guides
    const std::string guide_message_left = "Press [A] for moving Left   Press [D] for moving Right";
    const std::string guide_message_right = "Avoid Grass and Collect fuel/tyre";

    frame_buffer << TC::move_cursor(game_screen.Row - 1, ( game_screen.Col - (game_screen.Col - race_track->TrackStart) - static_cast<int>(guide_message_left.length())) / 2 ) << guide_message_left;
    frame_buffer << TC::move_cursor(game_screen.Row - 1, race_track->TrackEnd + (game_screen.Col - race_track->TrackEnd - static_cast<int>(guide_message_right.length())) / 2 ) << guide_message_right;

    //Drawing Infotainment screen
    frame_buffer << print_infotainment_screen(game_screen.Row, game_screen.Col, race_track->TrackEnd, static_cast<int>(game_state->GameTime), player_car);

    // Drawing player car
    frame_buffer << print_race_car(player_car);


    // Decrement player car fuel and tyre health
    if (static_cast<int>(game_state->GameTick) % 60 == 0)
    {
        player_car->TyreHealth -=  gameplay_settings.TyreDegradation;
        player_car->Fuel -= gameplay_settings.FuelDegradation;

    }

    // Decrement player car tyre and chassis health when driving on ground
    if (static_cast<int>(game_state->GameTick) % 10 == 0)
    {
        if (player_car->xPosition >= (race_track->TrackEnd - race_track->GroundSize) || player_car->xPosition <= (race_track->TrackStart + race_track->GroundSize))
        {
            player_car->TyreHealth -= gameplay_settings.TyreDegradation;
            player_car->ChassisHealth -= gameplay_settings.ChassisDegradation;
        }
    }


    // Increment player score
    // need to implement reward collecting score increment
    player_car->Score = static_cast<int>(game_state->GameTick * 0.25);

    return frame_buffer.str();
}



std::string render::render_score(const int high_score, const int score, const Screen game_screen, const std::string& game_over_message)
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

    return score_buffer.str();
}


/**
 * Prints car on the racing track
 * @param car pointer to an object of Class car
 * @return
 */
std::string render::print_race_car(const Cars* car)
{
    std::stringstream car_buffer;

    int a = 0;
    for (const auto& str : car->Model)
    {
        car_buffer << TC::move_cursor(car->yPosition + a, car->xPosition);

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
                screen_buffer << print_meter(player_car->ChassisHealth, 1000);
            }
            else if (i == 6)
            {
                screen_buffer << TC::move_cursor(screen_start_row + i, screen_start_col) << "│   Tyre      ";
                screen_buffer << print_meter(player_car->TyreHealth, 1000);
            }
            else
            {
                screen_buffer << TC::move_cursor(screen_start_row + i, screen_start_col) << "│   Fuel      ";
                screen_buffer << print_meter(player_car->Fuel, 1000);
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
                variable_digit = player_car->Score;
            } else
            {
                variable_digit = player_car->HighScore;
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
