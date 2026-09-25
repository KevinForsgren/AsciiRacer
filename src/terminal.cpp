#include "header/terminal.h"
#include <iostream>
#include <random>
#include <sstream>
#include <string>

#ifdef _WIN32
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
 * @param R red value
 * @param G green value
 * @param B blue value
 * @return
 */
std::string TerminalControl::tc_color(int const R, int const G, int const B)
{
    return "\033[38;2;" + std::to_string(R) + ";" + std::to_string(G) + ";" + std::to_string(B) + "m";

}


/**
 * return a ansi background color code with the provided RGB values
 * @param R red value
 * @param G green value
 * @param B blue value
 * @return
 */
std::string TerminalControl::tc_background(const int R, const int G, const int B)
{
    return "\033[48;2;" + std::to_string(R) + ";" + std::to_string(G) + ";" + std::to_string(B) + "m";
}


/**
 * Generates a random number from the given range
 * @param min Range starts from
 * @param max Range ends
 * @return the random integer
 */
int TerminalControl::random_int(const int min, const int max)
{
    std::random_device device;

    std::mt19937 gen(device());
    std::uniform_int_distribution<> distribution(min, max);

    return distribution(gen);

}


/**
 * move cursor to the specific location in the terminal, starts from (1, 1)
 * @param line row
 * @param col column
 * @return ANSI code for a particular location
 */
std::string TerminalControl::move_cursor(const int line, const int col)
{
    std::stringstream location;
    location << "\033[" << line  << ';'  << col << 'H';
    return location.str();
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
 * @return string
 */
std::string TerminalControl::clear_terminal(Screen game_screen)
{
    return "\033[2J";
}


/**
* freeze/unfreeze terminal based on input
* @param freeze Bool value for switching mode
* @return string
*/
std::string TerminalControl::toggle_terminal_freeze(bool freeze)
{
    if (freeze)
    {
        return "\033[?2026h";
    }
    else 
    {
        return "\033[?2026l";
    }
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
 * Get the current size of the terminal
 * @param row address of a variable
 * @param col address of a variable
 * @return void
 */
void TerminalControl::get_terminal_size(int* row, int* col)
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *col = (int)(csbi.srWindow.Right - csbi.srWindow.Left + 1); // Width (Columns)
    *row = (int)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1); // Height (Rows)

#elif defined(__linux__)
    winsize w{};

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *row = static_cast<int>(w.ws_row);
    *col = static_cast<int>(w.ws_col);

#endif // windows/linux
}


/**
 * toggle between raw and cooked terminal mode
 * @param toggle Bool value for switching terminal mode
 * @return bool
 */
bool TerminalControl::switch_raw_mode(const bool toggle)
{
#ifdef _WIN32
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
#ifdef _WIN32
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

        // setting timeout for reading input
        newT.c_cc[VMIN] &= 0;
        newT.c_cc[VTIME] &= 1; //waiting for 1/10th of a second

        // TCSANOW for applying changes immediately
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

/**
 * Reads and store user input from the terminal
 * @param c address pointer to store user input characters
 * @return bool
 */
bool TerminalControl::read_input(char* c)
{
#ifdef _WIN32
    //HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    //DWORD bytesRead;
    //if (ReadFile(hInput, c, 1, &bytesRead, nullptr) && bytesRead > 0)
    //{
    //    return true;
    //}

    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD events = 0;

    // Check if there are any events waiting in the input buffer
    GetNumberOfConsoleInputEvents(hInput, &events);

    while (events > 0)
    {
        INPUT_RECORD ir;
        DWORD read;

        // Read and remove the event from the buffer
        ReadConsoleInput(hInput, &ir, 1, &read);

        // Ensure the event is a Key Press (not a release) and contains an actual character
        if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown)
        {
            char asciiChar = ir.Event.KeyEvent.uChar.AsciiChar;
            if (asciiChar != 0)
            {
                *c = asciiChar;
                return true; // Valid key found, return immediately
            }
        }

        // If it was a mouse movement or a modifier key (like Shift), 
        // the loop continues and checks the next event.
        GetNumberOfConsoleInputEvents(hInput, &events);
    }
#elif defined(__linux__)
    // return true if i byte is read
    if (read(STDIN_FILENO, c, 1) == 1)
    {
       return true;
    }

#endif
    return false;
}