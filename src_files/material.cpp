
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

#include "material.h"

#include "Board.h"
#include "UCIAssert.h"

Material::Material() {}
Material::Material(const Material& other) {
    this->kingSquare[WHITE] = other.kingSquare[WHITE];
    this->kingSquare[BLACK] = other.kingSquare[BLACK];
    this->materialScore     = other.materialScore;
}

void Material::reset(Board* board) {
    materialScore     = 0;
    kingSquare[WHITE] = bitscanForward(board->getPieceBB<WHITE>(KING));
    kingSquare[BLACK] = bitscanForward(board->getPieceBB<BLACK>(KING));

    for (Color c : {WHITE, BLACK}) {
        for (PieceType t = PAWN; t <= KING; t++) {
            U64 bb = board->getPieceBB(c, t);
            while (bb) {
                Square s = bitscanForward(bb);
                setPiece(c, t, s);
                bb = lsbReset(bb);
            }
        }
    }
}

void Material::setPiece(Color c, PieceType t, Square s) { setPiece(getPiece(c, t), s); }

void Material::setPiece(Piece p, Square s) {
    UCI_ASSERT(s < 64);
    materialScore += piece_kk_square_tables[kingSquare[WHITE]][kingSquare[BLACK]][p][s];
}

void Material::unsetPiece(Color c, PieceType t, Square s) { unsetPiece(getPiece(c, t), s); }

void Material::unsetPiece(Piece p, Square s) {
    UCI_ASSERT(s < 64);
    materialScore -= piece_kk_square_tables[kingSquare[WHITE]][kingSquare[BLACK]][p][s];
}

Material& Material::operator=(const Material& other) {
    this->kingSquare[WHITE] = other.kingSquare[WHITE];
    this->kingSquare[BLACK] = other.kingSquare[BLACK];
    this->materialScore     = other.materialScore;
    return *this;
}

EvalScore Material::operator()() const { return materialScore; }
bool Material::operator==(const Material& rhs) const {
    return kingSquare[WHITE] == rhs.kingSquare[WHITE] && kingSquare[BLACK] == rhs.kingSquare[BLACK]
           && materialScore == rhs.materialScore;
}
bool Material::operator!=(const Material& rhs) const { return !(rhs == *this); }
std::ostream&  operator<<(std::ostream& os, const Material& material) {
    os << "Mat(wk=" << (int) material.kingSquare[WHITE] << ";bk=" << (int) material.kingSquare[BLACK] << ") = ("
       << MgScore(material.materialScore) << ", " << EgScore(material.materialScore) << ")";
    return os;
}
