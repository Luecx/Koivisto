#pragma once 
#include "../Board.h"
#include "../Move.h"
#include "../search.h"
#include <tuple>
#include <fstream>


class Game
{
public:
    static int RandomOpeningMoveCount;
    static int AdjudicationWinScoreLimit;
    static int AdjudicationDrawScoreLimit;
    static int AdjudicationDrawCount;
    static int AdjudicationWinCount;
    static int EngineGameSearchDepth;
    static int GameHashSize;
    static std::string WDLPath;

    static void init(int argc, char** argv);

    Game(std::ofstream&);

    bool positionIsFavourable(Move best);
    void run();
    void makeBookMove();
    void savePosition(int score);
    bool hasLegalLeft();
    bool isDrawn();
    void reset();
    void saveGame(std::string_view result);
    
    std::uint64_t totalGenerated() 
    {
        return m_SavedFens.size();
    }
    
    std::tuple<Move, int> searchPosition();
private:
    int    m_CurrentPly = 0;
    bool   m_UseTb      = false;
    Board  m_CurrentPosition;
    Search m_Searcher;
    std::ofstream& m_OutputBook;
    std::vector<std::pair<std::string, int>> m_SavedFens;
};
