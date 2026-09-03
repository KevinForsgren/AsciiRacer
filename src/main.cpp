#include <iostream>
#include "header/terminal.h"
#include <chrono>
#include <bits/this_thread_sleep.h>

#include "header/AsciiSprite.h"

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
        Menu,
        Racing,
        Score,
    };
}

int main()
{
    Modes current_mode = Menu;

    int row;
    int col;

    TerminalControl::new_window();
    TerminalControl::switch_raw_mode(true);

    while (true)
    {
        // store current time
        auto startTime = Clock::now();

        TerminalControl::clear_terminal();
        TerminalControl::hide_cursor();

        TerminalControl::get_terminal_size(&row, &col);

        if (current_mode == Menu)
        {
            AsciiSprite::print_title(row, col);
        }
        else if (current_mode == Racing)
        {
            AsciiSprite::print_game();
        }
        else if (current_mode == Score)
        {
            AsciiSprite::print_score();
        }

        char c;
        // change game mode here
        if (TerminalControl::read_input(&c))
        {
            if (c == 'r' || c == 'R')
            {
                current_mode = Racing;
            }
            else if (c == 's' || c == 'S')
            {
                current_mode = Score;
            }
            else if (c == 'h' || c == 'H')
            {
                current_mode = Menu;
            }

            if (c == 'q' || c == 'Q') break;
        }

        // checks for loop completion time and sleep if code executed before targeted time
        if (auto frameTime = Clock::now() - startTime; frameTime < targetFrameTime)
        {
            std::this_thread::sleep_for(targetFrameTime - frameTime);
        }


    }

    TerminalControl::switch_raw_mode(false);
    TerminalControl::main_window();
    TerminalControl::show_cursor();

    return 0;
}

