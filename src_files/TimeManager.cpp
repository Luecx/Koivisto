
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

#include <algorithm>
#include "TimeManager.h"
#include "Board.h"
#include "UCIAssert.h"
#include "History.h"

auto startTime =
    std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

/**
 * We use this constructor if the movetime has been specified.
 * This means that we do not want to extend the time in critical positions and want to stop the search exactly after
 * movetime milliseconds.
 */
TimeManager::TimeManager(int moveTime) : 
    mode(MOVETIME), 
    timeToUse(moveTime),
    nodesToUse(-1),
    upperTimeBound(moveTime),
    forceStop() {

    startTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();
}

/**
 * If the depth is specified, we use this constructor.
 * No constraints about time are made and the search wont be stopped by the time manager.
 */
TimeManager::TimeManager() : 
    mode(DEPTH), 
    timeToUse(1 << 29),
    nodesToUse(-1),
    upperTimeBound(1 << 30),
    forceStop() {

    startTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();
}

/**
 * If a real game is being played, this is used.
 * It computes the time which should be used an an upper bound which is always larger then the time which is the target
 * time. The search loop will continue until timeToUse has been reached, yet the pv search wont stop until the upper
 * time bound is reached.
 * @param white
 * @param black
 * @param whiteInc
 * @param blackInc
 * @param movesToGo
 * @param board
 */
TimeManager::TimeManager(int white, int black, int whiteInc, int blackInc, int movesToGo, Board* board) : 
    mode(TOURNAMENT), 
    timeToUse(),
    nodesToUse(-1),
    upperTimeBound(),
    forceStop() {
    UCI_ASSERT(board);
    UCI_ASSERT(white > 0);
    UCI_ASSERT(black > 0);
    UCI_ASSERT(movesToGo >= 0);

    double division = movesToGo+1;

    upperTimeBound = board->getActivePlayer() == WHITE ? (int(white / division)*3 + whiteInc) - 25
                                                       : (int(black / division)*3 + blackInc) - 25;

    timeToUse = upperTimeBound / 3;

    timeToUse = std::min(timeToUse, board->getActivePlayer() == WHITE ? white - 50 : black - 50);
    upperTimeBound = std::min(upperTimeBound, board->getActivePlayer() == WHITE ? white - 50 : black - 50);

    startTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();
}

/**
 * returns the elapsed time since the starttime has been set.
 * @return
 */
int TimeManager::elapsedTime() {
    auto end =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();
    auto diff = end - startTime;

    return diff;
    // std::cout << "measurement finished! [" << round(diff.count() * 1000) << " ms]" << std::endl;
}

/**
 * a destructor for the sake of completeness.
 */
TimeManager::~TimeManager() {
}

/**
 * stops the search. Next time isTimeLeft() is called, false will be returned so that the search finishes as soon as
 * possible.
 */
void TimeManager::stopSearch() { forceStop = true; }

/**
 * returns true if there is enough time left. This is used by the principal variation search.
 */
bool TimeManager::isTimeLeft(SearchData* sd) {


    // stop the search if requested
    if (forceStop)
        return false;
    
    int elapsed = elapsedTime();
    
    if (sd != nullptr && mode == TOURNAMENT) {
        if (elapsed * 10 < timeToUse) {
           sd->targetReached = false;
        } else {
            sd->targetReached = true;
        }
    }
    
    // if we are above the maximum allowed time, stope  
    if (elapsed >= upperTimeBound)
        return false;

    return true;
}

/**
 * returns true if there is enough root time. root time is used to increase the depth in between iterative deepening
 * iterations. It ensures that the search will mostly finish its iteration.
 * @return
 */
bool TimeManager::rootTimeLeft(int score) {
    int elapsed = elapsedTime();

    // stop the search if requested
    if (forceStop)
        return false;

    // if we are above the maximum allowed time at root, stop
    if (elapsed >= timeToUse*50/std::max(score, 30))
        return false;

    return true;
}

/**
 * returns the move of the time manager.
 * Its either,
 *   - DEPTH
 *   - MOVETIME
 *   - TOURNAMENT
 * @return
 */
TimeMode TimeManager::getMode() const { return mode; }

/**
 * sets the node limit for the search
 * @param maxNodes
 */
void     TimeManager::setNodeLimit(U64 maxNodes) {
    this->nodesToUse = maxNodes;
}

/**
 * checks if the search is stopped by force
 * @return
 */
bool TimeManager::isForceStopped() {
    return forceStop;
}

/**
 * gets the node limit for the search
 * @return
 */
U64  TimeManager::getNodeLimit() {
    return nodesToUse;
}

