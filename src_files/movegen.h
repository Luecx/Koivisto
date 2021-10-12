
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

#ifndef KOIVISTO_MOVEGEN_H
#define KOIVISTO_MOVEGEN_H

#include "Board.h"
#include "History.h"
#include "Move.h"

void generateMoves          (Board* b, MoveList* mv, Move hashMove = 0, SearchData* sd = nullptr, Depth ply = 0);
void generateNonQuietMoves  (Board* b, MoveList* mv, Move hashMove = 0, SearchData* sd = nullptr, Depth ply = 0, bool inCheck = 0);
void generatePerftMoves     (Board* b, MoveList* mv);


#endif    // KOIVISTO_MOVEGEN_H
