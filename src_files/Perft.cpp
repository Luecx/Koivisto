
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
#include "Perft.h"

#include "MovePicker.h"
#include "UCIAssert.h"
#include "movegen.h"

using namespace std;

TranspositionTable* perft_tt;

/**
 * called at the start of the program
 * @param hash
 */
void perft_init(bool hash) {
    if (hash)
        perft_tt = new TranspositionTable(512);
    
}

/**
 * called at the end of the program.
 */
void perft_cleanUp() {

    if (perft_tt != nullptr)
        delete perft_tt;

}

/**
 * does nothing yet.
 * Supposed to print an overview of the previous perft call.
 */
void perft_res() {}

/**
 * runs a performance test on the given board to the specified depth.
 *
 * @param b the board to use
 * @param depth to depth the reach
 * @param print determines if it should print the moves at the root
 * @param d1 bulk counting
 * @param hash use hash
 * @param ply internally for the ply
 * @return
 */
U64 perft(Board* b, int depth, bool print, bool d1, bool hash, int ply) {
    UCI_ASSERT(b);

    U64 zob = ZERO;
    if (hash) {

        if (ply == 0) {
            perft_tt->clear();
        }

        zob      = b->zobrist();
        Entry en = perft_tt->get(zob);
        if (en.depth == depth && en.zobrist == zob) {
            return en.move;
        }
    }

    int i;
    U64 nodes = 0;

    if (depth == 0)
        return 1;

    MovePicker<PERFT> movePicker{b};
    
    while(movePicker.hasNext()){

        Move m = movePicker.next();
        if(m == 0) continue;

        if (!b->isLegal(m)) {
            continue;
        }

        if (d1 && depth == 1) {
            nodes += 1;
        } else {
            
            b->move(m);

            U64 np = perft(b, depth - 1, false, d1, hash, ply + 1);

            if (print) {
                std::cout << move::toString(m) << " " << np << std::endl;
            }

            nodes += np;
            b->undoMove();
        }
    }

    if (hash) {
        perft_tt->put(zob, 0, nodes, 0, depth);
    }

    return nodes;
}