
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

#include "timemanager.h"
#include "uciassert.h"

using namespace bb;

/**
 * creates a new TM object
 * Resets the time-manager just to get to factory settings
 */
TimeManager::TimeManager() {
    this->reset();
}

/**
 * setDepthLimit sets the search depth limit for the chess engine.
 * The function takes in a single parameter, depth, which represents the maximum search depth.
 * The function asserts that the input depth is non-negative and then sets the depth_limit.depth to
 * the input depth and enables the depth_limit
 *
 *  @param depth The maximum search depth for the chess engine
 *  @return None
 */
void TimeManager::setDepthLimit(Depth depth) {
    // Ensure input depth is non-negative
    UCI_ASSERT(depth >= 0);

    // Set the depth limit to the input depth and enable the depth limit
    this->depth_limit.depth   = depth;
    this->depth_limit.enabled = true;
}

/**
 * setNodeLimit sets the search node limit for the chess engine.
 * The function takes in a single parameter, nodes, which represents the maximum number of search
 * nodes. The function asserts that the input number of nodes is non-negative and then sets the
 * node_limit.nodes to the input number of nodes and enables the node_limit
 *
 *  @param nodes The maximum number of search nodes for the chess engine
 *  @return None
 */
void TimeManager::setNodeLimit(S64 nodes) {
    // Ensure input number of nodes is non-negative
    UCI_ASSERT(nodes >= 0);

    // Set the node limit to the input number of nodes and enable the node limit
    this->node_limit.nodes   = nodes;
    this->node_limit.enabled = true;
}

/**
 *  setMoveTimeLimit sets the move time limit for the chess engine.
 *  The function takes in a single parameter, move_time, which represents the maximum time for a move.
 *  The function asserts that the input move time is non-negative and then sets the
 *  move_time_limit.upper_time_bound to the input move time and enables the move_time_limit
 *
 *  @param move_time The maximum time for a move for the chess engine
 *  @return None
 */
void TimeManager::setMoveTimeLimit  (S64 move_time) {
    // Ensure input move time is non-negative
    UCI_ASSERT(move_time >= 0);
    
    // Set the move time limit to the input move time and enable the move time limit
    this->move_time_limit.upper_time_bound = move_time;
    this->move_time_limit.enabled          = true;
}

/**
 * setMatchTimeLimit sets the match time limit for the chess engine, based on the input parameters of
 * time, inc, and moves_to_go. It also accounts for any overhead time that may be set, and calculates
 * an upper time bound for the match. Additionally, it ensures that the calculated time limits do not
 * exceed the original match time limit, and that they do not go below a certain minimum value.
 *
 * @param time : The total time for the match
 * @param inc : The time increment for each move
 * @param moves_to_go : The number of moves to the end of the match
 * @return None
 */
void TimeManager::setMatchTimeLimit (S64 time, S64 inc, int moves_to_go) {
    // Ensure input values are non-negative
    UCI_ASSERT(time >= 0);
    UCI_ASSERT(inc >= 0);
    UCI_ASSERT(moves_to_go >= 0);
    
    // Define overhead for the match time limit
    // If increment is 0, set overhead to 50, otherwise set it to 0
    const S64    overhead          = inc == 0 ? 50 : 0;
    
    // Define a constant division factor
    const double division          = 2;
    
    // Retrieve the per-game overhead time if set
    const S64    overhead_per_game = move_overhead.type == PER_GAME ? move_overhead.time : 0;
    
    // If the time is less than 1000 and inc is 0, multiply time by 0.7
    if(time < 1000 && inc == 0){
        time = time * 0.7;
    }
    
    // Calculate time to use for the match
    S64 timeToUse      = 2 * inc + 2 * time / moves_to_go;

    S64 upperTimeBound = std::min(time / division, (double)(2 * inc + 2 * time / moves_to_go) * 3);
    
    // Ensure timeToUse and upperTimeBound do not exceed the match time limit minus overhead
    timeToUse          = std::min(time - overhead - inc - overhead_per_game, timeToUse);
    upperTimeBound     = std::min(time - overhead - inc - overhead_per_game, upperTimeBound);
    
    // If per-move overhead is set, subtract it from timeToUse and upperTimeBound
    // But ensure they do not go below 10
    if(move_overhead.type == PER_MOVE){
        timeToUse      = std::max(static_cast<S64>(10) , timeToUse      - move_overhead.time);
        upperTimeBound = std::max(static_cast<S64>(10) , upperTimeBound - move_overhead.time);
    }
    
    // Set the move time limit to upperTimeBound
    this->setMoveTimeLimit(upperTimeBound);
    
    // Set the match time limit to timeToUse and enable it
    this->match_time_limit.time_to_use = timeToUse;
    this->match_time_limit.enabled     = true;
}


/**
 * resets the time manager into factory settings.
 * It disables all limits
 */
void TimeManager::reset() {
    this->setStartTime();
    this->force_stop       = false;
    this->depth_limit      = {};
    this->node_limit       = {};
    this->move_time_limit  = {};
    this->match_time_limit = {};
}

/**
 * sets the start time of the search. This is being called at the very beginning to avoid
 * any flagging. Setting up the search, allocating data etc is done after the start time has been set
 */
void TimeManager::setStartTime() {
    start_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now().time_since_epoch()).count();
}

/**
 * computes the elapsed time since the search has started in milliseconds.
 * @return
 */
bb::U64 TimeManager::elapsedTime() const {
    auto end   = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now().time_since_epoch()).count();
    auto diff = end - start_time;
    return diff;
}

/**
 * sets the overhead in milliseconds which acts as a anti-flagging mechanism in the search
 * If the engine is known to stall, this can lead to improved performance.
 * @param time : the amount of time which should be reserved either per move or per game.
 */
void TimeManager::setMoveOverhead(bb::S64 time) {
    move_overhead.time = time;
}

/**
 * sets the overhead type for the moves. It can be one of:
 *  PER_MOVE,
 *  PER_GAME
 * PER_MOVE will lead to the move-overhead being applied on a per-move basis.
 * This means that every move will be search x-milliseconds less.
 * PER_GAME will adjust the time manager to use less time per move so that the overall
 * time once the game finishes is x-milliseconds.
 * @param mode : the overhead-type, either PER_MOVE or PER_GAME
 */
void TimeManager::setMoveOverheadType(MoveOverheadType mode) {
    move_overhead.type = mode;
}

/**
 * force stops the search by setting the force_stop variable to true
 * This will lead to any active search being immediately stopped.
 */
void TimeManager::stopSearch() {
    force_stop = true;
}

/**
 * returns true if there is enough time left. This is used by the principal variation search.
 * If it returns false, the search should stop.
 * @param sd : search_data object which contains relevant search information
 * @return true if enough time is left and the search should continue, false otherwise.
 */
bool TimeManager::isTimeLeft(SearchData* sd) const {
    // stop the search if requested
    if (force_stop)
        return false;
    
    U64 elapsed = elapsedTime();
    
    // if sd is not null, update the targetReached field
    // targetReached is true if the target time to use has been reached, false otherwise
    if (sd != nullptr && this->match_time_limit.enabled) {
        if (elapsed * 10 < this->match_time_limit.time_to_use) {
            sd->targetReached = false;
        } else {
            sd->targetReached = true;
        }
    }
    
    // if the move_time_limit is enabled, and we have exceeded the upper_time_bound, return false
    if (   this->move_time_limit.enabled
        && this->move_time_limit.upper_time_bound < elapsed)
        return false;
    
    // if we have not returned false yet, we have time left
    return true;
}

/**
 * Function that checks if there is enough time left for the root node of the search tree. This is
 * used to increase the depth in between iterative deepening iterations and ensures that the search
 * will mostly finish its iteration.
 *
 * @param nodeScore - Score of the current node in the search tree
 * @param evalScore - Evaluation score of the current position
 * @return bool - Returns true if there is enough time left, false otherwise
 */
bool TimeManager::rootTimeLeft(int nodeScore, int evalScore) const {
    // stop the search if requested
    if (force_stop)
        return false;
    
    // calculate the node score as a value between 0 and 100
    // where 100 means that the entire time has been spent looking at the best move
    nodeScore = 110 - std::min(nodeScore, 90);
    // calculate the eval score as a value between 50 and 80
    evalScore = std::min(std::max(50, 50 + evalScore), 80);

    U64 elapsed = elapsedTime();
    
    // check if there is enough time left based on the move time limit
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

