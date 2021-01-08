
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

//void main_tune_features() {
//    psqt_init();
//    bb_init();
//    Evaluator* evaluator = new Evaluator();
//
//    using namespace tuning;
//
//    loadPositionFile("../resources/other/quiet-labeled2.epd", 10000000);
//    //loadPositionFile("../resources/other/E12.33-1M-D12-Resolved.book", 10000000);
//    //loadPositionFile("../resources/other/E12.41-1M-D12-Resolved.book", 10000000);
//    //loadPositionFile("../resources/other/E12.46FRC-1250k-D12-1s-Resolved.book", 10000000);
//    auto K = tuning::computeK(2.86681, 200, 1e-7, 16);
//
//    for (int i = 0; i < 100; i++) {
//
//        std::cout << "--------------------------------------------------- [" << i
//                  << "] ----------------------------------------------" << std::endl;
//
//        //        tuning::optimisePSTBlackBox(K, reinterpret_cast<EvalScore*>(piece_square_table[0][0]),64,10,3,16);
//        //        tuning::optimisePSTBlackBox(K, reinterpret_cast<EvalScore*>(piece_square_table[0][1]),64,10,3,16);
//        //        tuning::optimisePSTBlackBox(K,
//        //        reinterpret_cast<EvalScore*>(piece_our_king_square_table[0]),225,10,3,16);
//        tuning::optimisePSTBlackBox(K, evfeatures[21], 1, 10, 1, 4);
//
//        std::cout << MgScore(*evfeatures[21]) << " | " << EgScore(*evfeatures[21]) << std::endl;
//        //        for (int s = 0; s < 64; s++) {
//        //            if (s%8 == 0) std::cout << std::endl;
//        //            std::cout << "M(" << setw(5) << MgScore(piece_square_table[0][0][s]) << "," << setw(5)
//        //                      << EgScore(piece_square_table[0][0][s]) << "), ";
//        //
//        //        }
//        //        std::cout << std::endl;
//        //        for (int s = 0; s < 64; s++) {
//        //            if (s%8 == 0) std::cout << std::endl;
//        //            std::cout << "M(" << setw(5) << MgScore(piece_square_table[0][1][s]) << "," << setw(5)
//        //                      << EgScore(piece_square_table[0][1][s]) << "), ";
//        //        }
//        //        std::cout << std::endl;
//        //        for (int s = 0; s < 225; s++) {
//        //            if (s%15 == 0) std::cout << std::endl;
//        //            std::cout << "M(" << setw(5) << MgScore(piece_our_king_square_table[0][s]) << "," << setw(5)
//        //                      << EgScore(piece_our_king_square_table[0][s]) << "), ";
//        //        }
//        //        std::cout << std::endl;
//        /*for (int s = 0; s < 225; s++) {
//            if (s % 15 == 0)
//                std::cout << std::endl;
//            std::cout << "M(" << setw(5) << MgScore(piece_opp_king_square_table[0][s]) << "," << setw(5)
//                      << EgScore(piece_opp_king_square_table[0][s]) << "), ";
//        }
//        std::cout << std::endl;*/
//        //        std::cout << std::endl;
//
//        /*for (Square s = 0; s < 23; s++) {
//            std::cout << "M(" << setw(5) << MgScore(*evfeatures[s]) << "," << setw(5) << EgScore(*evfeatures[s])
//                      << "), ";
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;
//*/
//        /*for (Square s = 0; s < 15; s++) {
//            std::cout << "M(" << setw(5) << MgScore(pinnedEval[s]) << "," << setw(5) << EgScore(pinnedEval[s]) << "), ";
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;
//        for (Square s = 0; s < 5; s++) {
//            std::cout << "M(" << setw(5) << MgScore(hangingEval[s]) << "," << setw(5) << EgScore(hangingEval[s])
//                      << "), ";
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;*/
//    }
//
//    delete evaluator;
//    bb_cleanUp();
//}
#include "gradient.h"
int main(int argc, char* argv[]) {

    
    if (argc == 1) {
        uci_loop(false);
    } else if (argc > 1 && strcmp(argv[1], "bench") == 0) {
        uci_loop(true);
    }


//    bb_init();
//    psqt_init();
//
//    load_weights();
//
//    load_positions("../resources/E12.33-1M-D12-Resolved.book", 10000000 ,0 );
//    load_positions("../resources/E12.41-1M-D12-Resolved.book", 10000000 ,0 );
//    load_positions("../resources/E12.46FRD-1250k-D12-1s-Resolved.book", 10000000 ,0 );
//    compute_K(2.48617, 100, 1e-7);
//    for(int i = 0; i < 100; i++){
//        train(100, 2.48172, 60000);
//        display_params();
//    }


//    Board b{"r4rk1/1pq2pbp/p2pbnp1/2p1n3/3P4/2P1B1P1/PPN1PNBP/R2Q1RK1 b - - 0 16"};
//    EvalData evalData;
//    evalData.init(&b);
//    Evaluator evaluator{};
//    std::cout << (int)evalData.evaluate() << std::endl;
//    std::cout << "--------------------------------------------------" << std::endl;
//    std::cout << evaluator.evaluate(&b) << std::endl;

//    printf("%d\n", (int)sizeof(feature_data));
//    printf("%d\n", (int)sizeof(mobility_data));
//    printf("%d\n", (int)sizeof(hanging_data));
//    printf("%d\n", (int)sizeof(pinned_data));
//    printf("%d\n", (int)sizeof(passer_data));
//    printf("%d\n", (int)sizeof(bishop_pawn_table_data));
//    printf("%d\n", (int)sizeof(king_safety_data));
//    printf("%d\n", (int)sizeof(pst_data_64));
//    printf("%d\n", (int)sizeof(pst_data_225));
//    printf("%d\n", (int)sizeof(meta_data        ));
//
//    printf("%d\n", (int)sizeof(eval_data));






    /**********************************************************************************
     *                                  T U N I N G                                   *
     **********************************************************************************/

    // main_tune_pst_bb(PAWN);
    //    psqt_init();
    // main_tune_features();
    // main_tune_pst();
    // main_tune_features_bb();

    return 0;
}
