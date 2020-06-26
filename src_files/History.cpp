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

/*
 * Set killer
 */
void SearchData::setKiller(Move move, Depth ply, Color color) {
    killer[color][ply] = move;
}
/*
 * Is killer?
 */
bool SearchData::isKiller(Move move, Depth ply, Color color) {
    return sameMove(move, killer[color][ply]);
}

/*
 * Set historic eval
 */
void SearchData::setHistoricEval(Score ev, Color color, Depth ply) {
    eval[color][ply] = ev;
}
/*
 * Is improving
 */
bool SearchData::isImproving(Score ev, Color color, Depth ply) {
    if (ply>2){
        return (ev>eval[color][ply-2]);
    }else
    {
        return false;
    }
}