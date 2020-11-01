
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
#include "Move.h"

using namespace move;

/**
 * generates a new non-capture by taking the fromSq, toSq, type and the moving piece.
 * For a List of types, look at Move.h.
 * @param from
 * @param to
 * @param type
 * @param movingPiece
 * @return
 */
Move move::genMove(const bb::Square from, const bb::Square to, const Type type, const bb::Piece movingPiece) {

    Move m {0};
    setSquareFrom(m, from);
    setSquareTo(m, to);
    setType(m, type);
    setMovingPiece(m, movingPiece);
    return m;
}

/**
 * generates a new capture by taking the fromSq, toSq, type and the moving piece.
 * For a List of types, look at Move.h.
 * @param from
 * @param to
 * @param type
 * @param movingPiece
 * @return
 */
Move move::genMove(const bb::Square from, const bb::Square to, const Type type, const bb::Piece movingPiece,
                   const bb::Piece capturedPiece) {

    Move m {0};

    setSquareFrom(m, from);
    setSquareTo(m, to);
    setType(m, type);
    setMovingPiece(m, movingPiece);
    setCapturedPiece(m, capturedPiece);

    return m;
}

/**
 * prints the bits of the move.
 * Useful for debugging.
 * @param move
 */
void move::printMoveBits(Move move, bool bitInfo) {
    std::cout << " ";
    for (int i = 0; i < 8; i++) {
        std::cout << (bool) ((move >> (31 - i)) & 1UL);
    }
    std::cout << " ";
    for (int i = 0; i < 4; i++) {
        std::cout << (bool) ((move >> (23 - i)) & 1UL);
    }
    std::cout << " ";
    for (int i = 0; i < 4; i++) {
        std::cout << (bool) ((move >> (19 - i)) & 1UL);
    }
    std::cout << " ";
    for (int i = 0; i < 4; i++) {
        std::cout << (bool) ((move >> (15 - i)) & 1UL);
    }
    std::cout << " ";
    for (int i = 0; i < 6; i++) {
        std::cout << (bool) ((move >> (11 - i)) & 1UL);
    }
    std::cout << " ";
    for (int i = 0; i < 6; i++) {
        std::cout << (bool) ((move >> (5 - i)) & 1UL);
    }

    if (bitInfo) {
        std::cout << "\n";
        std::cout << "|        |    |    |    |      |      squareFrom\n"
                     "|        |    |    |    |      +--------------------------\n"
                     "|        |    |    |    |             squareTo\n"
                     "|        |    |    |    +---------------------------------\n"
                     "|        |    |    |                  type information\n"
                     "|        |    |    +--------------------------------------\n"
                     "|        |    |                       moving piece\n"
                     "|        |    +-------------------------------------------\n"
                     "|        |                            captured piece\n"
                     "|        +------------------------------------------------\n"
                     "|                                     score information\n"
                     "+---------------------------------------------------------"
                  << std::endl;
    } else {
        std::cout << std::endl;
    }
}

/**
 * checks if the given move is a doubled pawn push
 * @param move
 * @return
 */
bool move::isDoubledPawnPush(const Move move) { return getType(move) == DOUBLED_PAWN_PUSH; }

/**
 * checks if the given move is a capture
 * @param move
 * @return
 */
bool move::isCapture(Move move) { return move & 0x4000; }

/**
 * checks if the given move castles
 * @param move
 * @return
 */
bool move::isCastle(Move move) {
    Type t = getType(move);
    return t == KING_CASTLE || t == QUEEN_CASTLE;
}

/**
 * checks if the given move captures e.p.
 * @param move
 * @return
 */
bool move::isEnPassant(Move move) { return getType(move) == EN_PASSANT; }

/**
 * checks if the given move is a promotion
 * @param move
 * @return
 */
bool move::isPromotion(Move move) { return move & 0x8000; }

/**
 * returns the piece that has been promoted to.
 * @param move
 * @return
 */
bb::Piece move::promotionPiece(Move move) { return ((move & 0x3000) >> SHIFT_TYPE) + getMovingPiece(move) + 1; }

/**
 * returns a string used for uci-output of the given move like: a2e3
 * Does process promotions.
 *
 * @param move
 * @return
 */
std::string move::toString(const Move move) {

    std::string res {};
    res.append(bb::SQUARE_IDENTIFIER[getSquareFrom(move)]);
    res.append(bb::SQUARE_IDENTIFIER[getSquareTo(move)]);
    if (isPromotion(move)) {
        char c = tolower(bb::PIECE_IDENTIFER[promotionPiece(move)]);
        res.push_back(c);
    }

    return res;
}

MoveList::~MoveList() {}

MoveList::MoveList() {}

/**
 * swaps the move object at index i1 and index i2
 */
void MoveList::swap(int i1, int i2) {
    Move m1   = moves[i1];
    moves[i1] = moves[i2];
    moves[i2] = m1;

    MoveScore s1 = scores[i1];
    scores[i1]   = scores[i2];
    scores[i2]   = s1;
}

/**
 * returns the move stored at the given index
 * @param index
 * @return
 */
move::Move MoveList::getMove(int index) { return moves[index]; }

/**
 * removes all moves
 */
void MoveList::clear() { size = 0; }

/**
 * adds a move
 * @param move
 */
void MoveList::add(Move move) { moves[size++] = move; }

/**
 * returns the amount of stored moves
 * @return
 */
int MoveList::getSize() const { return size; }

/**
 * assigns the score to the move at the given index
 */
void MoveList::scoreMove(int index, MoveScore score) {
    //    setScore(moves[index], score);
    scores[index] = score;
}

/**
 *
 */
MoveScore MoveList::getScore(int index) { return scores[index]; }

/**
 * prints the bits of all the moves
 */
void MoveList::printMoveBits() {
    for (int i = 0; i < this->size; i++) {
        ::move::printMoveBits(getMove(i), false);
    }
}
