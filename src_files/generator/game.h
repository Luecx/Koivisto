
#define GENERATOR

#pragma once
#include "../Board.h"
#include "../Move.h"
#include "../search.h"

#include <fstream>
#include <random>
#include <tuple>
#ifdef GENERATOR

class Game {
    public:
    // opening settings
    static int openingBookPly;
    static int recordPly;
    static int maxPly;
    
    // adjudicate wins if the score is above x for n ply
    static bool adjudicateWins;
    static int  adjudicateWinPly;
    static int  adjudicateWinScore;
    
    // adjudicate draws if the score is below x for n ply
    static bool adjudicateDraws;
    static int  adjudicateDrawPly;
    static int  adjudicateDrawScore;
    
    // adjudicate tablebase scores
    static bool adjudicateTB;
    
    // search settings
    static int  searchNodes;
    static int  searchHash;
    static bool searchTB;
   
    // init settings above with command line arguments
    static void init(int argc, char** argv);

    Game(std::ofstream&, std::mt19937&);

    bool                  positionIsFavourable(move::Move best);
    void                  run();
    bool                  makeBookMove();
    void                  savePosition(int score);
    bool                  hasLegalLeft();
    bool                  isDrawn();
    void                  reset();
    void                  saveGame(std::string_view result);

    std::uint64_t         totalGenerated() { return m_savedFens.size(); }

    std::tuple<move::Move, int> searchPosition();

    private:
    int                                      m_currentPly = 0;
    bool                                     m_useTb      = false;
    Board                                    m_currentPosition;
    Search                                   m_searcher;
    std::ofstream&                           m_outputBook;
    std::vector<std::pair<std::string, int>> m_savedFens;
    std::mt19937&                            m_randomGenerator;
};

#endif