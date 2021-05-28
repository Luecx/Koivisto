//
// Created by Luecx on 28.05.2021.
//

#ifndef KOIVISTO_MOVELIST_H
#define KOIVISTO_MOVELIST_H

#include "Move.h"
namespace move {

class MoveList {

    private:
    move::Move      moves[256];
    move::MoveScore scores[256];
    int             size;
    int             searchedMoves = 0;

    public:
    void       swap(int i1, int i2);
    move::Move getMove(int index);
    void       clear();
    void       add(move::Move move);
    void       add(move::Move move, move::MoveScore moveScore);
    void       scoreMove(int index, MoveScore score);
    MoveScore  getScore(int index);
    void       printMoveBits();
    int        getSize() const;
    int        movesLeft() const;
    move::Move pollBest();
    move::Move pollNext();
};
}    // namespace move

#endif    // KOIVISTO_MOVELIST_H
