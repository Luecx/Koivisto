
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

#define MAX_THREADS 256

namespace search {

extern int                 threadCount;

extern int                 RAZOR_MARGIN;
extern int                 FUTILITY_MARGIN;
extern int                 SE_MARGIN_STATIC;
extern int                 LMR_DIV;
extern TranspositionTable* table;

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

void           stop();
void           setHashSize(int hashSize);
void           setThreads(int threads);
void           clearHash();
void           clearHistory();
void           useTB(bool val);
void           init(int hashSize);    // used to create arrays, movelists etc
void           initLMR();            // init lmr array. Used when clop tuning etc.
void           cleanUp();            // used to clean up the memory
SearchOverview overview();           // used to get information about the latest search
void           enable_infoStrings();
void           disable_infoStrings();

Move           bestMove(Board* b, Depth maxDepth, TimeManager* timeManager, int threadId = 0);
Score          pvSearch(Board* b, Score alpha, Score beta, Depth depth, Depth ply, ThreadData* sd,
                        Move skipMove, int behindNMP, Depth* lmrFactor = nullptr);
Score qSearch(Board* b, Score alpha, Score beta, Depth ply, ThreadData* sd, bool inCheck = false);

}    // namespace search
#endif    // KOIVISTO_SEARCH_H
