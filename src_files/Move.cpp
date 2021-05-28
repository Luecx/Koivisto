
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
#include "Move.h"

using namespace move;

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
    
    std::string res {};
    res.append(bb::SQUARE_IDENTIFIER[getSquareFrom(move)]);
    res.append(bb::SQUARE_IDENTIFIER[getSquareTo(move)]);
    if (isPromotion(move)) {
        char c = tolower(bb::PIECE_IDENTIFER[getPromotionPiece(move)]);
        res.push_back(c);
    }
    
    return res;
}
