
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

#include "MovePicker.h"
#include "search.h"
#include "uci.h"

using namespace std;
using namespace bb;
using namespace move;


int main(int argc, char *argv[]) {
    
//    bb::init();
//
//
//    Board b{"2kr3r/P2bq1pp/2np1n2/1pp1ppBb/1pP5/3P1NPB/4PP1P/RN1QR1K1 w Qk - 0 1"};
//    SearchData sd{};
//    MovePicker<PV_SEARCH> movePicker{&b, &sd, 10, 0};
//
//    while(movePicker.hasNext()){
//        Move m = movePicker.next();
//        std::cout << toString(m) << std::endl;
//    }

#ifndef TUNING
    if (argc == 1) {
        uci::mainloop(false);
    } else if (argc > 1 && strcmp(argv[1], "bench") == 0) {
        uci::mainloop(true);
    }
#else
using namespace tuning;

    bb_init();
    psqt_init();

    load_weights();

    load_positions("../resources/other/E12.33-1M-D12-Resolved.book", 10000000);
    load_positions("../resources/other/E12.41-1M-D12-Resolved.book", 10000000);
    load_positions("../resources/other/E12.46FRC-1250k-D12-1s-Resolved.book", 10000000);
    double K = compute_K(2.48617, 100, 1e-6);
    for(int i = 0; i < 10; i++){
        train(50, K, 0.001 * sqrt(positions.size()));
        display_params();
    }
#endif



    return 0;
}
