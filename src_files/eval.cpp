
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                           by. Kim Kahre, Finn Eggers and Eugenio Bruno                           *
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

#include "eval.h"

#include <immintrin.h>
#include <iomanip>



EvalScore psqt_pawn_same_side_castle[64] = {
    M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0),
    M( -12,  -4), M(  21,   8), M(  11,  31), M( -10,  38), M( -11,  26), M( -17,  22), M( -20,  25), M( -19,  35),
    M(   7,   2), M(  21,   4), M(  11,  19), M(   7,  21), M(  -4,  10), M( -13,  17), M( -20,  19), M( -19,  26),
    M(   5,  12), M(  16,  15), M(  15,  13), M(  19,   8), M(   7,   6), M(   2,  10), M( -17,  32), M( -15,  35),
    M(  17,  22), M(  22,  27), M(  44,  10), M(  35,   8), M(  22,   2), M(   1,  28), M(  -7,  43), M( -12,  56),
    M(  20,  36), M(  74,  28), M( 116,  11), M(  47,  14), M(  33,  22), M(  31,  55), M(  15,  85), M( -12,  96),
    M(  10, 128), M(   2, 117), M( 183,  68), M( 202,  69), M( 185, 106), M( 184, 140), M( 179, 168), M( 196, 169),
    M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0),
};

EvalScore psqt_pawn_opposite_side_castle[64] = {
    M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0),
    M(   7,  26), M(  29,  19), M(  11,  29), M( -12,  37), M( -20,  36), M( -29,  41), M( -47,  46), M( -45,  41),
    M(  21,  22), M(  30,  17), M(  11,  24), M(  -2,  25), M( -12,  25), M( -21,  32), M( -49,  37), M( -45,  36),
    M(  16,  43), M(  13,  43), M(  14,  29), M(  11,  22), M(   9,  19), M(  -2,  26), M( -29,  35), M( -37,  31),
    M(  12,  82), M(  12,  74), M(  17,  53), M(  20,  32), M(  34,  15), M(  37,  15), M( -11,  35), M( -20,  30),
    M(  -2, 152), M(  35, 143), M(  34, 113), M(  28,  66), M(  58,  19), M(  91,   1), M(  71,   8), M(   1,  31),
    M( 182, 236), M( 200, 230), M( 165, 210), M( 174, 160), M( 192,  91), M( 198,  42), M(  51,  82), M(  41,  96),
    M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0),
};

EvalScore psqt_knight_same_side_castle[64] = {
    M( -38, -62), M(  -5, -46), M(   2, -29), M(   5, -26), M(   3, -23), M( -10, -42), M(  -6, -72), M( -53, -65),
    M(  -3, -28), M(  -9, -12), M(   5, -24), M(   4,  -6), M(   6, -13), M( -16, -23), M( -28, -17), M( -36, -36),
    M(  -5, -29), M(   7,  -4), M(   9,  -1), M(  13,  12), M(   5,  16), M(  -9, -10), M( -13, -22), M( -26, -56),
    M(   4,   1), M(  34,  16), M(   8,  39), M(  14,  38), M(   0,  34), M(  -4,  23), M( -15,   4), M( -17, -27),
    M(   6,   7), M(   0,  24), M(  31,  39), M(   2,  45), M(  14,  48), M(   6,  23), M( -10,  -1), M( -17, -11),
    M( -14,   2), M(  15,  23), M(  72,  37), M(  51,  23), M(  21,  28), M(  21,  22), M( -12,   2), M( -50, -13),
    M(   7, -13), M( -48,  20), M(  62,  -7), M(  34,  17), M(  38,  24), M(  15,  -6), M( -36,  -6), M( -27, -30),
    M(-168, -57), M(-106,  17), M(-141,  56), M( -24,  13), M( -60,  18), M(-167,  43), M(-163,  14), M(-272, -28),
};

EvalScore psqt_knight_opposite_side_castle[64] = {
    M( -44, -69), M( -33, -13), M(   1, -22), M(   7,  -4), M(   3,   0), M( -13, -15), M( -13, -34), M( -61, -23),
    M( -12, -29), M( -17,  -4), M(   1, -13), M(   9,   0), M(  13,   3), M(  -6,   1), M( -39,  15), M( -42, -15),
    M( -15, -27), M(  -1,   0), M(  12,   6), M(  12,  27), M(  19,  29), M(   0,  15), M(  -1,  -4), M( -28, -20),
    M(   6,  -2), M(  23,  12), M(  25,  32), M(  22,  44), M(  35,  42), M(  19,  38), M(  30,  18), M(   6,  -3),
    M(   1,   4), M(  15,  12), M(  38,  29), M(  46,  49), M(  25,  55), M(  41,  48), M(  17,  17), M(  15,   0),
    M( -31,  -7), M(  24,   7), M(  47,  30), M(  49,  36), M(  68,  32), M(  85,  35), M(  42,   2), M(   8, -14),
    M( -13, -22), M(  -4,  -3), M(  53, -10), M(  80,  17), M(  52,  13), M(  52,   0), M( -14,   0), M(  13, -28),
    M(-206, -63), M(-153,  22), M(-128,  33), M( -49,  25), M( -12,   1), M(-100,  26), M( -38,   2), M(-202, -29),
};

EvalScore psqt_bishop_same_side_castle[64] = {
    M( -16, -20), M( -35,  -3), M( -19,   0), M( -34,   4), M( -26,  -2), M( -23,  -4), M( -15,   1), M(  -4, -18),
    M( -15, -26), M(   9, -18), M(  -9, -10), M( -15,   5), M( -22,   4), M( -11,  -6), M(  -8, -16), M(  -9, -18),
    M(   7,   0), M(  -6,   5), M(  -2,   8), M( -11,  17), M(  -8,  17), M( -13,  11), M(  -4,   2), M( -21,   2),
    M( -15,   7), M( -15,  17), M( -26,  26), M(   6,  19), M(  -1,  22), M( -10,  20), M( -21,   8), M( -34,  -1),
    M( -29,  21), M( -12,  30), M(  -1,  24), M(   1,  31), M(  22,  22), M( -16,  20), M(  -9,  13), M( -48,  19),
    M( -18,  26), M(  20,  29), M(  28,  32), M(  24,  20), M(  -3,  20), M(   7,  16), M( -27,  29), M( -35,  15),
    M( -55,  22), M( -22,  25), M(   6,  18), M( -39,  28), M( -49,  28), M( -38,  27), M( -39,  28), M( -88,  34),
    M( -75,  28), M( -33,  17), M(-153,  38), M(-115,  36), M(-138,  46), M(-151,  43), M(-117,  52), M( -91,  44),
};

EvalScore psqt_bishop_opposite_side_castle[64] = {
    M(   7,   9), M( -24,  15), M(  -6,  17), M( -24,  23), M( -21,  23), M( -27,  17), M( -24,  20), M( -21,  18),
    M(  12, -12), M(  41,   2), M(  13,  10), M(   5,  16), M( -15,  21), M( -15,  19), M( -20,   8), M( -13,   7),
    M(  12,  11), M(  28,  19), M(  33,  23), M(   6,  26), M(   4,  33), M( -13,  30), M( -10,  21), M( -21,  23),
    M( -11,  24), M(  10,  30), M(  10,  32), M(  37,  30), M(  12,  36), M(   0,  35), M(  -6,  23), M(  -7,  18),
    M( -16,  40), M(   5,  37), M(  17,  38), M(  29,  38), M(  48,  38), M(  16,  30), M(  20,  35), M(  -6,  22),
    M( -23,  37), M(  13,  41), M(  16,  39), M(  25,  39), M(  54,  31), M(  67,  37), M(  46,  25), M(   3,  25),
    M( -62,  48), M( -42,  53), M( -14,  47), M( -46,  48), M( -20,  45), M(  44,  23), M(   4,  30), M( -54,  24),
    M( -71,  67), M( -91,  50), M( -93,  48), M( -97,  58), M(-124,  51), M(-101,  49), M(  11,  31), M( -31,  48),
};

EvalScore psqt_rook_same_side_castle[64] = {
    M( -16, -10), M(  16,  -1), M(  20,   6), M(  21,  -1), M(  21,   0), M(  11,  11), M(   3,   5), M(  -4,   4),
    M( -81,  10), M(  13, -12), M(  18, -12), M(   5,   0), M(   5,   2), M(  -7,  10), M( -10,   2), M( -45,  10),
    M( -20,  -1), M(  22,   9), M(   7,  10), M(  -1,  11), M(  -1,  14), M( -18,  22), M(  -8,  21), M( -25,  13),
    M(  -9,  22), M(  21,  28), M(   3,  33), M(  -2,  32), M(  -4,  36), M( -14,  44), M( -15,  44), M( -18,  30),
    M(  29,  34), M(  49,  28), M(  51,  33), M(  36,  36), M(  47,  35), M(  19,  46), M(   7,  40), M(  -5,  44),
    M(  44,  37), M( 114,  19), M( 104,  30), M(  83,  23), M(  57,  34), M(  30,  46), M(  42,  36), M(  -7,  53),
    M(  41,  44), M(  55,  46), M( 117,  27), M(  36,  62), M(  39,  60), M(  20,  62), M(  -7,  66), M(   2,  58),
    M(  84,  44), M(  78,  47), M(  97,  42), M(  25,  55), M(  12,  62), M(   3,  68), M(  24,  61), M(  26,  56),
};

EvalScore psqt_rook_opposite_side_castle[64] = {
    M( -27,  20), M(   5,  15), M(  11,  18), M(  19,  11), M(  23,   6), M(  31,  10), M(  31,   6), M(   1,  11),
    M( -78,  26), M( -20,   9), M(  -3,  13), M(  -2,  17), M(   5,  13), M(  14,  10), M(  21,   0), M( -31,  15),
    M( -31,  16), M(   9,  20), M( -14,  27), M(  -7,  25), M(  -9,  23), M(   0,  23), M(  11,  22), M(  -4,  13),
    M(  -9,  37), M(  22,  36), M(   0,  47), M(  -2,  47), M(   0,  37), M(   0,  43), M(   7,  39), M(  -3,  30),
    M(  21,  48), M(  34,  40), M(  47,  39), M(  53,  45), M(  45,  40), M(  44,  44), M(  45,  32), M(  44,  30),
    M(  35,  46), M(  89,  24), M(  69,  43), M(  80,  36), M(  91,  32), M(  98,  32), M( 115,  18), M(  53,  33),
    M(  43,  50), M(  50,  51), M(  59,  51), M(  49,  67), M(  57,  57), M( 125,  26), M(  54,  42), M(  50,  38),
    M( 108,  32), M(  85,  45), M(  68,  53), M(  50,  56), M(  60,  49), M( 139,  24), M(  93,  38), M(  53,  52),
};

EvalScore psqt_queen_same_side_castle[64] = {
    M( -26, -25), M( -17, -42), M(  -7, -26), M(   8, -16), M(   7,   3), M(   1, -15), M(  -4, -10), M(  -9,   3),
    M( -23, -23), M(   9, -69), M(  19, -68), M(   7, -10), M(   6,  -4), M(  10, -35), M(   1, -12), M(  -9,  -2),
    M(  13, -11), M(  20,   0), M(   6,  14), M(   1,  -8), M(  -2,  -8), M(  -4,   9), M(   2,   3), M( -11,   5),
    M(  20,  28), M(  16,  27), M(   7,  25), M( -10,  35), M( -15,  31), M( -11,   8), M(  -9,  25), M( -17,  24),
    M(  14,  28), M(  21,  56), M(   8,  56), M( -15,  55), M( -26,  56), M( -30,  27), M( -31,  53), M( -34,  49),
    M( -18,  43), M(  42,  42), M(  30,  32), M(  14,  37), M( -15,  26), M( -35,  41), M( -37,  43), M( -49,  69),
    M(  12,  72), M( -51, 124), M(  96,  -6), M( -64,  90), M( -67,  81), M( -40,  57), M( -91, 109), M( -47,  82),
    M(  48,   7), M(  13,  52), M(  47,  10), M( -19,  31), M( -15,  31), M( -29,  59), M( -34,  71), M( -55,  88),
};

EvalScore psqt_queen_opposite_side_castle[64] = {
    M( -36,   3), M( -14, -24), M(  -1, -30), M(  -1,  17), M(  -2,  22), M(  -6,   9), M( -11,  45), M( -10,  30),
    M( -44,   4), M( -10,  -3), M(  20, -36), M(  15,  18), M(   6,  18), M(  15,  -5), M(  17,   6), M(   7,  24),
    M( -25,  36), M(   6,  25), M(   4,  44), M(   2,  29), M(  -2,  34), M(   8,  34), M(  16,  26), M(  18,  25),
    M( -16,  59), M(  -2,  63), M( -15,  71), M(  -5,  85), M(  -9,  73), M(   6,  58), M(  16,  46), M(  26,  26),
    M( -10,  69), M( -29, 105), M( -25,  74), M( -16,  96), M(   4,  86), M(  20,  66), M(  39,  56), M(  24,  35),
    M( -24,  96), M(  -3,  60), M(  -1,  68), M(  -2,  70), M(  33,  68), M(  32,  75), M(  65,  22), M(  14,  33),
    M( -15,  81), M( -61, 134), M( -16,  86), M( -57, 122), M( -67, 136), M(  86,  25), M( -15,  76), M(  13,  42),
    M(  -3,  79), M(   4,  82), M(  -5,  74), M( -16,  78), M( -13,  69), M( 126, -20), M(  -1,  54), M(  22,  22),
};

EvalScore psqt_king[64] = {
    M( -34, -96), M( 127, -50), M(  79, -21), M( 118, -16), M(  85, -26), M(  48, -13), M(  39, -10), M(-165, -64),
    M(  14, -32), M( 121,  10), M( 212, -16), M( 201, -20), M( 153, -13), M( 171,  -9), M(  60,  24), M(  18, -30),
    M(  32,   1), M( 141,  17), M( 195,   6), M( 176,   0), M( 178,  -4), M( 196,   6), M( 124,  18), M(  21,  -3),
    M( -14,  13), M(  99,   7), M( 134,  15), M( 107,  19), M( 119,  15), M( 130,  14), M(  81,   5), M( -57,  11),
    M( -22, -14), M(  84,  -3), M(  91,  16), M(  60,  27), M(  75,  28), M(  65,  21), M(  80,   1), M( -30, -13),
    M( -14, -13), M(  12,   4), M(  29,  15), M(  18,  29), M(  32,  28), M(  16,  23), M(  10,   8), M( -17,  -8),
    M(  40, -24), M(  15,  -2), M(   8,  10), M( -28,  19), M(  -5,  18), M( -14,  20), M(  20,  -3), M(  37, -28),
    M(  42, -74), M(  47, -39), M(  25, -20), M(  10, -32), M(  13, -40), M(  -7, -12), M(  56, -35), M(  65, -91),
};

EvalScore mobilityKnight[9] = {
    M(   20,  -50), M(   35,    0), M(   41,   28), M(   47,   37), M(   54,   44), M(   59,   53), M(   65,   57), M(   74,   51),
    M(   83,   39), };

EvalScore mobilityBishop[14] = {
    M(   20,  -52), M(   27,   11), M(   35,   44), M(   41,   61), M(   49,   73), M(   55,   88), M(   56,   99), M(   57,  107),
    M(   61,  111), M(   66,  112), M(   74,  111), M(   87,  105), M(   91,  125), M(  133,   84), };

EvalScore mobilityRook[15] = {
    M(   28,   22), M(   33,   40), M(   38,   58), M(   39,   76), M(   42,   87), M(   49,   93), M(   54,  101), M(   60,  102),
    M(   66,  106), M(   70,  111), M(   74,  113), M(   79,  116), M(   87,  114), M(  102,  101), M(  171,   67), };

EvalScore mobilityQueen[28] = {
    M(  -10, -160), M(    3,  -85), M(   12,   36), M(   15,  108), M(   18,  150), M(   20,  176), M(   23,  201), M(   25,  219),
    M(   27,  234), M(   30,  241), M(   34,  247), M(   37,  254), M(   38,  261), M(   38,  268), M(   37,  276), M(   36,  278),
    M(   37,  282), M(   34,  283), M(   37,  284), M(   44,  278), M(   51,  270), M(   48,  269), M(   39,  264), M(   62,  252),
    M(  -15,  286), M(   94,  223), M(   17,  291), M(   18,  299), };

EvalScore hangingEval[5] {
    M(   -1,    2), M(   -1,   -5), M(   -9,  -10), M(  -13,   -8), M(  -14,  -12),
};

EvalScore pinnedEval[15] {
    M(    0,  -20), M(  -22,   16), M(   -6,   33), M(  -22,  -76), M(  -32,   -6),
    M(  -20,   25), M(   -5,   -6), M(  -44,   -2), M(  -25,   19), M(  -46,  -51),
    M(    0,   -7), M(   -5,    5), M(   -8,  -76), M(  -56,  -33), M(  -17,   21),
};

EvalScore passer_rank_n[16] {
    M(    0,    0), M(   -9,  -29), M(   -9,  -24), M(   -5,    2), M(   22,   23), M(   47,   66), M(    4,   27), M(    0,    0),
    M(    0,    0), M(    1,    2), M(  -44,  -49), M(  -45,  -11), M(  -29,  -14), M(   35,   -7), M(   91, -143), M(    0,    0),
};

EvalScore bishop_pawn_same_color_table_o[8]{
    M(    0,   18),
    M(   -2,   19),
    M(   -5,    7),
    M(  -10,    0),
    M(  -13,  -12),
    M(  -19,  -29),
    M(  -25,  -32),
    M(  -27,  -34),
};

EvalScore bishop_pawn_same_color_table_e[8]{
    M(    0,   18),
    M(   -2,   19),
    M(   -5,    7),
    M(  -10,    0),
    M(  -13,  -12),
    M(  -19,  -29),
    M(  -25,  -32),
    M(  -27,  -34),
};

EvalScore SIDE_TO_MOVE                  = M(   10,   16);
EvalScore PAWN_STRUCTURE                = M(    9,   12);
EvalScore PAWN_PASSED                   = M(    6,   44);
EvalScore PAWN_ISOLATED                 = M(   -7,  -10);
EvalScore PAWN_DOUBLED                  = M(    2,   -6);
EvalScore PAWN_DOUBLED_AND_ISOLATED     = M(   -4,  -21);
EvalScore PAWN_BACKWARD                 = M(  -13,    0);
EvalScore PAWN_OPEN                     = M(    9,   -5);
EvalScore PAWN_BLOCKED                  = M(   -9,  -16);
EvalScore KNIGHT_OUTPOST                = M(   25,   17);
EvalScore KNIGHT_DISTANCE_ENEMY_KING    = M(   -6,    1);
EvalScore ROOK_OPEN_FILE                = M(   34,   -6);
EvalScore ROOK_HALF_OPEN_FILE           = M(   -2,   -4);
EvalScore ROOK_KING_LINE                = M(   20,    8);
EvalScore BISHOP_DOUBLED                = M(   40,   55);
EvalScore BISHOP_FIANCHETTO             = M(   -3,    9);
EvalScore QUEEN_DISTANCE_ENEMY_KING     = M(    4,  -27);
EvalScore KING_CLOSE_OPPONENT           = M(   -9,   49);
EvalScore KING_PAWN_SHIELD              = M(   27,    5);
EvalScore CASTLING_RIGHTS               = M(   25,   -8);
EvalScore BISHOP_PIECE_SAME_SQUARE_E    = M(    2,    3);

EvalScore kingSafetyTable[100] {
    M(  -18,   -6), M(    0,    0), M(  -22,   -4), M(  -12,  -10), M(  -14,   -6), M(   14,   -8), M(   10,  -16), M(   26,   -4),
    M(   14,   -2), M(   34,  -18), M(   68,  -14), M(   78,  -16), M(   36,    8), M(   96,  -14), M(  122,    2), M(  120,   -6),
    M(   94,    6), M(  180,  -10), M(  234,  -46), M(  214,  -26), M(  232,  -54), M(  256,  -36), M(  320,  -38), M(  304,  -40),
    M(  298,   10), M(  346,   10), M(  410,    0), M(  434,  124), M(  476,   -2), M(  464,   16), M(  500,  500), M(  500,  294),
    M(  500,   36), M(  500,  500), M(  500,  500), M(  500,  352), M(  500,  500), M(  500,  500), M(  494,  416), M(  500,  500),
    M(  314,   90), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M( -500, -500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
};


EvalScore pieceScores[6] = {
    M(90, 104), M(463, 326), M(474, 288), M(577, 594), M(1359, 1121), M(0, 0),
};

EvalScore* evfeatures[] {
    &SIDE_TO_MOVE,
    &PAWN_STRUCTURE,
    &PAWN_PASSED,
    &PAWN_ISOLATED,
    &PAWN_DOUBLED,
    &PAWN_DOUBLED_AND_ISOLATED,
    &PAWN_BACKWARD,
    &PAWN_OPEN,
    &PAWN_BLOCKED,                  // 8
    
    &KNIGHT_OUTPOST,
    &KNIGHT_DISTANCE_ENEMY_KING,    // 10
    
    &ROOK_OPEN_FILE,
    &ROOK_HALF_OPEN_FILE,
    &ROOK_KING_LINE,                // 13
    
    &BISHOP_DOUBLED,
    &BISHOP_FIANCHETTO,
    &BISHOP_PIECE_SAME_SQUARE_E,    // 17
    
    &QUEEN_DISTANCE_ENEMY_KING,     // 18
    
    &KING_CLOSE_OPPONENT,
    &KING_PAWN_SHIELD,              // 20
    
    &CASTLING_RIGHTS,               // 21

    
};



int mobEntryCount[6] {0, 9, 14, 15, 28, 0};

float* phaseValues = new float[6] {
    0, 1, 1, 2, 4, 0,
};


EvalScore* mobilities[6] {nullptr, mobilityKnight, mobilityBishop, mobilityRook, mobilityQueen, nullptr};

EvalScore* psqt[11] {psqt_pawn_same_side_castle,
                     psqt_pawn_opposite_side_castle,
                     psqt_knight_same_side_castle,
                     psqt_knight_opposite_side_castle,
                     psqt_bishop_same_side_castle,
                     psqt_bishop_opposite_side_castle,
                     psqt_rook_same_side_castle,
                     psqt_rook_opposite_side_castle,
                     psqt_queen_same_side_castle,
                     psqt_queen_opposite_side_castle,
                     psqt_king};

EvalScore fast_pawn_psqt[2][4][64];
EvalScore fast_knight_psqt[2][4][64];
EvalScore fast_bishop_psqt[2][4][64];
EvalScore fast_rook_psqt[2][4][64];
EvalScore fast_queen_psqt[2][4][64];
EvalScore fast_king_psqt[2][64];


void eval_init() {
    for (int i = 0; i < 64; i++) {
        for (int kside = 0; kside < 2; kside++) {
            fast_pawn_psqt[WHITE][psqt_kingside_indexing(kside, kside)][i] =
                psqt_pawn_same_side_castle[pst_index_white(i, kside)] + pieceScores[PAWN];
            fast_pawn_psqt[BLACK][psqt_kingside_indexing(kside, kside)][i] =
                psqt_pawn_same_side_castle[pst_index_black(i, kside)] + pieceScores[PAWN];
            fast_pawn_psqt[WHITE][psqt_kingside_indexing(kside, 1 - kside)][i] =
                psqt_pawn_opposite_side_castle[pst_index_white(i, kside)] + pieceScores[PAWN];
            fast_pawn_psqt[BLACK][psqt_kingside_indexing(1 - kside, kside)][i] =
                psqt_pawn_opposite_side_castle[pst_index_black(i, kside)] + pieceScores[PAWN];

            fast_knight_psqt[WHITE][psqt_kingside_indexing(kside, kside)][i] =
                psqt_knight_same_side_castle[pst_index_white(i, kside)] + pieceScores[KNIGHT];
            fast_knight_psqt[BLACK][psqt_kingside_indexing(kside, kside)][i] =
                psqt_knight_same_side_castle[pst_index_black(i, kside)] + pieceScores[KNIGHT];
            fast_knight_psqt[WHITE][psqt_kingside_indexing(kside, 1 - kside)][i] =
                psqt_knight_opposite_side_castle[pst_index_white(i, kside)] + pieceScores[KNIGHT];
            fast_knight_psqt[BLACK][psqt_kingside_indexing(1 - kside, kside)][i] =
                psqt_knight_opposite_side_castle[pst_index_black(i, kside)] + pieceScores[KNIGHT];

            fast_bishop_psqt[WHITE][psqt_kingside_indexing(kside, kside)][i] =
                psqt_bishop_same_side_castle[pst_index_white(i, kside)] + pieceScores[BISHOP];
            fast_bishop_psqt[BLACK][psqt_kingside_indexing(kside, kside)][i] =
                psqt_bishop_same_side_castle[pst_index_black(i, kside)] + pieceScores[BISHOP];
            fast_bishop_psqt[WHITE][psqt_kingside_indexing(kside, 1 - kside)][i] =
                psqt_bishop_opposite_side_castle[pst_index_white(i, kside)] + pieceScores[BISHOP];
            fast_bishop_psqt[BLACK][psqt_kingside_indexing(1 - kside, kside)][i] =
                psqt_bishop_opposite_side_castle[pst_index_black(i, kside)] + pieceScores[BISHOP];

            fast_rook_psqt[WHITE][psqt_kingside_indexing(kside, kside)][i] =
                psqt_rook_same_side_castle[pst_index_white(i, kside)] + pieceScores[ROOK];
            fast_rook_psqt[BLACK][psqt_kingside_indexing(kside, kside)][i] =
                psqt_rook_same_side_castle[pst_index_black(i, kside)] + pieceScores[ROOK];
            fast_rook_psqt[WHITE][psqt_kingside_indexing(kside, 1 - kside)][i] =
                psqt_rook_opposite_side_castle[pst_index_white(i, kside)] + pieceScores[ROOK];
            fast_rook_psqt[BLACK][psqt_kingside_indexing(1 - kside, kside)][i] =
                psqt_rook_opposite_side_castle[pst_index_black(i, kside)] + pieceScores[ROOK];

            fast_queen_psqt[WHITE][psqt_kingside_indexing(kside, kside)][i] =
                psqt_queen_same_side_castle[pst_index_white(i, kside)] + pieceScores[QUEEN];
            fast_queen_psqt[BLACK][psqt_kingside_indexing(kside, kside)][i] =
                psqt_queen_same_side_castle[pst_index_black(i, kside)] + pieceScores[QUEEN];
            fast_queen_psqt[WHITE][psqt_kingside_indexing(kside, 1 - kside)][i] =
                psqt_queen_opposite_side_castle[pst_index_white(i, kside)] + pieceScores[QUEEN];
            fast_queen_psqt[BLACK][psqt_kingside_indexing(1 - kside, kside)][i] =
                psqt_queen_opposite_side_castle[pst_index_black(i, kside)] + pieceScores[QUEEN];
        }

        fast_king_psqt[WHITE][i] = psqt_king[pst_index_white_s(i)];
        fast_king_psqt[BLACK][i] = psqt_king[pst_index_black_s(i)];
    }
}

/**
 * adds the factor to value of attacks if the piece attacks the kingzone
 * @param attacks
 * @param kingZone
 * @param pieceCount
 * @param valueOfAttacks
 * @param factor
 */

bool hasMatingMaterial(Board* b, bool side) {
    if ((b->getPieces()[QUEEN + side * 6] | b->getPieces()[ROOK + side * 6] | b->getPieces()[PAWN + side * 6])
        || (bitCount(b->getPieces()[BISHOP + side * 6] | b->getPieces()[KNIGHT + side * 6]) > 1
            && b->getPieces()[BISHOP + side * 6]))
        return true;
    return false;
}

void addToKingSafety(U64 attacks, U64 kingZone, int& pieceCount, int& valueOfAttacks, int factor) {
    if (attacks & kingZone) {
        pieceCount++;
        valueOfAttacks += factor * bitCount(attacks & kingZone);
    }
}

/**
 * checks if the given square is an outpost given the color and a bitboard of the opponent pawns
 */
bool isOutpost(Square s, Color c, U64 opponentPawns, U64 pawnCover) {
    U64 sq = ONE << s;

    if (c == WHITE) {
        if (((whitePassedPawnMask[s] & ~FILES[fileIndex(s)]) & opponentPawns) == 0 && (sq & pawnCover)) {
            return true;
        }
    } else {
        if (((blackPassedPawnMask[s] & ~FILES[fileIndex(s)]) & opponentPawns) == 0 && (sq & pawnCover)) {
            return true;
        }
    }
    return false;
}

bb::Score Evaluator::evaluateTempo(Board* b){
    phase = (24.0f + phaseValues[5] - phaseValues[0] * bitCount(b->getPieces()[WHITE_PAWN] | b->getPieces()[BLACK_PAWN])
        - phaseValues[1] * bitCount(b->getPieces()[WHITE_KNIGHT] | b->getPieces()[BLACK_KNIGHT])
        - phaseValues[2] * bitCount(b->getPieces()[WHITE_BISHOP] | b->getPieces()[BLACK_BISHOP])
        - phaseValues[3] * bitCount(b->getPieces()[WHITE_ROOK] | b->getPieces()[BLACK_ROOK])
        - phaseValues[4] * bitCount(b->getPieces()[WHITE_QUEEN] | b->getPieces()[BLACK_QUEEN]))
        / 24.0f;

    if (phase > 1)
        phase = 1;
    if (phase < 0)
        phase = 0;

   return MgScore(SIDE_TO_MOVE) * (1 - phase) + EgScore(SIDE_TO_MOVE) * (phase);
}

EvalScore Evaluator::computeHangingPieces(Board* b) {
    U64 WnotAttacked = ~b->getAttackedSquares(WHITE);
    U64 BnotAttacked = ~b->getAttackedSquares(BLACK);

    EvalScore res = M(0, 0);

    for (int i = PAWN; i <= QUEEN; i++) {
        res += hangingEval[i]
               * (+bitCount(b->getPieces(WHITE, i) & WnotAttacked) - bitCount(b->getPieces(BLACK, i) & BnotAttacked));
    }
    return res;
}

EvalScore Evaluator::computePinnedPieces(Board* b) {

    EvalScore res = M(0, 0);

    Square square;
    Square wkingSq = bitscanForward(b->getPieces(WHITE, KING));
    U64    pinner  = lookUpRookXRayAttack(wkingSq, *b->getOccupied(), b->getTeamOccupied()[WHITE])
                 & (b->getPieces(BLACK, ROOK) | b->getPieces(BLACK, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[wkingSq][square] & b->getTeamOccupied()[WHITE]);
        res += pinnedEval[3 * (b->getPiece(pinnedPlace) % 6) + (b->getPiece(square) % 6 - BISHOP)];

        pinner = lsbReset(pinner);
    }

    pinner = lookUpBishopXRayAttack(wkingSq, *b->getOccupied(), b->getTeamOccupied()[WHITE])
             & (b->getPieces(BLACK, BISHOP) | b->getPieces(BLACK, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[wkingSq][square] & b->getTeamOccupied()[WHITE]);

        res += pinnedEval[3 * (b->getPiece(pinnedPlace) % 6) + (b->getPiece(square) % 6 - BISHOP)];
        pinner = lsbReset(pinner);
    }

    Square bkingSq = bitscanForward(b->getPieces(BLACK, KING));
    pinner         = lookUpRookXRayAttack(bkingSq, *b->getOccupied(), b->getTeamOccupied()[BLACK])
             & (b->getPieces(WHITE, ROOK) | b->getPieces(WHITE, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[bkingSq][square] & b->getTeamOccupied()[BLACK]);
        res -= pinnedEval[3 * (b->getPiece(pinnedPlace) % 6) + (b->getPiece(square) % 6 - BISHOP)];
        pinner = lsbReset(pinner);
    }
    pinner = lookUpBishopXRayAttack(bkingSq, *b->getOccupied(), b->getTeamOccupied()[BLACK])
             & (b->getPieces(WHITE, BISHOP) | b->getPieces(WHITE, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[bkingSq][square] & b->getTeamOccupied()[BLACK]);

        res -= pinnedEval[3 * (b->getPiece(pinnedPlace) % 6) + (b->getPiece(square) % 6 - BISHOP)];
        pinner = lsbReset(pinner);
    }
    return res;
}

/**
 * evaluates the board.
 * @param b
 * @return
 */
bb::Score Evaluator::evaluate(Board* b) {

    Score res = 0;

    U64 whiteTeam = b->getTeamOccupied()[WHITE];
    U64 blackTeam = b->getTeamOccupied()[BLACK];
    U64 occupied  = *b->getOccupied();

    Square whiteKingSquare = bitscanForward(b->getPieces()[WHITE_KING]);
    Square blackKingSquare = bitscanForward(b->getPieces()[BLACK_KING]);

    U64 whiteKingZone = KING_ATTACKS[whiteKingSquare];
    U64 blackKingZone = KING_ATTACKS[blackKingSquare];

    Square square;
    U64    attacks;
    U64    k;

    phase = (24.0f + phaseValues[5] - phaseValues[0] * bitCount(b->getPieces()[WHITE_PAWN] | b->getPieces()[BLACK_PAWN])
             - phaseValues[1] * bitCount(b->getPieces()[WHITE_KNIGHT] | b->getPieces()[BLACK_KNIGHT])
             - phaseValues[2] * bitCount(b->getPieces()[WHITE_BISHOP] | b->getPieces()[BLACK_BISHOP])
             - phaseValues[3] * bitCount(b->getPieces()[WHITE_ROOK] | b->getPieces()[BLACK_ROOK])
             - phaseValues[4] * bitCount(b->getPieces()[WHITE_QUEEN] | b->getPieces()[BLACK_QUEEN]))
            / 24.0f;

    if (phase > 1)
        phase = 1;
    if (phase < 0)
        phase = 0;
    
    int wkingSafety_attPiecesCount = 0;
    int wkingSafety_valueOfAttacks = 0;

    int bkingSafety_attPiecesCount = 0;
    int bkingSafety_valueOfAttacks = 0;
    /**********************************************************************************
     *                                  P A W N S                                     *
     **********************************************************************************/

    U64 whitePawns = b->getPieces()[WHITE_PAWN];
    U64 blackPawns = b->getPieces()[BLACK_PAWN];

    bool   wKSide            = (fileIndex(bitscanForward(b->getPieces()[WHITE_KING])) > 3 ? 0 : 1);
    bool   bKSide            = (fileIndex(bitscanForward(b->getPieces()[BLACK_KING])) > 3 ? 0 : 1);
    Square psqtKingsideIndex = psqt_kingside_indexing(wKSide, bKSide);

    // all passed pawns for white/black
    U64 whitePassers = wPassedPawns(whitePawns, blackPawns);
    U64 blackPassers = bPassedPawns(blackPawns, whitePawns);

    // doubled pawns without the pawn least developed
    U64 whiteDoubledWithoutFirst = wFrontSpans(whitePawns) & whitePawns;
    U64 blackDoubledWithoutFirst = bFrontSpans(blackPawns) & blackPawns;

    // all doubled pawns
    U64 whiteDoubledPawns = whiteDoubledWithoutFirst | (wRearSpans(whiteDoubledWithoutFirst) & whitePawns);
    U64 blackDoubledPawns = blackDoubledWithoutFirst | (bRearSpans(blackDoubledWithoutFirst) & blackPawns);

    // all isolated pawns
    U64 whiteIsolatedPawns = whitePawns & ~(fillFile(shiftWest(whitePawns) | shiftEast(whitePawns)));
    U64 blackIsolatedPawns = blackPawns & ~(fillFile(shiftWest(blackPawns) | shiftEast(blackPawns)));

    U64 whiteBlockedPawns = shiftNorth(whitePawns) & (whiteTeam | blackTeam);
    U64 blackBlockedPawns = shiftSouth(blackPawns) & (whiteTeam | blackTeam);

    U64 openFilesWhite = ~fillFile(whitePawns);
    U64 openFilesBlack = ~fillFile(blackPawns);
    U64 openFiles      = openFilesBlack & openFilesWhite;

    k = whitePawns;

    EvalScore evalScore    = M(0, 0);
    EvalScore featureScore = M(0, 0);
    EvalScore mobScore     = M(0, 0);

    while (k) {
        square = bitscanForward(k);
        evalScore += fast_pawn_psqt[WHITE][psqtKingsideIndex][square];
        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_PAWN];
    while (k) {
        square = bitscanForward(k);
        evalScore -= fast_pawn_psqt[BLACK][psqtKingsideIndex][square];
        k = lsbReset(k);
    }
    k = whitePassers;
    while (k) {
        square = bitscanForward(k);
        featureScore += passer_rank_n[getBit(whiteBlockedPawns, square) * 8 + rankIndex(square)];
        k = lsbReset(k);
    }
    k = blackPassers;
    while (k) {
        square = bitscanForward(k);
        featureScore -= passer_rank_n[getBit(blackBlockedPawns, square) * 8 + 7 - rankIndex(square)];
        k = lsbReset(k);
    }

    U64 whitePawnEastCover = shiftNorthEast(whitePawns) & whitePawns;
    U64 whitePawnWestCover = shiftNorthWest(whitePawns) & whitePawns;
    U64 blackPawnEastCover = shiftSouthEast(blackPawns) & blackPawns;
    U64 blackPawnWestCover = shiftSouthWest(blackPawns) & blackPawns;

    U64 whitePawnCover = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
    U64 blackPawnCover = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);

    // clang-format off
    featureScore += PAWN_DOUBLED_AND_ISOLATED * (
            + bitCount(whiteIsolatedPawns & whiteDoubledPawns)
            - bitCount(blackIsolatedPawns & blackDoubledPawns));
    featureScore += PAWN_DOUBLED * (
            + bitCount(~whiteIsolatedPawns & whiteDoubledPawns)
            - bitCount(~blackIsolatedPawns & blackDoubledPawns));
    featureScore += PAWN_ISOLATED * (
            + bitCount(whiteIsolatedPawns & ~whiteDoubledPawns)
            - bitCount(blackIsolatedPawns & ~blackDoubledPawns));
    featureScore += PAWN_PASSED * (
            + bitCount(whitePassers)
            - bitCount(blackPassers));
    featureScore += PAWN_STRUCTURE * (
            + bitCount(whitePawnEastCover)
            + bitCount(whitePawnWestCover)
            - bitCount(blackPawnEastCover)
            - bitCount(blackPawnWestCover));
    featureScore += PAWN_OPEN * (
            + bitCount(whitePawns & ~fillSouth(blackPawns))
            - bitCount(blackPawns & ~fillNorth(whitePawns)));
    featureScore += PAWN_BACKWARD * (
            + bitCount(fillSouth(~wAttackFrontSpans(whitePawns) & blackPawnCover) & whitePawns)
            - bitCount(fillNorth(~bAttackFrontSpans(blackPawns) & whitePawnCover) & blackPawns));
    featureScore += PAWN_BLOCKED * (
            + bitCount(whiteBlockedPawns)
            - bitCount(blackBlockedPawns));
    
    
    /*
     * only these squares are counted for mobility
     */
    U64 mobilitySquaresWhite = ~whiteTeam & ~(blackPawnCover);
    U64 mobilitySquaresBlack = ~blackTeam & ~(whitePawnCover);
    // clang-format on
    /**********************************************************************************
     *                                  K N I G H T S                                 *
     **********************************************************************************/

    k = b->getPieces()[WHITE_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[square];

        evalScore += fast_knight_psqt[WHITE][psqtKingsideIndex][square];
        mobScore += mobilityKnight[bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresWhite)];

        featureScore += KNIGHT_OUTPOST * isOutpost(square, WHITE, blackPawns, whitePawnCover);
        featureScore += KNIGHT_DISTANCE_ENEMY_KING * manhattanDistance(square, blackKingSquare);

        addToKingSafety(attacks, blackKingZone, bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 2);

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[square];

        evalScore -= fast_knight_psqt[BLACK][psqtKingsideIndex][square];
        mobScore -= mobilityKnight[bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresBlack)];

        featureScore -= KNIGHT_OUTPOST * isOutpost(square, BLACK, whitePawns, blackPawnCover);
        featureScore -= KNIGHT_DISTANCE_ENEMY_KING * manhattanDistance(square, whiteKingSquare);

        addToKingSafety(attacks, whiteKingZone, wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, 2);

        k = lsbReset(k);
    }
    /**********************************************************************************
     *                                  B I S H O P S                                 *
     **********************************************************************************/

    k = b->getPieces()[WHITE_BISHOP];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpBishopAttack(square, occupied);

        evalScore += fast_bishop_psqt[WHITE][psqtKingsideIndex][square];
        mobScore += mobilityBishop[bitCount(attacks & mobilitySquaresWhite)];

        featureScore += bishop_pawn_same_color_table_e[bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
        featureScore += bishop_pawn_same_color_table_o[bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
        featureScore += BISHOP_PIECE_SAME_SQUARE_E 
                        * bitCount(blackTeam & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
        featureScore += BISHOP_FIANCHETTO
                        * (square == G2 && whitePawns & ONE << F2 && whitePawns & ONE << H2
                           && whitePawns & (ONE << G3 | ONE << G4));
        featureScore += BISHOP_FIANCHETTO
                        * (square == B2 && whitePawns & ONE << A2 && whitePawns & ONE << C2
                           && whitePawns & (ONE << B3 | ONE << B4));

        addToKingSafety(attacks, blackKingZone, bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 2);

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_BISHOP];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpBishopAttack(square, occupied);

        evalScore -= fast_bishop_psqt[BLACK][psqtKingsideIndex][square];
        mobScore -= mobilityBishop[bitCount(attacks & mobilitySquaresBlack)];
        featureScore -= bishop_pawn_same_color_table_e[bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
        featureScore -= bishop_pawn_same_color_table_o[bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
        featureScore -= BISHOP_PIECE_SAME_SQUARE_E 
                        * bitCount(whiteTeam & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
        featureScore -= BISHOP_FIANCHETTO
                        * (square == G7 && blackPawns & ONE << F7 && blackPawns & ONE << H7
                           && blackPawns & (ONE << G6 | ONE << G5));
        featureScore -= BISHOP_FIANCHETTO
                        * (square == B2 && blackPawns & ONE << A7 && blackPawns & ONE << C7
                           && blackPawns & (ONE << B6 | ONE << B5));
        addToKingSafety(attacks, whiteKingZone, wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, 2);

        k = lsbReset(k);
    }
    // clang-format off
    featureScore += BISHOP_DOUBLED * (
            + (bitCount(b->getPieces()[WHITE_BISHOP]) == 2)
            - (bitCount(b->getPieces()[BLACK_BISHOP]) == 2));
    // clang-format on
    /**********************************************************************************
     *                                  R O O K S                                     *
     **********************************************************************************/

    k = b->getPieces()[WHITE_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied);

        evalScore += fast_rook_psqt[WHITE][psqtKingsideIndex][square];
        mobScore += mobilityRook[bitCount(attacks & mobilitySquaresWhite)];

        addToKingSafety(attacks, blackKingZone, bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 3);

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied);

        evalScore -= fast_rook_psqt[BLACK][psqtKingsideIndex][square];
        mobScore -= mobilityRook[bitCount(attacks & mobilitySquaresBlack)];

        addToKingSafety(attacks, whiteKingZone, wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, 3);

        k = lsbReset(k);
    }

    // clang-format off
    featureScore += ROOK_KING_LINE * (
            + bitCount(lookUpRookAttack(blackKingSquare, occupied) & b->getPieces(WHITE, ROOK))
            - bitCount(lookUpRookAttack(whiteKingSquare, occupied) & b->getPieces(BLACK, ROOK)));
    featureScore += ROOK_OPEN_FILE * (
            + bitCount(openFiles & b->getPieces(WHITE, ROOK))
            - bitCount(openFiles & b->getPieces(BLACK, ROOK)));
    featureScore += ROOK_HALF_OPEN_FILE * (
            + bitCount(openFilesBlack & ~openFiles & b->getPieces(WHITE, ROOK))
            - bitCount(openFilesWhite & ~openFiles & b->getPieces(BLACK, ROOK)));
    // clang-format on

    /**********************************************************************************
     *                                  Q U E E N S                                   *
     **********************************************************************************/

    k = b->getPieces()[WHITE_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied) | lookUpBishopAttack(square, occupied);

        evalScore += fast_queen_psqt[WHITE][psqtKingsideIndex][square];
        mobScore += mobilityQueen[bitCount(attacks & mobilitySquaresWhite)];
        featureScore += QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, blackKingSquare);

        addToKingSafety(attacks, blackKingZone, bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 4);

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied) | lookUpBishopAttack(square, occupied);

        evalScore -= fast_queen_psqt[BLACK][psqtKingsideIndex][square];
        mobScore -= mobilityQueen[bitCount(attacks & mobilitySquaresBlack)];
        featureScore -= QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, whiteKingSquare);

        addToKingSafety(attacks, whiteKingZone, wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, 4);

        k = lsbReset(k);
    }

    /**********************************************************************************
     *                                  K I N G S                                     *
     **********************************************************************************/
    k = b->getPieces()[WHITE_KING];

    while (k) {
        square = bitscanForward(k);

        evalScore += fast_king_psqt[WHITE][square];
        featureScore += KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & whitePawns);
        featureScore += KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & blackTeam);

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_KING];
    while (k) {
        square = bitscanForward(k);

        evalScore -= fast_king_psqt[BLACK][square];
        featureScore -= KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & blackPawns);
        featureScore -= KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & whiteTeam);

        k = lsbReset(k);
    }

    EvalScore hangingEvalScore = computeHangingPieces(b);
    EvalScore pinnedEvalScore  = computePinnedPieces(b);

    evalScore += kingSafetyTable[bkingSafety_valueOfAttacks] - kingSafetyTable[wkingSafety_valueOfAttacks];
   
    // clang-format off
    featureScore += CASTLING_RIGHTS*(
            + b->getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
            + b->getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING));
    // clang-format on
    featureScore += SIDE_TO_MOVE * (b->getActivePlayer() == WHITE ? 1 : -1);

    EvalScore totalScore = evalScore + pinnedEvalScore + hangingEvalScore + featureScore + mobScore;

    res += MgScore(totalScore) * (1 - phase) + EgScore(totalScore) * (phase);

    if (!hasMatingMaterial(b, res > 0 ? WHITE : BLACK))
        res = res / 10;
    return res;
}

void printEvaluation(Board* board) {

    using namespace std;

    Evaluator ev {};
    Score     score = ev.evaluate(board);
    float     phase = ev.getPhase();

    std::cout <<
        setw(15) << right << "evaluation: " << left << setw(8) << score <<
        setw(15) << right << "phase: "      << left << setprecision(3) << setw(8) << phase << std::endl;
}

float Evaluator::getPhase() { return phase; }



