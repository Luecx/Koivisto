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

    U64 bytes      = MB * 1024 * 1024;
    U64 maxEntries = bytes / sizeof(TTBucket);

    // size must be a power of 2!
    m_size = 1;

    while (m_size <= maxEntries) {
        m_size *= 2;
    }

    m_size /= 2;
    m_mask = m_size - 1;

    m_entries = (TTBucket*) (calloc(m_size, sizeof(TTBucket)));
    clear();

    m_currentAge = 0;
}

/**
 * returns the maximum amount of entries that can be stored.
 * @return
 */
U64 TranspositionTable::getSize() { return m_size; }

/**
 * constructor which inits the table with a maximum size given by mb.
 * @param mb
 */
TranspositionTable::TranspositionTable(U64 mb) { init(mb); }

/**
 * destructor which deletes the table.
 */
TranspositionTable::~TranspositionTable() { free(m_entries); }

/**
 * clears the content and enlarges the table if possibles to a maximum size of mb
 * @param mb
 */
void TranspositionTable::setSize(U64 mb) {
    free(m_entries);
    init(mb);
}

/**
 * clears the content
 */
void TranspositionTable::clear() { std::memset(m_entries, 0, sizeof(Entry) * m_size); }

/**
 * returns a floating value for the amount of values used.
 * if it returns 0, no value is stored and if it returns 1, it is full.
 */
double TranspositionTable::usage() {

    double used = 0;
    // Thank you Andrew for this idea :)
    for (U64 i = 0; i < 100; i++) {
        if ((m_entries[i].entries[i/25].zobrist)) {
            used++;
        }
    }

    return used / 100;
}

/**
 * returns the Entry for the given key.
 * if there is no Entry found, it returns a nullptr.
 * @param zobrist
 * @return
 */
Entry TranspositionTable::get(U64 zobrist) {

    U64 index = zobrist & m_mask;

    for (int i = 0; i < TT_BUCKET_SIZE; i++){
        if (m_entries[index].entries[i].zobrist == zobrist) return m_entries[index].entries[i];
    }

    return m_entries[index].entries[0];
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

    U64 index = zobrist & m_mask;


    Entry* candidate;
    Entry* replaceThis = nullptr;
    Depth lowestDepth = MAX_INTERNAL_PLY;
    for (int i = 0; i < TT_BUCKET_SIZE; i++){
        candidate = &m_entries[index].entries[i];
        if (candidate->zobrist == zobrist) {
            replaceThis = candidate;
            break;
        }
        if (candidate->depth <= lowestDepth && (candidate->getAge() != m_currentAge || (candidate->depth<=depth && candidate->type!= PV_NODE) ||type == PV_NODE)){
            replaceThis = candidate;
            lowestDepth = candidate->depth;
        }
    }

    if (replaceThis == nullptr) return false;

    if (replaceThis->zobrist==zobrist && depth<replaceThis->depth-2 && type != PV_NODE) return false;

    replaceThis->set(zobrist, score, move, type, depth);
    replaceThis->setAge(m_currentAge);

    return true;
}

void TranspositionTable::incrementAge() {
    TranspositionTable::m_currentAge++;
    if (TranspositionTable::m_currentAge == 255) {
        TranspositionTable::m_currentAge = 0;
    }
}

/**
 * returns the maximum TT size in MB
 * @return
 */
int maxTTSize() { return (ONE << (32 - 20)) * sizeof(Entry); }
