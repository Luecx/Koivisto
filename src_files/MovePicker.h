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

#ifndef CHESSCOMPUTER_MOVEPICKER_H
#define CHESSCOMPUTER_MOVEPICKER_H

#include "Move.h"
#include "History.h"
#include "Board.h"
// Stages. If there is a hashMove in pvsearch, we try it without generating all moves. 
// Only then do we generate captures and after that quiet moves.
#define HASH_MOVE           0
#define GENERATE_CAPTURE    1
#define GOOD_CAPTURE        2
#define GENERATE_SILENT     3
#define SILENT              4
#define BAD_CAPTURE         5

// Principal variation vs qsearch. we skip SILENT in in qsearch
#define PVSEARCH            0
#define QSEARCH             1

struct MovePicker {

    public:

    // Which state we are in in staged move generation
    int stage;

    // Which type we are working on
    int type = PVSEARCH;

    //In case we had a hashMove we need to know what it was so we can exclude it from other movelists
    move::Move hash_move;

    // Once a move is tried it is added to the tried list. The last added move is always the move that caused the cutoff
    move::Move tried [256]  = {0};
    // Tells us at which index we currently are
    int tried_index         = 0;

    // Eque for moves that we are going to try. For example if we generate quiets we put them at index 100, 101, 102... etc. 
    // Capture index start at 0.
    move::Move eque [256]   = {0};
    // Tells us at which index we currently are in quiet move portion (100-255)
    int quiet_eque_index    = 0;
    // Tells us at which index we currently are in capture move portion (0-100)
    int capture_eque_index  = 0;

    // Movescoring scores for moves in eque
    move::MoveScore scores [256] = {0};

    // Kind of init movePicker. Actually just sets hashMove if there is one and resets index variabels. 
    void init(move::Move hashMove = 0);

    // Get next move in pv/qsearch
    move::Move nextMove(Board* b, SearchData* sd, Depth ply = 0);

    // Add move to tried (used for updating history stats)
    void addMoveToTried(move::Move m);

    // Add quiet move to eque 
    void addQuietMoveToEque(move::Move m);

    // Add move capture move to eque
    void addCaptureMoveToEque(move::Move m);

    // Score equed moves as captures
    void scoreCaptures(Board* b, SearchData* sd);

    // Score equed moves as silent moves
    void scoreQuiets(Board* b, SearchData* sd, Depth ply);

    // Pick best capture according to scores
    move::Move pickCapture(Board* b, SearchData* sd);

    // Pick best quiet move according to scores
    move::Move pickQuiet(Board* b, SearchData* sd);

    // Adds all silent moves to MovePicker moveList
    void getSilentMoves(Board* b);

    // Get all capture moves
    void getCaptureMoves(Board* b);

};

#endif //CHESSCOMPUTER_MOVEPICKER_H