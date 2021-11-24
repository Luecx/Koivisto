
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

int SearchData::getHistories(Move m, Color side, Move previous, Move followup, Square threatSquare) {
    if (isCapture(m)) {
        return captureHistory[side][getSqToSqFromCombination(m)];
    } else {
        return (2 * (followup != 0 ? fmh[getPieceTypeSqToCombination(followup)][side][getPieceTypeSqToCombination(m)] : 0)
               + 2 * cmh[getPieceTypeSqToCombination(previous)][side][getPieceTypeSqToCombination(m)]
               + 2 * th[side][threatSquare][getSqToSqFromCombination(m)]) / 3;
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