#pragma once 
#include "../Board.h"
#include "../Move.h"
#include "../search.h"
#include <tuple>
#include <fstream>


class Game
{
public:
    static int randomOpeningMoveCount;
    static int adjudicationWinScoreLimit;
    static int adjudicationDrawScoreLimit;
    static int ajudicationDrawCount;
    static int adjudicationWinCount;
    static int angineGameSearchDepth;
    static int gameHashSize;
    static std::string wdlPath;

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
        return m_savedFens.size();
    }
    
    std::tuple<Move, int> searchPosition();
private:
    int    m_currentPly = 0;
    bool   m_useTb      = false;
    Board  m_currentPosition;
    Search m_searcher;
    std::ofstream& m_outputBook;
    std::vector<std::pair<std::string, int>> m_savedFens;
};
