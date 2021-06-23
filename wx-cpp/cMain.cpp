#include "cMain.h"
#include <iostream>
#include <iomanip>

#ifdef linux
#include <ncurses.h>
#endif

#include "curses.h"

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
    label = new wxStaticText(this, wxID_ANY, "Score: 0/70");
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
    
    wxBoxSizer* buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->Add(_allMines, 0, wxEXPAND | wxALL);
    buttonsSizer->Add(_neighbouringMines, 0, wxEXPAND | wxALL);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(label);
    mainSizer->Add(grid, 1, wxEXPAND | wxALL);
    mainSizer->Add(buttonsSizer);
    this->SetSizer(mainSizer);
    mainSizer->Layout();

    // Initialise curses
    //initscr();
    //raw();
    //curs_set(0);
    //// printw("Hello World !!!");
    //refresh();
}

cMain::~cMain()
{
    delete[]btn;
    delete[]_mines;
    //endwin();
}

void cMain::printSheetCode()
{
    for (auto i = 0; i < (3 * _column); ++i)
    //    printw("=");
    //printw("\n");
         std::cout << "=";
     std::cout << '\n';

    for (auto i = 0; i < _row; ++i) {
        for (auto j = 0; j < _column; ++j) {
            if (_mines[i * _column + j] == -1) {
                 std::cout << " X ";
                //printw(" X ");
            }
            else {
                //printw(" . ");
                 std::cout << " . ";
            }
        }
        //printw("\n");
         std::cout << "\n";
    }
    for (auto i = 0; i < (3 * _column); ++i)
        //printw("*");
         std::cout << "*";
     std::cout << '\n';
    //printw("\n");
    //refresh();
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
    for (auto p : cells) {
        std::cout << "(" << p.first << ", " << p.second << ")\n";
    }
    return cells;
}


void cMain::OnButtonClicked(wxCommandEvent& evt)
{
    //m_list1->AppendString(m_txt1->GetValue());
    int x = (evt.GetId() - 1000) / _column;
    int y = (evt.GetId() - 1000) % _column;
    
    int32_t clickedBtnIdx = evt.GetId() - 1000;

    /*move(0, 0);
    printw("(%d, %d)\n", x, y);
    refresh();*/
    wxString msg;
    msg.Printf("Position (%d, %d) clicked\n", x, y);

    if (!_firstClick) {
        int mines = _mineCount;

        while (mines) {
            auto mineIdx = id(dre);
            
            if (clickedBtnIdx != mineIdx && _mines[mineIdx] == 0) {
                wxString msg;
                _mines[mineIdx] = -1;
                --mines;
            }
        }
        for (auto m = 0; m < _gridSize; ++m) {
            if (m % 10 == 0) std::cout << "\n";
            std::cout << std::setw(3) << _mines[m];
        }
        //printSheetCode();
        _firstClick = true;
    }

    btn[clickedBtnIdx]->Enable(false);
    
    if (_lastButton != -1) {
        btn[_lastButton]->SetBackgroundColour(wxNullColour);
        // btn[nPrevButton]->SetBackgroundStyle(wxBG_STYLE_SYSTEM);
        auto cells = getNeighouringCells(_lastButton);
        for (auto c : cells) {
            btn[c.first * _column + c.second]->SetBackgroundColour(wxNullColour);
        }
    }

    if (_mines[clickedBtnIdx] == -1) {
        btn[clickedBtnIdx]->SetBackgroundColour(*wxRED);
        wxString msg;
        msg.Printf("BOOOOM !! - You hit the mine!! Game Over :(-\nYour Score is %d/70", _score);
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

        if(_score == 70) {
            wxMessageBox("Congratulations!! You Won The Game.");
            resetGame();
            return;
        }
        // move(2, (3*x)+1);
        // printw("%d", (3*x));
    }
    
    _lastButton = clickedBtnIdx;
    
    
    wxString score;
    score.Printf("Score: %d/70", _score);
    label->SetLabel(score);
    evt.Skip();
}

void cMain::OnAllMinesPressed(wxMouseEvent& evt)
{
    std::cout << "Pressed";
    evt.Skip();
}

void cMain::OnAllMinesRelease(wxMouseEvent& evt)
{
    std::cout << "Release";
    evt.Skip();
}

void cMain::resetGame() {
    _firstClick = false;
    _score = 0;
    for (auto i = 0; i < _gridSize; ++i) {
        btn[i]->SetLabel("");
        btn[i]->Enable(true);
        _mines[i] = 0;
    }
}