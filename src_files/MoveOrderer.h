
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

#ifndef KOIVISTO_MOVEORDERER_H
#define KOIVISTO_MOVEORDERER_H

#include "Board.h"
#include "History.h"
#include "Move.h"

class MoveOrderer {

    private:
    move::MoveList* moves;
    int             counter;

    public:
    bool skip;

    MoveOrderer();

    virtual ~MoveOrderer();

    void setMovesPVSearch(move::MoveList* p_moves, move::Move hashMove, SearchData* sd, Board* board, Depth ply);

    void setMovesQSearch(move::MoveList* p_moves, Board* b);

    bool hasNext();

    move::Move next();
};

#endif    // KOIVISTO_MOVEORDERER_H
