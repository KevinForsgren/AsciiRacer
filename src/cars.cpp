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


/**
 * Reset the car to its initial state
 * @param Row game screen total rows
 * @param Col game screen total columns
 */
void Cars::reset_car(const int Row, const int Col)
{
    x_position = (Col - width) / 2;
    y_position = Row - height - 1;

    fuel = 1000;
    chassis_health = 1000;
    tyre_health = 1000;

}



