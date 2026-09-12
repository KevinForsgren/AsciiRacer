
#include "header/AsciiSprite.h"

#include <iostream>
#include <random>
#include <sstream>
#include <string>

#include "header/terminal.h"
#include "header/AsciiArt.h"
#include "header/cars.h"

static std::string print_race_car(const Cars* car);
static std::string print_race_light(const int second);
static int random_int(int min, int max);

// Aliases
using TC = TerminalControl;

constexpr  int Lane_size = 11;
constexpr int Track_size = 36;

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
void AsciiSprite::print_main_menu(const int row, const int col)
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
void AsciiSprite::print_pause_menu(const int row, const int col)
{
    std::stringstream frame_buffer;

    constexpr int pause_menu_height = game_difficulty_rows + 6;

    int i = (row - pause_menu_height) / 2;

    const std::string message_1 = "Select Difficulty";
    frame_buffer << TC::move_cursor(i, (col - static_cast<int>(message_1.length())) / 2) << message_1;
    i += 2;

    for (const auto& str: game_difficulty)
    {
        frame_buffer << TC::move_cursor(i, (col - game_difficulty_cols) /  2) << str;
        i++;
    }

    i += 2;

    const std::string message_2 = "Press the appropriate highlighted key";

    frame_buffer << TC::move_cursor(i, (col - static_cast<int>(message_2.length())) / 2) << message_2 <<
        std::endl;

    std::cout << frame_buffer.str() << std::flush;
}


/**
 * Prints the main racing track, cars and other ui to the terminal
 * @param main_car pointer to a class Car's object
 * @param screen_row
 * @param screen_col
 * @param game_over_message
 */
int AsciiSprite::print_game(const Cars* main_car, const int screen_row, const int screen_col, std::string* game_over_message)
{
    std::stringstream frame_buffer;

    // Drawing track light
    frame_buffer << print_race_light(1);

    // Drawing track
    const int track_start = (screen_col - Track_size) / 2 ;
    for (int i = 0; i < screen_row; i++)
    {
        frame_buffer << TC::move_cursor(i, track_start) << "┃";
        const int first_lane = track_start + Lane_size + 1;
        frame_buffer << TC::move_cursor(i, first_lane) << "।";
        const int middle_lane = first_lane + Lane_size + 1;
        frame_buffer << TC::move_cursor(i, middle_lane) << "।" ;
        const int last_lane = middle_lane + Lane_size + 1;
        frame_buffer << TC::move_cursor(i, last_lane) << "┃";
    }

    // Drawing user car
   frame_buffer << print_race_car(main_car);

    // Detecting track collision
    if (main_car->x_position <= track_start || (main_car->x_position + main_car->width) >= (track_start + Track_size))
    {
        *game_over_message = "Car Collide";
        // needs to update car->score and also compare score with high score
        return 1;
    }

    std::cout << frame_buffer.str() << std::flush;

    return 0;
}



void AsciiSprite::print_score(const int high_score, const int score, const int row, const int col, const std::string& game_over_message)
{
    std::stringstream score_buffer;

    constexpr int score_page_height = 13;

    int i = (row - score_page_height)/2;
    if (!game_over_message.empty())
    {
        score_buffer << TC::move_cursor(i, (col - 9)/ 2) << "GAME OVER";
        i += 2;
        score_buffer << TC::move_cursor(i, (col - static_cast<int>(game_over_message.length())) / 2) << game_over_message;
        i += 2;
    } else
    {
        for (const auto& str: score_title)
        {
            score_buffer << TC::move_cursor(i, (col - score_title_cols) / 2) << str;
            i++;
        }
        i += 2;
    }

    score_buffer << TC::move_cursor(i, (col - 15) / 2) << "HIGH SCORE: " << high_score;
    i += 2;
    score_buffer << TC::move_cursor(i, (col - 15) / 2) << "YOUR SCORE: " << score;
    i += 2;

    const std::string message = "Press [H] for Main Menu";
    score_buffer << TC::move_cursor(i, (col - static_cast<int>(message.length())) / 2) << message <<
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


static std::string print_race_light(const int second)
{
    std::stringstream frame_buffer;

    const std::vector<std::string> race_light = AsciiArt::race_light_dynamic(second);

    int i = 2;
    for (const auto& str: race_light)
    {
        frame_buffer << TC::move_cursor(i, 2) << str;
        i++;
    }
    i++;
    frame_buffer << TC::move_cursor(i, 2) << "Race Starts in: " << second << "s";

    return frame_buffer.str();
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