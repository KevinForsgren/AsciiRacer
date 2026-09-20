#ifndef ASCIIRACER_ENVIRONMENT_H
#define ASCIIRACER_ENVIRONMENT_H
#include <string>

#include "cars.h"
#include "gameSettings.h"


class Collector
{

public:
    int value = 150;
    int height = 3;
    int width = 3;
    int x_position{};
    int y_position{};
    bool isActive = false;
    std::vector<std::string> collector_model;

    [[nodiscard]] bool collision(const Cars& player) const;
    void reset_collector(Screen game_screen, int lane_index);
    void manage_collector(Screen game_screen);
    [[nodiscard]] std::string spawn_collector() const;

};

class Grass
{

};

class Stone
{

};


#endif //ASCIIRACER_ENVIRONMENT_H
