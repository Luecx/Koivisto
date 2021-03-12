//
// Created by Luecx on 12.03.2021.
//

#ifndef KOIVISTO_MOVEGEN_H
#define KOIVISTO_MOVEGEN_H

#include "Board.h"
#include "History.h"
#include "Move.h"

void generateMoves          (Board* b, MoveList* mv, Move hashMove = 0, SearchData* sd = nullptr, Depth ply = 0);
void generateNonQuietMoves  (Board* b, MoveList* mv, Move hashMove = 0, SearchData* sd = nullptr, Depth ply = 0);
void generatePerftMoves     (Board* b, MoveList* mv);


#endif    // KOIVISTO_MOVEGEN_H
