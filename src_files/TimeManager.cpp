
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
void TimeManager::setMatchTimeLimit (U64 time, U64 inc, int moves_to_go) {
    UCI_ASSERT(time >= 0);
    UCI_ASSERT(inc >= 0);
    UCI_ASSERT(moves_to_go >= 0);
    
    constexpr U64 overhead = 50;
    const double  division = moves_to_go + 1;
    
    U64 upperTimeBound = (int(time / division) * 3 + std::min(time * 0.9 + inc, inc * 3.0) - 25);
    U64 timeToUse      = upperTimeBound / 3;
    
    timeToUse          = std::min(time - inc, timeToUse);
    upperTimeBound     = std::min(time - inc, upperTimeBound);
    
    this->setMoveTimeLimit(upperTimeBound);
    this->match_time_limit.time_to_use      = timeToUse;
    this->match_time_limit.enabled          = true;
}

void TimeManager::setStartTime() {
    start_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

int  TimeManager::elapsedTime() const {
    auto end  = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    auto diff = end - start_time;
    return diff;
}

void TimeManager::stopSearch() {
    force_stop = true;
}

bool TimeManager::isTimeLeft(SearchData* sd) {
    // stop the search if requested
    if (force_stop)
        return false;
    
    int elapsed = elapsedTime();
    
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

bool TimeManager::rootTimeLeft(int score) {
    // stop the search if requested
    if (force_stop)
        return false;
    
    int elapsed = elapsedTime();
    
    if(    move_time_limit.enabled
           && move_time_limit.upper_time_bound < elapsed)
        return false;
    
    if(    match_time_limit.enabled
           && match_time_limit.time_to_use *50.0 / std::max(score, 30) < elapsed)
        return false;
    
    return true;
}
