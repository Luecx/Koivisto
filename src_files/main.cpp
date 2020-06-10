
#include "uci.h"


#include "Bitboard.h"
#include "Board.h"
#include "Move.h"
#include "MoveOrderer.h"
#include "Verification.h"
#include "Tuning.h"

using namespace std;
using namespace bb;
using namespace move;


int main() {
    
  


      uci_loop();
   

    /**********************************************************************************
     *                                  T U N I N G                                   *
     **********************************************************************************/

//    bb_init();
//    Evaluator *evaluator = new Evaluator();
//
//    using namespace tuning;
//
//
//    loadPositionFile("..\\tuning\\quiet-labeled.epd", 1000000);
//
//
//    auto K = tuning::computeK(evaluator, 2.53741, 100, 1e-7);
//
//    for(int i = 0; i < 1000; i++){
//        std::cout << tuning::optimise(evaluator, K, 5e4) << std::endl;
//    }
//
//    for(int k = 0; k < evaluator->paramCount(); k++){
//        std::cout << evaluator->getEarlyGameParams()[k]<< std::endl;
//    }
//    std::cout << std::endl;
//    for(int k = 0; k < evaluator->paramCount(); k++){
//        std::cout << evaluator->getLateGameParams()[k] << std::endl;
//    }
//
//
//
//    delete evaluator;
//    bb_cleanUp();

    
    return 0;
    
}



