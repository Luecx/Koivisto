
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

#define mirrorSquare(s) (bb::squareIndex(7 - bb::rankIndex(s), bb::fileIndex(s)))

typedef uint64_t U64;
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
    MAX_INTERNAL_PLY = 255,
};

enum Scores{
    TB_CURSED_SCORE = (Score)(1),
    TB_WIN_SCORE    = (Score)((1 << 13) - MAX_INTERNAL_PLY),
    MAX_MATE_SCORE  = (Score)((1 << 14) - 1),
    MIN_MATE_SCORE  = (Score)(MAX_MATE_SCORE - MAX_INTERNAL_PLY),
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


constexpr U64 ONE  = (U64) 1;
constexpr U64 ZERO = (U64) 0;

constexpr U64 FILE_H_BB = 0x8080808080808080L;
constexpr U64 FILE_G_BB = FILE_H_BB >> 1;
constexpr U64 FILE_F_BB = FILE_H_BB >> 2;
constexpr U64 FILE_E_BB = FILE_H_BB >> 3;
constexpr U64 FILE_D_BB = FILE_H_BB >> 4;
constexpr U64 FILE_C_BB = FILE_H_BB >> 5;
constexpr U64 FILE_B_BB = FILE_H_BB >> 6;
constexpr U64 FILE_A_BB = FILE_H_BB >> 7;

constexpr U64 RANK_1_BB = 0x00000000000000FFL;
constexpr U64 RANK_2_BB = RANK_1_BB << 8;
constexpr U64 RANK_3_BB = RANK_1_BB << 16;
constexpr U64 RANK_4_BB = RANK_1_BB << 24;
constexpr U64 RANK_5_BB = RANK_1_BB << 32;
constexpr U64 RANK_6_BB = RANK_1_BB << 40;
constexpr U64 RANK_7_BB = RANK_1_BB << 48;
constexpr U64 RANK_8_BB = RANK_1_BB << 56;

constexpr U64 ANTI_DIAGONAL_7_BB = 0x102040810204080L;
constexpr U64 ANTI_DIAGONAL_6_BB = ANTI_DIAGONAL_7_BB << 8;
constexpr U64 ANTI_DIAGONAL_5_BB = ANTI_DIAGONAL_7_BB << 16;
constexpr U64 ANTI_DIAGONAL_4_BB = ANTI_DIAGONAL_7_BB << 24;
constexpr U64 ANTI_DIAGONAL_3_BB = ANTI_DIAGONAL_7_BB << 32;
constexpr U64 ANTI_DIAGONAL_2_BB = ANTI_DIAGONAL_7_BB << 40;
constexpr U64 ANTI_DIAGONAL_1_BB = ANTI_DIAGONAL_7_BB << 48;
constexpr U64 ANTI_DIAGONAL_0_BB = ANTI_DIAGONAL_7_BB << 56;

constexpr U64 ANTI_DIAGONAL_8_BB  = ANTI_DIAGONAL_7_BB >> 8;
constexpr U64 ANTI_DIAGONAL_9_BB  = ANTI_DIAGONAL_7_BB >> 16;
constexpr U64 ANTI_DIAGONAL_10_BB = ANTI_DIAGONAL_7_BB >> 24;
constexpr U64 ANTI_DIAGONAL_11_BB = ANTI_DIAGONAL_7_BB >> 32;
constexpr U64 ANTI_DIAGONAL_12_BB = ANTI_DIAGONAL_7_BB >> 40;
constexpr U64 ANTI_DIAGONAL_13_BB = ANTI_DIAGONAL_7_BB >> 48;
constexpr U64 ANTI_DIAGONAL_14_BB = ANTI_DIAGONAL_7_BB >> 56;

constexpr U64 DIAGONAL_7_BB = 0x8040201008040201L;
constexpr U64 DIAGONAL_8_BB  = DIAGONAL_7_BB >> 8;
constexpr U64 DIAGONAL_9_BB  = DIAGONAL_7_BB >> 16;
constexpr U64 DIAGONAL_10_BB = DIAGONAL_7_BB >> 24;
constexpr U64 DIAGONAL_11_BB = DIAGONAL_7_BB >> 32;
constexpr U64 DIAGONAL_12_BB = DIAGONAL_7_BB >> 40;
constexpr U64 DIAGONAL_13_BB = DIAGONAL_7_BB >> 48;
constexpr U64 DIAGONAL_14_BB = DIAGONAL_7_BB >> 56;

constexpr U64 DIAGONAL_6_BB = DIAGONAL_7_BB << 8;
constexpr U64 DIAGONAL_5_BB = DIAGONAL_7_BB << 16;
constexpr U64 DIAGONAL_4_BB = DIAGONAL_7_BB << 24;
constexpr U64 DIAGONAL_3_BB = DIAGONAL_7_BB << 32;
constexpr U64 DIAGONAL_2_BB = DIAGONAL_7_BB << 40;
constexpr U64 DIAGONAL_1_BB = DIAGONAL_7_BB << 48;
constexpr U64 DIAGONAL_0_BB = DIAGONAL_7_BB << 56;

constexpr U64 NOT_FILE_A_BB = ~FILE_A_BB;
constexpr U64 NOT_FILE_H_BB = ~FILE_H_BB;
constexpr U64 NOT_RANK_1_BB = ~RANK_1_BB;
constexpr U64 NOT_RANK_8_BB = ~RANK_8_BB;

constexpr U64 CIRCLE_A_BB = 0xFF818181818181FFL;
constexpr U64 CIRCLE_B_BB = 0x7E424242427E00L;
constexpr U64 CIRCLE_C_BB = 0x3C24243C0000L;
constexpr U64 CIRCLE_D_BB = 0x1818000000L;

constexpr U64 WHITE_SQUARES_BB = 0x55AA55AA55AA55AA;
constexpr U64 BLACK_SQUARES_BB = ~WHITE_SQUARES_BB;

constexpr U64  CENTER_SQUARES_BB          = CIRCLE_D_BB;
constexpr U64  CENTER_SQUARES_EXTENDED_BB = CIRCLE_C_BB | CIRCLE_D_BB;

constexpr U64  RANKS_BB[] {RANK_1_BB, RANK_2_BB, RANK_3_BB, RANK_4_BB,
                           RANK_5_BB, RANK_6_BB, RANK_7_BB, RANK_8_BB};
constexpr U64  FILES_BB[] {FILE_A_BB, FILE_B_BB, FILE_C_BB, FILE_D_BB,
                           FILE_E_BB, FILE_F_BB, FILE_G_BB, FILE_H_BB};
constexpr U64  FILES_NEIGHBOUR_BB[] {FILE_B_BB,
                                     FILE_A_BB | FILE_C_BB,
                                     FILE_B_BB | FILE_D_BB,
                                     FILE_C_BB | FILE_E_BB,
                                     FILE_D_BB | FILE_F_BB,
                                     FILE_E_BB | FILE_G_BB,
                                     FILE_F_BB | FILE_H_BB,
                                     FILE_G_BB};
constexpr U64  CIRCLES_BB[] {CIRCLE_A_BB, CIRCLE_B_BB, CIRCLE_C_BB, CIRCLE_D_BB};
constexpr U64  DIAGONALS_BB[] {DIAGONAL_0_BB,  DIAGONAL_1_BB,  DIAGONAL_2_BB,  DIAGONAL_3_BB,
                               DIAGONAL_4_BB,  DIAGONAL_5_BB,  DIAGONAL_6_BB,  DIAGONAL_7_BB,
                               DIAGONAL_8_BB,  DIAGONAL_9_BB,  DIAGONAL_10_BB, DIAGONAL_11_BB,
                               DIAGONAL_12_BB, DIAGONAL_13_BB, DIAGONAL_14_BB};
constexpr U64  ANTI_DIAGONALS_BB[] {ANTI_DIAGONAL_0_BB,  ANTI_DIAGONAL_1_BB,  ANTI_DIAGONAL_2_BB,
                                    ANTI_DIAGONAL_3_BB,  ANTI_DIAGONAL_4_BB,  ANTI_DIAGONAL_5_BB,
                                    ANTI_DIAGONAL_6_BB,  ANTI_DIAGONAL_7_BB,  ANTI_DIAGONAL_8_BB,
                                    ANTI_DIAGONAL_9_BB,  ANTI_DIAGONAL_10_BB, ANTI_DIAGONAL_11_BB,
                                    ANTI_DIAGONAL_12_BB, ANTI_DIAGONAL_13_BB, ANTI_DIAGONAL_14_BB};

constexpr U64 CASTLING_WHITE_QUEENSIDE_MASK = 0x000000000000000EL;
constexpr U64 CASTLING_WHITE_KINGSIDE_MASK  = 0x0000000000000060L;
constexpr U64 CASTLING_BLACK_QUEENSIDE_MASK = CASTLING_WHITE_QUEENSIDE_MASK << (7 * 8);
constexpr U64 CASTLING_BLACK_KINGSIDE_MASK  = CASTLING_WHITE_KINGSIDE_MASK << (7 * 8);

// describes the fields that must not be attacked
constexpr U64 CASTLING_WHITE_QUEENSIDE_SAFE = 0x000000000000001CL;
constexpr U64 CASTLING_WHITE_KINGSIDE_SAFE  = CASTLING_WHITE_QUEENSIDE_SAFE << 2;
constexpr U64 CASTLING_BLACK_QUEENSIDE_SAFE = CASTLING_WHITE_QUEENSIDE_SAFE << (7 * 8);
constexpr U64 CASTLING_BLACK_KINGSIDE_SAFE  = CASTLING_WHITE_KINGSIDE_SAFE << (7 * 8);

constexpr U64 CASTLING_MASKS[] = {CASTLING_WHITE_QUEENSIDE_MASK, CASTLING_WHITE_KINGSIDE_MASK,
                                  CASTLING_BLACK_QUEENSIDE_MASK, CASTLING_BLACK_KINGSIDE_MASK};

constexpr U64 seed = 12398123;

constexpr U64  bishopMasks[] {
    0x0040201008040200L, 0x0000402010080400L, 0x0000004020100a00L, 0x0000000040221400L,
    0x0000000002442800L, 0x0000000204085000L, 0x0000020408102000L, 0x0002040810204000L,
    0x0020100804020000L, 0x0040201008040000L, 0x00004020100a0000L, 0x0000004022140000L,
    0x0000000244280000L, 0x0000020408500000L, 0x0002040810200000L, 0x0004081020400000L,
    0x0010080402000200L, 0x0020100804000400L, 0x004020100a000a00L, 0x0000402214001400L,
    0x0000024428002800L, 0x0002040850005000L, 0x0004081020002000L, 0x0008102040004000L,
    0x0008040200020400L, 0x0010080400040800L, 0x0020100a000a1000L, 0x0040221400142200L,
    0x0002442800284400L, 0x0004085000500800L, 0x0008102000201000L, 0x0010204000402000L,
    0x0004020002040800L, 0x0008040004081000L, 0x00100a000a102000L, 0x0022140014224000L,
    0x0044280028440200L, 0x0008500050080400L, 0x0010200020100800L, 0x0020400040201000L,
    0x0002000204081000L, 0x0004000408102000L, 0x000a000a10204000L, 0x0014001422400000L,
    0x0028002844020000L, 0x0050005008040200L, 0x0020002010080400L, 0x0040004020100800L,
    0x0000020408102000L, 0x0000040810204000L, 0x00000a1020400000L, 0x0000142240000000L,
    0x0000284402000000L, 0x0000500804020000L, 0x0000201008040200L, 0x0000402010080400L,
    0x0002040810204000L, 0x0004081020400000L, 0x000a102040000000L, 0x0014224000000000L,
    0x0028440200000000L, 0x0050080402000000L, 0x0020100804020000L, 0x0040201008040200L};

constexpr U64 rookMasks[] {
    0x000101010101017eL, 0x000202020202027cL, 0x000404040404047aL, 0x0008080808080876L,
    0x001010101010106eL, 0x002020202020205eL, 0x004040404040403eL, 0x008080808080807eL,
    0x0001010101017e00L, 0x0002020202027c00L, 0x0004040404047a00L, 0x0008080808087600L,
    0x0010101010106e00L, 0x0020202020205e00L, 0x0040404040403e00L, 0x0080808080807e00L,
    0x00010101017e0100L, 0x00020202027c0200L, 0x00040404047a0400L, 0x0008080808760800L,
    0x00101010106e1000L, 0x00202020205e2000L, 0x00404040403e4000L, 0x00808080807e8000L,
    0x000101017e010100L, 0x000202027c020200L, 0x000404047a040400L, 0x0008080876080800L,
    0x001010106e101000L, 0x002020205e202000L, 0x004040403e404000L, 0x008080807e808000L,
    0x0001017e01010100L, 0x0002027c02020200L, 0x0004047a04040400L, 0x0008087608080800L,
    0x0010106e10101000L, 0x0020205e20202000L, 0x0040403e40404000L, 0x0080807e80808000L,
    0x00017e0101010100L, 0x00027c0202020200L, 0x00047a0404040400L, 0x0008760808080800L,
    0x00106e1010101000L, 0x00205e2020202000L, 0x00403e4040404000L, 0x00807e8080808000L,
    0x007e010101010100L, 0x007c020202020200L, 0x007a040404040400L, 0x0076080808080800L,
    0x006e101010101000L, 0x005e202020202000L, 0x003e404040404000L, 0x007e808080808000L,
    0x7e01010101010100L, 0x7c02020202020200L, 0x7a04040404040400L, 0x7608080808080800L,
    0x6e10101010101000L, 0x5e20202020202000L, 0x3e40404040404000L, 0x7e80808080808000L};

constexpr int bishopShifts[] {58, 59, 59, 59, 59, 59, 59, 58, 59, 59, 59, 59, 59, 59, 59, 59,
                              59, 59, 57, 57, 57, 57, 59, 59, 59, 59, 57, 55, 55, 57, 59, 59,
                              59, 59, 57, 55, 55, 57, 59, 59, 59, 59, 57, 57, 57, 57, 59, 59,
                              59, 59, 59, 59, 59, 59, 59, 59, 58, 59, 59, 59, 59, 59, 59, 58};

constexpr int rookShifts[] {52, 53, 53, 53, 53, 53, 53, 52, 53, 54, 54, 54, 54, 54, 54, 53,
                            53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53,
                            53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53,
                            53, 54, 54, 54, 54, 54, 54, 53, 52, 53, 53, 53, 53, 53, 53, 52};

constexpr U64 bishopMagics[] {0x0002020202020200L, 0x0002020202020000L, 0x0004010202000000L,
                              0x0004040080000000L, 0x0001104000000000L, 0x0000821040000000L,
                              0x0000410410400000L, 0x0000104104104000L, 0x0000040404040400L,
                              0x0000020202020200L, 0x0000040102020000L, 0x0000040400800000L,
                              0x0000011040000000L, 0x0000008210400000L, 0x0000004104104000L,
                              0x0000002082082000L, 0x0004000808080800L, 0x0002000404040400L,
                              0x0001000202020200L, 0x0000800802004000L, 0x0000800400A00000L,
                              0x0000200100884000L, 0x0000400082082000L, 0x0000200041041000L,
                              0x0002080010101000L, 0x0001040008080800L, 0x0000208004010400L,
                              0x0000404004010200L, 0x0000840000802000L, 0x0000404002011000L,
                              0x0000808001041000L, 0x0000404000820800L, 0x0001041000202000L,
                              0x0000820800101000L, 0x0000104400080800L, 0x0000020080080080L,
                              0x0000404040040100L, 0x0000808100020100L, 0x0001010100020800L,
                              0x0000808080010400L, 0x0000820820004000L, 0x0000410410002000L,
                              0x0000082088001000L, 0x0000002011000800L, 0x0000080100400400L,
                              0x0001010101000200L, 0x0002020202000400L, 0x0001010101000200L,
                              0x0000410410400000L, 0x0000208208200000L, 0x0000002084100000L,
                              0x0000000020880000L, 0x0000001002020000L, 0x0000040408020000L,
                              0x0004040404040000L, 0x0002020202020000L, 0x0000104104104000L,
                              0x0000002082082000L, 0x0000000020841000L, 0x0000000000208800L,
                              0x0000000010020200L, 0x0000000404080200L, 0x0000040404040400L,
                              0x0002020202020200L
    
};
constexpr U64 rookMagics[] {
    0x0080001020400080L, 0x0040001000200040L, 0x0080081000200080L, 0x0080040800100080L,
    0x0080020400080080L, 0x0080010200040080L, 0x0080008001000200L, 0x0080002040800100L,
    0x0000800020400080L, 0x0000400020005000L, 0x0000801000200080L, 0x0000800800100080L,
    0x0000800400080080L, 0x0000800200040080L, 0x0000800100020080L, 0x0000800040800100L,
    0x0000208000400080L, 0x0000404000201000L, 0x0000808010002000L, 0x0000808008001000L,
    0x0000808004000800L, 0x0000808002000400L, 0x0000010100020004L, 0x0000020000408104L,
    0x0000208080004000L, 0x0000200040005000L, 0x0000100080200080L, 0x0000080080100080L,
    0x0000040080080080L, 0x0000020080040080L, 0x0000010080800200L, 0x0000800080004100L,
    0x0000204000800080L, 0x0000200040401000L, 0x0000100080802000L, 0x0000080080801000L,
    0x0000040080800800L, 0x0000020080800400L, 0x0000020001010004L, 0x0000800040800100L,
    0x0000204000808000L, 0x0000200040008080L, 0x0000100020008080L, 0x0000080010008080L,
    0x0000040008008080L, 0x0000020004008080L, 0x0000010002008080L, 0x0000004081020004L,
    0x0000204000800080L, 0x0000200040008080L, 0x0000100020008080L, 0x0000080010008080L,
    0x0000040008008080L, 0x0000020004008080L, 0x0000800100020080L, 0x0000800041000080L,
    0x00FFFCDDFCED714AL, 0x007FFCDDFCED714AL, 0x003FFFCDFFD88096L, 0x0000040810002101L,
    0x0001000204080011L, 0x0001000204000801L, 0x0001000082000401L, 0x0001FFFAABFAD1A2L};

constexpr U64 KING_ATTACKS[] {
    0x0000000000000302L, 0x0000000000000705L, 0x0000000000000e0aL, 0x0000000000001c14L,
    0x0000000000003828L, 0x0000000000007050L, 0x000000000000e0a0L, 0x000000000000c040L,
    0x0000000000030203L, 0x0000000000070507L, 0x00000000000e0a0eL, 0x00000000001c141cL,
    0x0000000000382838L, 0x0000000000705070L, 0x0000000000e0a0e0L, 0x0000000000c040c0L,
    0x0000000003020300L, 0x0000000007050700L, 0x000000000e0a0e00L, 0x000000001c141c00L,
    0x0000000038283800L, 0x0000000070507000L, 0x00000000e0a0e000L, 0x00000000c040c000L,
    0x0000000302030000L, 0x0000000705070000L, 0x0000000e0a0e0000L, 0x0000001c141c0000L,
    0x0000003828380000L, 0x0000007050700000L, 0x000000e0a0e00000L, 0x000000c040c00000L,
    0x0000030203000000L, 0x0000070507000000L, 0x00000e0a0e000000L, 0x00001c141c000000L,
    0x0000382838000000L, 0x0000705070000000L, 0x0000e0a0e0000000L, 0x0000c040c0000000L,
    0x0003020300000000L, 0x0007050700000000L, 0x000e0a0e00000000L, 0x001c141c00000000L,
    0x0038283800000000L, 0x0070507000000000L, 0x00e0a0e000000000L, 0x00c040c000000000L,
    0x0302030000000000L, 0x0705070000000000L, 0x0e0a0e0000000000L, 0x1c141c0000000000L,
    0x3828380000000000L, 0x7050700000000000L, 0xe0a0e00000000000L, 0xc040c00000000000L,
    0x0203000000000000L, 0x0507000000000000L, 0x0a0e000000000000L, 0x141c000000000000L,
    0x2838000000000000L, 0x5070000000000000L, 0xa0e0000000000000L, 0x40c0000000000000L};
constexpr U64 KNIGHT_ATTACKS[] {
    0x0000000000020400L, 0x0000000000050800L, 0x00000000000a1100L, 0x0000000000142200L,
    0x0000000000284400L, 0x0000000000508800L, 0x0000000000a01000L, 0x0000000000402000L,
    0x0000000002040004L, 0x0000000005080008L, 0x000000000a110011L, 0x0000000014220022L,
    0x0000000028440044L, 0x0000000050880088L, 0x00000000a0100010L, 0x0000000040200020L,
    0x0000000204000402L, 0x0000000508000805L, 0x0000000a1100110aL, 0x0000001422002214L,
    0x0000002844004428L, 0x0000005088008850L, 0x000000a0100010a0L, 0x0000004020002040L,
    0x0000020400040200L, 0x0000050800080500L, 0x00000a1100110a00L, 0x0000142200221400L,
    0x0000284400442800L, 0x0000508800885000L, 0x0000a0100010a000L, 0x0000402000204000L,
    0x0002040004020000L, 0x0005080008050000L, 0x000a1100110a0000L, 0x0014220022140000L,
    0x0028440044280000L, 0x0050880088500000L, 0x00a0100010a00000L, 0x0040200020400000L,
    0x0204000402000000L, 0x0508000805000000L, 0x0a1100110a000000L, 0x1422002214000000L,
    0x2844004428000000L, 0x5088008850000000L, 0xa0100010a0000000L, 0x4020002040000000L,
    0x0400040200000000L, 0x0800080500000000L, 0x1100110a00000000L, 0x2200221400000000L,
    0x4400442800000000L, 0x8800885000000000L, 0x100010a000000000L, 0x2000204000000000L,
    0x0004020000000000L, 0x0008050000000000L, 0x00110a0000000000L, 0x0022140000000000L,
    0x0044280000000000L, 0x0088500000000000L, 0x0010a00000000000L, 0x0020400000000000L};

extern U64* ROOK_ATTACKS[N_SQUARES];
extern U64* BISHOP_ATTACKS[N_SQUARES];

extern U64  ALL_HASHES[N_PIECES][N_SQUARES];

extern U64  IN_BETWEEN_SQUARES[N_SQUARES][N_SQUARES];
extern U64  PASSED_PAWN_MASK[N_COLORS][N_SQUARES];

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
 * fill the bitboard to the north
 * @param b
 * @return
 */
inline U64 fillNorth(U64 b) {
    b |= (b << 8);
    b |= (b << 16);
    b |= (b << 32);
    return b;
}

/**
 * fill the bitboard to the south
 * @param b
 * @return
 */
inline U64 fillSouth(U64 b) {
    b |= (b >> 8);
    b |= (b >> 16);
    b |= (b >> 32);
    return b;
}

/**
 * fills all the files where a bit is set
 * @param b
 * @return
 */
inline U64 fillFile(U64 b) {
    return fillSouth(b) | fillNorth(b);
}

/**
 *
 * white attack        white attack     attack filefill with least one
 * frontspan           rearspan         square attacked by white
 * . . 1 . 1 . . .     . . . . . . . .     . . 1 . 1 . . .
 * . . 1 . 1 . . .     . . . . . . . .     . . 1 . 1 . . .
 * . . 1 . 1 . . .     . . . . . . . .     . . 1 . 1 . . .
 * . . 1 . 1 . . .     . . . . . . . .     . . 1 . 1 . . .
 * . . . w . . . .     . . 1 w 1 . . .     . . 1 w 1 . . .
 * . . . . . . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 * . . . . . . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 * . . . . . . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 */
inline U64 wAttackFrontSpans(U64 b) {
    return fillNorth(shiftNorthEast(b) | shiftNorthWest(b));
}

/**
 *
 * white attack        white attack     attack filefill with least one
 * frontspan           rearspan         square attacked by white
 * . . 1 . 1 . . .     . . . . . . . .     . . 1 . 1 . . .
 * . . 1 . 1 . . .     . . . . . . . .     . . 1 . 1 . . .
 * . . 1 . 1 . . .     . . . . . . . .     . . 1 . 1 . . .
 * . . 1 . 1 . . .     . . . . . . . .     . . 1 . 1 . . .
 * . . . w . . . .     . . 1 w 1 . . .     . . 1 w 1 . . .
 * . . . . . . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 * . . . . . . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 * . . . . . . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 */
inline U64 wAttackRearSpans(U64 b) {
    return fillSouth(shiftEast(b) | shiftWest(b));
}

/**
 *
 * black attack        black attack     attack filefill with least one
 * frontspan           rearspan         square attacked by black
 * . . . . . . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 * . . . . . . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 * . . . . . . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 * . . . . . . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 * . . . b . . . .     . . 1 b 1 . . .     . . 1 b 1 . . .
 * . . 1 . 1 . . .     . . . . . . . .     . . 1 . 1 . . .
 * . . 1 . 1 . . .     . . . . . . . .     . . 1 . 1 . . .
 * . . 1 . 1 . . .     . . . . . . . .     . . 1 . 1 . . .
 */
inline U64 bAttackFrontSpans(U64 b) {
    return fillSouth(shiftSouthEast(b) | shiftSouthWest(b));
}

/**
 *
 * black attack        black attack     attack filefill with least one
 * frontspan           rearspan         square attacked by black
 * . . . . . . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 * . . . . . . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 * . . . . . . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 * . . . . . . . .     . . 1 . 1 . . .     . . 1 . 1 . . .
 * . . . b . . . .     . . 1 b 1 . . .     . . 1 b 1 . . .
 * . . 1 . 1 . . .     . . . . . . . .     . . 1 . 1 . . .
 * . . 1 . 1 . . .     . . . . . . . .     . . 1 . 1 . . .
 * . . 1 . 1 . . .     . . . . . . . .     . . 1 . 1 . . .
 */
inline U64 bAttackRearSpans(U64 b) {
    return fillNorth(shiftEast(b) | shiftWest(b));
}

/**
 * front span for white pawns
 * @param wpawns
 * @return
 */
inline U64 wFrontSpans(U64 wpawns) {
    return shiftNorth(fillNorth(wpawns));
}

/**
 * front span for black pawns
 * @param bpawns
 * @return
 */
inline U64 bRearSpans(U64 bpawns) {
    return shiftNorth(fillNorth(bpawns));
}

/**
 * rear span for white pawns
 * @param wpawns
 * @return
 */
inline U64 wRearSpans(U64 wpawns) {
    return shiftSouth(fillSouth(wpawns));
}

/**
 * rear span for black pawns
 * @param bpawns
 * @return
 */
inline U64 bFrontSpans(U64 bpawns) {
    return shiftSouth(fillSouth(bpawns));
}

/**
 * computes all white passed pawns
 * @param wpawns
 * @param bpawns
 * @return
 */
inline U64 wPassedPawns(U64 wpawns, U64 bpawns) {
    U64 allFrontSpans = bFrontSpans(bpawns);
    allFrontSpans |= shiftEast(allFrontSpans) | shiftWest(allFrontSpans);
    return wpawns & ~allFrontSpans;
}

/**
 * computes all black passed pawns
 * @param bpawns
 * @param wpawns
 * @return
 */
inline U64 bPassedPawns(U64 bpawns, U64 wpawns) {
    U64 allFrontSpans = wFrontSpans(wpawns);
    allFrontSpans |= shiftEast(allFrontSpans) | shiftWest(allFrontSpans);
    return bpawns & ~allFrontSpans;
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
 * this is mainly used for generating the entries for the fancy magic bitboard move generation.
 * It populates the bits of a mask by using an index.
 * @param mask
 * @param index
 * @return
 */
U64        populateMask(U64 mask, U64 index);

/**
 * generate sliding attacks
 * @param sq
 * @param direction
 * @param occ
 * @return
 */
U64        generateSlidingAttacks(Square sq, Direction direction, U64 occ);

/**
 * Generates all attackable squares by a rook from the given square considering the occupied squares.
 * Assumes that the occupied squares can be captured.
 * @param sq
 * @param occupied
 * @return
 */
U64        generateRookAttack(Square sq, U64 occupied);

/**
 * Generates all attackable squares by a bishop from the given square considering the occupied
 * squares. Assumes that the occupied squares can be captured.
 * @param sq
 * @param occupied
 * @return
 */
U64        generateBishopAttack(Square sq, U64 occupied);

/**
 * generates some relevant data
 */
void       generateData();

/**
 * initiates the entries for fancy magic bitboard and zobrist hash values.
 */
void       init();

/**
 * deletes allocated arrays
 */
void       cleanUp();

/**
 * generates a random Bitboard
 * @return
 */
U64        randU64();

/**
 * generates a random double between min and max
 * @param min
 * @param max
 * @return
 */
double     randDouble(double min = 0, double max = 1);

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
 * looks up the rook attack for a rook on the given square.
 * It returns a bitmap with all attackable squares highlighted.
 * @param index
 * @param occupied
 * @return
 */
inline U64 lookUpRookAttack(Square index, U64 occupied) {
    return ROOK_ATTACKS[index][static_cast<int>((occupied & rookMasks[index]) * rookMagics[index]
        >> (rookShifts[index]))];
}

/**
 * looks up the xray rook attack for a rook on the given square
 * It returns a bitmap with all attackable squares highlighted including those after the first
 * blockers.
 */
inline U64 lookUpRookXRayAttack(Square index, U64 occupied, U64 opponent) {
    U64 attacks  = lookUpRookAttack(index, occupied);
    U64 blockers = opponent & attacks;
    return attacks ^ lookUpRookAttack(index, occupied ^ blockers);
}

/**
 * looks up the bishop attack using magic bitboards
 * @param index
 * @param occupied
 * @return
 */
inline U64 lookUpBishopAttack(Square index, U64 occupied) {
    return BISHOP_ATTACKS[index][static_cast<int>(
        (occupied & bishopMasks[index]) * bishopMagics[index] >> (bishopShifts[index]))];
}

/**
 * looks up the xray bishop attack for a bishop on the given square
 * It returns a bitmap with all attackable squares highlighted including those after the first
 * blockers.
 */
inline U64 lookUpBishopXRayAttack(Square index, U64 occupied, U64 opponent) {
    U64 attacks  = lookUpBishopAttack(index, occupied);
    U64 blockers = opponent & attacks;
    return attacks ^ lookUpBishopAttack(index, occupied ^ blockers);
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
