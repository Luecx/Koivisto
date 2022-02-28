
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

#ifndef CHESSCOMPUTER_BITMAP_H
#define CHESSCOMPUTER_BITMAP_H

#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <string>

namespace bb {
typedef uint64_t U64;
typedef uint32_t U32;
typedef int8_t   Square;
typedef int8_t   Diagonal;
typedef int8_t   AntiDiagonal;
typedef int8_t   Direction;

typedef int8_t  File;
typedef int8_t  Rank;
typedef int8_t  Piece;
typedef int8_t  PieceType;
typedef bool    Color;

typedef uint8_t Depth;
typedef int16_t Score;
typedef int32_t EvalScore;


enum Plies{
    ONE_PLY          = 1,
    MAX_PLY          = 128,
    MAX_PVSEARCH_PLY = 235,
    MAX_INTERNAL_PLY = 255,
};

enum Scores{
    TB_CURSED_SCORE = static_cast<Score>(1),
    TB_WIN_SCORE    = static_cast<Score>((1 << 13) - MAX_INTERNAL_PLY),
    MAX_MATE_SCORE  = static_cast<Score>((1 << 14) - 1),
    MIN_MATE_SCORE  = static_cast<Score>(MAX_MATE_SCORE - MAX_INTERNAL_PLY),
    TB_FAILED       = MAX_MATE_SCORE
};

enum Colors{
    WHITE,
    BLACK,
    N_COLORS = 2
};

enum PieceTypes{
    PAWN   = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK   = 3,
    QUEEN  = 4,
    KING   = 5,
    N_PIECE_TYPES = 6
};

enum Pieces{
    WHITE_PAWN   = 0,
    WHITE_KNIGHT = 1,
    WHITE_BISHOP = 2,
    WHITE_ROOK   = 3,
    WHITE_QUEEN  = 4,
    WHITE_KING   = 5,
    BLACK_PAWN   = 8,
    BLACK_KNIGHT = 9,
    BLACK_BISHOP = 10,
    BLACK_ROOK   = 11,
    BLACK_QUEEN  = 12,
    BLACK_KING   = 13,
    N_PIECES     = 14
};

enum Squares{
    A1,B1,C1,D1,E1,F1,G1,H1,
    A2,B2,C2,D2,E2,F2,G2,H2,
    A3,B3,C3,D3,E3,F3,G3,H3,
    A4,B4,C4,D4,E4,F4,G4,H4,
    A5,B5,C5,D5,E5,F5,G5,H5,
    A6,B6,C6,D6,E6,F6,G6,H6,
    A7,B7,C7,D7,E7,F7,G7,H7,
    A8,B8,C8,D8,E8,F8,G8,H8,
    N_SQUARES = 64
};

enum Directions{
    NORTH      =  8,
    SOUTH      = -8,
    WEST       = -1,
    EAST       =  1,
    NORTH_WEST =  7,
    NORTH_EAST =  9,
    SOUTH_WEST = -9,
    SOUTH_EAST = -7,
    N_DIRECTIONS = 8
};

enum Ranks{
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    N_RANKS = 8
};

enum Files{
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
    N_FILES = 8
};


constexpr char const* SQUARE_IDENTIFIER[] {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
};

constexpr char PIECE_IDENTIFER[] {'P', 'N', 'B', 'R', 'Q', 'K',' ' ,' ','p', 'n', 'b', 'r', 'q', 'k'};


constexpr U64 ONE  = 1ULL;
constexpr U64 ZERO = 0ULL;

constexpr U64 FILE_H_BB = 0x8080808080808080ULL;
constexpr U64 FILE_G_BB = FILE_H_BB >> 1;
constexpr U64 FILE_F_BB = FILE_H_BB >> 2;
constexpr U64 FILE_E_BB = FILE_H_BB >> 3;
constexpr U64 FILE_D_BB = FILE_H_BB >> 4;
constexpr U64 FILE_C_BB = FILE_H_BB >> 5;
constexpr U64 FILE_B_BB = FILE_H_BB >> 6;
constexpr U64 FILE_A_BB = FILE_H_BB >> 7;

constexpr U64 RANK_1_BB = 0x00000000000000FFULL;
constexpr U64 RANK_2_BB = RANK_1_BB << 8;
constexpr U64 RANK_3_BB = RANK_1_BB << 16;
constexpr U64 RANK_4_BB = RANK_1_BB << 24;
constexpr U64 RANK_5_BB = RANK_1_BB << 32;
constexpr U64 RANK_6_BB = RANK_1_BB << 40;
constexpr U64 RANK_7_BB = RANK_1_BB << 48;
constexpr U64 RANK_8_BB = RANK_1_BB << 56;

constexpr U64 NOT_FILE_A_BB = ~FILE_A_BB;
constexpr U64 NOT_FILE_H_BB = ~FILE_H_BB;
constexpr U64 NOT_RANK_1_BB = ~RANK_1_BB;
constexpr U64 NOT_RANK_8_BB = ~RANK_8_BB;

constexpr U64 OUTER_SQUARES_BB = 0xFF818181818181FFULL;
constexpr U64 WHITE_SQUARES_BB = 0x55AA55AA55AA55AAULL;
constexpr U64 BLACK_SQUARES_BB = ~WHITE_SQUARES_BB;


constexpr U64 CASTLING_WHITE_QUEENSIDE_MASK = 0x000000000000000EULL;
constexpr U64 CASTLING_WHITE_KINGSIDE_MASK  = 0x0000000000000060ULL;
constexpr U64 CASTLING_BLACK_QUEENSIDE_MASK = CASTLING_WHITE_QUEENSIDE_MASK << (7 * 8);
constexpr U64 CASTLING_BLACK_KINGSIDE_MASK  = CASTLING_WHITE_KINGSIDE_MASK << (7 * 8);

// describes the fields that must not be attacked
constexpr U64 CASTLING_WHITE_QUEENSIDE_SAFE = 0x000000000000001CULL;
constexpr U64 CASTLING_WHITE_KINGSIDE_SAFE  = CASTLING_WHITE_QUEENSIDE_SAFE << 2;
constexpr U64 CASTLING_BLACK_QUEENSIDE_SAFE = CASTLING_WHITE_QUEENSIDE_SAFE << (7 * 8);
constexpr U64 CASTLING_BLACK_KINGSIDE_SAFE  = CASTLING_WHITE_KINGSIDE_SAFE << (7 * 8);

extern U64 seed;


extern U64  ALL_HASHES[N_PIECES][N_SQUARES];

extern U64  IN_BETWEEN_SQUARES[N_SQUARES][N_SQUARES];
inline Rank rankIndex(Square square_index) {
    return square_index >> 3;
}

inline File fileIndex(Square square_index) {
    return square_index & 7;
}

inline Square squareIndex(Rank rank, File file) {
    return 8 * rank + file;
}

inline Square squareIndex(std::string& str) {
    Rank r = str.at(1) - '1';
    File f = toupper(str.at(0)) - 'A';
    
    return squareIndex(r, f);
}

inline Diagonal diagonalIndex(const Square& square_index) {
    return 7 + rankIndex(square_index) - fileIndex(square_index);
}

inline AntiDiagonal antiDiagonalIndex(const Square& square_index) {
    return rankIndex(square_index) + fileIndex(square_index);
}

inline Diagonal diagonalIndex(Rank rank, File file) {
    return 7 + rank - file;
}

inline AntiDiagonal antiDiagonalIndex(Rank rank, File file) {
    return rank + file;
}

inline Square mirrorVertically(Square square){
    return square ^ 56;
}

inline Square mirrorHorizontally(Square square){
    return square ^ 7;
}

inline Color getPieceColor(Piece p) {
    return p & 0x8;
}

inline PieceType getPieceType(Piece p) {
    return p & 0x7;
}

inline Piece getPiece(Color c, PieceType pt) {
    return c * 8 + pt;
}

/**
 * toggles the bit
 * @param number    number to manipulate
 * @param index     index of bit starting at the LST
 * @return          the manipulated number
 */
inline void toggleBit(U64& number, Square index) {
    number ^= (1ULL << index);
}

/**
 * set the bit
 * @param number    number to manipulate
 * @param index     index of bit starting at the LST
 * @return          the manipulated number
 */
inline void setBit(U64& number, Square index) {
    number |= (1ULL << index);
}

/**
 * unset the bit
 * @param number    number to manipulate
 * @param index     index of bit starting at the LST
 * @return          the manipulated number
 */
inline void unsetBit(U64& number, Square index) {
    number &= ~(1ULL << index);
}

/**
 * get the bit
 * @param number    number to manipulate
 * @param index     index of bit starting at the LST
 * @return          the manipulated number
 */
inline bool getBit(U64 number, Square index) {
    return ((number >> index) & 1ULL) == 1;
}

inline U64 shiftWest(U64 b) {
    b = (b >> 1) & NOT_FILE_H_BB;
    return b;
}

inline U64 shiftEast(U64 b) {
    b = (b << 1) & NOT_FILE_A_BB;
    return b;
}

inline U64 shiftSouth(U64 b) {
    b = b >> 8;
    return b;
}

inline U64 shiftNorth(U64 b) {
    b = b << 8;
    return b;
}

inline U64 shiftNorthEast(U64 b) {
    b = (b << 9) & NOT_FILE_A_BB;
    return b;
}

inline U64 shiftSouthEast(U64 b) {
    b = (b >> 7) & NOT_FILE_A_BB;
    return b;
}

inline U64 shiftSouthWest(U64 b) {
    b = (b >> 9) & NOT_FILE_H_BB;
    return b;
}

inline U64 shiftNorthWest(U64 b) {
    b = (b << 7) & NOT_FILE_H_BB;
    return b;
}

/**
 * isolates the lsb in the given number and returns the result.
 * @param number
 * @return
 */
inline U64 lsbIsolation(U64 number) {
    return number & -number;
}

/**
 * resets the lsb in the given number and returns the result.
 * @param number
 * @return
 */
inline U64 lsbReset(U64 number) {
    return number & (number - 1);
}

/**
 * prints the given bitboard as a bitmap to the standard output stream
 * @param bb
 */
void       printBitmap(U64 bb);

/**
 * initialises the zobrist keys to random values
 */
void       generateZobristKeys();

/**
 * generates some relevant data
 */
void       generateData();

/**
 * initiates the entries for fancy magic bitboard and zobrist hash values.
 */
void       init();

/**
 * generates a random Bitboard
 * @return
 */
U64        randU64();

/**
 * returns the zobrist hash key for a given piece on a given square.
 * @param piece
 * @param sq
 * @return
 */
inline U64 getHash(Piece piece, Square sq) {
    return ALL_HASHES[piece][sq];
}

/**
 * returns the index of the LSB
 * @param bb
 * @return
 */
inline Square bitscanForward(U64 bb) {
    //    UCI_ASSERT(bb != 0);
    return __builtin_ctzll(bb);
}

/**
 * returns the index of the MSB
 * @param bb
 * @return
 */
inline Square bitscanReverse(U64 bb) {
    //    UCI_ASSERT(bb != 0);
    return __builtin_clzll(bb) ^ 63;
}

/**
 * returns the amount of set bits in the given bitboard.
 * @param bb
 * @return
 */
inline int bitCount(U64 bb) {
    return __builtin_popcountll(bb);
    //        int counter = 0;
    //        while(bb != 0){
    //            bb = lsbReset(bb);
    //            counter ++;
    //        }
    //        return counter;
}

/**
 * The Chebyshev distance is the maximum of the absolute rank- and file-distance of both squares.
 * @param f1
 * @param r1
 * @param f2
 * @param r2
 * @return
 */
inline int chebyshevDistance(File f1, Rank r1, File f2, Rank r2) {
    return std::max(std::abs(r2 - r1), std::abs(f2 - f1));
}

/**
 * The Chebyshev distance is the maximum of the absolute rank- and file-distance of both squares.
 * @param sq1
 * @param sq2
 * @return
 */
inline int chebyshevDistance(Square sq1, Square sq2) {
    File fI1 = fileIndex(sq1);
    Rank rI1 = rankIndex(sq1);
    File fI2 = fileIndex(sq2);
    Rank rI2 = rankIndex(sq2);
    
    return chebyshevDistance(fI1, rI1, fI2, rI2);
}

/**
 * the orthogonal Manhattan-Distance is the sum of both absolute rank- and file-distance distances
 * @param f1
 * @param r1
 * @param f2
 * @param r2
 * @return
 */
inline int manhattanDistance(File f1, Rank r1, File f2, Rank r2) {
    return std::max(std::abs(r2 - r1), std::abs(f2 - f1));
}

/**
 * the orthogonal Manhattan-Distance is the sum of both absolute rank- and file-distance distances
 * @param sq1
 * @param sq2
 * @return
 */
inline int manhattanDistance(Square sq1, Square sq2) {
    File fI1 = fileIndex(sq1);
    Rank rI1 = rankIndex(sq1);
    File fI2 = fileIndex(sq2);
    Rank rI2 = rankIndex(sq2);
    
    return manhattanDistance(fI1, rI1, fI2, rI2);
}
}    // namespace bb

#endif    // CHESSCOMPUTER_BITMAP_H
