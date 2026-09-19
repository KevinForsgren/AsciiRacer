
#include "header/cars.h"
#include "header/gameSettings.h"

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
 * @param game_Screen
 * @param car_y_position
 */
void Cars::reset_car(const Screen game_Screen, const int car_y_position)
{
    // Adding one if total col is even cause car size is odd and calculating center will give float
    // Adding one will no floor that float number
    int screen_col = game_Screen.Col;
    if (screen_col % 2 == 0)
    {
        screen_col++;
    }

    int spawn_x[3];
    spawn_x[0] = (screen_col - width) / 2;
    spawn_x[1] = spawn_x[0] + 12;
    spawn_x[2] = spawn_x[0] - 12;

    const int index = TC::random_int(0, 2);
    x_position = spawn_x[index];

    y_position = car_y_position;


    fuel = 1000;
    chassis_health = 1000;
    tyre_health = 1000;
    score = 0;

}


EnemyCars::EnemyCars(const std::string& base_color, const std::string& bumper_color, const std::string& tyre_color, const int id) :
    Cars(base_color, bumper_color, tyre_color)
{
    enemy_id = id;
}



bool EnemyCars::collision(const int player_x, const int player_y) const
{
    // Player is either on the Right || Left
    if ((x_position + width) < player_x || ( player_x + width ) < x_position  )
    {
        return false;
    }


    // Player is either on Top || Bottom
    if ((y_position) > ( player_y + height ) || player_y > ( y_position + height ) )
    {
        return false;
    }

    // Else collision happened
    return true;
}

void EnemyCars::update_car_model(const std::string& base_color, const std::string& bumper_color,
    const std::string& tyre_color)
{
    car_model = AsciiArt::change_car_color(base_color, bumper_color, tyre_color);
}



