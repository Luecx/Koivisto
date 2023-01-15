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

//
// Created by Luecx on 15.01.2023.
//

#include "pv.h"
move::Move& PVLine::operator()(bb::Depth depth) {
    // This operator overload allows the struct to be accessed like an array,
    // so we can use pvLine(depth) to access the move at a specific depth
    return pv[depth];
}

move::Move PVLine::operator()(bb::Depth depth) const {
    // This is the const version of the above operator
    return pv[depth];
}

PVLine& PVTable::operator()(bb::Depth depth) {
    // This operator overload allows the struct to be accessed like an array,
    // so we can use pvTable(depth) to access the PVLine at a specific depth
    return pvs[depth];
}

PVLine PVTable::operator()(bb::Depth depth) const {
    // This is the const version of the above operator
    return pvs[depth];
}

void PVTable::reset() {
    // reset all the lengths of PVLine structs to 0
    for (auto &pvLine : pvs) {
        pvLine.length = 0;
    }
}

void PVTable::updatePV(bb::Depth ply, move::Move m) {
    // Add the move to the beginning of the PVLine at the current depth
    pvs[ply](0) = m;
    // Copy the PVLine at the next depth to the current depth
    memcpy(&pvs[ply](1), &pvs[ply + 1](0), sizeof(move::Move) * pvs[ply + 1].length);
    // Update the length of the PVLine at the current depth
    pvs[ply].length = pvs[ply + 1].length + 1;
}
