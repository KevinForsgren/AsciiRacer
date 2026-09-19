//
// Created by kevin on 9/18/26.
//

#include "header/environment.h"

#include "header/AsciiArt.h"

bool Collector::collision(const Cars& player) const
{
    // Player is either on the Right || Left
    if ((x_position + width) < player.x_position || ( player.x_position + player.width ) < x_position  )
    {
        return false;
    }

    // Player is either on Top || Bottom
    if ((y_position) > ( player.y_position + player.height ) || player.y_position > ( y_position + height ) )
    {
        return false;
    }

    // Else collision happened
    return true;
}

void Collector::reset_collector(const Screen game_screen)
{
    int screen_col = game_screen.Col;
    if (screen_col % 2 == 0)
    {
        screen_col++;
    }

    int spawn_x[3];
    spawn_x[0] = (screen_col - width) / 2;
    spawn_x[1] = spawn_x[0] + 12;
    spawn_x[2] = spawn_x[0] - 12;

    const int index = TC::random_int(0, 2);

    x_position = spawn_x[index];

    // provide y position
    y_position = 10;

}


void Collector::manage_collector(const Screen game_screen)
{
    if ( (y_position + height ) >= (game_screen.Row - 1)  )
    {
        reset_collector(game_screen);
        isActive = false;
        // for (int i = 0; i < 3; i++)
        // {
        //     if (enemy->x_position == Enemies[i]->x_position && enemy->enemy_id != Enemies[i]->enemy_id)
        //     {
        //         enemy->y_position = Enemies[i]->y_position + Enemies[i]->height + 10;
        //     }
        // }

    } else
    {
        y_position++;
    }
}

std::string Collector::spawn_collector() const
{
    std::stringstream buffer;

    int a = 0;
    for (const auto& row: collector_model)
    {
        buffer << TC::move_cursor(y_position + 0, x_position) << row;
        a++;
    }

    return buffer.str();
}

