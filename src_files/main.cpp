
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
#include "Tuning.h"
#include "Verification.h"
#include "uci.h"

#include <iomanip>

using namespace std;
using namespace bb;
using namespace move;

void main_tune_features_bb() {
    bb_init();
    Evaluator* evaluator = new Evaluator();

    using namespace tuning;

    loadPositionFile("resources/other/quiet-labeled.epd", 1e7);
    auto K = tuning::computeK(evaluator, 2.86681, 200, 1e-7);

    // tune Phase specificly
    float* params     = evaluator->getPhaseValues();
    int    paramCount = 16;

    for (int i = 0; i < 5000; i++) {

        std::cout << "--------------------------------------------------- [" << i
                  << "] ----------------------------------------------" << std::endl;

        std::cout << std::setprecision(8) << tuning::optimiseBlackBox(evaluator, K, params, paramCount, 0.3)
                  << std::endl;

        for (int e = 0; e < paramCount; e++) {
            std::cout << std::setw(14) << evaluator->getPhaseValues()[e] << ",";
        }

        std::cout << std::endl;
    }

    delete evaluator;
    bb_cleanUp();
}

void main_tune_pst_bb(Piece piece) {
    eval_init();
    bb_init();
    Evaluator* evaluator = new Evaluator();

    using namespace tuning;

    loadPositionFile("../resources/other/tuningset_clear2.epd", 2500000);

    eval_init();
    auto K = tuning::computeK(evaluator, 2.86681, 200, 1e-7);

    int lr = 200;

    for (int i = 1; i < 5000; i++) {

        std::cout << "--------------------------------------------------- [" << i
                  << "] ----------------------------------------------" << std::endl;

        std::cout << tuning::optimisePSTBlackBox(evaluator, K, psqt[2], 64, lr) << std::endl;
        std::cout << tuning::optimisePSTBlackBox(evaluator, K, psqt[3], 64, lr) << std::endl;

        std::cout << tuning::optimisePSTBlackBox(evaluator, K, psqt[4], 64, lr) << std::endl;
        std::cout << tuning::optimisePSTBlackBox(evaluator, K, psqt[5], 64, lr) << std::endl;

        std::cout << tuning::optimisePSTBlackBox(evaluator, K, psqt[6], 64, lr) << std::endl;
        std::cout << tuning::optimisePSTBlackBox(evaluator, K, psqt[7], 64, lr) << std::endl;

        std::cout << tuning::optimisePSTBlackBox(evaluator, K, psqt[8], 64, lr) << std::endl;
        std::cout << tuning::optimisePSTBlackBox(evaluator, K, psqt[9], 64, lr) << std::endl;

        for (Square s = 0; s < 64; s++) {
            std::cout << "M(" << setw(5) << MgScore(psqt[2][s]) << "," << setw(5) << EgScore(psqt[2][s]) << "), ";
            if ((i + 1) % 8 == 0)
                std::cout << std::endl;
        }
        std::cout << std::endl;
        for (Square s = 0; s < 64; s++) {
            std::cout << "M(" << setw(5) << MgScore(psqt[3][s]) << "," << setw(5) << EgScore(psqt[3][s]) << "), ";
            if ((i + 1) % 8 == 0)
                std::cout << std::endl;
        }
        std::cout << std::endl;
        for (Square s = 0; s < 64; s++) {
            std::cout << "M(" << setw(5) << MgScore(psqt[4][s]) << "," << setw(5) << EgScore(psqt[4][s]) << "), ";
            if ((i + 1) % 8 == 0)
                std::cout << std::endl;
        }
        std::cout << std::endl;
        for (Square s = 0; s < 64; s++) {
            std::cout << "M(" << setw(5) << MgScore(psqt[5][s]) << "," << setw(5) << EgScore(psqt[5][s]) << "), ";
            if ((i + 1) % 8 == 0)
                std::cout << std::endl;
        }
        std::cout << std::endl;
        for (Square s = 0; s < 64; s++) {
            std::cout << "M(" << setw(5) << MgScore(psqt[6][s]) << "," << setw(5) << EgScore(psqt[6][s]) << "), ";
            if ((i + 1) % 8 == 0)
                std::cout << std::endl;
        }
        std::cout << std::endl;
        for (Square s = 0; s < 64; s++) {
            std::cout << "M(" << setw(5) << MgScore(psqt[7][s]) << "," << setw(5) << EgScore(psqt[7][s]) << "), ";
            if ((i + 1) % 8 == 0)
                std::cout << std::endl;
        }
        std::cout << std::endl;
        for (Square s = 0; s < 64; s++) {
            std::cout << "M(" << setw(5) << MgScore(psqt[8][s]) << "," << setw(5) << EgScore(psqt[8][s]) << "), ";
            if ((i + 1) % 8 == 0)
                std::cout << std::endl;
        }
        std::cout << std::endl;
        for (Square s = 0; s < 64; s++) {
            std::cout << "M(" << setw(5) << MgScore(psqt[9][s]) << "," << setw(5) << EgScore(psqt[9][s]) << "), ";
            if ((i + 1) % 8 == 0)
                std::cout << std::endl;
        }
        std::cout << std::endl;

        if (i % 3 == 0 && lr > 1) {
            lr /= 2;
            std::cout << "Lr Dropped to " << lr;
            std::cout << std::endl;
        }
    }

    delete evaluator;
    bb_cleanUp();
}

void main_tune_features() {
    eval_init();
    bb_init();
    Evaluator* evaluator = new Evaluator();

    using namespace tuning;

    loadPositionFile("../resources/other/quiet-labeled2.epd", 2500000);
    auto K = tuning::computeK(evaluator, 2.86681, 200, 1e-7);

    for (int i = 0; i < 5000; i++) {

        std::cout << "--------------------------------------------------- [" << i
                  << "] ----------------------------------------------" << std::endl;

        //std::cout << tuning::optimisePSTBlackBox(evaluator, K, pieceScores, 6, 1) << std::endl;
        //std::cout << tuning::optimisePSTBlackBox(evaluator, K, &evfeatures[21], 2, 1) << std::endl;
        //std::cout << tuning::optimisePSTBlackBox(evaluator, K, pinnedEval, 15, 1) << std::endl;
        //std::cout << tuning::optimisePSTBlackBox(evaluator, K, hangingEval, 5, 1) << std::endl;
        std::cout << tuning::optimisePSTBlackBox(evaluator, K, &bishop_pawn_same_color_table_o[0], 8, 1) << std::endl;
        std::cout << tuning::optimisePSTBlackBox(evaluator, K, &bishop_pawn_same_color_table_e[0], 8, 1) << std::endl;

        for (Square s = 0; s < 8; s++) {
            std::cout << "M(" << setw(5) << MgScore(bishop_pawn_same_color_table_o[s]) << "," << setw(5) << EgScore(bishop_pawn_same_color_table_o[s])
                      << "), ";
            std::cout << std::endl;
        }
        std::cout << std::endl;

        for (Square s = 0; s < 8; s++) {
            std::cout << "M(" << setw(5) << MgScore(bishop_pawn_same_color_table_e[s]) << "," << setw(5) << EgScore(bishop_pawn_same_color_table_e[s])
                      << "), ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
        /*for (Square s = 0; s < 23; s++) {
            std::cout << "M(" << setw(5) << MgScore(*evfeatures[s]) << "," << setw(5) << EgScore(*evfeatures[s])
                      << "), ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
*/
        /*for (Square s = 0; s < 15; s++) {
            std::cout << "M(" << setw(5) << MgScore(pinnedEval[s]) << "," << setw(5) << EgScore(pinnedEval[s]) << "), ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
        for (Square s = 0; s < 5; s++) {
            std::cout << "M(" << setw(5) << MgScore(hangingEval[s]) << "," << setw(5) << EgScore(hangingEval[s])
                      << "), ";
            std::cout << std::endl;
        }
        std::cout << std::endl;*/
    }

    delete evaluator;
    bb_cleanUp();
}


int main(int argc, char* argv[]) {

    if (argc == 1) {
        uci_loop(false);
    } else if (argc > 1 && strcmp(argv[1], "bench") == 0) {
        uci_loop(true);
    }
    

    /**********************************************************************************
     *                                  T U N I N G                                   *
     **********************************************************************************/

    //        bb_init();
    //    eval_init();
    //    tuning::loadPositionFile("../resources/other/quiet-labeled.epd", 1000000);
    //    std::cout << tuning::computeError(new Evaluator(), 3);
    //     for(int i = 0; i < 10; i++)
    //     tuning::evalSpeed();
    //     bb_cleanUp();

    // main_tune_pst_bb(PAWN);
    //    eval_init();
    //main_tune_features();
    // main_tune_pst();
    // main_tune_features_bb();

    return 0;
}
