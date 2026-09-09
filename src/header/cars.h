//
// Created by kevin on 9/6/26.
//

#ifndef ASCIIRACER_CARS_H
#define ASCIIRACER_CARS_H
#include <string>
#include <vector>

struct Car_Designs
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
    std::vector<std::string> car_model;
    unsigned tyre_health;
    unsigned fuel;
    unsigned chassis_health;
    const int height = 7;
    const int width = 9;

    Cars(const std::string& base_color, const std::string& bumper_color, const std::string& tyre_color);
    void move_left(int steps);
    void move_right(int steps);
    void reset_car(int Row, int Col);

};


#endif //ASCIIRACER_CARS_H
