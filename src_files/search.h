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



constexpr Depth ONE_PLY = 1;
constexpr Depth MAX_PLY = 127;
constexpr Score MAX_MATE_SCORE = (Score)((1 << 15) - 1);
constexpr Score MIN_MATE_SCORE = (Score)(1 << 14);

void search_setHashSize(int hashSize);
void search_init(int hashSize);     //used to create arrays, movelists etc
void search_cleanUp();              //used to clean up the memory

Move bestMove(Board *b, Depth maxDepth, int maxTime);
Score pvSearch(Board *b, Score alpha, Score beta, Depth depth, Depth ply, bool expectedCut);
Score  qSearch(Board *b, Score alpha, Score beta, Depth ply);



#endif //KOIVISTO_SEARCH_H
