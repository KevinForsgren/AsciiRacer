#include <iostream>
#include <chrono>
#include <bits/this_thread_sleep.h>
#include <fstream>
#include <filesystem>

#include "header/AsciiArt.h"
#include "header/terminal.h"
#include "header/render.h"
#include "header/cars.h"
#include "header/environment.h"
#include "header/gameSettings.h"

// #if defined(_WIN32)
// #include <windows.h>
// #elif defined(__linux__)
// #include <unistd.h>
// #include <termios.h>
// #endif

// TODO: place ground area materials

static void spawn_traffic(EnemyCars* Enemies[], int enemies_size, Screen game_screen, int* current_car, const int traffic_distribution_row[]);
static void manage_traffic(EnemyCars* Enemies[], Screen game_screen, int seed, int* current_car, int* current_distribution_row, int enemies_size = 5);

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
    Track race_track{
        .LaneSize = 12,
        .GroundSize = 17
    };
    GameState game_state{};
    TrafficSetting traffic_setting{};

    //Getting Screen Properties
    TC::get_terminal_size(&game_screen.Row, &game_screen.Col);

    // Initializing Player Car
    Cars player_car{TC::tc_color(35,125,235), TC::tc_color(225,215,65), TC::tc_color(220,220,225)};
    player_car.reset_car(game_screen, game_screen.Row - player_car.height - 1);

    handle_high_score(&player_car.high_score, false, FilePath);


    // Collectors
    Collector fuel{};
    Collector tyre{};
    fuel.collector_model = fuel_collector;
    tyre.collector_model = tyre_collector;

    //Making enemy cars
    EnemyCars* enemies[5];

    const int rand_1 = TC::random_int(0, 5);
    const int rand_2 = TC::random_int(0, 5);

    EnemyCars enemy1{car_designs[rand_1].body, car_designs[rand_1].bumper, car_designs[rand_1].tyre, 100};
    EnemyCars enemy2{car_designs[rand_2].body, car_designs[rand_2].bumper, car_designs[rand_2].tyre, 200};
    EnemyCars enemy3{car_designs[rand_1].body, car_designs[rand_1].bumper, car_designs[rand_1].tyre, 300};
    EnemyCars enemy4{car_designs[rand_2].body, car_designs[rand_2].bumper, car_designs[rand_2].tyre, 400};
    EnemyCars enemy5{car_designs[rand_1].body, car_designs[rand_1].bumper, car_designs[rand_1].tyre, 500};

    enemies[0] = &enemy1;
    enemies[1] = &enemy2;
    enemies[2] = &enemy3;
    enemies[3] = &enemy4;
    enemies[4] = &enemy5;

    // Initialize inGames time and tick
    auto previousTime = Clock::now();

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

            // Reset game state and traffic state
            game_state.gameTick = 0;
            game_state.gameTime = 0;
            game_state.gameSpeed = 5;
            traffic_setting.current_traffic_car = 1;
            traffic_setting.current_traffic_distribution_row = 0;
            traffic_setting.seed = TC::random_int(0, 4);

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

            std::stringstream frameBuffer;

            //Grass
            Ground grass;
            frameBuffer << grass.manage_grass(race_track, game_screen);

            frameBuffer << render::render_game(&player_car, game_screen, &race_track, &game_state, gameplay_settings);

            if (game_state.gameTick % 5 == 0)
            {
                if (fuel.isActive) fuel.manage_collector(game_screen);
                if (tyre.isActive) tyre.manage_collector(game_screen);
            }


            // Printing collector on screen
            if (fuel.isActive) frameBuffer << fuel.spawn_collector();
            if (tyre.isActive) frameBuffer << tyre.spawn_collector();

            // Spawning collector
            if ( game_state.gameTick % 300 == 0  && player_car.fuel < 450)
            {
                const auto [FirstRow, SecondRow, ThirdRow] = traffic_distributions[traffic_setting.seed];
                const int* row[] = {
                    ThirdRow,
                    SecondRow,
                    FirstRow
                };

                for (int i = 0; i < 3; i++)
                {
                    if (row[traffic_setting.current_traffic_distribution_row][i] == 0)
                    {
                        fuel.isActive = true;
                        fuel.reset_collector(game_screen, i);
                    }
                }
            }

            if ( game_state.gameTick % 300 == 0  && player_car.tyre_health < 450)
            {
                const auto [FirstRow, SecondRow, ThirdRow] = traffic_distributions[traffic_setting.seed];
                const int* row[] = {
                    ThirdRow,
                    SecondRow,
                    FirstRow
                };

                for (int i = 0; i < 3; i++)
                {
                    if (row[traffic_setting.current_traffic_distribution_row][i] == 0)
                    {
                        tyre.isActive = true;
                        tyre.reset_collector(game_screen, i);
                    }
                }
            }


            // Checking collector collision with player
            if (fuel.isActive && fuel.collision(player_car))
            {
                (player_car.fuel += fuel.value) >= 1000 ? player_car.fuel = 1000 : player_car.fuel += fuel.value;
                player_car.score += 100;
                fuel.isActive = false;
            }

            if (tyre.isActive && tyre.collision(player_car))
            {
                (player_car.tyre_health += tyre.value) >= 1000 ? player_car.tyre_health = 1000 : player_car.tyre_health += tyre.value;
                player_car.score += 100;
                tyre.isActive = false;
            }

            // Detecting traffic Collision and rendering them
            for (const auto& enemy: enemies)
            {
                if (enemy->isActive) frameBuffer << render::print_race_car(enemy);

                if (enemy->collision(player_car.x_position, player_car.y_position) && enemy->isActive) Message = "Car crashed with incoming traffic";
            }

            // Detecting track collision and car status
            if (player_car.x_position <= (race_track.TrackStart + 1) || (player_car.x_position + player_car.width) >= (race_track.TrackEnd))
            {
                Message = "Car Collides with track";
            }

            if (player_car.tyre_health <= 0 || player_car.fuel <= 0 || player_car.chassis_health <= 0)
            {
                if (player_car.tyre_health <= 0)
                {
                    Message = "Tyre Punctured";
                }
                else if (player_car.chassis_health == 0)
                {
                    Message = "Car Chassis Destroyed";
                }
                else
                {
                    Message = "Empty Fuel Tank";
                }
            }

            // Quitting game and Resetting Player and traffic
            if (!Message.empty())
            {
                PlayerScore = player_car.score;
                player_car.reset_car(game_screen, game_screen.Row - player_car.height - 1);

                for (const auto& enemy: enemies)
                {
                    enemy->isActive = false;
                }

                if (player_car.score > player_car.high_score)
                {
                    player_car.high_score = player_car.score;
                }

                current_screen_mode = ScoreBoard;
            }

            // Activating traffic
            if (game_state.gameTick == 80 )
            {
                enemies[0]->isActive = true;
                enemies[0]->reset_car(game_screen, 1);
            }

            // Increasing Game Speed over time
            if (game_state.gameTime >= 100)
            {
                game_state.gameSpeed = 4;
            }
            if (game_state.gameTime >= 160)
            {
                game_state.gameSpeed = 3;
            }
            if (game_state.gameTime >= 240)
            {
                game_state.gameSpeed = 2;
            }

            if (game_state.gameTick >= 100 && game_state.gameTick % game_state.gameSpeed == 0)
            {
                // Getting new random traffic distribution whenever current one ends
               if (traffic_setting.current_traffic_distribution_row == 0)
                {
                    traffic_setting.seed = TC::random_int(0, 4);
                }

                manage_traffic(enemies, game_screen, traffic_setting.seed, &traffic_setting.current_traffic_car, &traffic_setting.current_traffic_distribution_row);
            }

            std::cout << frameBuffer.str() << std::flush;
            game_state.gameTick++;

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


        // Checks for loop completion time and sleep if code executed before targeted time
        if (auto frameTime = Clock::now() - startTime; frameTime < targetFrameTime)
        {
            std::this_thread::sleep_for(targetFrameTime - frameTime);
        }

    }

    handle_high_score(&player_car.high_score, true, FilePath);

    // Turning terminal back to normal
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


static void spawn_traffic(EnemyCars* Enemies[], const int enemies_size, const Screen game_screen, int* current_car, const int traffic_distribution_row[])
{
    for (int i = 0; i < 3; i++)
    {
        if (traffic_distribution_row[i] == 1)
        {
            if (Enemies[*current_car]->isActive)
            {
                *current_car = (*current_car + 1) % enemies_size;
                continue;
            }

            Enemies[*current_car]->isActive = true;

            // Resetting car colors and attributes
            const int rand = TC::random_int(0, 5);
            Enemies[*current_car]->update_car_model(car_designs[rand].body, car_designs[rand].bumper, car_designs[rand].tyre);

            Enemies[*current_car]->reset_car(game_screen, 1 , i);

            *current_car = (*current_car + 1) % enemies_size;

        }
    }
}


static void manage_traffic(EnemyCars* Enemies[], const Screen game_screen, const int seed, int* current_car, int* current_distribution_row, const int enemies_size)
{

    const auto [FirstRow, SecondRow, ThirdRow] = traffic_distributions[seed];
    const int* row[] = {
        ThirdRow,
        SecondRow,
        FirstRow
    };

    const int previous_car = (*current_car + enemies_size - 1) % enemies_size;
    if (Enemies[previous_car]->y_position >= 2 * Enemies[*current_car]->height + 4)
    {
        spawn_traffic(Enemies, enemies_size, game_screen, current_car, row[*current_distribution_row]);

        *current_distribution_row = ( *current_distribution_row + 1 ) % 3;
    }


    // Deactivating any cars that are out of screen;
    for (int i = 0; i < enemies_size; i++)
    {
        if ( Enemies[i]->y_position + Enemies[i]->height >= game_screen.Row - 1 )
        {
            Enemies[i]->isActive = false;
        }
        else
        {
            if (Enemies[i]->isActive) Enemies[i]->y_position++;
        }
    }
}