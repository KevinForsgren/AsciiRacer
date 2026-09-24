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

    /** Return whether the collector overlaps the player's rectangular bounds. */
    [[nodiscard]] bool collision(const Cars& player) const;
    /** Place the collector at the top of the requested lane. */
    void reset_collector(Screen game_screen, int lane_index);
    /** Move the collector down one row or deactivate it at the screen edge. */
    void manage_collector(Screen game_screen);
    /** Render the collector at its current position. */
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
    /** Build a scrolling ground belt from four repeating environment objects. */
    GroundSystem(Screen game_screen, EnvironmentObject environment_objects[]);
    /** Move all ground blocks down and recycle blocks beyond the screen. */
    void update(int scroll_speed);
    /** Render visible ground objects on both sides of the track. */
    [[nodiscard]] std::string render(const Track& track) const;


};


#endif //ASCIIRACER_ENVIRONMENT_H
