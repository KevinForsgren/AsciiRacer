//
// Created by kevin on 9/6/26.
//

#include "header/cars.h"

#include <random>
#include <sstream>

#include "header/AsciiArt.h"
#include "header/terminal.h"

using TC = TerminalControl;

Cars::Cars()
{
    tyre_health = 1000;
    fuel = 1000;
    chassis_health = 1000;
}

void Cars::move_left(const int steps)
{
    x_position -= steps;
}

void Cars::move_right(const int steps)
{
    x_position += steps;
}

// std::vector<std::string> Cars::new_car()
// {
//     std::vector<std::string> new_color;
//
//     int a = 0;
//     for (const auto& str: car_model)
//     {
//        new_color << TC::tc_color(255, 200, 0) << str;
//     }
//
//     return new_color;
// }


