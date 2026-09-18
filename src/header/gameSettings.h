//
// Created by kevin on 9/13/26.
//

#ifndef ASCIIRACER_GAMESETTINGS_H
#define ASCIIRACER_GAMESETTINGS_H
#include <cstdint>
#include <chrono>

using Clock = std::chrono::high_resolution_clock;

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

struct Track
{
    int TrackStart;
    int LaneSize;
    int GroundSize;
    int TrackEnd;
};

struct GameState
{
    uint64_t gameTick = 0;
    double gameTime = 0;
};

struct GameplaySettings
{
    int steps;
    int fuel_degradation;
    int chassis_degradation;
    int tyre_degradation;
};

#endif //ASCIIRACER_GAMESETTINGS_H
