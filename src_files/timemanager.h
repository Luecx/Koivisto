
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


struct Limit {
    bool enabled = false;
};

struct DepthLimit : public Limit {
    bb::Depth depth;
};

struct NodeLimit : public Limit {
    bb::U64 nodes;
};

struct MoveTimeLimit : public Limit {
    bb::U64 upper_time_bound;
};

struct MatchTimeLimit : public Limit {
    bb::U64 time_to_use;
};


enum MoveOverheadType{
    PER_MOVE,
    PER_GAME
};

struct MoveOverhead{
    MoveOverheadType type{PER_MOVE};
    bb::S64          time{0};
};

class TimeManager {
    public:
    DepthLimit     depth_limit      {};
    NodeLimit      node_limit       {};
    MoveTimeLimit  move_time_limit  {};
    MatchTimeLimit match_time_limit {};
    
    // move overhead
    MoveOverhead   move_overhead    {};

    bool           force_stop       {};
    bb::S64        start_time       {};

    TimeManager();

    void setDepthLimit(bb::Depth depth);
    
    void setNodeLimit(bb::S64 nodes);
    
    void setMoveTimeLimit(bb::S64 move_time);
    
    void setMatchTimeLimit(bb::S64 time, bb::S64 inc, int moves_to_go);
    
    void setStartTime();
    
    /**
     * resets the limits and the other informatino except for the overhead
     */
    void reset();
    
    /**
     * returns the time elapsed since the constructor call
     * @return
     */
    [[nodiscard]] bb::U64 elapsedTime() const;
    
    /**
     * sets the move overhead
     * @param time
     */
    void setMoveOverhead(bb::S64 time);
    
    /**
     * sets the overhead type.
     * the normal type is per_move which would reduce the time to search by the given move overhead.
     * Alternatively, the move overhead can be applied to the total time left for each move. This
     * will lead to each move having atleast x ms left on the clock whereas the per_move method also
     * ensures that. PER_MOVE is more conservative. This is ONLY active IF the match time limit is set
     * and used.
     * @param mode
     */
    void setMoveOverheadType(MoveOverheadType mode);
    
    /**
     * stops the search. this should be considered to check if time is left
     */
    void stopSearch();

    /**
     * returns true if there is enough time left. This is used by the principal variation search.
     */
    [[nodiscard]] bool isTimeLeft(SearchData* sd = nullptr) const;

    /**
     * returns true if there is enough root time. root time is used to increase the depth in between
     * iterative deepening iterations. It ensures that the search will mostly finish its iteration.
     * @return
     */
    [[nodiscard]] bool rootTimeLeft(int nodeScore, int evalScore) const;
};

#endif    // KOIVISTO_TIMEMANAGER_H
