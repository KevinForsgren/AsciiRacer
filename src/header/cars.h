//
// Created by kevin on 9/6/26.
//

#ifndef ASCIIRACER_CARS_H
#define ASCIIRACER_CARS_H
#include <asm-generic/int-ll64.h>


class Cars
{
public:
    int velocity{};
    int x_position{};
    int y_position{};
    unsigned tyre_health;
    unsigned fuel;
    unsigned chassis_health;
    const int height = 7;
    const int width = 10;

    Cars();
    void move_left(int steps);
    void move_right(int steps);
    void reset_car(int Row, int Col);

};


#endif //ASCIIRACER_CARS_H
