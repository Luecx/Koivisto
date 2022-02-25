
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

#include "Bitboard.h"
#include "Board.h"
#include "Move.h"

#include <ostream>
#include <stdint.h>

using namespace bb;
using namespace std;
using namespace move;

typedef uint8_t NodeType;
typedef uint8_t NodeAge;

constexpr NodeType PV_NODE  = 0;
constexpr NodeType CUT_NODE = 1;
constexpr NodeType ALL_NODE = 2;
constexpr NodeType FORCED_ALL_NODE = 3;
constexpr NodeType EMPTY_NODE = 4;
constexpr NodeType FORMER_CUT_NODE = 6;

struct Entry {

    //    Entry() = default;
    //    ~Entry() = default;

    friend ostream& operator<<(ostream& os, const Entry& entry) {
        os << "zobrist: " << entry.zobrist << " move: " << entry.move << " depth: " << (int) entry.depth
           << " type: " << (int) entry.type << " score: " << entry.score;
        return os;
    }

    void set(U32 p_key, Score p_score, Move p_move, NodeType p_type, Depth p_depth, Score p_eval) {
        this->zobrist = p_key;
        this->score   = p_score;
        this->move    = p_move;
        this->type    = p_type;
        this->depth   = p_depth;
        this->eval    = p_eval;
    }

    NodeAge getAge() { return getScore(move); }

    void setAge(NodeAge p_age) { setScore(move, p_age); }

    U32      zobrist;    // 32 bit
    Move     move;       // 32 bit  (using the 8 msb for age)
    Depth    depth;      // 8 bit
    NodeType type;       // 8 bit
    Score    score;      // 16 bit
    Score    eval;  	 // 16 bit -> 112 bit = 14 byte
};

class TranspositionTable {

    private:
    NodeAge m_currentAge;
    U64     m_size;

    void init(U64 MB);

    public:
    
    Entry*  m_entries;
    U64     m_mask;
    TranspositionTable(U64 mb);

    TranspositionTable(const TranspositionTable& other) = delete;

    TranspositionTable& operator=(const TranspositionTable& other) = delete;

    ~TranspositionTable();

    Entry get(U64 zobrist);

    bool put(U64 zobrist, Score score, Move move, NodeType type, Depth depth, Score eval);

    void incrementAge();

    void setSize(U64 mb);

    void clear();

    double usage();

    U64 getSize();
};


int maxTTSize();

#endif    // CHESSCOMPUTER_HASHMAP_H
