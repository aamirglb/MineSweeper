#include "cMain.h"
#include "cConsole.h"

#include <iostream>
#include <iomanip>

// wxBEGIN_EVENT_TABLE(cMain, wxFrame)
// 	EVT_BUTTON(10001, OnButtonClicked)
// wxEND_EVENT_TABLE()

cMain::cMain() : wxFrame(nullptr, 
    wxID_ANY, 
    "Mine Sweeper", 
    wxPoint(30, 30), 
    wxSize(800, 600))
{
    wxFont font(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false);

    btn = new wxButton*[_row * _column];
    label = new wxStaticText(this, wxID_ANY, "");
    label->SetFocus();
    label->SetFont(font);

    _mineCount = static_cast<int>(_gridSize * .3);
    _winningScore = _gridSize - _mineCount;

    wxGridSizer* grid = new wxGridSizer(_row, _column, 0, 0);
    _mines = new int[_row * _column];

    for (auto x = 0; x < _row; ++x) {
        for (auto y = 0; y < _column; ++y) {
            auto btnIdx = x * _column + y;
            btn[btnIdx] = new wxButton(this, 1000 + btnIdx);
            btn[btnIdx]->SetFont(font);
            btn[btnIdx]->SetBackgroundColour(*wxLIGHT_GREY);

            /*wxString l;
            l.Printf("%d - (%d, %d)", btnIdx, x, y);
            btn[btnIdx]->SetLabel(l);*/
            grid->Add(btn[btnIdx], 1, wxEXPAND | wxALL);

            // Bind event handler
            btn[btnIdx]->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::OnButtonClicked, this);
            _mines[btnIdx] = 0;
        }
    }

    _allMines = new wxButton(this, 4000, "All Mines", wxDefaultPosition, wxSize(400, 50));
    _allMines->SetFont(font);
    _allMines->Bind(wxEVT_LEFT_DOWN, &cMain::OnAllMinesPressed, this);
    _allMines->Bind(wxEVT_LEFT_UP, &cMain::OnAllMinesRelease, this);

    _neighbouringMines = new wxButton(this, 4001, "Neighbouring Mines", wxDefaultPosition, wxSize(400, 50));
    _neighbouringMines->SetFont(font);
    _neighbouringMines->Bind(wxEVT_LEFT_DOWN, &cMain::OnNeighbourMinesPressed, this);
    _neighbouringMines->Bind(wxEVT_LEFT_UP, &cMain::OnNeighbourMinesRelease, this);
    
    wxBoxSizer* buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->Add(_allMines, 0, wxEXPAND | wxALL);
    buttonsSizer->Add(_neighbouringMines, 0, wxEXPAND | wxALL);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(label);
    mainSizer->Add(grid, 1, wxEXPAND | wxALL);
    mainSizer->Add(buttonsSizer);
    this->SetSizer(mainSizer);
    mainSizer->Layout();
    updateScore();

    _console = new cConsole(_mines, _row, _column);
    _console->initConsole();
}

cMain::~cMain()
{
    delete[]btn;
    delete[]_mines;
    delete _console;
}


cMain::ListOfCells cMain::getNeighouringCells(int32_t btnIdx)
{
    int32_t x = btnIdx / _column;
    int32_t y = btnIdx % _column;
    std::vector<std::pair<int32_t, int32_t>> cells;

    for (auto i = -1; i < 2; ++i) {
        for (auto j = -1; j < 2; ++j) {
            if (x + i >= 0 && x + i < _row && y + j >= 0 && y + j < _column) {
                cells.emplace_back(std::make_pair(x + i, y + j));
            }
        }
    }
    return cells;
}


void cMain::OnButtonClicked(wxCommandEvent& evt)
{    
    int32_t clickedBtnIdx = evt.GetId() - 1000;

    if (!_firstClick) {
        int32_t mines = _mineCount;

        while (mines) {
            auto mineIdx = id(dre);
            if (clickedBtnIdx != mineIdx && _mines[mineIdx] == 0) {
                wxString msg;
                _mines[mineIdx] = -1;
                --mines;
            }
        }
        _console->printCheatSheet();
        _firstClick = true;
    }

    btn[clickedBtnIdx]->Enable(false);
    
    if (_lastButton != -1) {
        btn[_lastButton]->SetBackgroundColour(wxNullColour);
        
        auto cells = getNeighouringCells(_lastButton);
        for (auto c : cells) {
            btn[c.first * _column + c.second]->SetBackgroundColour(*wxLIGHT_GREY);
        }

        _console->setButtonState(_lastButton, A_NORMAL);

        //// Update console
        //auto x = _lastButton / _column;
        //auto y = _lastButton % _column;
        //move(1 + x, 3 * y);
        //attron(A_NORMAL);
        //printw(" . ");
        //attroff(A_NORMAL);
        //refresh();
    }

    if (_mines[clickedBtnIdx] == -1) {
        btn[clickedBtnIdx]->SetBackgroundColour(*wxRED);
        wxString msg;
        msg.Printf("BOOOOM !! - You hit the mine!! Game Over :(-\nYour Score is %d/%d", _score, _winningScore);
        wxMessageBox(msg);
        // Reset game
        resetGame();
    }
    else {
        // count neighbouring mines
        int32_t mineCount = 0;
        auto cells = getNeighouringCells(clickedBtnIdx);
        for (const auto c : cells) {
            auto idx = c.first * _column + c.second;
            btn[idx]->SetBackgroundColour(*wxCYAN);
            if (_mines[idx] == -1)
                ++mineCount;
        }
        btn[clickedBtnIdx]->SetLabel(std::to_string(mineCount));
        btn[clickedBtnIdx]->Enable(false);
        btn[clickedBtnIdx]->SetBackgroundColour(*wxBLUE);
        ++_score;
        updateScore();

        if(_score == _winningScore) {
            wxMessageBox("Congratulations!! You Won The Game.");
            resetGame();
            return;
        }
    }
    
    _console->setButtonState(clickedBtnIdx, A_BLINK | A_REVERSE | A_BOLD);
    //auto x = clickedBtnIdx / _column;
    //auto y = clickedBtnIdx % _column;
    //move(1 + x, 3 * y);
    //attron(A_BLINK | A_REVERSE | A_BOLD);
    //printw(" . ");
    //attroff(A_BLINK | A_REVERSE | A_BOLD);
    //refresh();

    _lastButton = clickedBtnIdx;
    evt.Skip();
}

void cMain::OnAllMinesPressed(wxMouseEvent& evt)
{
    if (_firstClick) {
        for (auto i = 0; i < _gridSize; ++i) {
            if (_mines[i] == -1) {
                btn[i]->SetBackgroundColour(*wxRED);
            }
        }
    }
    evt.Skip();
}

void cMain::OnAllMinesRelease(wxMouseEvent& evt)
{
    if (_firstClick) {
        for (auto i = 0; i < _gridSize; ++i) {
            btn[i]->SetBackgroundColour(*wxLIGHT_GREY);
        }

        // color neighbouring cell cyan
        auto cells = getNeighouringCells(_lastButton);
        for (const auto c : cells) {
            auto idx = c.first * _column + c.second;
            if (_lastButton != idx) {
                btn[idx]->SetBackgroundColour(*wxCYAN);
            }
        }
    }
    evt.Skip();
}

void cMain::OnNeighbourMinesPressed(wxMouseEvent& evt)
{
    if (_firstClick) {
        auto cells = getNeighouringCells(_lastButton);
        for (const auto c : cells) {
            auto idx = c.first * _column + c.second;
            if (_mines[idx] == -1) {
                btn[idx]->SetBackgroundColour(*wxRED);
            }
        }
    }
}

void cMain::OnNeighbourMinesRelease(wxMouseEvent& evt)
{
    if (_firstClick) {
        auto cells = getNeighouringCells(_lastButton);
        for (const auto c : cells) {
            auto idx = c.first * _column + c.second;
            if (_mines[idx] == -1) {
                btn[idx]->SetBackgroundColour(*wxCYAN);
            }
        }
    }
}

void cMain::updateScore()
{
    wxString score;
    score.Printf("Score: %d/%d", _score, _winningScore);
    label->SetLabel(score);
}

void cMain::resetGame() {
    _firstClick = false;
    _score = 0;
    for (auto i = 0; i < _gridSize; ++i) {
        btn[i]->SetBackgroundColour(*wxLIGHT_GREY);
        btn[i]->SetLabel("");
        btn[i]->Enable(true);
        _mines[i] = 0;
    }
    label->SetFocus();
    _console->clearConsole();
}