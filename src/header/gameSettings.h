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
    uint64_t GameTick = 0;
    double GameTime = 0;
    int GameSpeed = 5;
};

struct TrafficSetting
{
    int CurrentTrafficCar{};
    int CurrentTrafficDistributionRow{};
    int Seed{};
};

struct GameplaySettings
{
    int Steps;
    int FuelDegradation;
    int ChassisDegradation;
    int TyreDegradation;
};

#endif //ASCIIRACER_GAMESETTINGS_H
