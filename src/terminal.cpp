#include "header/terminal.h"
#include <iostream>
#include <string>

#if defined(_WIN32)
#define WIN32_LEAN_AND_CLEAN
#define VC_EXTRALEAN
#include <windows.h>
#elif defined(__linux__)
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#endif

/**
 * return a ansi color code with the provided RGB values
 * @param R
 * @param G
 * @param B
 * @return
 */
std::string TerminalControl::tc_color(int const R, int const G, int const B)
{
    return "\033[38;2;" + std::to_string(R) + ";" + std::to_string(G) + ";" + std::to_string(B) + "m";
}

/**
 * return a ansi background color code with the provided RGB values
 * @param R
 * @param G
 * @param B
 * @return
 */
std::string TerminalControl::tc_background(const int R, const int G, const int B)
{
    return "\033[48;2;" + std::to_string(R) + ";" + std::to_string(G) + ";" + std::to_string(B) + "m";
}

/**
 * Get the current size of the terminal
 * @param row update the row with the terminal current max row
 * @param col update the column with the terminal current max column
 * @return void
 */
void TerminalControl::get_terminal_size(int* row, int* col)
{
    #if defined(_WIN32)
        CONSOLE_SCREEN_BUFFER_INFO csbi;

        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        *row = (int)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
        *col = (int)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);

    #elif defined(__linux__)
        winsize w{};

        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        *row = static_cast<int>(w.ws_row);
        *col = static_cast<int>(w.ws_col);

    #endif // windows/linux
}


/**
 * move cursor to the specific location in the terminal
 * @param X row
 * @param Y column
 * @return void
 */
void TerminalControl::move_cursor(const int X, const int Y)
{
    std::cout << "\033[" << X << ';' << Y << 'H';
}


/**
 * create a new terminal window
 * @return void
 */
void TerminalControl::new_window()
{
    std::cout << "\033[?1049h";
}


/**
 * return to the main terminal window
 * @return void
 */
void TerminalControl::main_window()
{
    std::cout << "\033[?1049l";
}

/**
 * clear current content of the terminal
 * @return void
 */
void TerminalControl::clear_terminal()
{
    std::cout << "\033[2J";
}

/**
 * hide the cursor
 * @return void
 */
void TerminalControl::hide_cursor()
{
    std::cout << "\033[?25l";
}

/**
 * show the cursor
 * @return void
 */
void TerminalControl::show_cursor()
{
    std::cout << "\033[?25h";
}


/**
 * toggle between raw and cooked terminal mode
 * @param toggle Bool
 */
bool TerminalControl::switch_raw_mode(const bool toggle)
{
#if defined (_WIN32)
    static DWORD originalInputMode = 0;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

    // Only fetch original attributes if haven't stored them yet
    if (originalInputMode == 0 && !GetConsoleMode(hStdin, &originalInputMode)) {
        return false;
    }

#elif defined (__linux__)
    static termios oldT, newT;
    if (tcgetattr(STDIN_FILENO, &oldT) != 0) return false;
#endif

    if (toggle)
    {
#if defined (_WIN32)
        DWORD rawInputMode = originalInputMode;
        rawInputMode &= ~ENABLE_LINE_INPUT;
        rawInputMode &= ~ENABLE_ECHO_INPUT;
        rawInputMode &= ~ENABLE_PROCESSED_INPUT;

        if (!SetConsoleMode(hStdin, rawInputMode)) return false;

#elif defined (__linux__)
        newT = oldT;
        // Bitwise not and AND operation to turn off the bits
        newT.c_iflag &= ~(ICRNL | IXON);
        // Turn off terminal ECHO, ICANON (canonical mode), IEXTEN, and ISIG (signals interruption)
        newT.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

        // TCSANOW for reading one char at a time
        if (tcsetattr(STDIN_FILENO, TCSANOW, &newT) != 0) return false;
#endif

        return true;
    }

    // Restore original settings (toggle == false)
#if defined(_WIN32)
    if (!SetConsoleMode(hStdin, originalInputMode)) return false;
#elif defined(__linux__)
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldT) != 0) return false;
#endif

    return true;
}

