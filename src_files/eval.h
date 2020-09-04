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
        float getPhase();
        
        /**
         * returns a list of features of the last calculation
         * @return
         */
        float *getFeatures();
        
        /**
         * returns a list of early game parameters
         * @return
         */
        float *getEarlyGameParams();
        
        /**
         * returns a list of late game parameters
         */
        float *getLateGameParams();
    
        /**
         * returns the amount of tunable parameters
         */
        int paramCount();
        
        #ifdef TUNE_PST
        float* getTunablePST_MG();
        float* getTunablePST_EG();
        float* getTunablePST_MG_grad();
        float* getTunablePST_EG_grad();
        #endif
    
};

void printEvaluation(Board* b);




#endif //KOIVISTO_EVAL_H
