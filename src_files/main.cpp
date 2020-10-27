
#include "Bitboard.h"
#include "Board.h"
#include "Move.h"
#include "MoveOrderer.h"
#include "Tuning.h"
#include "Verification.h"
#include "uci.h"
#include "gradients.h"

#include <iomanip>

using namespace std;
using namespace bb;
using namespace move;

void main_tune_pst_bb(Piece piece) {
    eval_init();
    bb_init();

    using namespace tuning;

    loadPositionFile("../resources/other/tuningset_clear2.epd", 2500000);

    eval_init();
    auto K = tuning::computeK(2.86681, 200, 1e-7);

    int lr = 200;

    for (int i = 1; i < 5000; i++) {

        std::cout << "--------------------------------------------------- [" << i
                  << "] ----------------------------------------------" << std::endl;

        std::cout << tuning::optimisePSTBlackBox(K, psqt[2], 64, lr) << std::endl;
        std::cout << tuning::optimisePSTBlackBox(K, psqt[3], 64, lr) << std::endl;

        std::cout << tuning::optimisePSTBlackBox(K, psqt[4], 64, lr) << std::endl;
        std::cout << tuning::optimisePSTBlackBox(K, psqt[5], 64, lr) << std::endl;

        std::cout << tuning::optimisePSTBlackBox(K, psqt[6], 64, lr) << std::endl;
        std::cout << tuning::optimisePSTBlackBox(K, psqt[7], 64, lr) << std::endl;

        std::cout << tuning::optimisePSTBlackBox(K, psqt[8], 64, lr) << std::endl;
        std::cout << tuning::optimisePSTBlackBox(K, psqt[9], 64, lr) << std::endl;

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

    bb_cleanUp();
}

void main_tune_features() {
    eval_init();
    bb_init();

    using namespace tuning;

    loadPositionFile("../resources/other/quiet-labeled.epd", 1e6);
    auto K = tuning::computeK(2.86681, 200, 1e-7);

    for (int i = 0; i < 5000; i++) {

        std::cout << "--------------------------------------------------- [" << i
                  << "] ----------------------------------------------" << std::endl;

        //        std::cout << tuning::optimisePSTBlackBox(K, pieceScores, 6, 1) << std::endl;
        //        std::cout << tuning::optimisePSTBlackBox(K, evfeatures, 20, 1) << std::endl;
        //        std::cout << tuning::optimisePSTBlackBox(K, pinnedEval, 15, 1) << std::endl;
        //        std::cout << tuning::optimisePSTBlackBox(K, hangingEval, 5, 1) << std::endl;
        //
        //        for (Square s = 0; s < 6; s++) {
        //            std::cout << "M(" << setw(5) << MgScore(pieceScores[s]) << "," << setw(5) <<
        //            EgScore(pieceScores[s])
        //                      << "), ";
        //            std::cout << std::endl;
        //        }
        //        std::cout << std::endl;
        //        for (Square s = 0; s < 20; s++) {
        //            std::cout << "M(" << setw(5) << MgScore(*evfeatures[s]) << "," << setw(5) <<
        //            EgScore(*evfeatures[s])
        //                      << "), ";
        //            std::cout << std::endl;
        //        }
        //        std::cout << std::endl;
        //
        //        for (Square s = 0; s < 15; s++) {
        //            std::cout << "M(" << setw(5) << MgScore(pinnedEval[s]) << "," << setw(5) << EgScore(pinnedEval[s])
        //            << "), "; std::cout << std::endl;
        //        }
        //        std::cout << std::endl;
        //        for (Square s = 0; s < 5; s++) {
        //            std::cout << "M(" << setw(5) << MgScore(hangingEval[s]) << "," << setw(5) <<
        //            EgScore(hangingEval[s])
        //                      << "), ";
        //            std::cout << std::endl;
        //        }
        //        std::cout << std::endl;
    }

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

//    bb_init();
//    eval_init();
//    tuning::loadPositionFile("../resources/other/E12.33-1M-D12-Resolved.book", 10000000);
//    for(int i = 0; i < 100; i++){
//
//        std::cout << tuning::tune_mobility_gradients(tuning::boards, tuning::results, tuning::dataCount, 3.1) << std::endl;
//    }
    
    //     for(int i = 0; i < 10; i++)
    //     tuning::evalSpeed();
    //     bb_cleanUp();

    // main_tune_pst_bb(PAWN);
    //    eval_init();
    //    main_tune_features();
    // main_tune_pst();
    // main_tune_features_bb();

    return 0;
}
