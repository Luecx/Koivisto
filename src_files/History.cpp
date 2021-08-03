
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

int SearchData::combineHistory(Depth depth, bool side, Move m) {
    return depth * 2 > maxDepth ? slowHistory[side][getSqToSqFromCombination(m)][0] * 512 / (1 + slowHistory[side][getSqToSqFromCombination(m)][1]) : history[side][getSqToSqFromCombination(m)];
}

void SearchData::updateHistories(Move m, Depth depth, MoveList* mv, Color side, Move previous) {
    Move  m2;

    Color color = getMovingPieceColor(m);

    for (int i = 0; i < mv->getSize(); i++) {
        m2         = mv->getMove(i);

        int score  = mv->getScore(i);
        if (score > 18) score = 18;
        int scalar = score * score + 5 * score;
        int fastScalar = score * score + 5 * score;
        int slowScalar  = score * score * score / 12;
        if (!isCapture(m2)) slowHistory[side][getSqToSqFromCombination(m)][1] += score * score + 5 * score;
        if (sameMove(m, m2)) {
            if (isCapture(m)) {
                captureHistory[side][getSqToSqFromCombination(m)] +=
                    + scalar
                    - scalar * captureHistory[side][getSqToSqFromCombination(m)]
                          / MAX_HISTORY_SCORE;
            } else {
                history[side][getSqToSqFromCombination(m)] +=
                    + fastScalar
                    - fastScalar * history[side][getSqToSqFromCombination(m)]
                          / MAX_HISTORY_SCORE;
                slowHistory[side][getSqToSqFromCombination(m)][0] +=
                    + slowScalar;
                cmh[getPieceTypeSqToCombination(previous)][color][getPieceTypeSqToCombination(m2)] +=
                    + scalar
                    - scalar * cmh[getPieceTypeSqToCombination(previous)][color][getPieceTypeSqToCombination(m2)]
                          / MAX_HISTORY_SCORE;
            }
        
            // we can return at this point because all moves searched are in front of this move
            return;
        } else if (isCapture(m2)) {
            captureHistory[side][getSqToSqFromCombination(m2)] +=
                - scalar
                - scalar * captureHistory[side][getSqToSqFromCombination(m2)]
                      / MAX_HISTORY_SCORE;
        } else if (!isCapture(m)) {
            history[side][getSqToSqFromCombination(m2)] +=
                - fastScalar
                - fastScalar * history[side][getSqToSqFromCombination(m2)]
                      / MAX_HISTORY_SCORE;
            slowHistory[side][getSqToSqFromCombination(m)][0] +=
                - slowScalar;
            cmh[getPieceTypeSqToCombination(previous)][color][getPieceTypeSqToCombination(m2)] +=
                - scalar
                - scalar * cmh[getPieceTypeSqToCombination(previous)][color][getPieceTypeSqToCombination(m2)]
                      / MAX_HISTORY_SCORE;
        }
    }
}

int SearchData::getHistories(Move m, Color side, Move previous, Depth depth) {
    if (isCapture(m)) {
        return captureHistory[side][getSqToSqFromCombination(m)];
    } else {
        return cmh[getPieceTypeSqToCombination(previous)][side][getPieceTypeSqToCombination(m)]
               + combineHistory(depth, side, m);
    }
}

/*
 * Set killer
 */
void SearchData::setKiller(Move move, Depth ply, Color color) {
    if (!sameMove(move, killer[color][ply][0])) {
        killer[color][ply][1] = killer[color][ply][0];
        killer[color][ply][0] = move;
    }
}

/*
 * Is killer?
 */
int SearchData::isKiller(Move move, Depth ply, Color color) {
    if (sameMove(move, killer[color][ply][0]))
        return 2;
    return sameMove(move, killer[color][ply][1]);
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
    if (ply >= 2) {
        return (ev > eval[color][ply - 2]);
    } else {
        return true;
    }
}
ThreadData::ThreadData(int threadId) : threadID(threadId) {
}
ThreadData::ThreadData() {
}