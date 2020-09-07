//
// Created by finne on 5/18/2020.
//

#ifndef CHESSCOMPUTER_HASHMAP_H
#define CHESSCOMPUTER_HASHMAP_H


#include <stdint.h>
#include <ostream>
#include <mutex>

#include "Bitboard.h"
#include "Move.h"
#include "Board.h"

using namespace bb;
using namespace std;
using namespace move;

typedef uint8_t NodeType;
typedef uint8_t NodeAge;

constexpr NodeType PV_NODE  = 0;
constexpr NodeType CUT_NODE = 1;
constexpr NodeType ALL_NODE = 2;


struct Entry {

//    Entry() = default;
//    ~Entry() = default;
    
    friend ostream &operator<<(ostream &os, const Entry &entry) {
        os << "zobrist: " << entry.zobrist << " move: " << entry.move << " depth: " << (int) entry.depth << " type: "
           << (int) entry.type << " score: " << entry.score;
        return os;
    }
    
    void set(U64 p_zobrist, Score p_score, Move p_move, NodeType p_type, Depth p_depth) {
        this->zobrist = p_zobrist;
        this->score   = p_score;
        this->move    = p_move;
        this->type    = p_type;
        this->depth   = p_depth;
    }
    
    NodeAge getAge() {
        return getScore(move);
    }
    
    void setAge(NodeAge p_age) {
        setScore(move, p_age);
    }
    
    
    U64      zobrist;        //64 bit
    Move     move;           //32 bit  (using the 8 msb for age)
    Depth    depth;          //8 bit
    NodeType type;           //8 bit
    Score    score;          //16 bit -> 128 bit = 16 byte
};


class TranspositionTable {

    private:
        NodeAge m_currentAge;
        Entry   *m_entries;
        U64     m_size;
        U64     m_mask;
        
        void init(U64 MB);
    
    public:
        TranspositionTable(U64 mb);
        
        TranspositionTable(const TranspositionTable &other) = delete;
        
        TranspositionTable &operator=(const TranspositionTable &other) = delete;
        
        ~TranspositionTable();
        
        
        Entry get(U64 zobrist);
        
        bool put(U64 zobrist, Score score, Move move, NodeType type, Depth depth);
        
        void incrementAge();
        
        void setSize(U64 mb);
        
        void clear();
        
        double usage();
        
        U64 getSize();
        
};

/**
 * returns the maximum TT size in MB
 * @return
 */
int maxTTSize();

#endif //CHESSCOMPUTER_HASHMAP_H
