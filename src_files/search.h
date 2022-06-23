
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                           by. Kim Kahre, Finn Eggers and Eugenio Bruno                           *
 *                                                                                                  *
 *                 Koivisto is free software: you can redistribute it and/or modify                 *
 *               it under the terms of the GNU General Public License as published by               *
 *                 the Free Software Foundation, either version 3 of the License, or                *
 *                                (at your option) any later version.                               *
 *                    Koivisto is distributed in the hope that it will be useful,                   *
 *                  but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
 *                   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                  *
 *                           GNU General Public License for more details.                           *
 *                 You should have received a copy of the GNU General Public License                *
 *                 along with Koivisto.  If not, see <http://www.gnu.org/licenses/>.                *
 *                                                                                                  *
 ****************************************************************************************************/

#ifndef KOIVISTO_SEARCH_H
#define KOIVISTO_SEARCH_H

#include "bitboard.h"
#include "board.h"
#include "history.h"
#include "timemanager.h"
#include "transpositiontable.h"
#include "eval.h"
#include "newmovegen.h"

#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <stdint.h>
#include <string>
#include <tgmath.h>
#include <thread>
#include <vector>

#define MAX_THREADS 256
#define MAX_MULTIPV 256

struct RootMove {
    int seldepth;
    bb::Score score;
    bb::Score prevScore;
    move::Move pv[bb::MAX_INTERNAL_PLY + 1];
    uint16_t pvLen;

    inline bool operator==(const move::Move& m) const {
        return move::sameMove(pv[0], m);
    }
    inline bool operator <(const RootMove& other) const {
        return other.score != score ? other.score < score
                                    : other.prevScore < prevScore;
    }
};

/**
 * data about each thread
 */
struct ThreadData {
    // threadID indicates what thread this data belongs to. threadID = 0 is the mainthread
    int        threadID = 0;
    // nodes searched by this thread
    bb::U64    nodes    = 0;
    // maximum depth reached in pvsearch / qsearch in this thread
    int        seldepth = 0;
    // amount of tablenbase hits in this thread
    int        tbhits   = 0;
    // if we dropout from search (due to timeout for example)
    bool       dropOut  = false;
    // search data which contains additional information like history tables etc
    SearchData searchData {};
    // move generators to not reallocate
    moveGen    generators[bb::MAX_INTERNAL_PLY] {};
    
    // pv information...
    // the pvIdx indicates what index of the multipv we are analysing
    int        pvIdx    = 0;
    // we use a triangular pv table to track the pv during search for each thread
    move::Move pv[bb::MAX_INTERNAL_PLY + 1][bb::MAX_INTERNAL_PLY + 1] {};
    // we also need to track the partial pv length of each subtree
    uint16_t   pvLen[bb::MAX_INTERNAL_PLY + 1];
    
    // each thread gets informations
    RootMove   rootMoves[256];
    uint16_t   rootMoveCount;

    ThreadData();

    explicit ThreadData(int threadId);
} __attribute__((aligned(4096)));

/**
 * used to store information about a search
 */
struct SearchOverview {
    int        nodes;
    bb::Score  score;
    int        depth;
    int        time;
    move::Move move;
} __attribute__((aligned(32)));

class Search {
    // how many threads to use for smp
    int threadCount = 1;
    // compute multiPv lines at the same time
    // since multipv will be lowered if there are not enough legal moves, we store
    // the default multiPv value which is adjusted by uci and multiPv which is just the value
    // being used in search
    int multiPv        = 1;
    int multiPvDefault = 1;
    // use a transposition table to store transpositions
    TranspositionTable* table;
    // the search overview stores information from the latest search and includes information
    // defined above (SearchOverview struct)
    SearchOverview searchOverview;
    // the search keeps a reference to the time manager which tells the search when
    // to stop the search based on enabled limits like node-limits, time-limits and depth-limits.
    TimeManager* timeManager;
    // if smp is enabled (threadCount > 1), we need to keep track of all the threads spawned
    std::vector<std::thread> runningThreads;
    // if specified below, the search will attempt to use tablebases
    // this will only work if tablebases have been initialised before
    bool useTB = false;
    // printInfo specifies if uci strings shall be displayed or not
    bool printInfo = true;

    public:
    // initialise the search including the transposition table
    void init(int hashsize);
    // cleans up the search class. Can be moved to the destructor in the future
    void cleanUp();

    private:
    // functions internally used to compute node counts, seldepth and tbhits across all threads
    [[nodiscard]] bb::U64 totalNodes() const;
    [[nodiscard]] int     selDepth() const;
    [[nodiscard]] bb::U64 tbHits() const;

    // function to compute get all the legal moves for the board
    [[nodiscard]] move::MoveList legals(Board* board) const;

    public:
    // returns the overview of the latest search
    [[nodiscard]] SearchOverview overview() const;
    // beside storing each thread, we need to also track the data per thread
    std::vector<ThreadData> tds;
    // enable / disable info strings
    void enableInfoStrings();
    void disableInfoStrings();

    // enable tablebase usage for the search
    void useTableBase(bool val);
    // clears history tables
    void clearHistory();
    // clears transposition table
    void clearHash();
    // sets threads to be used for smp
    void setThreads(int threads);
    // set the hash size for the transposition table
    void setHashSize(int hashSize);
    // sets the amount of lines to analyse
    void setMultiPv(int multiPvCount);
    
    // stops the search
    void stop();
    void resetTd();

    void printInfoString(bb::Depth depth, int sel_depth, bb::Score score, move::Move* pv, uint16_t pvLen,int pvIdx);

    // basic move functions
    move::Move               bestMove(Board* b, TimeManager* timeManager, int threadId = 0);
    [[nodiscard]] bb::Score  pvSearch(Board* b, bb::Score alpha, bb::Score beta, bb::Depth depth,
                                      bb::Depth ply, ThreadData* sd, move::Move skipMove,
                                      int behindNMP, bb::Depth* lmrFactor = nullptr);
    [[nodiscard]] bb::Score  qSearch(Board* b, bb::Score alpha, bb::Score beta, bb::Depth ply,
                                     ThreadData* sd, bool inCheck = false);
    [[nodiscard]] bb::Score  probeWDL(Board* board);
    [[nodiscard]] move::Move probeDTZ(Board* board);
};

extern int RAZOR_MARGIN;
extern int FUTILITY_MARGIN;
extern int SE_MARGIN_STATIC;
extern int LMR_DIV;

void       initLMR();

#endif    // KOIVISTO_SEARCH_H