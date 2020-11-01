
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

#ifndef KOIVISTO_TIMEMANAGER_H
#define KOIVISTO_TIMEMANAGER_H

#include "Board.h"
#include "Move.h"

using namespace move;
using namespace bb;

enum TimeMode{
    DEPTH,
    MOVETIME,
    TOURNAMENT
};

class TimeManager {

    private:
    //        int white;
    //        int black;
    //        int whiteInc;
    //        int blackInc;
    //        int movesToGo;
    TimeMode mode;

    int timeToUse;
    int upperTimeBound;

    bool ignorePV;
    bool isSafeToStop;
    bool forceStop;

    int    historyCount;
    Move*  moveHistory;
    Score* scoreHistory;
    Depth* depthHistory;

    /**
     * updates isSafeToStop
     */

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

    bool rootTimeLeft();
    
    /**
     * returns the timemode
     * @return
     */
    TimeMode getMode() const;
};

#endif    // KOIVISTO_TIMEMANAGER_H
