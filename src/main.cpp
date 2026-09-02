#include <iostream>
#include "header/terminal.h"
#include <unistd.h>
#include <chrono>
#include <bits/this_thread_sleep.h>

// creating Clock alias by using
using Clock = std::chrono::high_resolution_clock;
static auto targetFrameTime = std::chrono::microseconds(16'666); // (1'000'000 / 60)

int main()
{

    while (true)
    {
        auto startTime = Clock::now();






        // checks for loop completion time and sleep if code executed before targeted time
        if (auto frameTime = Clock::now() - startTime; frameTime < targetFrameTime)
        {
            std::this_thread::sleep_for(targetFrameTime - frameTime);
        }

    }

    return 0;
}

