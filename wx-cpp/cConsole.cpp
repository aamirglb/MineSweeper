#include "cConsole.h"

cConsole::cConsole(int32_t *mines, int32_t row, int32_t col) : 
      _mines{mines}
    , _row{row}
    , _column{col}
{
}

cConsole::~cConsole()
{
    endwin();
}

void cConsole::initConsole()
{
    // Initialise curses
    initscr();
    raw();
    curs_set(0);
    refresh();
}

void cConsole::clearConsole()
{
    clear();
}

void cConsole::printCheatSheet()
{
    for (auto i = 0; i < (3 * _column); ++i)
        printw("=");
    printw("\n");

    for (auto i = 0; i < _row; ++i) {
        for (auto j = 0; j < _column; ++j) {
            if (_mines[i * _column + j] == -1) {
                printw(" X ");
            }
            else {
                printw(" . ");
            }
        }
        printw("\n");
    }
    for (auto i = 0; i < (3 * _column); ++i)
        printw("*");
    printw("\n");
    refresh();
}

void cConsole::setButtonState(int32_t buttonIdx, int32_t state)
{
    // Update console
    auto x = buttonIdx / _column;
    auto y = buttonIdx % _column;
    move(1 + x, 3 * y);
    attron(state);
    printw(" . ");
    attroff(state);
    refresh();
}
