#include <iostream>
#include "header/terminal.h"
#include <chrono>
#include <bits/this_thread_sleep.h>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <termios.h>
#endif

// creating Clock alias by using
using Clock = std::chrono::high_resolution_clock;
static auto targetFrameTime = std::chrono::microseconds(16'666); // (1'000'000 / 60)

int main()
{
    int a = 0;

    TerminalControl::new_window();
    TerminalControl::switch_raw_mode(true);

    while (true)
    {
        // store current time
        auto startTime = Clock::now();

        TerminalControl::clear_terminal();
        TerminalControl::hide_cursor();

        std::cout << a << std::endl;
        a++;

        char c;

        // reads inputs from the terminal
        if (TerminalControl::read_input(&c))
        {
            std::cout << "You pressed: " << c << std::endl;
        }

        // checks for loop completion time and sleep if code executed before targeted time
        if (auto frameTime = Clock::now() - startTime; frameTime < targetFrameTime)
        {
            std::this_thread::sleep_for(targetFrameTime - frameTime);
        }

        if ( a == 590) break;
    }

    TerminalControl::switch_raw_mode(false);
    TerminalControl::main_window();
    TerminalControl::show_cursor();

    return 0;
}

