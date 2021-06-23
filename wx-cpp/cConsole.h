#pragma once
#include <cinttypes>
#include <curses.h>

class cConsole
{
public:
	cConsole(int32_t* mines, int32_t row, int32_t col);
	~cConsole();
	void initConsole();
	void clearConsole();
	void printCheatSheet();
	void setButtonState(int32_t buttonIdx, int32_t state);

private:
	const int32_t* _mines;
	int32_t _row;
	int32_t _column;
};

