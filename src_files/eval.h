//
// Created by finne on 5/31/2020.
//

#ifndef KOIVISTO_EVAL_H
#define KOIVISTO_EVAL_H


#include "Bitboard.h"
#include "Board.h"


class Evaluator{
    public:
        
        bb::Score evaluate(Board *b);
        
        /**
         * returns the phase of the last calculation
         * @return
         */
        double getPhase();
        
        /**
         * returns a list of features of the last calculation
         * @return
         */
        double *getFeatures();
        
        /**
         * returns a list of early game parameters
         * @return
         */
        double *getEarlyGameParams();
        
        /**
         * returns a list of late game parameters
         */
        double *getLateGameParams();
    
        /**
         * returns the amount of tunable parameters
         */
        int paramCount();
};

void printEvaluation(Board* b);




#endif //KOIVISTO_EVAL_H
