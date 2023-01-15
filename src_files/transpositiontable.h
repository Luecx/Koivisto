
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

#define TT_INDEX(zob, mask) ((zob) & (mask))
#define TT_KEY(zob)         ((zob) >> 32)

/**
 * TTEntry struct represents a transposition table entry.
 * It stores information about a certain position that has been searched
 * in the past, such as the best move, score, depth, type and evaluation score.
 */
struct TTEntry {
    bb::U32    key;      // 32-bit key used to identify the entry
    move::Move move;     // 32-bit move stored in the entry, using the 8 most significant bits for age
    bb::Depth  depth;    // 8-bit depth stored in the entry
    bb::NodeType type;     // 8-bit node type stored in the entry
    bb::Score    score;    // 16-bit score stored in the entry
    bb::Score    eval;     // 16-bit evaluation score stored in the entry

    // set function is used to set the values of the entry
    void set(bb::U32 p_key, bb::Score p_score, move::Move p_move, bb::NodeType p_type,
             bb::Depth p_depth, bb::Score p_eval);

    // getAge function is used to get the age of the entry.
    // @return The age of the entry as a NodeAge value.
    [[nodiscard]] bb::NodeAge getAge() const;

    // setAge function is used to set the age of the entry.
    // @param p_age : the new age of the entry as a NodeAge value.
    void setAge(bb::NodeAge p_age);
};

/**
 * TranspositionTable class is used to store information about positions that have been searched in
 * the past. It uses a hash table to store TTEntry structs, which contain information about the
 * position such as the best move, score, depth, type and evaluation score. The class also includes a
 * current age variable, which is used to keep track of the age of the entries in the table. When the
 * age is incremented, all entries with an age less than the current age will be considered old and
 * will be replaced by new entries.
 */
class TranspositionTable {
    private:
    bb::NodeAge                 m_currentAge;  // variable to keep track of the current age of entries
    bb::U64                     m_size;        // size of the transposition table
    std::unique_ptr<TTEntry[]>  m_entries;     // pointer to an array of TTEntry structs
    bb::U64                     m_mask;        // bit mask used to convert a zobrist key
                                               // to an index in the table
    
    //init function is used to initialize the transposition table with a given size
    void init(bb::U64 MB);

    public:
    // Constructor for TranspositionTable class, it initializes the table with a given size in
    // megabytes
    explicit TranspositionTable(bb::U64 mb);

    // Copy constructor for TranspositionTable class, it is deleted to prevent copying
    TranspositionTable(const TranspositionTable& other) = delete;

    // Assignment operator for TranspositionTable class, it is deleted to prevent assignment
    TranspositionTable& operator=(const TranspositionTable& other) = delete;

    // get function returns the TTEntry associated with a given zobrist key
    [[nodiscard]] TTEntry get(bb::U64 zobrist) const;
    
    // put function is used to add an entry to the transposition table
    bool put(bb::U64 zobrist, bb::Score score, move::Move move, bb::NodeType type, bb::Depth depth,
             bb::Score eval);
    
    // IncrementAge function increments the current age of the transposition table,
    // indicating that a new search iteration has begun.
    void incrementAge();

    // SetSize function sets the size of the transposition table to the given value in MB.
    void setSize(bb::U64 mb);
    
    // getSize function is used to get the size of the transposition table
    [[nodiscard]] bb::U64 getSize() const;
    
    
    // Clear function resets the transposition table to its initial state,
    // clearing all stored entries and resetting the current age.
    void clear();

    // Returns a floating value that represents the usage of the transposition table.
    // The value is computed as the ratio of used entries to the total number of entries.
    // A value of 0 indicates that no entries are used, while a value of 1 indicates that all entries
    // are used.
    [[nodiscard]] double usage() const;
    
    // prefetch function is used to preload the data associated with the given zobrist hash into the
    // cache. This can potentially improve performance by reducing the number of cache misses when
    // accessing the data later on. The function uses the __builtin_prefetch function provided by the
    // compiler, which generates a prefetch instruction for the processor.
    void prefetch(bb::U64 zobrist) const;
};

// returns the maximum TT size in MB
int maxTTSize();

// scoreToTT adjust the score s to be stored in the transposition table.
// This is necessary because the transposition table does not store plies to mate.
// If the score s is a mate score, it is increased by plies.
bb::Score scoreToTT(bb::Score s, int plies);

// scoreFromTT adjust the score s retrieved from the transposition table.
// This is necessary because the transposition table does not store plies to mate.
// If the score s is a mate score, it is decreased by plies.
bb::Score scoreFromTT(bb::Score s, int plies);

#endif    // CHESSCOMPUTER_HASHMAP_H
