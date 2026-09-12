#include <iostream>
#include <chrono>
#include <bits/this_thread_sleep.h>
#include <fstream>
#include <filesystem>

#include "header/terminal.h"
#include "header/AsciiSprite.h"
#include "header/cars.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <termios.h>
#endif

static bool handle_high_score(int* high_score, bool write_mode = false, const std::string& file_path = "data.dat");

// creating aliases
using Clock = std::chrono::high_resolution_clock;
using TC = TerminalControl;
namespace fs = std::filesystem;

static std::string Message;
constexpr std::string FilePath = "./data.dat";

static auto targetFrameTime = std::chrono::microseconds(16'666); // (1'000'000 / 60)

namespace
{
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

    struct GameplaySettings
    {
        int steps;
        int fuel_degradation;
    };
}


int main()
{
    TC::new_window();
    TC::switch_raw_mode(true);

    // Initializing structures;
    ScreenMode current_screen_mode = MainMenu;
    Screen game_screen{};
    GameplaySettings gameplay_settings{};

    //Getting screen properties
    TC::get_terminal_size(&game_screen.Row, &game_screen.Col);

    // Initializing hero car
    Cars hero_car{TC::tc_color(35,125,235), TC::tc_color(225,215,65), TC::tc_color(220,220,225)};
    hero_car.reset_car(game_screen.Row, game_screen.Col);

    handle_high_score(&hero_car.high_score, false, FilePath);

    while (true)
    {
        // store current time
        auto startTime = Clock::now();

        TC::get_terminal_size(&game_screen.Row, &game_screen.Col);

        // creating clean terminal
        TC::clear_terminal();
        TC::hide_cursor();

        if (current_screen_mode == MainMenu)
        {
            // Home screen logic here
            AsciiSprite::print_main_menu(game_screen.Row, game_screen.Col);

            char home_inpT;
            if (TC::read_input(&home_inpT))
            {
                if (home_inpT == 'r' || home_inpT == 'R')
                {
                    current_screen_mode = Pause;
                }
                else if (home_inpT == 's' || home_inpT == 'S')
                {
                    current_screen_mode = ScoreBoard;
                }

                if (home_inpT == 'q' || home_inpT == 'Q') break;
            }
        }
        else if (current_screen_mode == Pause)
        {
            // prints pause screen

            char home_inpT;
            if (TC::read_input(&home_inpT))
            {
                if (home_inpT == 'e' || home_inpT == 'E')
                {
                    gameplay_settings.steps = 12;
                    gameplay_settings.fuel_degradation = 1;
                    current_screen_mode = Gameplay;
                }
                else if (home_inpT == 'm' || home_inpT == 'M')
                {
                    gameplay_settings.steps = 2;
                    gameplay_settings.fuel_degradation = 25;
                    current_screen_mode = Gameplay;
                }
                else if (home_inpT == 'h' || home_inpT == 'H')
                {
                    gameplay_settings.steps = 1;
                    gameplay_settings.fuel_degradation = 50;
                    current_screen_mode = Gameplay;
                }
            }

            AsciiSprite::print_pause_menu(game_screen.Row, game_screen.Col);
        }
        else if (current_screen_mode == Gameplay)
        {
            // Gaming screen logic here
            char racing_inpT;

            if (TC::read_input(&racing_inpT))
            {
                switch (racing_inpT)
                {
                case 'a':
                    hero_car.move_left(gameplay_settings.steps);
                    break;
                case 'd':
                    hero_car.move_right(gameplay_settings.steps);
                    break;
                default: break;
                }
            }

            if (AsciiSprite::print_game(&hero_car, game_screen.Row, game_screen.Col, &Message) > 0)
            {
                hero_car.reset_car(game_screen.Row, game_screen.Col);
                current_screen_mode = ScoreBoard;
            }

        }
        else if (current_screen_mode == ScoreBoard)
        {

            char score_inpT;

            // change game mode here
            if (TC::read_input(&score_inpT))
            {
                if (score_inpT == 'h' || score_inpT == 'H')
                {
                    current_screen_mode = MainMenu;
                    Message.clear();
                }
            }

            AsciiSprite::print_score(hero_car.high_score, hero_car.score, game_screen.Row, game_screen.Col, Message);
        }


        // checks for loop completion time and sleep if code executed before targeted time
        if (auto frameTime = Clock::now() - startTime; frameTime < targetFrameTime)
        {
            std::this_thread::sleep_for(targetFrameTime - frameTime);
        }


    }

    handle_high_score(&hero_car.high_score, true, FilePath);

    // turning terminal back to normal
    TC::switch_raw_mode(false);
    TC::main_window();
    TC::show_cursor();

    return 0;
}

static bool handle_high_score(int* high_score, bool write_mode, const std::string& file_path)
{
    // Create a consistent file path for all systems
    fs::path targetPath(file_path);

    // 2. Ensure parent directories exist if a custom path is supplied (e.g., "saves/data.dat")
    if (targetPath.has_parent_path() && !fs::exists(targetPath.parent_path())) {
        std::error_code ec;
        fs::create_directories(targetPath.parent_path(), ec);
        if (ec) {
            std::cerr << "Error creating directories: " << ec.message() << '\n';
            return false;
        }
    }

    // Creating new file with high Score 0 if file not exist
    if (!fs::exists(targetPath)) {
        std::ofstream createOut(targetPath, std::ios::binary);
        if (!createOut.is_open()) {
            std::cerr << "Error: Unable to create file at " << targetPath << '\n';
            return false;
        }

        int score = 0;
        createOut.write(reinterpret_cast<const char*>(&score), sizeof(int));
        createOut.close();
        return createOut.good();
    }

    // 3. WRITE MODE
    if (write_mode) {
        std::ofstream outFile(targetPath, std::ios::binary);
        if (!outFile.is_open()) {
            std::cerr << "Error: Could not open " << targetPath << " for writing.\n";
            return false;
        }
        outFile.write(reinterpret_cast<const char*>(high_score), sizeof(int));
        outFile.close();
        return outFile.good();
    }

    // 5. READ MODE
    std::ifstream inFile(targetPath, std::ios::binary);

    if (inFile.is_open()) {
        inFile.read(reinterpret_cast<char*>(high_score), sizeof(int));
    }
    else
    {
        std::cerr << "Warning: File was corrupted or empty. Resetting high Score to 0.\n";
        *high_score = 0;
    }

    inFile.close();
    return inFile.good();

}