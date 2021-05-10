
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

#ifndef KOIVISTO_MATERIAL_H
#define KOIVISTO_MATERIAL_H

#include "psqt.h"

#include <ostream>

class Board;

struct Material {

    Square    kingSquare[N_COLORS] {};

    EvalScore materialScore {};

    Material();

    Material(const Material& other);

    void                 reset(Board* board);

    void                 setPiece(Color c, PieceType t, Square s);

    void                 setPiece(Piece p, Square s);

    void                 unsetPiece(Color c, PieceType t, Square s);

    void                 unsetPiece(Piece p, Square s);

    bool                 operator==(const Material& rhs) const;

    bool                 operator!=(const Material& rhs) const;

    friend std::ostream& operator<<(std::ostream& os, const Material& material);

    Material&            operator=(const Material& other);

    EvalScore            operator()() const;
};

#endif    // KOIVISTO_MATERIAL_H
