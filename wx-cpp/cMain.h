#pragma once

#include "wx/wx.h"
#include <vector>
#include <random>

class cConsole;

class cMain : public wxFrame
{
public:
    cMain();
    ~cMain();
    using ListOfCells = std::vector<std::pair<int32_t, int32_t>>;

public:
    void OnButtonClicked(wxCommandEvent& evt);
    void OnAllMinesPressed(wxMouseEvent& evt);
    void OnAllMinesRelease(wxMouseEvent& evt);
    void OnNeighbourMinesPressed(wxMouseEvent& evt);
    void OnNeighbourMinesRelease(wxMouseEvent& evt);

private:
    void updateScore();
    void resetGame();
    ListOfCells getNeighouringCells(int32_t btnIdx);

    // wxDECLARE_EVENT_TABLE();

private:
    int _row{ 10 };
    int _column{ 10 };
    int _gridSize{ _row * _column };
    int _score{};
    int _lastButton{ -1 };
    bool _firstClick{};
    int _mineCount{};
    int _winningScore{};

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::default_random_engine dre{ rd() };
    std::uniform_int_distribution<int32_t> id{ 0, _gridSize - 1 };

    wxButton** btn{};
    wxButton* _allMines{};
    wxButton* _neighbouringMines{};

    int* _mines{};
    wxStaticText* label{};
    cConsole* _console;
};

