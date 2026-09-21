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

void Collector::reset_collector(const Screen game_screen, const int lane_index)
{
    int screen_col = game_screen.Col;
    if (screen_col % 2 == 0)
    {
        screen_col++;
    }

    int spawn_x[3];
    spawn_x[1] = (screen_col - width) / 2;
    spawn_x[0] = spawn_x[1] - 12;
    spawn_x[2] = spawn_x[1] + 12;

    x_position = spawn_x[lane_index];
    y_position = 5;

}


void Collector::manage_collector(const Screen game_screen)
{
    if ( (y_position + height ) >= (game_screen.Row - 1)  )
    {
        isActive = false;
    } else
    {
        y_position++;
    }
}

std::string Collector::spawn_collector() const
{
    std::stringstream collector_buffer;

    for (int i = 0; i < 3; i++)
    {
        collector_buffer << TC::move_cursor(y_position + i, x_position);
        collector_buffer << collector_model[i];
    }

    return collector_buffer.str();
}

std::string Ground::manage_grass(const Track track, const Screen game_screen) const
{
    std::stringstream grass_buffer;

    // grass_buffer << TC::move_cursor(i, track.TrackStart + 2) << model[random];
    grass_buffer << TC::move_cursor(0, track.TrackEnd - environment_objects[1].Width - (15 - environment_objects[1].Width) );

    for (int i = 0; i < game_screen.Row; i++)
    {
        const int random = TC::random_int(0, 2);
    }

    for (int i = 0; i < game_screen.Row; i++)
    {
        const int random = TC::random_int(0, 2);
    }

    return grass_buffer.str();
}



