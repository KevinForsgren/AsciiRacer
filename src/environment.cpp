#include "header/environment.h"
#include "header/AsciiArt.h"

/**
 * Check axis-aligned overlap between a collector and the player.
 * @param player player car to test
 * @return true when the object bounds overlap; otherwise false
 */
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


/**
 * Place a collector at the top of a lane and leave its active state unchanged.
 * @param game_screen terminal dimensions
 * @param lane_index lane index in the range 0 through 2
 */
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


/**
 * Move an active collector one row toward the player.
 * @param game_screen terminal dimensions used for deactivation
 */
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


/**
 * Render the collector's three-row model at its current position.
 * @return terminal escape sequences and collector artwork
 */
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


/**
 * Create the repeating ground-object belt used by the gameplay renderer.
 * @param environment_objects four environment objects forming one pattern
 */
GroundSystem::GroundSystem (EnvironmentObject environment_objects[])
{
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
    int current_y = -pattern_height; // Start one full pattern above the screen

    for (int p = 0; p < num_blocks; ++p)
    {
        for (int i = 0; i < 4; ++i)
        {
            // Sequentially add 1, 2, 3, 4
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


/**
 * Move each ground block down and recycle blocks that leave the screen.
 * @param scroll_speed number of rows to move
 */
void GroundSystem::update(const int scroll_speed = 1)
{

    for (auto& block : belt)
    {
        block.y += scroll_speed;

        // Recycle: if the block moves completely off the bottom
        if (block.y > screen_height)
        {
            // Teleport to the top exactly as before.
            // This still works perfectly because total_belt_height is the exact sum of all varying heights.
            block.y -= total_belt_height;
        }
    }
}


/**
 * Render visible ground blocks on the left and right sides of the track.
 * @param track current track geometry
 * @param game_screen terminal dimension
 * @return terminal escape sequences and ground artwork
 */
std::string GroundSystem::render(const Track& track, const Screen game_screen) const
{
    std::stringstream screen_buffer;

    for (const auto& block : belt)
    {
        const int block_height = block.Height;

        for (int row = 0; row < block_height; ++row)
        {
            int screen_y = block.y + row;

            // Only draw if within vertical screen bounds
            if (screen_y >= 0 && screen_y <= game_screen.Row)
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

