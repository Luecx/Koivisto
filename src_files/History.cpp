//
// Created by kimka on 6.6.2020.
//

#include "History.h"


/*
 * Increment the history score of a move. Used when a cutoff occurs.
 */
void SearchData::addHistoryScore(Square from, Square to, Depth depth) {
    if (depth>20)return;
    history[from][to] -= (depth*depth+5*depth) * history[from][to]/512;
    history[from][to] += (depth*depth+5*depth);
    return;
};
/*
 * Decrement the history score of a move. used when a cutoff doesnt occur.
 */
void SearchData::subtractHistoryScore(Square from, Square to, Depth depth) {
    if (depth>20)return;
    history[from][to] -= (depth*depth+5*depth) * history[from][to]/512;
    return;
};