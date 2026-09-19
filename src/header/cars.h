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
    std::string body;
    std::string bumper;
    std::string tyre;
};

class Cars
{
public:
    int velocity{};
    int x_position{};
    int y_position{};
    mutable int high_score{};
    std::vector<std::string> car_model;
    int score;
    int tyre_health;
    int fuel;
    int chassis_health;
    const int height = 7;
    const int width = 9;

    Cars(const std::string& base_color, const std::string& bumper_color, const std::string& tyre_color);
    void move_left(int steps);
    void move_right(int steps);
    void reset_car(Screen game_screen, int car_y_position);

};

class EnemyCars: public Cars
{
public:
    bool isActive = false;
    int enemy_id;
    EnemyCars(const std::string& base_color, const std::string& bumper_color, const std::string& tyre_color, int id);
    bool collision(int player_x, int player_y) const;
    void update_car_model(const std::string& base_color, const std::string& bumper_color, const std::string& tyre_color);
};


#endif //ASCIIRACER_CARS_H
