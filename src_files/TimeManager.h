
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                                   by. Kim Kahre and Finn Eggers                                  *
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

#ifndef KOIVISTO_TIMEMANAGER_H
#define KOIVISTO_TIMEMANAGER_H

#include "Board.h"
#include "Move.h"
#include "History.h"

using namespace move;
using namespace bb;

enum TimeMode{
    DEPTH,
    MOVETIME,
    TOURNAMENT,
};

class TimeManager {

    private:
    
    TimeMode mode;

    int timeToUse;
    U64 nodesToUse;
    int upperTimeBound;
    bool forceStop;
    
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
     * stops the search. this should be considered to check if time is left
     */
    void stopSearch();

    /**
     * returns true if the search should continue. false otherwise.
     * @return
     */
    bool isTimeLeft(SearchData* sd = nullptr);
    
    /**
     * checks if time at the root is left
     * @return
     */
    bool rootTimeLeft(int score);
    
    /**
     * returns the timemode
     * @return
     */
    TimeMode getMode() const;
    
    /**
     * set a node limit for the search
     */
    void setNodeLimit(U64 maxNodes=-1);
    
    /**
     * check if the search shall be stopped by force
     */
    bool isForceStopped();
    
    /**
     * returns the node limit for the search
     */
    U64 getNodeLimit();
};

#endif    // KOIVISTO_TIMEMANAGER_H
