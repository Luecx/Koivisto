
#include <iomanip>
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

    bb_init();
    Evaluator *evaluator = new Evaluator();

    using namespace tuning;


    loadPositionFile("..\\resources\\quiet-labeled.epd", 2e6);


//    generateHeatMap(PAWN, true, false);

    auto K = tuning::computeK(evaluator,2.86681, 200, 1e-7);

    for(int i = 0; i < 5000; i++){

        std::cout << "--------------------------------------------------- ["<<i << "] ----------------------------------------------" << std::endl;

        for(int k = 0; k < evaluator->paramCount(); k++){
            std::cout << std::setw(14) << evaluator->getEarlyGameParams()[k]<< ",";
        }
        std::cout << std::endl;
        for(int k = 0; k < evaluator->paramCount(); k++){
            std::cout << std::setw(14) <<evaluator->getLateGameParams()[k] << ",";
        }
        std::cout << std::endl;


        std::cout << tuning::optimise(evaluator, K, 5e4) << std::endl;
    }




    delete evaluator;
    bb_cleanUp();

    
    return 0;
    
}



