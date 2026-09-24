#include "header/environment.h"

#include "header/AsciiArt.h"

bool Collector::collision(const Cars& player) const
{
    // Player is either on the Right || Left
    if ((this->xPosition + this->Width) < player.xPosition || ( player.xPosition + player.Width ) < this->xPosition  )
    {
        return false;
    }

    // Player is either on Top || Bottom
    if ((this->yPosition) > ( player.yPosition + player.Height ) || player.yPosition > ( this->yPosition + this->Height ) )
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
    spawn_x[1] = (screen_col - this->Width) / 2;
    spawn_x[0] = spawn_x[1] - 12;
    spawn_x[2] = spawn_x[1] + 12;

    this->xPosition = spawn_x[lane_index];
    this->yPosition = 5;

}


void Collector::manage_collector(const Screen game_screen)
{
    if ( (this->yPosition + this->Height ) >= (game_screen.Row - 1)  )
    {
        this->isActive = false;
    } else
    {
        this->yPosition++;
    }
}

std::string Collector::spawn_collector() const
{
    std::stringstream collector_buffer;

    for (int i = 0; i < 3; i++)
    {
        collector_buffer << TC::move_cursor(this->yPosition + i, this->xPosition);
        collector_buffer << this->Model[i];
    }

    return collector_buffer.str();
}

GroundSystem::GroundSystem (const Screen game_screen, EnvironmentObject environment_objects[])
{
    this->screen_height = game_screen.Row;
    int pattern_height = 0;
    constexpr int VerticalSpacing = 3;

    for (int i = 0; i < 4; i++)
    {
        pattern_height += environment_objects[i].Height + VerticalSpacing;
    }

    // Calculate how many blocks we need to cover the screen.
    // Adding 2 to handle partial scrolling off-screen at the top and bottom.
    int num_blocks = (screen_height / pattern_height) + 2;

    // To keep an alternating pattern perfectly looping, the total number of blocks MUST be even.
    if (num_blocks % 2 != 0) {
        num_blocks++;
    }

    total_belt_height = num_blocks * pattern_height;


    // 4. Initialize the single belt buffer
    int current_y = -pattern_height; // Start one full pattern above the screen FIXME

    for (int p = 0; p < num_blocks; ++p)
    {
        for (int i = 0; i < 4; ++i)
        { // Sequentially add 1, 2, 3, 4
            GroundBlock block{};
            block.art = &environment_objects[i].Model;
            block.Height = environment_objects[i].Height;
            block.Width = environment_objects[i].Width;
            block.y = current_y;

            belt.push_back(block);

            // Increment Y by this specific block's height to handle variable sizes
            current_y += block.Height + VerticalSpacing;
        }
    }
}


void GroundSystem::update(const int scroll_speed = 1)
{
    for (auto& block : belt)
    {
        block.y += scroll_speed;

        // Recycle: if the block moves completely off the bottom
        if (block.y >= screen_height)
        {
            // Teleport to the top exactly as before.
            // This still works perfectly because total_belt_height is the exact sum of all varying heights.
            block.y -= total_belt_height;
        }
    }
}


std::string GroundSystem::render(const Track& track) const
{
    std::stringstream screen_buffer;

    for (const auto& block : belt)
    {
        const int block_height = block.Height;

        for (int row = 0; row < block_height; ++row)
        {
            int screen_y = block.y + row;

            // Only draw if within vertical screen bounds
            if (screen_y >= 0 && screen_y < screen_height)
            {

                // Calculate offset to center the object within the ground area
                int center_offset = (track.GroundSize - block.Width) / 2;
                if (center_offset < 0) center_offset = 0; // Fallback if art is wider than ground

                // Dynamically calculate X positions based on the center offset
                // (Assumes Left Ground is to the left of TrackStart, Right Ground is right of TrackEnd)
                const int left_x = (track.TrackStart + 2) + center_offset;
                const int right_x = track.TrackEnd - (track.GroundSize - center_offset);

                const std::string& art_row = (*block.art)[row];

                // Render LEFT side
                screen_buffer << TC::move_cursor(screen_y, left_x) << art_row;

                // Render RIGHT side
                screen_buffer << TC::move_cursor(screen_y, right_x) << art_row;
            }
        }
    }
    return screen_buffer.str();
}


