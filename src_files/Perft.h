//
// Created by finne on 5/16/2020.
//

#ifndef CHESSCOMPUTER_PERFT_H
#define CHESSCOMPUTER_PERFT_H


#include "Board.h"
#include "TranspositionTable.h"

U64 perft(Board &b, int depth, bool print=true, bool d1=true, int ply=0);

void printRes();

#endif //CHESSCOMPUTER_PERFT_H
