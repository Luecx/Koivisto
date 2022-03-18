
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

#include "TimeManager.h"
#include "UCIAssert.h"

using namespace bb;

TimeManager::TimeManager() {
    this->setStartTime();
}

void TimeManager::setDepthLimit     (Depth depth) {
    UCI_ASSERT(depth >= 0);
    
    this->depth_limit.depth   = depth;
    this->depth_limit.enabled = true;
}
void TimeManager::setNodeLimit      (U64 nodes) {
    UCI_ASSERT(nodes >= 0);
    
    this->node_limit.nodes   = nodes;
    this->node_limit.enabled = true;
}
void TimeManager::setMoveTimeLimit  (U64 move_time) {
    UCI_ASSERT(move_time >= 0);
    
    this->move_time_limit.upper_time_bound = move_time;
    this->move_time_limit.enabled          = true;
}
void TimeManager::setMatchTimeLimit (U64 time, U64 inc, int moves_to_go, bool ponder_move_played) {
    UCI_ASSERT(time >= 0);
    UCI_ASSERT(inc >= 0);
    UCI_ASSERT(moves_to_go >= 0);
    
    const U64    overhead = inc == 0 ? 50 : 0;
    const double division = 2;
    
    if(time < 1000 && inc == 0){
        time = time * 0.7;
    }
    
    U64 upperTimeBound = time / division;
    U64 timeToUse      = 2 * inc + 2 * time / moves_to_go;
    
    timeToUse          = std::min(time - overhead - inc, timeToUse);
    upperTimeBound     = std::min(time - overhead - inc, upperTimeBound);
    
    if(ponder_move_played){
        timeToUse      *= 0.8;
        upperTimeBound *= 0.8;
    }
    
    this->setMoveTimeLimit(upperTimeBound);
    this->match_time_limit.time_to_use = timeToUse;
    this->match_time_limit.enabled     = true;
}
void TimeManager::setStartTime() {
    start_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now().time_since_epoch()).count();
}

bb::U64 TimeManager::elapsedTime() const {
    auto end   = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now().time_since_epoch()).count();
    auto diff = end - start_time;
    return diff;
}

void TimeManager::stopSearch() {
    force_stop = true;
}

bool TimeManager::isTimeLeft(SearchData* sd) const {
    // stop the search if requested
    if (force_stop)
        return false;
    
    U64 elapsed = elapsedTime();
    
    if (sd != nullptr && this->match_time_limit.enabled) {
        if (elapsed * 10 < this->match_time_limit.time_to_use) {
            sd->targetReached = false;
        } else {
            sd->targetReached = true;
        }
    }
    
    // if we are above the maximum allowed time, stope
    if (    this->move_time_limit.enabled
         && this->move_time_limit.upper_time_bound < elapsed)
        return false;
    
    return true;
}

bool TimeManager::rootTimeLeft(int nodeScore, int evalScore) const {
    // stop the search if requested
    if (force_stop)
        return false;

    nodeScore = 110 - std::min(nodeScore, 90);
    evalScore = std::min(std::max(50, 50 + evalScore), 80);

    U64 elapsed = elapsedTime();
    
    if(    move_time_limit.enabled
        && move_time_limit.upper_time_bound < elapsed)
        return false;
    
    // the score is a value between 0 and 100 where 100 means that
    // the entire time has been spent looking at the best move.
    // this indicates that there is most likely just a single best move
    // which means we could spend less time searching. In case of the score being
    // 100, we half the time to use. If it's lower than 30, it reaches a maximum of 1.4 times the
    // original time to use.
    if(    match_time_limit.enabled
        && match_time_limit.time_to_use * nodeScore / 100 * evalScore / 65 < elapsed)
        return false;
    
    return true;
}
