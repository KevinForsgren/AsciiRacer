
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
    this->TyreHealth = 1000;
    this->Fuel = 1000;
    this->ChassisHealth = 1000;
    this->Score = 0;

    this->Model = AsciiArt::change_car_color(base_color, bumper_color, tyre_color);
}


/**
 * Move car to the left
 * @param steps number of steps to move in a direction
 */
void Cars::move_left(const int steps)
{
    this->xPosition -= steps;
}


/**
 * Move car to the right
 * @param steps number of steps to move in a direction
 */
void Cars::move_right(const int steps)
{
    this->xPosition += steps;
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
    spawn_x[0] = (screen_col - this->Width) / 2;
    spawn_x[1] = spawn_x[0] + 12;
    spawn_x[2] = spawn_x[0] - 12;

    const int index = TC::random_int(0, 2);
    this->xPosition = spawn_x[index];
    this->yPosition = car_y_position;

    this->Fuel = 1000;
    this->ChassisHealth = 1000;
    this->TyreHealth = 1000;
    this->Score = 0;

}

/**
 * Reset position and all gameplay resources in a specific lane.
 * @param game_Screen terminal dimensions
 * @param car_y_position vertical spawn position
 * @param lane_index lane index in the range 0 through 2
 */
void Cars::reset_car(const Screen game_Screen, const int car_y_position, const int lane_index)
{

    int screen_col = game_Screen.Col;
    if (screen_col % 2 == 0)
    {
        screen_col++;
    }

    int spawn_x[3];
    spawn_x[1] = (screen_col - this->Width) / 2;
    spawn_x[0] = spawn_x[1] - 12;
    spawn_x[2] = spawn_x[1] + 12;

    this->xPosition = spawn_x[lane_index];
    this->yPosition = car_y_position;

    this->Fuel = 1000;
    this->ChassisHealth = 1000;
    this->TyreHealth = 1000;
    this->Score = 0;

}


/**
 * Construct an enemy car and assign its traffic-management identifier.
 * @param base_color body color sequence
 * @param bumper_color bumper color sequence
 * @param tyre_color tyre color sequence
 * @param id enemy identifier
 */
EnemyCars::EnemyCars(const std::string& base_color, const std::string& bumper_color, const std::string& tyre_color, const int id) :
    Cars(base_color, bumper_color, tyre_color)
{
    this->EnemyId = id;
}


/**
 * Check axis-aligned overlap between this enemy and the player.
 * @param player_x player's left coordinate
 * @param player_y player's top coordinate
 * @return true when the car bounds overlap; otherwise false
 */
bool EnemyCars::collision(const int player_x, const int player_y) const
{
    // Player is either on the Right || Left
    if ((this->xPosition + this->Width) < player_x || ( player_x + this->Width ) < this->xPosition  )
    {
        return false;
    }


    // Player is either on Top || Bottom
    if ((this->yPosition) > ( player_y + this->Height ) || player_y > ( this->yPosition + this->Height ) )
    {
        return false;
    }

    // Else collision happened
    return true;
}


/**
 * Rebuild the enemy sprite with the supplied colors.
 * @param base_color body color sequence
 * @param bumper_color bumper color sequence
 * @param tyre_color tyre color sequence
 */
void EnemyCars::update_car_model(const std::string& base_color, const std::string& bumper_color,
                                 const std::string& tyre_color)
{
    this->Model = AsciiArt::change_car_color(base_color, bumper_color, tyre_color);
}


