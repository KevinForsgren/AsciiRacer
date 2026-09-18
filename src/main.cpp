#include <iostream>
#include <chrono>
#include <bits/this_thread_sleep.h>
#include <fstream>
#include <filesystem>

#include "header/AsciiArt.h"
#include "header/terminal.h"
#include "header/render.h"
#include "header/cars.h"
#include "header/gameSettings.h"

// #if defined(_WIN32)
// #include <windows.h>
// #elif defined(__linux__)
// #include <unistd.h>
// #include <termios.h>
// #endif

static void manage_enemies(EnemyCars* enemy, Screen game_screen);

static bool handle_high_score(int* high_score, bool write_mode = false, const std::string& file_path = "data.dat");

// Handling Clock
static auto targetFrameTime = std::chrono::microseconds(16'666); // (1'000'000 / 60)

using TC = TerminalControl;
static std::string Message;
static int PlayerScore;

// Handling Files
namespace fs = std::filesystem;
constexpr std::string FilePath = "./data.dat";


int main()
{
    TC::new_window();
    TC::switch_raw_mode(true);

    // Initializing Structures;
    ScreenMode current_screen_mode = MainMenu;
    Screen game_screen{};
    GameplaySettings gameplay_settings{};
    GameState game_state{};

    //Getting Screen Properties
    TC::get_terminal_size(&game_screen.Row, &game_screen.Col);

    // Initializing Player Car
    Cars player_car{TC::tc_color(35,125,235), TC::tc_color(225,215,65), TC::tc_color(220,220,225)};
    player_car.reset_car(game_screen, game_screen.Row - player_car.height - 1);

    handle_high_score(&player_car.high_score, false, FilePath);

    // Initialize inGames time and tick
    auto previousTime = Clock::now();

    //Making enemy cars
    EnemyCars* enemies[4];

    const int rand_1 = TC::random_int(0, 5);
    const int rand_2 = TC::random_int(0, 5);
    const int rand_3 = TC::random_int(0, 5);
    const int rand_4 = TC::random_int(0, 5);

    EnemyCars enemy1{car_designs[rand_1].body, car_designs[rand_1].bumper, car_designs[rand_1].tyre};
    EnemyCars enemy2{car_designs[rand_2].body, car_designs[rand_2].bumper, car_designs[rand_2].tyre};
    EnemyCars enemy3{car_designs[rand_3].body, car_designs[rand_3].bumper, car_designs[rand_3].tyre};
    EnemyCars enemy4{car_designs[rand_4].body, car_designs[rand_4].bumper, car_designs[rand_4].tyre};

    enemies[0] = &enemy1;
    enemies[1] = &enemy2;
    enemies[2] = &enemy3;
    enemies[3] = &enemy4;

    while (true)
    {
        // Managing clocks and game fps
        auto startTime = Clock::now();
        const double deltaTime = std::chrono::duration<double>(startTime - previousTime).count();
        game_state.gameTime += deltaTime;
        previousTime = startTime;

        // Uncomment for getting terminal size every iteration
        //// TC::get_terminal_size(&game_screen.Row, &game_screen.Col);

        // Creating a clean terminal
        TC::clear_terminal();
        TC::hide_cursor();

        if (current_screen_mode == MainMenu)
        {
            // Managing Game's main menu
            render::render_main_menu(game_screen.Row, game_screen.Col);

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
            // Manage PauseScreen

            // Reset gameTick and gameTime to 0 for every new gameplay
            game_state.gameTick = 0;
            game_state.gameTime = 0;

            // Resetting all enemies
            for (const auto& enemy: enemies)
            {
                enemy->reset_car(game_screen, 1);
            }

            render::render_pause_menu(game_screen);

            char pause_inpT;
            if (TC::read_input(&pause_inpT))
            {
                if (pause_inpT == 'e' || pause_inpT == 'E')
                {
                    gameplay_settings.steps = 12;
                    gameplay_settings.fuel_degradation = 11;
                    gameplay_settings.tyre_degradation = 7;
                    gameplay_settings.chassis_degradation = 5;
                    current_screen_mode = Gameplay;
                }
                else if (pause_inpT == 'm' || pause_inpT == 'M')
                {
                    gameplay_settings.steps = 2;
                    gameplay_settings.fuel_degradation = 15;
                    gameplay_settings.tyre_degradation = 10;
                    gameplay_settings.chassis_degradation = 10;
                    current_screen_mode = Gameplay;
                }
                else if (pause_inpT == 'h' || pause_inpT == 'H')
                {
                    gameplay_settings.steps = 1;
                    gameplay_settings.fuel_degradation = 20;
                    gameplay_settings.tyre_degradation = 13;
                    gameplay_settings.chassis_degradation = 15;
                    current_screen_mode = Gameplay;
                }
            }

        }
        else if (current_screen_mode == Gameplay)
        {
            // Gaming screen logic here

            char gameplay_inpT;

            if (TC::read_input(&gameplay_inpT))
            {
                switch (gameplay_inpT)
                {
                case 'a':
                    player_car.move_left(gameplay_settings.steps);
                    break;
                case 'd':
                    player_car.move_right(gameplay_settings.steps);
                    break;
                default: break;
                }
            }

            if (game_state.gameTick % 5 == 0)
            {
                for (const auto& enemy: enemies)
                {
                    manage_enemies(enemy, game_screen);
                }
            }


            for (const auto& enemy: enemies)
            {

                if (enemy->isActive)
                {
                    std::cout << render::print_race_car(enemy);
                }

                if (enemy->collision(player_car.x_position, player_car.y_position))
                {
                    Message = "Car crashed with incoming traffic";
                    PlayerScore = player_car.score;
                    player_car.reset_car(game_screen, game_screen.Row - player_car.height - 1);
                    current_screen_mode = ScoreBoard;
                }
            }

            if (render::render_game(&player_car, game_screen, &Message, &game_state, gameplay_settings) > 0)
            {
                PlayerScore = player_car.score;
                player_car.reset_car(game_screen, game_screen.Row - player_car.height - 1);
                current_screen_mode = ScoreBoard;
            }

        }
        else if (current_screen_mode == ScoreBoard)
        {
            // Manage ScoreBoard here
            render::render_score(player_car.high_score, PlayerScore, game_screen, Message);


            char score_inpT;

            if (TC::read_input(&score_inpT))
            {
                if (score_inpT == 'h' || score_inpT == 'H')
                {
                    current_screen_mode = MainMenu;
                    Message.clear();
                }
            }

        }


        // checks for loop completion time and sleep if code executed before targeted time
        if (auto frameTime = Clock::now() - startTime; frameTime < targetFrameTime)
        {
            std::this_thread::sleep_for(targetFrameTime - frameTime);
        }

    }

    handle_high_score(&player_car.high_score, true, FilePath);

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


void manage_enemies(EnemyCars* enemy, const Screen game_screen)
{
    if ( (enemy->y_position + enemy->height ) >= (game_screen.Row - 1)  )
    {
        enemy->reset_car(game_screen, 1);

    } else
    {
        enemy->y_position++;
    }
}