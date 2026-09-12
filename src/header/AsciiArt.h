//
// Created by kevin on 9/4/26.
//

#ifndef ASCIIRACER_ASCIIART_H
#define ASCIIRACER_ASCIIART_H
#include <string>
#include <vector>
#include "terminal.h"

using TC = TerminalControl;

inline const std::vector<std::string> game_title_big = {
    "       d8888                   d8b d8b          8888888b.                                    ",
    "      d88888                   Y8P Y8P          888   Y88b                                   ",
    "     d88P888                                    888    888                                   ",
    "    d88P 888 .d8888b   .d8888b 888 888          888   d88P  8888b.   .d8888b .d88b.  888d888 ",
    R"(   d88P  888 88K      d88P"    888 888          8888888P"      "88b d88P"   d8P  Y8b 888P"   )",
    R"(  d88P   888 "Y8888b. 888      888 888          888 T88b   .d888888 888     88888888 888     )",
    " d8888888888      X88 Y88b.    888 888          888  T88b  888  888 Y88b.   Y8b.     888     ",
    R"(d88P     888  88888P'  "Y8888P 888 888 88888888 888   T88b "Y888888  "Y8888P "Y8888  888     )",
};
constexpr int game_title_big_rows = 93;

inline const std::vector<std::string> game_title_small = {
    "   _            _ _     __                       ",
    R"(  /_\  ___  ___(_|_)   /__\\ __ _  ___ ___ _ __  )",
    R"( //_\\/ __|/ __| | |  / \/// _` |/ __/ _ \ '__|)",
    R"(/  _  \__ \ (__| | | / _  \ (_| | (_|  __/ |  )",
    R"(\_/ \_/___/\___|_|_| \/ \_/\__,_|\___\___|_|  )",
};
constexpr int game_title_small_rows = 49;

inline const std::vector<std::string> game_menu = {
    "╭───────────────╮",
    "│    \033[38;2;0;200;0mR\033[37macing     │",
    "╰───────────────╯",
    "",
    "╭───────────────╮",
    "│  \033[38;2;200;200;0mS\033[37mcoreBoard   │",
    "╰───────────────╯",
    "",
    "╭───────────────╮",
    "│     \033[38;2;200;0;0mQ\033[37mUIT      │",
    "╰───────────────╯",
};
constexpr int game_menu_cols = 17;

inline const std::vector<std::string> game_difficulty = {
    "╭─────────────╮",
    "│    \033[38;2;0;200;0mE\033[37masy     │",
    "╰─────────────╯",
    "",
    "╭─────────────╮",
    "│   \033[38;2;200;200;0mM\033[37medium    │",
    "╰─────────────╯",
    "",
    "╭─────────────╮",
    "│    \033[38;2;200;0;0mH\033[37mard     │",
    "╰─────────────╯",
};
constexpr int game_difficulty_cols = 15;
constexpr int game_difficulty_rows = 11;

inline const std::vector<std::string> score_title = {
    "╔══════════════╗",
    "║  SCOREBOARD  ║",
    "╚══════════════╝"
};
constexpr int score_title_cols = 16;

constexpr std::string block_lower = "▄";
constexpr std::string block_full = "▇";
constexpr std::string non_block = "░";

const std::vector<std::string> stone_dotted = {
    "  ⡾⠛⢦⡀",
    "⣾⢋⣀⣤⡴⣿",
    "⠈⠛⠛⠛⠛⠋"
};

const std::vector<std::string> stone_dash = {
    " .-..+-._ ",
    "( ,'     )",
    " '-___-_+'"
};

const std::vector<std::string> wooden_log = {
    R"(_ __\ \_______,)",
    "[_[_____>⌢<_____#"
};

class AsciiArt
{
public:
    static std::vector<std::string> change_car_color(const std::string& base_color, const std::string& bumper_color, const std::string& tyre_color)
    {
        std::vector<std::string> car_model(7);

        car_model[0] = base_color + "    _    ";
        car_model[1] = " " + tyre_color + "0" + base_color + "=[_]=" + tyre_color +"0" + base_color + " ";
        car_model[2] = R"(   /T\   )";
        car_model[3] = "  |(o)|  ";
        car_model[4] = tyre_color + "[]" + base_color + "=\\_/=" + tyre_color + "[]" + base_color ;
        car_model[5] = bumper_color + "  __" + base_color + "V" + bumper_color + "__  ";
        car_model[6] = bumper_color + " '-----'  ";

        return  car_model;
    }

    static std::vector<std::string> race_light_dynamic(int seconds)
    {
        std::vector<std::string> race_light(5);

        std::string color = TC::tc_color(255, 0, 0); // red

        if (seconds < 2)
        {
            color = TC::tc_color(0, 255, 0); // green
        }

        race_light[0] = "══════════════╦═══════╦═══════╦══";
        race_light[1] = "              ║       ║       ║  ";
        if (seconds <= 1)
        {
            race_light[2] = color + "            [|||]   [|||]   [|||]";
        }
        else
        {
            race_light[2] = "            [   ]   [   ]   [   ]";
        }

        if (seconds <= 2)
        {
            race_light[3] = color + "            [|||]   [|||]   [|||]";
        }
        else
        {
            race_light[3] = "            [   ]   [   ]   [   ]";
        }

        if (seconds <= 3)
        {
            race_light[4] = color + "            [|||]   [|||]   [|||]" + TC::tc_color(255, 255, 255);
        }
        else
        {
            race_light[4] = "            [   ]   [   ]   [   ]";
        }

        return race_light;
    }
};

#endif //ASCIIRACER_ASCIIART_H
