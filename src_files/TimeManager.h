
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

#include "Bitboard.h"
#include "Board.h"
#include "History.h"
#include "Move.h"

using namespace move;
using namespace bb;

struct Limit {
    bool enabled = false;
};

struct DepthLimit : public Limit {
    Depth depth;
};

struct NodeLimit : public Limit {
    U64 nodes;
};

struct MoveTimeLimit : public Limit {
    U64 upper_time_bound;
};

struct MatchTimeLimit : public Limit {
    U64 time_to_use;
};

class TimeManager {
    public:
    DepthLimit     depth_limit      {};
    NodeLimit      node_limit       {};
    MoveTimeLimit  move_time_limit  {};
    MatchTimeLimit match_time_limit {};

    bool           force_stop       {};
    U64            start_time       {};

    TimeManager();

    void setDepthLimit(Depth depth);
    
    void setNodeLimit(U64 nodes);
    
    void setMoveTimeLimit(U64 move_time);
    
    void setMatchTimeLimit(U64 time, U64 inc, int moves_to_go);
    
    void setStartTime();

    /**
     * returns the time elapsed since the constructor call
     * @return
     */
    [[nodiscard]] int  elapsedTime() const;

    /**
     * stops the search. this should be considered to check if time is left
     */
    void stopSearch();

    /**
     * returns true if there is enough time left. This is used by the principal variation search.
     */
    [[nodiscard]] bool isTimeLeft(SearchData* sd = nullptr);

    /**
     * returns true if there is enough root time. root time is used to increase the depth in between
     * iterative deepening iterations. It ensures that the search will mostly finish its iteration.
     * @return
     */
    [[nodiscard]] bool rootTimeLeft(int score);
};

#endif    // KOIVISTO_TIMEMANAGER_H
