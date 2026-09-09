
#include "header/AsciiSprite.h"

#include <iostream>
#include <random>
#include <sstream>
#include <string>

#include "header/terminal.h"
#include "header/AsciiArt.h"
#include "header/cars.h"

static std::string print_race_car(const Cars* car);
static int random_int(int min, int max);
using TC = TerminalControl;

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
void AsciiSprite::print_title(const int row, const int col)
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

    // printing border
    i += 5;
    for (const auto& str : game_menu)
    {
        frame_buffer << TC::move_cursor(i, (col - 102)/ 2) << str << std::endl;
        i++;
    }

    i += 2;

    const std::string message = "Press the appropriate highlighted key";

    frame_buffer << TC::move_cursor(i, (col - static_cast<int>(message.length())) / 2) << message <<
        std::endl;

    std::cout << frame_buffer.str() << std::flush;
}


/**
 * Prints the main racing track, cars and other ui to the terminal
 * @param main_car pointer to a class Car's object
 */
void AsciiSprite::print_game(const Cars* main_car)
{
    std::stringstream frame_buffer;

    // Drawing user car
   frame_buffer << print_race_car(main_car);

    std::cout << frame_buffer.str() << std::flush;

}


//will improve
void AsciiSprite::print_score(const char c)
{
    std::cout << TerminalControl::move_cursor(0,0) << "You pressed: " << c << std::endl;
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