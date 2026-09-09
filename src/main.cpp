#include <iostream>
#include "header/terminal.h"
#include <chrono>
#include <bits/this_thread_sleep.h>

#include "header/AsciiSprite.h"
#include "header/cars.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <termios.h>
#endif

// creating Clock alias by using
using Clock = std::chrono::high_resolution_clock;
static auto targetFrameTime = std::chrono::microseconds(16'666); // (1'000'000 / 60)

namespace
{
    //Game mode options
    enum Modes
    {
        Home,
        Racing,
        Score,
    };

    struct Screen
    {
        int Row;
        int Col;
    };
}


int main()
{
    Modes current_mode = Home;
    TerminalControl::new_window();
    TerminalControl::switch_raw_mode(true);

    //Getting screen properties
    Screen game_screen{};
    TerminalControl::get_terminal_size(&game_screen.Row, &game_screen.Col);

    // Initializing cars
    Cars hero_car{};
    hero_car.reset_car(game_screen.Row, game_screen.Col);

    while (true)
    {
        // store current time
        auto startTime = Clock::now();

        TerminalControl::get_terminal_size(&game_screen.Row, &game_screen.Col);

        // creating clean terminal
        TerminalControl::clear_terminal();
        TerminalControl::hide_cursor();

        if (current_mode == Home)
        {
            // Home screen logic here
            AsciiSprite::print_title(game_screen.Row, game_screen.Col);

            char home_inpT;
            if (TerminalControl::read_input(&home_inpT))
            {
                if (home_inpT == 'r' || home_inpT == 'R')
                {
                    current_mode = Racing;
                }
                else if (home_inpT == 's' || home_inpT == 'S')
                {
                    current_mode = Score;
                }

                if (home_inpT == 'q' || home_inpT == 'Q') break;
            }
        }
        else if (current_mode == Racing)
        {
            // Gaming screen logic here

            char racing_inpT;

            if (TerminalControl::read_input(&racing_inpT))
            {
                switch (racing_inpT)
                {
                case 'a':
                    hero_car.move_left(2);
                    break;
                case 'd':
                    hero_car.move_right(2);
                    break;
                default: break;
                }

                if (racing_inpT == 'h' || racing_inpT == 'H')
                {
                    hero_car.reset_car(game_screen.Row, game_screen.Col);
                    current_mode = Home;
                }
            }

            AsciiSprite::print_game(&hero_car);
        }
        else if (current_mode == Score)
        {

            char score_inpT;
            // change game mode here
            if (TerminalControl::read_input(&score_inpT))
            {
                if (score_inpT == 'h' || score_inpT == 'H')
                {
                    current_mode = Home;
                }
            }

            AsciiSprite::print_score(score_inpT);
        }


        // checks for loop completion time and sleep if code executed before targeted time
        if (auto frameTime = Clock::now() - startTime; frameTime < targetFrameTime)
        {
            std::this_thread::sleep_for(targetFrameTime - frameTime);
        }


    }

    // turning terminal back to normal
    TerminalControl::switch_raw_mode(false);
    TerminalControl::main_window();
    TerminalControl::show_cursor();

    return 0;
}

