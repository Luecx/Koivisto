//
// Created by finne on 5/30/2020.
//

#ifndef KOIVISTO_SEARCH_H
#define KOIVISTO_SEARCH_H

#include <stdint.h>
#include <tgmath.h>
#include <cassert>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <string>
#include "Bitboard.h"
#include "Board.h"
//#include "Move.h"
#include "History.h"
#include "MoveOrderer.h"
#include "TimeManager.h"
#include "TranspositionTable.h"
#include "eval.h"

extern int threadCount;

extern int RAZOR_MARGIN;
extern int FUTILITY_MARGIN;
extern int SE_MARGIN_STATIC;
extern int LMR_DIV;

#define MAX_THREADS 32

/**
 * used to store information about a search
 */
struct SearchOverview {
    int nodes;
    Score score;
    int depth;
    int time;
    Move move;
};

void initLmr();
Score getWDL(Board *board);
void search_stop();
void search_setHashSize(int hashSize);
void search_clearHash();
void search_useTB(bool val);
void search_init(int hashSize);    // used to create arrays, movelists etc
void search_cleanUp();             // used to clean up the memory
SearchOverview search_overview();  // used to get information about the latest search
void search_enable_infoStrings();
void search_disable_infoStrings();

Move bestMove(Board *b, Depth maxDepth, TimeManager *timeManager, int threadId = 0);
Score pvSearch(Board *b,
               Score alpha,
               Score beta,
               Depth depth,
               Depth ply,
               ThreadData *sd,
               Move skipMove);
Score qSearch(Board *b, Score alpha, Score beta, Depth ply, ThreadData *sd);

#endif  // KOIVISTO_SEARCH_H
