//
// Created by kevin on 9/13/26.
//

#ifndef ASCIIRACER_GAMESETTINGS_H
#define ASCIIRACER_GAMESETTINGS_H
#include <cstdint>

enum ScreenMode
{
    MainMenu,
    Gameplay,
    ScoreBoard,
    Pause,
};

struct Screen
{
    int Row;
    int Col;
};

struct GameState
{
    uint64_t gameTick;
    double gameTime;
};

struct GameplaySettings
{
    int steps;
    int fuel_degradation;
    int chassis_degradation;
    int tyre_degradation;
};

#endif //ASCIIRACER_GAMESETTINGS_H
