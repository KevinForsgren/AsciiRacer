//
// Created by kevin on 9/18/26.
//

#ifndef ASCIIRACER_ENVIRONMENT_H
#define ASCIIRACER_ENVIRONMENT_H
#include <string>
#include "cars.h"
#include "gameSettings.h"


class Collector
{

public:
    int Value = 150;
    int Height = 3;
    int Width = 3;
    int xPosition{};
    int yPosition{};
    bool isActive = false;
    std::vector<std::string> Model;

    [[nodiscard]] bool collision(const Cars& player) const;
    void reset_collector(Screen game_screen, int lane_index);
    void manage_collector(Screen game_screen);
    [[nodiscard]] std::string spawn_collector() const;

};


struct EnvironmentObject
{
    std::vector<std::string> Model;
    int Height;
    int Width;
};


// A single segment of the conveyor belt
struct GroundBlock {
    int x;
    int y;
    const std::vector<std::string>* art;
    int Height;
    int Width;
};

class GroundSystem
{
private:
    std::vector<GroundBlock> belt;

    int screen_height;
    int total_belt_height;

public:
    GroundSystem(Screen game_screen, EnvironmentObject environment_objects[]);
    void update(int scroll_speed);
    std::string render(const Track& track) const;


};


#endif //ASCIIRACER_ENVIRONMENT_H
