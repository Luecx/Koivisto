
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

#include "Bitboard.h"
#include "Board.h"
#include "History.h"
#include "MoveOrderer.h"
#include "TimeManager.h"
#include "TranspositionTable.h"
#include "eval.h"

#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <stdint.h>
#include <string>
#include <tgmath.h>
#include <thread>

#define MAX_THREADS 256

/**
 * used to store information about a search
 */
struct SearchOverview {
    int   nodes;
    Score score;
    int   depth;
    int   time;
    Move  move;
};

class Search {
    int                      threadCount = 1;
    TranspositionTable*      table;
    SearchOverview           searchOverview;

    TimeManager*             timeManager;
    std::vector<std::thread> runningThreads;
    bool                     useTB     = false;
    bool                     printInfo = true;

    ThreadData*              tds[MAX_THREADS] {};

    public:
//    Search(int hashsize = 16);
//    virtual ~Search();
    
    void init(int hashsize);
    void cleanUp();
    private:

    U64  totalNodes();
    int  selDepth();
    U64  tbHits();

    bool isTimeLeft();
    bool rootTimeLeft(int score);

    public:
    SearchOverview overview();
    void           enableInfoStrings();
    void           disableInfoStrings();

    void           useTableBase(bool val);
    void           clearHistory();
    void           clearHash();
    void           setThreads(int threads);
    void           setHashSize(int hashSize);
    void           stop();

    void           printInfoString(Board* b, Depth d, Score score);
    void           extractPV(Board* b, MoveList* mvList, Depth depth);
    
    Move           bestMove(Board* b, Depth maxDepth, TimeManager* timeManager, int threadId=0);
    Score          pvSearch(Board* b, Score alpha, Score beta, Depth depth, Depth ply, ThreadData* sd,
                            Move skipMove, int behindNMP, Depth* lmrFactor = nullptr);
    Score           qSearch(Board* b, Score alpha, Score beta, Depth ply, ThreadData* sd, bool inCheck = false);
    Score           qSearch(Board* b);
    Score           probeWDL(Board* board);
    Move            probeDTZ(Board* board);
};

extern int                 RAZOR_MARGIN;
extern int                 FUTILITY_MARGIN;
extern int                 SE_MARGIN_STATIC;
extern int                 LMR_DIV;

void initLMR();

#endif    // KOIVISTO_SEARCH_H
