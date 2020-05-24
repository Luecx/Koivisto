//
// Created by finne on 5/18/2020.
//

#include "TranspositionTable.h"

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
    
    printBitmap(mask);
    std::cout << size << std::endl;
    
    entries = (Entry *)(calloc(size, sizeof(Entry)));
    clear();
    
}

U64 TranspositionTable::getSize() {
    return size;
}

TranspositionTable::TranspositionTable(U64 mb) {
    init(mb);
}

TranspositionTable::~TranspositionTable() {
    delete entries;
}

void TranspositionTable::setSize(U64 mb) {
    free(entries);
    init(mb);
}

void TranspositionTable::clear() {
    used = 0;
    std::memset(entries, 0, sizeof(Entry) * size);
}

double TranspositionTable::usage() {
    return (double)used / size;
}

ostream &operator<<(ostream &os, const TranspositionTable &map) {
    os << "used: " << map.used << " size: " << map.size;
    return os;
}

Entry *TranspositionTable::get(U64 zobrist) {
    
    U64 index = zobrist & mask;
    
    Entry* enP = &entries[index];
    
    if(enP->zobrist == zobrist){
        return enP;
    }
    
    return nullptr;
}

bool TranspositionTable::put(U64 zobrist, int score, Move move, int type, int depth) {
    
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
