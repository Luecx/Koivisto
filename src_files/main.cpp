
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

#include "Bitboard.h"
#include "Board.h"
#include "Move.h"
#include "MoveOrderer.h"
#include "Verification.h"
#include "uci.h"

#include <iomanip>
#include "gradient.h"
#include "movegen.h"

using namespace std;
using namespace bb;
using namespace move;


int main(int argc, char *argv[]) {

#ifndef TUNING
    if (argc == 1) {
        uci_loop(false);
    } else if (argc > 1 && strcmp(argv[1], "bench") == 0) {
        uci_loop(true);
    }
#else
using namespace tuning;

    bb_init();
    psqt_init();

    load_weights();

    load_positions("../resources/make_fens_from_lichess_pgn/output/0.epd", 25000000);
    load_positions("../resources/make_fens_from_lichess_pgn/output/1.epd", 25000000);
    double K = compute_K(1.9, 200, 1e-7);
    for(int i = 0; i < 10; i++){
        train(50, K, 0.001 * sqrt(positions.size()));
        display_params();
    }
#endif



    return 0;
}
