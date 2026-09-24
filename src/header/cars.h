//
// Created by kevin on 9/6/26.
//

#ifndef ASCIIRACER_CARS_H
#define ASCIIRACER_CARS_H
#include <string>
#include <vector>

#include "gameSettings.h"

struct Car_Design
{
    std::string Body;
    std::string Bumper;
    std::string Tyre;
};

struct Traffic_Distribution
{
    int FirstRow[3];
    int SecondRow[3];
    int ThirdRow[3];
};

inline std::vector<Traffic_Distribution> traffic_distributions = {
    {.FirstRow = {1, 1, 0}, .SecondRow = {0, 0, 1}, .ThirdRow = {1, 0, 0}},
    {.FirstRow = {0, 1, 1}, .SecondRow = {1, 0, 0}, .ThirdRow = {0, 0, 1}},
    {.FirstRow = {1, 0, 1}, .SecondRow = {0, 1, 0}, .ThirdRow = {1, 0, 0}},
    {.FirstRow = {1, 0, 0}, .SecondRow = {0, 1, 1}, .ThirdRow = {0, 0, 1}},
    {.FirstRow = {0, 0, 1}, .SecondRow = {1, 1, 0}, .ThirdRow = {1, 0, 0}},
};

class Cars
{
public:
    int Velocity{};
    int xPosition{};
    int yPosition{};
    int HighScore;
    std::vector<std::string> Model;
    int Score;
    int TyreHealth;
    int Fuel;
    int ChassisHealth;
    const int Height = 7;
    const int Width = 9;

    /** Create a car model using the supplied ANSI color sequences. */
    Cars(const std::string& base_color, const std::string& bumper_color, const std::string& tyre_color);
    /** Move the car horizontally left by the requested number of columns. */
    void move_left(int steps);
    /** Move the car horizontally right by the requested number of columns. */
    void move_right(int steps);
    /** Reset position and health, placing the car in a random lane. */
    void reset_car(Screen game_screen, int car_y_position);
    /** Reset position and health, placing the car in the requested lane. */
    void reset_car(Screen game_Screen, int car_y_position, int lane_index);
};

class EnemyCars: public Cars
{
public:
    bool isActive = false;
    int EnemyId;
    /** Create an enemy car with an identifier used by traffic management. */
    EnemyCars(const std::string& base_color, const std::string& bumper_color, const std::string& tyre_color, int id);
    /** Return whether this enemy overlaps the player's rectangular bounds. */
    bool collision(int player_x, int player_y) const;
    /** Replace the enemy's colored ASCII model. */
    void update_car_model(const std::string& base_color, const std::string& bumper_color, const std::string& tyre_color);
};


#endif //ASCIIRACER_CARS_H
