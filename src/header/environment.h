//
// Created by kevin on 9/18/26.
//

#ifndef ASCIIRACER_ENVIRONMENT_H
#define ASCIIRACER_ENVIRONMENT_H
#include <string>

#include "AsciiArt.h"
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

struct Environment_object
{
    std::vector<std::string> Model;
    int Height;
    int Width;
};

class Ground
{

public:
    int grass_width = 15;
    std::vector<Environment_object> environment_objects = {
        {grass_patch, 7, 12},
        {grass_patch_alternative, 3, 13},
        {stone_dotted, 3, 6},
        {stone_dash, 3, 10}
    };
    // std::string model;

    [[nodiscard]] std::string manage_grass(Track track, Screen game_screen) const;
};



#endif //ASCIIRACER_ENVIRONMENT_H
