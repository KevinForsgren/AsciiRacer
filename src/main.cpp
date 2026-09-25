#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <filesystem>

#include "header/AsciiArt.h"
#include "header/terminal.h"
#include "header/render.h"
#include "header/cars.h"
#include "header/environment.h"
#include "header/gameSettings.h"

#ifdef _WIN32
#include <windows.h>
#endif

// TODO: place ground area materials

/** Activate enemy cars for the occupied lanes in one traffic-pattern row. */
static void spawn_traffic(EnemyCars* Enemies[], int enemies_size, Screen game_screen, int* current_car, const int traffic_distribution_row[]);
/** Advance active traffic and spawn the next pattern row when appropriate. */
static void manage_traffic(EnemyCars* Enemies[], Screen game_screen, int seed, int* current_car, int* current_distribution_row, int enemies_size = 5);
/** Read or persist the binary high-score value. */
static bool handle_high_score(int* high_score, bool write_mode = false, const std::string& file_path = "data.dat");

// Handling Clock
static auto targetFrameTime = std::chrono::microseconds(16'666); // (1'000'000 / 60)

using TC = TerminalControl;
static std::string Message;
static int PlayerScore;

// Handling Files
namespace fs = std::filesystem;
const std::string FilePath = "./data.dat";


/**
 * Initialize terminal state and run the menu, gameplay, and scoreboard loop.
 * @return zero after restoring the terminal
 */
int main()
{
 #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    TC::new_window();
    TC::switch_raw_mode(true);
    TC::hide_cursor();

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
    player_car.reset_car(game_screen, game_screen.Row - player_car.Height - 1);

    handle_high_score(&player_car.HighScore, false, FilePath);

    // Ground object
    std::vector<EnvironmentObject> environment_objects = {
        {.Model = grass_patch, .Height = 7, .Width = 12},
        {.Model = grass_patch_alternative, .Height = 3, .Width = 13},
        {.Model = stone_dotted, .Height = 3, .Width = 6},
        {.Model = stone_dash, .Height = 3, .Width = 10}
    };
    GroundSystem ground_system(game_screen, (environment_objects.data()));

    // Collectors
    Collector fuel{};
    Collector tyre{};
    fuel.Model = fuel_collector;
    tyre.Model = tyre_collector;

    //Initializing all enemy cars
    EnemyCars* enemies[5];

    const int rand_1 = TC::random_int(0, 5);
    const int rand_2 = TC::random_int(0, 5);

    EnemyCars enemy1{car_designs[rand_1].Body, car_designs[rand_1].Bumper, car_designs[rand_1].Tyre, 100};
    EnemyCars enemy2{car_designs[rand_2].Body, car_designs[rand_2].Bumper, car_designs[rand_2].Tyre, 200};
    EnemyCars enemy3{car_designs[rand_1].Body, car_designs[rand_1].Bumper, car_designs[rand_1].Tyre, 300};
    EnemyCars enemy4{car_designs[rand_2].Body, car_designs[rand_2].Bumper, car_designs[rand_2].Tyre, 400};
    EnemyCars enemy5{car_designs[rand_1].Body, car_designs[rand_1].Bumper, car_designs[rand_1].Tyre, 500};

    enemies[0] = &enemy1;
    enemies[1] = &enemy2;
    enemies[2] = &enemy3;
    enemies[3] = &enemy4;
    enemies[4] = &enemy5;

    // Initialize inGame time
    auto previousTime = Clock::now();

    while (true)
    {
        // Managing clocks and game fps
        auto startTime = Clock::now();
        const double deltaTime = std::chrono::duration<double>(startTime - previousTime).count();
        game_state.GameTime += deltaTime;
        previousTime = startTime;

        // Uncomment for getting terminal size every iteration
        TC::get_terminal_size(&game_screen.Row, &game_screen.Col);

        // Creating a clean terminal
        //TC::clear_terminal(game_screen);


        if (current_screen_mode == MainMenu)
        {
            // Managing Game's main menu
            std::stringstream main_menu_buffer;

            main_menu_buffer << TC::clear_terminal(game_screen);

            main_menu_buffer << render::render_main_menu(game_screen.Row, game_screen.Col);
            
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

            std::cout << main_menu_buffer.str() << std::flush;
        }
        else if (current_screen_mode == Pause)
        {
            std::stringstream pause_buffer;

            // Reset Game State and Player Car for every new gameplay
            player_car.reset_car(game_screen, game_screen.Row - player_car.Height - 1);
            game_state.GameTick = 0;
            game_state.GameTime = 0;
            traffic_setting.CurrentTrafficCar = 1;
            traffic_setting.CurrentTrafficDistributionRow = 0;
            traffic_setting.Seed = TC::random_int(0, 4);
            
            pause_buffer << TC::clear_terminal(game_screen);
            pause_buffer << render::render_pause_menu(game_screen);

            char pause_inpT;
            if (TC::read_input(&pause_inpT))
            {
                if (pause_inpT == 'e' || pause_inpT == 'E')
                {
                    gameplay_settings.Steps = 12;
                    gameplay_settings.FuelDegradation = 11;
                    gameplay_settings.TyreDegradation = 7;
                    gameplay_settings.ChassisDegradation = 5;
                    current_screen_mode = Gameplay;
                }
                else if (pause_inpT == 'm' || pause_inpT == 'M')
                {
                    gameplay_settings.Steps = 2;
                    gameplay_settings.FuelDegradation = 15;
                    gameplay_settings.TyreDegradation = 10;
                    gameplay_settings.ChassisDegradation = 10;
                    current_screen_mode = Gameplay;
                }
                else if (pause_inpT == 'h' || pause_inpT == 'H')
                {
                    gameplay_settings.Steps = 1;
                    gameplay_settings.FuelDegradation = 20;
                    gameplay_settings.TyreDegradation = 13;
                    gameplay_settings.ChassisDegradation = 15;
                    current_screen_mode = Gameplay;
                }
            }

            std::cout << pause_buffer.str() << std::flush;
        }
        else if (current_screen_mode == Gameplay)
        {
            std::stringstream frameBuffer;

            char gameplay_inpT;
            if (TC::read_input(&gameplay_inpT))
            {
                switch (gameplay_inpT)
                {
                case 'a':
                    player_car.move_left(gameplay_settings.Steps);
                    break;
                case 'd':
                    player_car.move_right(gameplay_settings.Steps);
                    break;
                default: break;
                }
            }

            // Printing and Managing track ground
            if (game_state.GameTick % 3 == 0)
            {
                // Move ground down by 1 unit
                ground_system.update(1);
            }
            frameBuffer << ground_system.render(race_track);


            frameBuffer << render::render_game(&player_car, game_screen, &race_track, &game_state, gameplay_settings);

            if (game_state.GameTick % 5 == 0)
            {
                if (fuel.isActive) fuel.manage_collector(game_screen);
                if (tyre.isActive) tyre.manage_collector(game_screen);
            }


            // Printing collector on screen
            if (fuel.isActive) frameBuffer << fuel.spawn_collector();
            if (tyre.isActive) frameBuffer << tyre.spawn_collector();

            // Spawning collector
            if ( game_state.GameTick % 300 == 0  && player_car.Fuel < 450)
            {
                const auto [FirstRow, SecondRow, ThirdRow] = traffic_distributions[traffic_setting.Seed];
                const int* row[] = {
                    ThirdRow,
                    SecondRow,
                    FirstRow
                };

                for (int i = 0; i < 3; i++)
                {
                    if (row[traffic_setting.CurrentTrafficDistributionRow][i] == 0)
                    {
                        fuel.isActive = true;
                        fuel.reset_collector(game_screen, i);
                    }
                }
            }

            if ( game_state.GameTick % 300 == 0  && player_car.TyreHealth < 450)
            {
                const auto [FirstRow, SecondRow, ThirdRow] = traffic_distributions[traffic_setting.Seed];
                const int* row[] = {
                    ThirdRow,
                    SecondRow,
                    FirstRow
                };

                for (int i = 0; i < 3; i++)
                {
                    if (row[traffic_setting.CurrentTrafficDistributionRow][i] == 0)
                    {
                        tyre.isActive = true;
                        tyre.reset_collector(game_screen, i);
                    }
                }
            }


            // Checking collector collision with player
            if (fuel.isActive && fuel.collision(player_car))
            {
                (player_car.Fuel += fuel.Value) >= 1000 ? player_car.Fuel = 1000 : player_car.Fuel += fuel.Value;
                player_car.Score += 100;
                fuel.isActive = false;
            }

            if (tyre.isActive && tyre.collision(player_car))
            {
                (player_car.TyreHealth += tyre.Value) >= 1000 ? player_car.TyreHealth = 1000 : player_car.TyreHealth += tyre.Value;
                player_car.Score += 100;
                tyre.isActive = false;
            }

            // Detecting traffic Collision and rendering them
            for (const auto& enemy: enemies)
            {
                if (enemy->isActive) frameBuffer << render::print_race_car(enemy);

                if (enemy->collision(player_car.xPosition, player_car.yPosition) && enemy->isActive) Message = "Car crashed with incoming traffic";
            }

            // Detecting track collision and car status
            if (player_car.xPosition <= (race_track.TrackStart + 1) || (player_car.xPosition + player_car.Width) >= (race_track.TrackEnd))
            {
                Message = "Car Collides with track";
            }

            if (player_car.TyreHealth <= 0 || player_car.Fuel <= 0 || player_car.ChassisHealth <= 0)
            {
                if (player_car.TyreHealth <= 0)
                {
                    Message = "Tyre Punctured";
                }
                else if (player_car.ChassisHealth == 0)
                {
                    Message = "Car Chassis Destroyed";
                }
                else
                {
                    Message = "Empty Fuel Tank";
                }
            }

            // Quitting game and Resetting traffic
            if (!Message.empty())
            {
                PlayerScore = player_car.Score;
                
                for (const auto& enemy: enemies)
                {
                    enemy->isActive = false;
                }

                current_screen_mode = ScoreBoard;
            }

            // Activating traffic
            if (game_state.GameTick == 80 )
            {
                enemies[0]->isActive = true;
                enemies[0]->reset_car(game_screen, 1);
            }

            // Increasing Game Speed over time
            if (game_state.GameTime >= 90)
            {
                game_state.GameSpeed = 4;
            }
            if (game_state.GameTime >= 160)
            {
                game_state.GameSpeed = 3;
            }
            if (game_state.GameTime >= 240)
            {
                game_state.GameSpeed = 2;
            }

            if (game_state.GameTick >= 100 && game_state.GameTick % game_state.GameSpeed == 0)
            {
                // Getting new random traffic distribution whenever current one ends
               if (traffic_setting.CurrentTrafficDistributionRow == 0)
                {
                    traffic_setting.Seed = TC::random_int(0, 4);
                }

                manage_traffic(enemies, game_screen, traffic_setting.Seed, &traffic_setting.CurrentTrafficCar, &traffic_setting.CurrentTrafficDistributionRow);
            }

            // Updating player highScore
            if (player_car.Score > player_car.HighScore)
            {
                player_car.HighScore = player_car.Score;
            }

            std::cout << frameBuffer.str() << std::flush;
            game_state.GameTick++;

        }
        else if (current_screen_mode == ScoreBoard)
        {
            // Manage ScoreBoard here
            std::stringstream score_board_buffer;

            score_board_buffer << TC::clear_terminal(game_screen);
            score_board_buffer << render::render_score(player_car.HighScore, PlayerScore, game_screen, Message);
            
            char score_inpT;

            if (TC::read_input(&score_inpT))
            {
                if (score_inpT == 'h' || score_inpT == 'H')
                {
                    current_screen_mode = MainMenu;
                    Message.clear();
                }
            }

            std::cout << score_board_buffer.str() << std::flush;
        }


        // Checks for loop completion time and sleep if code executed before targeted time
        if (auto frameTime = Clock::now() - startTime; frameTime < targetFrameTime)
        {
            std::this_thread::sleep_for(targetFrameTime - frameTime);
        }

    }

    handle_high_score(&player_car.HighScore, true, FilePath);

    // Turning terminal back to normal
    TC::switch_raw_mode(false);
    TC::main_window();
    TC::show_cursor();

    return 0;
}


/**
 * Read or persist the binary high-score value, creating the file when needed.
 * @param high_score value to read into or write from
 * @param write_mode false to read, true to write
 * @param file_path binary score-file path
 * @return true when the requested file operation succeeds
 */
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


/**
 * Activate enemy cars for the occupied lanes in one traffic-pattern row.
 * @param Enemies rotating pool of enemy cars
 * @param enemies_size number of entries in Enemies
 * @param game_screen current terminal dimensions
 * @param current_car index of the next reusable enemy
 * @param traffic_distribution_row three-lane occupancy pattern
 */
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
            Enemies[*current_car]->update_car_model(car_designs[rand].Body, car_designs[rand].Bumper, car_designs[rand].Tyre);

            Enemies[*current_car]->reset_car(game_screen, 1 , i);

            *current_car = (*current_car + 1) % enemies_size;

        }
    }
}


/**
 * Advance active traffic and spawn the next row when the previous row is clear.
 * @param Enemies rotating pool of enemy cars
 * @param game_screen current terminal dimensions
 * @param seed selected traffic-distribution index
 * @param current_car index of the next reusable enemy
 * @param current_distribution_row current pattern row
 * @param enemies_size number of entries in Enemies
 */
static void manage_traffic(EnemyCars* Enemies[], const Screen game_screen, const int seed, int* current_car, int* current_distribution_row, const int enemies_size)
{

    const auto [FirstRow, SecondRow, ThirdRow] = traffic_distributions[seed];
    const int* row[] = {
        ThirdRow,
        SecondRow,
        FirstRow
    };

    const int previous_car = (*current_car + enemies_size - 1) % enemies_size;
    if (Enemies[previous_car]->yPosition >= 2 * Enemies[*current_car]->Height + 4)
    {
        spawn_traffic(Enemies, enemies_size, game_screen, current_car, row[*current_distribution_row]);

        *current_distribution_row = ( *current_distribution_row + 1 ) % 3;
    }


    // Deactivating any cars that are out of screen;
    for (int i = 0; i < enemies_size; i++)
    {
        if ( Enemies[i]->yPosition + Enemies[i]->Height >= game_screen.Row - 1 )
        {
            Enemies[i]->isActive = false;
        }
        else
        {
            if (Enemies[i]->isActive) Enemies[i]->yPosition++;
        }
    }
}

