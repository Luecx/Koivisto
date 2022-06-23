
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
#include "move.h"

using namespace move;

/**
 * prints the bits of the move.
 * Useful for debugging.
 * @param move
 */
void move::printMoveBits(Move move, bool bitInfo) {
    std::cout << " ";
    for (int i = 0; i < 8; i++) {
        std::cout << static_cast<bool>((move >> (31 - i)) & 1UL);
    }
    std::cout << " ";
    for (int i = 0; i < 4; i++) {
        std::cout << static_cast<bool>((move >> (23 - i)) & 1UL);
    }
    std::cout << " ";
    for (int i = 0; i < 4; i++) {
        std::cout << static_cast<bool>((move >> (19 - i)) & 1UL);
    }
    std::cout << " ";
    for (int i = 0; i < 4; i++) {
        std::cout << static_cast<bool>((move >> (15 - i)) & 1UL);
    }
    std::cout << " ";
    for (int i = 0; i < 6; i++) {
        std::cout << static_cast<bool>((move >> (11 - i)) & 1UL);
    }
    std::cout << " ";
    for (int i = 0; i < 6; i++) {
        std::cout << static_cast<bool>((move >> (5 - i)) & 1UL);
    }
    
    if (bitInfo) {
        std::cout << "\n";
        std::cout << "|        |    |    |    |      |      squareFrom\n"
                     "|        |    |    |    |      +--------------------------\n"
                     "|        |    |    |    |             squareTo\n"
                     "|        |    |    |    +---------------------------------\n"
                     "|        |    |    |                  moving piece\n"
                     "|        |    |    +--------------------------------------\n"
                     "|        |    |                       type information\n"
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
 * returns a string used for uci-output of the given move like: a2e3
 * Does process promotions.
 *
 * @param move
 * @return
 */
std::string move::toString(const Move& move) {
    std::string res;
    res.append(bb::SQUARE_IDENTIFIER[getSquareFrom(move)]);
    res.append(bb::SQUARE_IDENTIFIER[getSquareTo(move)]);
    if (isPromotion(move)) {
        char chr = tolower(bb::PIECE_IDENTIFER[getPromotionPiece(move)]);
        res.push_back(chr);
    }
    
    return res;
}

/**
 * swaps the move object at index i1 and index i2
 */
void MoveList::swap(int i1, int i2) {
    Move m1      = moves[i1];
    moves[i1]    = moves[i2];
    moves[i2]    = m1;
    
    MoveScore s1 = scores[i1];
    scores[i1]   = scores[i2];
    scores[i2]   = s1;
}

/**
 * returns the move stored at the given index
 * @param index
 * @return
 */
move::Move MoveList::getMove(int index) const {
    return moves[index];
}

/**
 * removes all moves
 */
void MoveList::clear() {
    size = 0;
}

/**
 * adds a move
 * @param move
 */
void MoveList::add(Move move) {
    moves[size++] = move;
}

/**
 * returns the amount of stored moves
 * @return
 */
int MoveList::getSize() const {
    return size;
}

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
MoveScore MoveList::getScore(int index) const {
    return scores[index];
}

/**
 * prints the bits of all the moves
 */
void MoveList::printMoveBits() const {
    for (int i = 0; i < this->size; i++) {
        move::printMoveBits(getMove(i), false);
    }
}

move::Move MoveList::next() {
    move::Move      best        = moves[0];
    move::MoveScore bestScore   = scores[0];
    int bestIndex = 0;
    for (int i = 1; i < size; i++) {
        if (scores[i] > bestScore) {
            bestIndex = i;
            best      = moves[i];
            bestScore = scores[i];
        }
    }
    size--;
    this->swap(bestIndex, size);
    return best;
}