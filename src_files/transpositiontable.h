
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

#ifndef CHESSCOMPUTER_HASHMAP_H
#define CHESSCOMPUTER_HASHMAP_H

#include "bitboard.h"
#include "move.h"

#include <memory>
#include <ostream>
#include <stdint.h>

using NodeType = uint8_t;
using NodeAge = uint8_t;

constexpr NodeType PV_NODE  = 0;
constexpr NodeType CUT_NODE = 1;
constexpr NodeType ALL_NODE = 2;
constexpr NodeType FORCED_ALL_NODE = 3;
constexpr NodeType FORMER_CUT_NODE = 6;

#define NODE_AGE_DIFF(v1, v2) (((int)(v1) - (int)(v2) + (((int)(v1) - (int)(v2) < 0) ? 256 : 0)))

struct Entry {
    //    Entry() = default;
    //    ~Entry() = default;

    friend std::ostream& operator<<(std::ostream& os, const Entry& entry) {
        os << "zobrist: " << entry.zobrist << " move: " << entry.move << " depth: " << static_cast<int>(entry.depth)
           << " type: " << static_cast<int>(entry.type) << " score: " << entry.score;
        return os;
    }

    void set(bb::U32 p_key, bb::Score p_score, move::Move p_move, NodeType p_type, bb::Depth p_depth, bb::Score p_eval) {
        this->zobrist = p_key;
        this->score   = p_score;
        this->move    = p_move;
        this->type    = p_type;
        this->depth   = p_depth;
        this->eval    = p_eval;
    }

    [[nodiscard]] NodeAge getAge() const { return move::getScore(move); }

    void setAge(NodeAge p_age) { move::setScore(move, p_age); }

    bb::U32     zobrist;    // 32 bit
    move::Move  move;       // 32 bit  (using the 8 msb for age)
    bb::Depth   depth;      // 8 bit
    NodeType    type;       // 8 bit
    bb::Score   score;      // 16 bit
    bb::Score   eval;  	    // 16 bit -> 112 bit = 14 byte
};

class TranspositionTable {
    private:
    NodeAge                  m_currentAge;
    bb::U64                  m_size;
    std::unique_ptr<Entry[]> m_entries;
    bb::U64                  m_mask;

    void init(bb::U64 MB);

    public:
    TranspositionTable(bb::U64 mb);

    TranspositionTable(const TranspositionTable& other) = delete;

    TranspositionTable& operator=(const TranspositionTable& other) = delete;

    [[nodiscard]] Entry get(bb::U64 zobrist) const;

    bool put(bb::U64 zobrist, bb::Score score, move::Move move, NodeType type, bb::Depth depth, bb::Score eval);

    void incrementAge();

    void setSize(bb::U64 mb);

    void clear();

    [[nodiscard]] double usage() const;

    [[nodiscard]] bb::U64 getSize() const;

    void prefetch(const bb::U64 zobrist) const;
};


int maxTTSize();

#endif    // CHESSCOMPUTER_HASHMAP_H
