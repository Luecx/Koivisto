
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

#include "bitboard.h"
#include "board.h"
#include "history.h"
#include "move.h"

/**
 * a basic class which is inherited by other structs below.
 * It indicates a limit of some form which is usually specified by the user via th uci protocol.
 * It impacts the search by stopping it once any of the criteria is met.
 */
struct Limit {
    bool enabled = false;
};

/**
 * a simple criteria is the depth.
 * It will stop the search once the given depth is reached
 * The seldepth may be larger than the depth limit
 */
struct DepthLimit : public Limit {
    bb::Depth depth;
};

/**
 * NodeLimit describes a limit which limits the amount of nodes searched by the engine.
 * Note that it may search a few nodes more since this criterion is not checked during qsearch.
 */
struct NodeLimit : public Limit {
    bb::U64 nodes;
};

/**
 * MoveTimeLimit acts as a timely limit for each move. This criteria may not be exactly fulfilled
 * since, especially in multithreaded applications, stopping the search can take some time.
 * Furthermore it is only checked every 2^14 nodes to avoid too many system-calls.
 */
struct MoveTimeLimit : public Limit {
    bb::U64 upper_time_bound;
};

/**
 * the MatchTimeLimit is more a buffer which holds the target time to use. Commonly used in
 * normal matches, it has a time value which should approximate the amount of time to be used
 * for this search. it can be extended or shortened depending on the search itself.
 * Most commonly used in conjunction with he MoveTimeLimit to avoid the engine flagging.
 */
struct MatchTimeLimit : public Limit {
    bb::U64 time_to_use;
};

/**
 * Enum to describe two types of time overhead.
 * PER_MOVE describes an overhead used for each move
 * PER_GAME describes an overhead used for the totality of the match
 */
enum MoveOverheadType{
    PER_MOVE,
    PER_GAME
};

/**
 * The MoveOverhead does not impact the search directly but the limits which impact the search.
 * Based on its type, the Move- and MatchTimeLimits are adjusted.
 */
struct MoveOverhead{
    MoveOverheadType type{PER_MOVE};
    bb::S64          time{0};
};

/**
 * The TimeManager is responsible for managing the stopping criteria for the search.
 * Despite being named "TimeManager", it is also responsible for other criteria like depth and nodes.
 * It contains a list of limits which can all be enabled. Once one of them fails, the search
 * will be stopped.
 */
class TimeManager {
    public:
    // depth limit to limit the search depth
    DepthLimit     depth_limit      {};
    // node limit to limit the search node count
    NodeLimit      node_limit       {};
    // hard upper bound for the time of the search
    MoveTimeLimit  move_time_limit  {};
    // soft upper bound for the time of the search
    MatchTimeLimit match_time_limit {};
    
    // move overhead
    MoveOverhead   move_overhead    {};
    
    // enforce a stop of the search. The
    bool           force_stop       {};
    // track the start time (in ms)
    bb::S64        start_time       {};

    TimeManager();

    // sets and enables the depth limit of the search to the given value
    void setDepthLimit(bb::Depth depth);
    
    // sets and enables the node limit of the search to the given value
    void setNodeLimit(bb::S64 nodes);
    
    // sets and enables the upper time limit of the search to the given move time
    void setMoveTimeLimit(bb::S64 move_time);
    
    // sets and enables the target time value for the search.
    // its based on a complicated equation based on the time thats left for the entire game
    // as well as the increment and the amount of moves which are left until the 40-move rule comes
    // into play
    void setMatchTimeLimit(bb::S64 time, bb::S64 inc, int moves_to_go, bool ponder_hit);
    
    // sets the start time. Mostly just used internally
    void setStartTime();
    
    //  resets the limits and the other informatino except for the overhead
    void reset();
    
    // returns the time elapsed since the constructor call
    [[nodiscard]] bb::U64 elapsedTime() const;
    
    // sets the move overhead
    void setMoveOverhead(bb::S64 time);
    
    // sets the overhead type.
    // the normal type is per_move which would reduce the time to search by the given move overhead.
    // Alternatively, the move overhead can be applied to the total time left for each move. This
    // will lead to each move having at least x ms left on the clock whereas the per_move method also
    // ensures that. PER_MOVE is more conservative. This is ONLY active IF the match time limit is set
    // and used.
    void setMoveOverheadType(MoveOverheadType mode);
    
    // stops the search. this should be considered to check if time is left
    void stopSearch();

    // returns true if there is enough time left. This is used by the principal variation search.
    [[nodiscard]] bool isTimeLeft(SearchData* sd = nullptr) const;

    // returns true if there is enough root time. root time is used to increase the depth in between
    // iterative deepening iterations. It ensures that the search will mostly finish its iteration.
    [[nodiscard]] bool rootTimeLeft(int nodeScore, int evalScore) const;
};

#endif    // KOIVISTO_TIMEMANAGER_H
