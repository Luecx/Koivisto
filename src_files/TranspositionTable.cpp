//
// Created by finne on 5/18/2020.
//

#include "TranspositionTable.h"

/**
 * inits the table to the given size.
 * Calculates the amount of entries that can fit.
 * @param MB
 */
void TranspositionTable::init(U64 MB) {

    U64 bytes = MB * 1024 * 1024;
    U64 maxEntries = bytes / sizeof(Entry);
    
    //size must be a power of 2!
    size = 1;
    
    while(size <= maxEntries){
        size *= 2;
    }
    
    size /= 2;
    mask = size-1;
    
    
    entries = (Entry *)(calloc(size, sizeof(Entry)));
    clear();
    
}

/**
 * returns the maximum amount of entries that can be stored.
 * @return
 */
U64 TranspositionTable::getSize() {
    return size;
}

/**
 * constructor which inits the table with a maximum size given by mb.
 * @param mb
 */
TranspositionTable::TranspositionTable(U64 mb) {
    init(mb);
}

/**
 * destructor which deletes the table.
 */
TranspositionTable::~TranspositionTable() {
    delete entries;
}

/**
 * clears the content and enlarges the table if possibles to a maximum size of mb
 * @param mb
 */
void TranspositionTable::setSize(U64 mb) {
    free(entries);
    init(mb);
}

/**
 * clears the content
 */
void TranspositionTable::clear() {
    used = 0;
    std::memset(entries, 0, sizeof(Entry) * size);
}

/**
 * returns a floating value for the amount of values used.
 * if it returns 0, no value is stored and if it returns 1, it is full.
 */
double TranspositionTable::usage() {
    return (double)used / size;
}

/**
 * returns the amount of entries stored in the table
 * @return
 */
int TranspositionTable::entryCount() {
    return used;
}

/**
 * some basic output-stream appending.
 * @param os
 * @param map
 * @return
 */
ostream &operator<<(ostream &os, const TranspositionTable &map) {
    os << "used: " << map.used << " size: " << map.size;
    return os;
}

/**
 * returns the Entry for the given key.
 * if there is no Entry found, it returns a nullptr.
 * @param zobrist
 * @return
 */
Entry *TranspositionTable::get(U64 zobrist) {
    
    U64 index = zobrist & mask;
    
    Entry* enP = &entries[index];
    
    if(enP->zobrist == zobrist){
        return enP;
    }
    
    return nullptr;
}

/**
 * puts the given Score, Move, NodeType, Depth into the table if possible.
 * if there is already a value, it will only override if the following is fulfilled:
 *  - a non-pv value cannot override a pv value
 *  - a pv node will always override a pv node
 *  - a non-pv value will only override a non-pv value if the oldDepth <= newDepth
 *
 *
 * @param zobrist
 * @param score
 * @param move
 * @param type
 * @param depth
 * @return
 */
bool TranspositionTable::put(U64 zobrist, Score score, Move move, NodeType type, Depth depth) {
    
    U64 index = zobrist & mask;
    
    Entry *enP = &entries[index];
    
    if (enP->zobrist == 0) {
        enP->set(zobrist, score, move, type, depth);
        used ++;
        return true;
    }else{
        if(type == PV_NODE || enP->type != PV_NODE && enP->depth <= depth){
            enP->set(zobrist, score, move, type, depth);
            return true;
        }
    }
    
    return false;
    
    
}
