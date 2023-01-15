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

#ifndef KOIVISTO_PV_H
#define KOIVISTO_PV_H

#include "move.h"

/**
 * PVLine struct represents a principal variation line.
 * This struct is used to store a sequence of moves that are expected to be the best
 * move at each ply.
 */
struct PVLine {
    // pv is an array that stores the sequence of moves that form the principal variation
    move::Move pv[bb::MAX_INTERNAL_PLY + 1];
    
    // length is the number of moves in the principal variation
    uint16_t length;

    // operator () allows the struct to be accessed like an array,
    // so we can use pvLine(depth) to access the move at a specific depth
    move::Move& operator()(bb::Depth depth);
    
    // const version of the above operator
    move::Move operator()(bb::Depth depth) const;

} __attribute__((aligned(128)));

/**
 * PVTable struct represents a table of principal variations.
 * This struct is used to store multiple PVLine structs, one for each depth.
 */
struct PVTable {
    // pvs is an array of PVLine structs, one for each depth
    PVLine pvs[bb::MAX_INTERNAL_PLY + 1];

    // operator () allows the struct to be accessed like an array,
    // so we can use pvTable(depth) to access the PVLine at a specific depth
    PVLine& operator()(bb::Depth depth);
    
    // const version of the above operator
    PVLine operator()(bb::Depth depth) const;

    // reset all the lengths of PVLine structs to 0
    void reset();

    // updatePV updates the PVLine at a specific ply with the given move.
    // The move is added to the beginning of the PVLine and the PVLine at the next depth is appended
    // to it.
    void updatePV(bb::Depth ply, move::Move m);
};

#endif    // KOIVISTO_PV_H
