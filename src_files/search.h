
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
    int        threadID = 0;
    bb::U64    nodes    = 0;
    int        seldepth = 0;
    int        tbhits   = 0;
    bool       dropOut  = false;
    int        pvIdx    = 0;
    SearchData searchData {};
    moveGen    generators[bb::MAX_INTERNAL_PLY] {};
    move::Move pv[bb::MAX_INTERNAL_PLY + 1][bb::MAX_INTERNAL_PLY + 1] {};
    uint16_t   pvLen[bb::MAX_INTERNAL_PLY + 1];
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
    int                      threadCount = 1;
    int                      multiPv = 1;
    TranspositionTable*      table;
    SearchOverview           searchOverview;

    TimeManager*             timeManager;
    std::vector<std::thread> runningThreads;
    bool                     useTB     = false;
    bool                     printInfo = true;

    std::vector<ThreadData>  tds;

    public:
    void init(int hashsize);
    void cleanUp();

    private:
    [[nodiscard]] bb::U64 totalNodes() const;
    [[nodiscard]] int     selDepth() const;
    [[nodiscard]] bb::U64 tbHits() const;

    public:
    [[nodiscard]] SearchOverview overview() const;
    // enable / disable info strings
    void enableInfoStrings();
    void disableInfoStrings();

    void useTableBase(bool val);
    void clearHistory();
    void clearHash();
    void setThreads(int threads);
    void setHashSize(int hashSize);
    void setMultiPv(int multiPvCount);
    void stop();

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