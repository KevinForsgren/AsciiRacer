#ifndef ASCIIRACER_TERMINAL_H
#define ASCIIRACER_TERMINAL_H
#include <string>
#include "gameSettings.h"



class TerminalControl
{
    public:
        /** Clear the visible terminal contents. */
        static std::string clear_terminal(Screen game_screen);
        /** Freeze/Unfreeze terminal based on input. */
        static std::string toggle_terminal_freeze(bool freeze);
        /** Store the current terminal row and column counts. */
        static void get_terminal_size(int* row, int* col);
        /** Hide the terminal cursor using an ANSI control sequence. */
        static void hide_cursor();
        /** Return to the main terminal screen buffer. */
        static void main_window();
        /** Return an ANSI sequence that moves the cursor to a row and column. */
        static std::string move_cursor(int, int);
        /** Enter the terminal's alternate screen buffer. */
        static void new_window();
        /** Read one available input character without waiting for a line. */
        static bool read_input(char* c);
        /** Show the terminal cursor using an ANSI control sequence. */
        static void show_cursor();
        /** Enable or disable platform-specific raw input mode. */
        static bool switch_raw_mode(bool);
        /** Build an ANSI true-color foreground sequence. */
        static std::string tc_color(int, int, int);
        /** Build an ANSI true-color background sequence. */
        static std::string tc_background(int, int, int);
        /** Return a random integer in the inclusive range [min, max]. */
        static int random_int(int min, int max);
        /** Toggle virtual terminal processing inside windows machine. */
        static bool setup_windows_terminal(bool toogle);
};


#endif
