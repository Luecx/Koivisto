
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

#include "attacks.h"
#include "uci.h"

using namespace move;
using namespace bb;

int main(int argc, char *argv[]) {
    uci::mainloop(argc, argv);
//    bb::init();
//    attacks::init();
//
//    Board b{"4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - -"};
//    Move m = genMove(H5,G4,CAPTURE, BLACK_PAWN, WHITE_PAWN);
//
//    std::cout << b.staticExchangeEvaluationAbove(m, 0) << std::endl;
//    std::cout << b.staticExchangeEvaluation(m) << std::endl;
    
    return 0;
}
