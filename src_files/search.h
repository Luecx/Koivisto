
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
#include "pv.h"
#include "newmovegen.h"

#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <cstdint>
#include <string>
#include <ctgmath>
#include <thread>
#include <vector>

#define MAX_THREADS 256



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
    
    SearchData searchData {};
    // move generators to not reallocate
    moveGen    generators[bb::MAX_INTERNAL_PLY] {};
    
    // pv information...
    PVTable pvTable{};
    
//    // we use a triangular pv table to track the pv during search for each thread
//    move::Move pv[bb::MAX_INTERNAL_PLY + 1][bb::MAX_INTERNAL_PLY + 1] {};
//    // we also need to track the partial pv length of each subtree
//    uint16_t   pvLen[bb::MAX_INTERNAL_PLY + 1];

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
    int                      threadCount = 1;
    TranspositionTable*      table;
    SearchOverview           searchOverview;

    TimeManager*             timeManager;
    std::vector<std::thread> runningThreads;
    // beside storing each thread, we need to also track the data per thread
    std::vector<ThreadData> tds;
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
    void stop();

    void printInfoString(bb::Depth depth, bb::Score score, PVLine& pvLine);

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