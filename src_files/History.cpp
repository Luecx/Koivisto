
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                                   by. Kim Kahre and Finn Eggers                                  *
 *                                                                                                  *
 *                 Koivisto is free software: you can redistribute it and/or modify                 *
 *               it under the terms of the GNU General Public License as published by               *
 *                 the Free Software Foundation, either version 3 of the License, or                *
 *                                (at your option) any later version.                               *
 *                    Koivisto is distributed in the hope that it will be useful,                   *
 *                  but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
 *                   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                  *
 *                           GNU General Public License for more details.                           *
 *                 You should have received a copy of the GNU General Public License                *
 *                 along with Koivisto.  If not, see <http://www.gnu.org/licenses/>.                *
 *                                                                                                  *
 ****************************************************************************************************/
#include "History.h"

#define MAX_HISTORY_SCORE 512;

void SearchData::updateHistories(Move m, Depth depth, MoveList* mv, Move previous) {
    if (depth > 20)
        return;
    Move  m2;

    Color color = getMovingPieceColor(m);

    for (int i = 0; i < mv->getSize(); i++) {
        m2         = mv->getMove(i);

        int score  = mv->getScore(i);
        int scalar = score * score + 5 * score;

        if (sameMove(m, m2)) {
            if (isCapture(m)) {
                captureHistory[getSqToSqFromCombination(m)] +=
                    + scalar
                    - scalar * captureHistory[getSqToSqFromCombination(m)]
                          / MAX_HISTORY_SCORE;
            } else {
                history[getSqToSqFromCombination(m)] +=
                    + scalar
                    - scalar * history[getSqToSqFromCombination(m)]
                          / MAX_HISTORY_SCORE;
                cmh[getPieceTypeSqToCombination(previous)][getPieceTypeSqToCombination(m2)] +=
                    + scalar
                    - scalar * cmh[getPieceTypeSqToCombination(previous)][getPieceTypeSqToCombination(m2)]
                          / MAX_HISTORY_SCORE;
            }
        
            // we can return at this point because all moves searched are in front of this move
            return;
        } else if (isCapture(m2)) {
            captureHistory[getSqToSqFromCombination(m2)] +=
                - scalar
                - scalar * captureHistory[getSqToSqFromCombination(m2)]
                      / MAX_HISTORY_SCORE;
        } else if (!isCapture(m)) {
            history[getSqToSqFromCombination(m2)] +=
                - scalar
                - scalar * history[getSqToSqFromCombination(m2)]
                      / MAX_HISTORY_SCORE;
            cmh[getPieceTypeSqToCombination(previous)][getPieceTypeSqToCombination(m2)] +=
                - scalar
                - scalar * cmh[getPieceTypeSqToCombination(previous)][getPieceTypeSqToCombination(m2)]
                      / MAX_HISTORY_SCORE;
        }
    }
}

int SearchData::getHistories(Move m, Move previous) {
    if (isCapture(m)) {
        return captureHistory[getSqToSqFromCombination(m)];
    } else {
        return cmh[getPieceTypeSqToCombination(previous)][getPieceTypeSqToCombination(m)]
               + history[getSqToSqFromCombination(m)];
    }
}

/*
 * Set killer
 */
void SearchData::setKiller(Move move, Depth ply) {
    if (!sameMove(move, killer[ply][0])) {
        killer[ply][1] = killer[ply][0];
        killer[ply][0] = move;
    }
}

/*
 * Is killer?
 */
int SearchData::isKiller(Move move, Depth ply) {
    if (sameMove(move, killer[ply][0]))
        return 2;
    return sameMove(move, killer[ply][1]);
}

/*
 * Set historic eval
 */
void SearchData::setHistoricEval(Score ev, Depth ply) {
    eval[ply] = ev;
}

/*
 * Is improving
 */
bool SearchData::isImproving(Score ev, Depth ply) {
    if (ply >= 2) {
        return (ev > eval[ply - 2]);
    } else {
        return true;
    }
}
ThreadData::ThreadData(int threadId) : threadID(threadId) {
}
ThreadData::ThreadData() {
}