
#include "header/AsciiSprite.h"

#include <iostream>
#include <random>
#include <sstream>
#include <string>

#include "header/terminal.h"
#include "header/AsciiArt.h"
#include "header/cars.h"

using TC = TerminalControl;

static std::string print_container(const int* fuel);

/**
 * Prints the game's menu screen
 * @param row
 * @param col
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


void AsciiSprite::print_game(const Cars* main_car)
{
    std::stringstream frame_buffer;

    frame_buffer << TC::move_cursor(1, 1) << "Racing logic here" << std::endl;

    int a = 0;
    for (const auto& str : car_model)
    {
        frame_buffer << TC::move_cursor(main_car->y_position + a, main_car->x_position);

        frame_buffer << TC::tc_color(255, 0, 0) << str << TC::tc_color(255, 255, 255);
        frame_buffer << std::endl;
        a++;
    }

    std::cout << frame_buffer.str() << std::flush;

}


//will improve
void AsciiSprite::print_score(const char c)
{
    int fuel = 10;
    std::random_device device;

    std::mt19937 gen(device());
    std::uniform_int_distribution<> distribution(1, 10);

    if (distribution(gen) == 1)
    {
        fuel--;
    }


    std::cout << print_container(&fuel);
    std::cout << TerminalControl::move_cursor(0,0) << "You pressed: " << c << std::endl;
}

static std::string print_container(const int* fuel)
{
    std::stringstream frame_buffer;

    frame_buffer << "[";
    constexpr int capacity = 10;

    //fill logic here
    for (int i = 0; i < capacity; i++)
    {
        if (i < *fuel)
        {
            frame_buffer << block_full;
            // fuel--;
            continue;
        }

        frame_buffer << non_block;
    }

    frame_buffer << "]";

    return frame_buffer.str();
}