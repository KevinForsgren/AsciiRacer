#include "header/cars.h"

#include <random>

#include "header/AsciiArt.h"
#include "header/terminal.h"

using TC = TerminalControl;

/**
 *
 * @param base_color primary color for the race_car
 * @param bumper_color color for car bumper
 * @param tyre_color cars tyre color
 */
Cars::Cars(const std::string& base_color, const std::string& bumper_color, const std::string& tyre_color)
{
    tyre_health = 1000;
    fuel = 1000;
    chassis_health = 1000;
    score = 0;

    car_model = AsciiArt::change_car_color(base_color, bumper_color, tyre_color);
}


/**
 * Move car to the left
 * @param steps number of steps to move in a direction
 */
void Cars::move_left(const int steps)
{
    x_position -= steps;
}


/**
 * Move car to the right
 * @param steps number of steps to move in a direction
 */
void Cars::move_right(const int steps)
{
    x_position += steps;
}


/**
 * Reset the car to its initial state
 * @param Row game screen total rows
 * @param Col game screen total columns
 */
void Cars::reset_car(const int Row, int Col)
{
    // Adding one if total col is even cause car size is odd and calculating center will give float
    // Adding one will no floor that float number
    if (Col % 2 == 0)
    {
        Col++;
    }

    int spawn_x[3];
    spawn_x[0] = (Col - width) / 2;
    spawn_x[1] = spawn_x[0] + 12;
    spawn_x[2] = spawn_x[0] - 12;

    std::random_device device;

    std::mt19937 gen(device());
    std::uniform_int_distribution<> distribution(0, 2);

    const int index = distribution(gen);
    x_position = spawn_x[index];
    y_position = Row - height - 1;

    fuel = 1000;
    chassis_health = 1000;
    tyre_health = 1000;

}



