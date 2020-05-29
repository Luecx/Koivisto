//
// Created by finne on 5/16/2020.
//

#ifndef CHESSCOMPUTER_PERFT_H
#define CHESSCOMPUTER_PERFT_H


#include "Board.h"
#include "TranspositionTable.h"



U64 perft(Board &b, int depth, bool print=true, bool d1=true, bool hash=false, int ply=0);

void perft_prepare(bool hash);

void perft_clean();

void perft_res();

#endif //CHESSCOMPUTER_PERFT_H
