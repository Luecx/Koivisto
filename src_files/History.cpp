
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                           by. Kim Kahre, Finn Eggers and Eugenio Bruno                           *
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

void SearchData::updateHistories(Move m, Depth depth, MoveList* mv, bool side, Move previous) {
    if (depth > 20)
        return;
    Move m2;
    
    Piece  prevPiece = getMovingPiece(previous) % 6;
    Square prevTo    = getSquareTo(previous);
    Color  color     = getMovingPiece(m) / 6;

    for (int i = 0; i < mv->getSize(); i++) {
        m2 = mv->getMove(i);
                
        Piece  movingPiece = getMovingPiece(m2) % 6;
        Square squareTo    = getSquareTo(m2);

        if (sameMove(m, m2)) {
            if (isCapture(m)){
                 captureHistory[side][getSquareFrom(m)][getSquareTo(m)] +=
                    (depth * depth + 5 * depth)
                    - (depth * depth + 5 * depth) * captureHistory[side][getSquareFrom(m)][getSquareTo(m)] / MAX_HISTORY_SCORE;
            } else {
                history[side][getSquareFrom(m)][getSquareTo(m)] +=
                    (depth * depth + 5 * depth)
                    - (depth * depth + 5 * depth) * history[side][getSquareFrom(m)][getSquareTo(m)] / MAX_HISTORY_SCORE;
                cmh[prevPiece][prevTo][color][movingPiece][squareTo] +=
                    (depth * depth + 5 * depth)
                    - (depth * depth + 5 * depth) * cmh[prevPiece][prevTo][color][movingPiece][squareTo] / MAX_HISTORY_SCORE;
            }

            // we can return at this point because all moves searched are in front of this move
            return;
        }
        if (!(isCapture(m)^isCapture(m2))) {
            if (isCapture(m2)) {
                captureHistory[side][getSquareFrom(m2)][getSquareTo(m2)] +=
                    -(depth * depth + 5 * depth)
                    - (depth * depth + 5 * depth) * captureHistory[side][getSquareFrom(m2)][getSquareTo(m2)] / MAX_HISTORY_SCORE;
            } else {
                history[side][getSquareFrom(m2)][getSquareTo(m2)] +=
                    -(depth * depth + 5 * depth)
                    - (depth * depth + 5 * depth) * history[side][getSquareFrom(m2)][getSquareTo(m2)] / MAX_HISTORY_SCORE;
                cmh[prevPiece][prevTo][color][movingPiece][squareTo] +=
                    -(depth * depth + 5 * depth)
                    - (depth * depth + 5 * depth) * cmh[prevPiece][prevTo][color][movingPiece][squareTo]
                        / MAX_HISTORY_SCORE;
            }
        }
    }
    return;
}

int SearchData::getHistories(Move m, bool side, Move previous){
    if (isCapture(m)){
        return captureHistory[side][getSquareFrom(m)][getSquareTo(m)];
    } else {
        Piece  prevPiece   = getMovingPiece(previous) % 6;
        Square prevTo      = getSquareTo(previous);
        Color  color       = getMovingPiece(m) / 6;
        Piece  movingPiece = getMovingPiece(m) % 6;
        Square squareTo    = getSquareTo(m);

        return cmh[prevPiece][prevTo][color][movingPiece][squareTo] + history[side][getSquareFrom(m)][getSquareTo(m)];
    }
}

/*
 * Set killer
 */
void SearchData::setKiller(Move move, Depth ply, Color color) { killer[color][ply] = move; }
/*
 * Is killer?
 */
bool SearchData::isKiller(Move move, Depth ply, Color color) { return sameMove(move, killer[color][ply]); }

/*
 * Set historic eval
 */
void SearchData::setHistoricEval(Score ev, Color color, Depth ply) { eval[color][ply] = ev; }
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

SearchData::~SearchData() {
    for (int i = 0; i < MAX_INTERNAL_PLY; i++) {
        delete moves[i];
    }
    delete[] moves;
}

SearchData::SearchData() {
    moves = new MoveList*[MAX_INTERNAL_PLY];
    for (int i = 0; i < MAX_INTERNAL_PLY; i++) {
        moves[i] = new MoveList();
    }
}

ThreadData::ThreadData(int threadId) : threadID(threadId) {}
