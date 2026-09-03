#ifndef ASCIIRACER_TERMINAL_H
#define ASCIIRACER_TERMINAL_H
#include <string>



class TerminalControl
{
    public:
        static void clear_terminal();
        static void get_terminal_size(int* row, int* col);
        static void hide_cursor();
        static void main_window();
        static std::string move_cursor(int, int);
        static void new_window();
        static bool read_input(char* c);
        static void show_cursor();
        static bool switch_raw_mode(bool);
        static std::string tc_color(int, int, int);
        static std::string tc_background(int, int, int);
};


#endif
