
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
    bb_init();
    Evaluator* evaluator = new Evaluator();

    using namespace tuning;

    loadPositionFile("resources/other/quiet-labeled.epd", 1e7);
    auto K = tuning::computeK(evaluator, 2.86681, 200, 1e-7);

    for (int i = 0; i < 64; i++) {
        evaluator->getPSQT(piece, true)[i]  = round(evaluator->getPSQT(piece, true)[i]);
        evaluator->getPSQT(piece, false)[i] = round(evaluator->getPSQT(piece, false)[i]);
    }

    for (int i = 0; i < 5000; i++) {

        std::cout << "--------------------------------------------------- [" << i
                  << "] ----------------------------------------------" << std::endl;
        
        for(Piece p = PAWN; p <= KING; p++){
            std::cout << std::setprecision(8)
                      << tuning::optimiseBlackBox(evaluator, K, evaluator->getPSQT(p, true), 64, 1) << std::endl;
            std::cout << std::setprecision(8)
                      << tuning::optimiseBlackBox(evaluator, K, evaluator->getPSQT(piece, false), 64, 1) << std::endl;
    
            std::cout << std::endl;
            for (int n = 0; n < 64; n++) {
                std::cout << std::right << std::setw(6) << evaluator->getPSQT(piece, true)[n] << ",";
                if (n % 8 == 7)
                    std::cout << std::endl;
            }
            for (int n = 0; n < 64; n++) {
                std::cout << std::right << std::setw(6) << evaluator->getPSQT(piece, false)[n] << ",";
                if (n % 8 == 7)
                    std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        
        

        std::cout << std::endl;
    }

    delete evaluator;
    bb_cleanUp();
}

void main_tune_features() {
    bb_init();
    Evaluator* evaluator = new Evaluator();

    using namespace tuning;

    loadPositionFile("../resources/other/quiet-labeled.epd", 1e6);
    auto K = tuning::computeK(evaluator, 2.86681, 200, 1e-7);

    for (int i = 0; i < 5000; i++) {

        std::cout << "--------------------------------------------------- [" << i
                  << "] ----------------------------------------------" << std::endl;
//        std::cout << std::setprecision(8) << tuning::optimiseGD(evaluator, K, 1e4) << std::endl;
        
        for(Piece p = BISHOP; p <= KING; p++){
    
            std::cout << tuning::optimisePSTBlackBox(new Evaluator,K, psqt[p], 64, 3);
        }
//        std::cout << tuning::optimisePSTBlackBox(new Evaluator,K, psqt[KNIGHT], 64, 5);
//        std::cout << tuning::optimisePSTBlackBox(new Evaluator,K, psqt[PAWN], 64, 5);
//        std::cout << tuning::optimisePSTBlackBox(new Evaluator,K, psqt[PAWN], 64, 5);
//        std::cout << tuning::optimisePSTBlackBox(new Evaluator,K, psqt[PAWN], 64, 5);
//        std::cout << tuning::optimisePSTBlackBox(new Evaluator,K, psqt[PAWN], 64, 5);
        
//        for (int k = 0; k < evaluator->paramCount(); k++) {
//            std::cout << std::setw(14) << evaluator->getEarlyGameParams()[k] << ",";
//        }
//        std::cout << std::endl;
//        for (int k = 0; k < evaluator->paramCount(); k++) {
//            std::cout << std::setw(14) << evaluator->getLateGameParams()[k] << ",";
//        }
//        std::cout << std::endl;
        for(Piece p = BISHOP; p <= KING; p++){
            for(Square s = 0; s < 64; s++){
                std::cout << "M(" << setw(5) << MgScore(psqt[p][s]) << "," << setw(5) <<EgScore(psqt[p][s]) << "), ";
                if(s % 8 == 7){
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
        }
    
    
    }

    delete evaluator;
    bb_cleanUp();
}

#ifdef TUNE_PST
void main_tune_pst() {
    bb_init();
    Evaluator* evaluator = new Evaluator();

    using namespace tuning;

    loadPositionFile("resources/quiet-labeled.epd", 1e6);

    // auto K = tuning::computeK(evaluator,2.86681, 200, 1e-7);

    for (int i = 0; i < 5000; i++) {

        std::cout << "--------------------------------------------------- [" << i
                  << "] ----------------------------------------------" << std::endl;

        std::cout << std::setprecision(8) << tuning::optimisePST(evaluator, 2.86681, 1e6) << std::endl;

        for (int k = 0; k < 64; k++) {
            std::cout << std::setprecision(1) << fixed << std::setw(10) << evaluator->getTunablePST_MG()[k] << ",";
            if (k % 8 == 7)
                std::cout << "\n";
        }
        std::cout << std::endl;
        for (int k = 0; k < 64; k++) {
            std::cout << std::setprecision(1) << fixed << std::setw(10) << evaluator->getTunablePST_EG()[k] << ",";
            if (k % 8 == 7)
                std::cout << "\n";
        }
        std::cout << std::endl;
    }

    delete evaluator;
    bb_cleanUp();
}
#endif

#include "eun/generation/Generator.h"

int main(int argc, char* argv[]) {
    
    if (argc == 1) {
        uci_loop(false);
    } else if (argc > 1 && strcmp(argv[1], "bench") == 0) {
        uci_loop(true);
    } else if (argc > 2 && strcmp(argv[1], "gen") == 0){
        generator::generate(std::string(argv[2]));
    }
    
    
    
    /**********************************************************************************
     *                                  T U N I N G                                   *
     **********************************************************************************/

    // bb_init();
    // tuning::loadPositionFile("resources/quiet-labeled.epd", 1000000);
    // tuning::evalSpeed();
    // bb_cleanUp();

    // main_tune_pst_bb(PAWN);
    //eval_init();
    //main_tune_features();
    // main_tune_pst();
    //main_tune_features_bb();

    return 0;
}
