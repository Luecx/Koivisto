//
// Created by finne on 5/18/2020.
//

#ifndef CHESSCOMPUTER_HASHMAP_H
#define CHESSCOMPUTER_HASHMAP_H



#include <stdint.h>
#include <ostream>

#include "Bitboard.h"
#include "Move.h"
#include "Board.h"

using namespace bb;
using namespace std;
using namespace move;

typedef uint8_t NodeType;

constexpr NodeType PV_NODE = 0;
constexpr NodeType CUT_NODE = 1;
constexpr NodeType ALL_NODE = 2;


struct Entry{
    
//    Entry() = default;
//    ~Entry() = default;
    
    friend ostream &operator<<(ostream &os, const Entry &entry) {
        os << "zobrist: " << entry.zobrist << " move: " << entry.move << " depth: " << (int)entry.depth << " type: "
           << (int)entry.type << " score: " << entry.score;
        return os;
    }
    
    void set(U64 zobrist, Score score, Move move, NodeType type, Depth depth){
        this->zobrist = zobrist;
        this->score = score;
        this->move = move;
        this->type = type;
        this->depth = depth;
    }
    
    U64         zobrist;        //64 bit
    Move        move;           //32 bit
    Depth       depth;          //8 bit
    NodeType    type;           //8 bit
    Score       score;          //16 bit -> 128 bit = 16 byte
};


class TranspositionTable {
    
    private:
        Entry* entries;
        U64 used;
        U64 size;
        U64 mask;
        
        void init(U64 MB);
    
    public:
        TranspositionTable(U64 mb);
        TranspositionTable(const TranspositionTable &other) = delete;
        TranspositionTable& operator=(const TranspositionTable &other) = delete;
        ~TranspositionTable();
        
        Entry* get(U64 zobrist);
        bool put(U64 zobrist, Score score, Move move, NodeType type, Depth depth);
        
        void clear();
        U64 getSize();
        void setSize(U64 mb);
        double usage();
        int entryCount();
        
        friend ostream &operator<<(ostream &os, const TranspositionTable &map);
        
};

/**
 * returns the maximum TT size in MB
 * @return
 */
int maxTTSize();

#endif //CHESSCOMPUTER_HASHMAP_H
