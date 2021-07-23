#pragma once 
#include "Board.h"
#include "Move.h"
#include "search.h"
#include <tuple>
#include <fstream>

constexpr int RandomOpeningMoveCount = 8;
constexpr int AdjudicationWinScoreLimit = 1000;
constexpr int AdjudicationDrawScoreLimit  = 40;
constexpr int AdjudicationDrawCount  = 4;
constexpr int AdjudicationWinCount   = 4;
constexpr int EngineGameSearchDepth  = 9;
constexpr int GameHashSize           = 32;
constexpr const char* GameOutputBook = "generate_fens.txt";

class Game
{
public:
    Game();

    bool positionIsFavourable(Move best);
    void run();
    void makeBookMove();
    void savePosition(int score);
    bool hasLegalLeft();
    bool isDrawn();
    void saveGame(std::string_view result);
    
    std::tuple<Move, int> searchPosition();
private:
    int    m_CurrentPly = 0;
    Board  m_CurrentPosition;
    Search m_Searcher;
    std::vector<std::pair<std::string, int>> m_SavedFens;
};