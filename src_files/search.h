//
// Created by finne on 5/30/2020.
//

#ifndef KOIVISTO_SEARCH_H
#define KOIVISTO_SEARCH_H

#include <iostream>
#include <cmath>
#include <tgmath.h>
#include <chrono>
#include <ctime>
#include <cassert>
#include <stdint.h>
#include <string>
#include "Bitboard.h"
#include "Board.h"
//#include "Move.h"
#include "TranspositionTable.h"
#include "eval.h"
#include "MoveOrderer.h"
#include "History.h"
#include "TimeManager.h"

extern int RAZOR_MARGIN;
extern int FUTILITY_MARGIN;
extern int SE_MARGIN_STATIC;
extern int LMR_DIV;


void initLmr();
Score getWDL(Board* board);
void search_stop();
void search_setHashSize(int hashSize);
void search_useTB(bool val);
void search_init(int hashSize);     //used to create arrays, movelists etc
void search_cleanUp();              //used to clean up the memory


Move bestMove(Board *b, Depth maxDepth, TimeManager* timeManager);
Score pvSearch(Board *b, Score alpha, Score beta, Depth depth, Depth ply, bool expectedCut, SearchData *sd, Move skipMove);
Score  qSearch(Board *b, Score alpha, Score beta, Depth ply);



#endif //KOIVISTO_SEARCH_H
