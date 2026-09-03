
#include "header/AsciiSprite.h"

#include <iostream>
#include <sstream>

#include "header/terminal.h"

static std::vector<std::string> Game_Title = {
    "       d8888                   d8b d8b          8888888b.                                    ",
    "      d88888                   Y8P Y8P          888   Y88b                                   ",
    "     d88P888                                    888    888                                   ",
    "    d88P 888 .d8888b   .d8888b 888 888          888   d88P  8888b.   .d8888b .d88b.  888d888 ",
    R"(   d88P  888 88K      d88P"    888 888          8888888P"      "88b d88P"   d8P  Y8b 888P"   )",
    "  d88P   888 \"Y8888b. 888      888 888          888 T88b   .d888888 888     88888888 888     ",
    " d8888888888      X88 Y88b.    888 888          888  T88b  888  888 Y88b.   Y8b.     888     ",
    R"(d88P     888  88888P'  "Y8888P 888 888 88888888 888   T88b "Y888888  "Y8888P "Y8888  888     )",
};

static std::vector<std::string> Game_Board = {
    ".----------------------------------------------------------------------------------------------------.",
    "| .------------------------------------------------------------------------------------------------. |",
    "| |                                                                                                | |",
    "| |                                                                                                | |",
    "| |                                                                                                | |",
    "| |                                          Start \033[38;2;0;200;0mR\033[38;2;255;255;255macing                                          | |",
    "| |                                                                                                | |",
    "| |                                           High \033[38;2;204;204;0mS\033[38;2;255;255;255mcore                                           | |",
    "| |                                                                                                | |",
    "| |                                              \033[38;2;200;0;0mQ\033[38;2;255;255;255muit                                              | |",
    "| |                                                                                                | |",
    "| |                                                                                                | |",
    "| |                                                                                                | |",
    "| |________________________________________________________________________________________________| |",
    "|____________________________________________________________________________________________________|",
};

/**
 * Prints the game's menu screen
 * @param row
 * @param col
 */
void AsciiSprite::print_title(const int row, const int col)
{
    std::stringstream frame_buffer;
    int i = 0;
    for (const auto& str : Game_Title)
    {
        frame_buffer << TerminalControl::move_cursor(i + 2, (col - 93)/ 2) << str << std::endl;
        i++;
    }

    // printing border
    i += 5;
    for (const auto& str : Game_Board)
    {
        frame_buffer << TerminalControl::move_cursor(i, (col - 102)/ 2) << str << std::endl;
        i++;
    }

    i += 2;

    frame_buffer << TerminalControl::move_cursor(i, (col - 21)/2) << "Press appropriate key"<< std::endl;

    std::cout << frame_buffer.str() << std::flush;
}

void AsciiSprite::print_game()
{
    std::cout << TerminalControl::move_cursor(0,0) << "Racing logic here" << std::endl;
}

void AsciiSprite::print_score()
{

    std::cout << TerminalControl::move_cursor(0,0) << "Score Board logic here" << std::endl;
}