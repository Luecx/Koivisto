//
// Created by finne on 7/11/2020.
//

#ifndef KOIVISTO_TIMEMANAGER_H
#define KOIVISTO_TIMEMANAGER_H

#include "Move.h"
#include "Board.h"

using namespace move;
using namespace bb;

class TimeManager {
    
    private:
//        int white;
//        int black;
//        int whiteInc;
//        int blackInc;
//        int movesToGo;
        
        int timeToUse;
        int upperTimeBound;
        
        bool ignorePV;
        bool isSafeToStop;
        bool forceStop;
        
        
        int historyCount;
        Move* moveHistory;
        Score* scoreHistory;
        Depth *depthHistory;
    
        /**
         * updates isSafeToStop
         */
        void computeSafetyToStop();
        
    public:
        TimeManager(int white, int black, int whiteInc, int blackInc, int movesToGo, Board* board);
        
        TimeManager();
        
        TimeManager(int moveTime);
        
        virtual ~TimeManager();
        
        /**
         * returns the time elapsed since the constructor call
         * @return
         */
        int elapsedTime();
        
        /**
         * updates the pv etc
         * @param move
         * @param score
         */
        void updatePV(Move move, Score score, Depth depth);
        
        /**
         * stops the search. this should be considered to check if time is left
         */
        void stopSearch();
        
        /**
         * returns true if the search should continue. false otherwise.
         * @return
         */
        bool isTimeLeft();
        
};


#endif //KOIVISTO_TIMEMANAGER_H
