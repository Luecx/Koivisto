#pragma once 
#include "../Board.h"
#include "../Move.h"
#include "../search.h"
#include <tuple>
#include <fstream>
#include <random>

class Game
{
public:
    static int randomOpeningMoveCount;
    static int adjudicationWinScoreLimit;
    static int adjudicationDrawScoreLimit;
    static int adjudicationDrawCount;
    static int adjudicationWinCount;
    static int engineGameSearchDepth;
    static int gameHashSize;
    static bool useTbAdjudication;
    static bool useDrawAdjudication;
    static bool useWinAdjudication;
    static bool useTbSearch;
    static std::string wdlPath;

    static void init(int argc, char** argv);

    Game(std::ofstream&, std::mt19937&);

    bool positionIsFavourable(Move best);
    void run();
    bool makeBookMove();
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
    std::mt19937& m_randomGenerator;
};