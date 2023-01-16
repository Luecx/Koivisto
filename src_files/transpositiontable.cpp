
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

#include "transpositiontable.h"

#include <cstring>

/**
 * set function is used to set the values of the entry
 * @param p_key : the zobrist hash of the position
 * @param p_score : the score of the position
 * @param p_move : the best move found for the position
 * @param p_type : the type of node that was searched
 * @param p_depth : the depth at which the entry was stored
 * @param p_eval : the evaluation score of the position
 */
void TTEntry::set(bb::U32 p_key, bb::Score p_score, move::Move p_move, bb::NodeType p_type,
                  bb::Depth p_depth, bb::Score p_eval) {
    // set all the values of the entry
    this->key     = p_key;
    this->score   = p_score;
    this->move    = p_move;
    this->type    = p_type;
    this->depth   = p_depth;
    this->eval    = p_eval;
}

/**
 * getAge function is used to get the age of the entry
 * @return age of the entry
 */
bb::NodeAge TTEntry::getAge() const { return move::getScore(move); }

/**
 * setAge function is used to set the age of the entry
 * @param p_age : the age to set
 */
void TTEntry::setAge(bb::NodeAge p_age) { move::setScore(move, p_age); }

/**
 * inits the table to the given size.
 * Calculates the amount of entries that can fit.
 * @param MB
 */
void TranspositionTable::init(bb::U64 MB) {
    bb::U64 bytes      = MB * 1024 * 1024;
    bb::U64 maxEntries = bytes / sizeof(TTEntry);

    // size must be a power of 2!
    m_size = 1;

    while (m_size <= maxEntries) {
        m_size *= 2;
    }

    m_size /= 2;
    m_mask = m_size - 1;

    m_entries = std::make_unique<TTEntry[]>(m_size);
    clear();

    m_currentAge = 0;
}

/**
 * TranspositionTable constructor, initializes the table with a maximum size given by the input
 * parameter 'mb'.
 * @param mb : the maximum size of the transposition table in MB
 */
TranspositionTable::TranspositionTable(bb::U64 mb) { init(mb); }

/**
 * get function returns the TTEntry associated with a given zobrist key
 * @param zobrist : the zobrist key of the position
 * @return TTEntry : the transposition table entry associated with the given key
 */
TTEntry TranspositionTable::get(bb::U64 zobrist) const {
    bb::U64 index = zobrist & m_mask;
    
    TTEntry enP = m_entries[index];

    return enP;
}


/**
 * puts the given Score, Move, NodeType, Depth into the table if possible.
 * if there is already a value, it will only override if the following is fulfilled:
 *  - a non-pv value cannot override a pv value
 *  - a pv node will always override a pv node
 *  - a non-pv value will only override a non-pv value if the oldDepth <= newDepth
 *
 * @param zobrist : the zobrist hash of the position
 * @param score : the score of the position
 * @param move : the best move found for the position
 * @param type : the type of node that was searched
 * @param depth : the depth at which the entry was stored
 * @param eval : the evaluation score of the position
 *
 * @return true if the entry was added/updated, false otherwise
 */
bool TranspositionTable::put(bb::U64 zobrist, bb::Score score, move::Move move, bb::NodeType type,
                             bb::Depth depth, bb::Score eval) {
    // calculate the index in the table where the entry should be stored
    bb::U64 index  = TT_INDEX(zobrist, m_mask);
    // extract the 32-bit key from the 64-bit zobrist hash
    bb::U32 key    = TT_KEY(zobrist);
    
    // pointer to the entry at the calculated index
    TTEntry* enP = &m_entries[index];
    
    // check if the entry at this index is empty
    if (enP->key == 0) {
        // if it is, set the values and the age
        enP->set(key, score, move, type, depth, eval);
        enP->setAge(m_currentAge);
        return true;
    } else {
        // if the entry is not empty, check if it should be overridden
        if (   enP->getAge() != m_currentAge                                // check if the age is different, if it is, override
            || type          == bb::PV_NODE                                 // PV nodes always override
            || (enP->type    != bb::PV_NODE && enP->depth <= depth)         // non-PV nodes only override if the depth is greater
            || (enP->key     == key         && enP->depth <= depth + 3)) {  // if the key is equal and the depth is greater, override
            // set the new values and age
            enP->set(key, score, move, type, depth, eval);
            enP->setAge(m_currentAge);
            return true;
        }
    }

    // if the entry is not overridden, return false
    return false;
}

/**
 * Increments the age of the transposition table.
 * To avoid overflowing as only 8 bits are used, we wrap at 255 and go back to 0.
 */
void TranspositionTable::incrementAge() {
    TranspositionTable::m_currentAge++;
    if (TranspositionTable::m_currentAge == 255) {
        TranspositionTable::m_currentAge = 0;
    }
}

/**
 * setSize function is used to resize the transposition table. It clears the current content and
 * enlarges the table if possible to the maximum size given by the input parameter 'mb'.
 * @param mb : the new maximum size of the transposition table in MB
 */
void TranspositionTable::setSize(bb::U64 mb) {
    init(mb);
}

/**
 * getSize function returns the current size of the transposition table, which is the maximum number
 * of entries that can be stored.
 * @return bb::U64 : the current size of the transposition table
 */
bb::U64 TranspositionTable::getSize() const { return m_size; }

/**
 * clear function is used to clear the current content of the transposition table.
 * It sets all entries to 0, effectively resetting the table.
 */
void TranspositionTable::clear() {
    if (m_entries) {
        // memset is a C library function that sets a block of memory to a specific value.
        // In this case, it is setting the memory block of size sizeof(TTEntry) * m_size to 0.
        // m_entries.get() returns a pointer to the underlying data, which is passed as the first
        // argument to memset.
        std::memset(m_entries.get(), 0, sizeof(TTEntry) * m_size);
    }
    
}

/**
 * Returns a floating value between 0 and 1 representing the usage of the transposition table.
 * 0 indicates no entries are stored, while 1 indicates the table is full.
 */
double TranspositionTable::usage() const {
    // Initialize a variable to keep track of the number of used entries
    double used = 0;
    // Sample 100 entries from the transposition table
    for (bb::U64 i = 0; i < 100; i++) {
        // Check if the current entry has a non-zero key, indicating it is being used
        if ((m_entries[i].key)) {
            // Increment the used counter
            used++;
        }
    }
    
    // Return the percentage of used entries by dividing the number of used entries by the total number of sampled entries
    return used / 100;

}


/**
 * prefetch function is used to preload the data associated with the given zobrist hash into the
 * cache. This can potentially improve performance by reducing the number of cache misses when
 * accessing the data later on. The function uses the __builtin_prefetch function provided by the
 * compiler, which generates a prefetch instruction for the processor.
 * @param zobrist: zobrist hash of the position that we want to prefetch the data for.
 */
void TranspositionTable::prefetch(bb::U64 zobrist) const {
    __builtin_prefetch(&m_entries[zobrist & m_mask]);
}

/**
 * returns the maximum TT size in MB
 * @return
 */
int maxTTSize() { return (bb::ONE << (32 - 20)) * sizeof(TTEntry); }


/**
 * scoreToTT adjust the score s to be stored in the transposition table.
 * This is necessary because the transposition table does not store plies to mate.
 * If the score s is a mate score, it is increased by plies.
 * @param s the score to be stored
 * @param plies the number of plies to be added to mate scores
 * @return the adjusted score
 */
bb::Score scoreToTT(bb::Score s, int plies)
{
    if (s >= bb::TB_WIN_SCORE_MIN) {
        return s + plies;
    }
    else if (s <= -bb::TB_WIN_SCORE_MIN) {
        return s - plies;
    }
    else {
        return s;
    }
}

/**
 * scoreFromTT adjust the score s retrieved from the transposition table.
 * This is necessary because the transposition table does not store plies to mate.
 * If the score s is a mate score, it is decreased by plies.
 * @param s the score to be retrieved
 * @param plies the number of plies to be subtracted from mate scores
 * @return the adjusted score
 */
bb::Score scoreFromTT(bb::Score s, int plies)
{
    if (s >= bb::TB_WIN_SCORE_MIN) {
        return s - plies;
    }
    else if (s <= -bb::TB_WIN_SCORE_MIN) {
        return s + plies;
    }
    else {
        return s;
    }
}
