//
// Created by Luecx on 28.05.2021.
//

#include "MoveList.h"

/**
 * swaps the move object at index i1 and index i2
 */
void move::MoveList::swap(int i1, int i2) {
    Move m1      = moves[i1];
    moves[i1]    = moves[i2];
    moves[i2]    = m1;

    MoveScore s1 = scores[i1];
    scores[i1]   = scores[i2];
    scores[i2]   = s1;
}

/**
 * returns the move stored at the given index
 * @param index
 * @return
 */
move::Move move::MoveList::getMove(int index) { return moves[index]; }

/**
 * removes all moves
 */
void       move::MoveList::clear() { size = 0; }

/**
 * adds a move
 * @param move
 */
void       move::MoveList::add(Move move) { moves[size++] = move; }

/**
 * adds a move
 * @param move
 */
void       move::MoveList::add(move::Move move, move::MoveScore moveScore) {
    scores[size]  = moveScore;
    moves[size++] = move;
}

/**
 * returns the amount of stored moves
 * @return
 */
int  move::MoveList::getSize() const { return size; }

/**
 * assigns the score to the move at the given index
 */
void move::MoveList::scoreMove(int index, MoveScore score) {
    //    setScore(moves[index], score);
    scores[index] = score;
}

/**
 *
 */
move::MoveScore move::MoveList::getScore(int index) { return scores[index]; }

/**
 * prints the bits of all the moves
 */
void            move::MoveList::printMoveBits() {
    for (int i = 0; i < this->size; i++) {
        move::printMoveBits(getMove(i), false);
    }
}

int        move::MoveList::movesLeft() const { return getSize() - searchedMoves; }

move::Move move::MoveList::pollBest() {

    int bestIndex = searchedMoves;
    for (int i = searchedMoves + 1; i < getSize(); i++) {
        if (scores[i] > scores[bestIndex]) {
            bestIndex = i;
        }
    }
    scores[bestIndex] = 0;
    swap(bestIndex, searchedMoves);
    return moves[searchedMoves++];
}

move::Move move::MoveList::pollNext() {
    scores[searchedMoves] = 0;
    return moves[searchedMoves++];
}
