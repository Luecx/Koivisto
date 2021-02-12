
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

#include "Bitboard.h"
#include "Board.h"
#include "Move.h"
#include "MoveOrderer.h"
#include "Verification.h"
#include "uci.h"

#include <iomanip>
#include "gradient.h"

using namespace std;
using namespace bb;
using namespace move;


std::vector<Board> boards{};

void load_positions(const std::string &path, int count, int start=0) {

    boards.reserve(count);
    fstream newfile;
    newfile.open(path, ios::in);
    Evaluator evaluator{};
    if (newfile.is_open()) {
        string tp;
        int lineCount = 0;
        int posCount = 0;
        while (getline(newfile, tp)) {

            if (lineCount < start) {
                lineCount++;
                continue;
            }

            // finding the first "c" to check where the fen ended
            auto firstC = tp.find_first_of('c');
            auto lastC = tp.find_last_of('c');
            if (firstC == string::npos || lastC == string::npos) {
                continue;
            }

            // extracting the fen and result and removing bad characters.
            string fen = tp.substr(0, firstC);
            string res = tp.substr(lastC + 2, string::npos);

            fen = trim(fen);
            res = findAndReplaceAll(res, "\"", "");
            res = findAndReplaceAll(res, ";", "");
            res = trim(res);

            boards.emplace_back(Board{fen});

            lineCount++;
            posCount++;

            if (posCount % 10000 == 0) {

                std::cout << "\r" << loadingBar(posCount, count, "Loading data") << std::flush;
            }

            if (posCount >= count)
                break;
        }

        std::cout << std::endl;
        newfile.close();
    }
}

int main(int argc, char *argv[]) {





    if (argc == 1) {
        uci_loop(false);
    } else if (argc > 1 && strcmp(argv[1], "bench") == 0) {
        uci_loop(true);
    }

//
//using namespace tuning;
//
//    bb_init();
//    psqt_init();
////
////    load_weights();
////
//    Evaluator ev{};
//    load_positions("../resources/E12.33-1M-D12-Resolved.book", 1000000);
//    double total_elapsed = 0;
//    for(int k = 0; k < 10; k++){
//        Score v = 0;
//        startMeasure();
//        for(int i = 0; i < boards.size(); i++){
//            v += ev.evaluate(&(boards[i]));
//        }
//        double elapsed = stopMeasure();
//        total_elapsed += elapsed;
//        printf("#%-5d time= %-5.0f eps= %-8.0f sum= %-6d\n", k, elapsed, boards.size() * 1000.0 / elapsed, v);
//    }
//    printf("#TOTAL time= %-5.0f eps= %-8.0f\n", total_elapsed, boards.size() * 10 * 1000.0 / total_elapsed);


//    load_positions("../resources/E12.41-1M-D12-Resolved.book", 10000000);
//    load_positions("../resources/E12.46FRC-1250k-D12-1s-Resolved.book", 10000000);
//
//    compute_K(2.48617, 100, 1e-7);
//    for(int i = 0; i < 1; i++){
//        train(50, 2.48172, 0.001 * sqrt(1000000));
//        display_params();
//    }




    return 0;
}
