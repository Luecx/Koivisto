//
// Created by kimka on 6.6.2020.
//

#include "History.h"

#define MAX_HISTORY_SCORE 4096;

/*
 * Increment the history score of a move. Used when a cutoff occurs.
 */
void SearchData::addHistoryScore(Move m, Depth depth, MoveList *mv, bool side) {
    if (depth>20)return;
    Move m2;
    for (int i = 0; i<mv->getSize(); i++){
        m2 = mv->getMove(i);
        if (sameMove(m, m2)){
            history[side][getSquareFrom(m)][getSquareTo(m)] += (depth*depth+5*depth) - (depth*depth+5*depth) * history[side][getSquareFrom(m)][getSquareTo(m)]/MAX_HISTORY_SCORE;
        }else if (!(m2 >> 24)){
            history[side][getSquareFrom(m2)][getSquareTo(m2)] += -(depth*depth+5*depth) - (depth*depth+5*depth) * history[side][getSquareFrom(m2)][getSquareTo(m2)]/MAX_HISTORY_SCORE;
        }
    }
    return;
};

MoveScore SearchData::getHistoryMoveScore(Move m, bool side) {
    int score = history[side][getSquareFrom(m)][getSquareTo(m)]+MAX_HISTORY_SCORE;
    score = score/40;
    MoveScore ms = score;
    return ms;
}

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