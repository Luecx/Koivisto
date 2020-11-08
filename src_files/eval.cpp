
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



EvalScore passer_rank_n[16] = {
    M(0, 0), M(-9, -31), M(-7, -24),  M(1, 4),    M(10, 33),  M(13, 42),   M(8, 29),    M(0, 0),
    M(0, 0), M(1, 2),    M(-22, -73), M(-9, -29), M(-7, -24), M(-15, -51), M(-17, -55), M(0, 0),
};

EvalScore psqt_pawn_same_side_castle[64] = {
    M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0),
    M( -12,  -4), M(  23,   8), M(  11,  31), M( -12,  40), M( -11,  28), M( -19,  24), M( -20,  27), M( -19,  37),
    M(   7,   2), M(  21,   4), M(  11,  19), M(   7,  21), M(  -2,   8), M( -13,  15), M( -20,  19), M( -19,  26),
    M(   3,  10), M(  16,  17), M(  15,  13), M(  19,   6), M(   7,   6), M(   2,   8), M( -17,  32), M( -15,  35),
    M(  19,  18), M(  24,  27), M(  46,   8), M(  35,   8), M(  26,  -2), M(   3,  28), M(  -5,  43), M( -10,  56),
    M(  22,  58), M(  82,  50), M( 120,  31), M(  69,  38), M(  59,  48), M(  49,  81), M(  27, 107), M(  -8, 118),
    M(   6, 128), M(  -4, 117), M( 181,  68), M( 204,  67), M( 185, 104), M( 182, 140), M( 177, 168), M( 190, 171),
    M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0),
};

EvalScore psqt_pawn_opposite_side_castle[64] = {
    M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0),
    M(   7,  28), M(  31,  21), M(  11,  29), M( -14,  39), M( -22,  38), M( -31,  43), M( -49,  48), M( -47,  45),
    M(  21,  22), M(  30,  17), M(  11,  24), M(  -2,  25), M( -12,  25), M( -21,  32), M( -51,  37), M( -47,  34),
    M(  16,  43), M(  15,  43), M(  14,  29), M(  11,  22), M(   9,  19), M(  -2,  26), M( -29,  35), M( -39,  31),
    M(  14,  80), M(  14,  70), M(  19,  51), M(  20,  32), M(  36,  13), M(  39,  13), M( -11,  33), M( -22,  30),
    M(   4, 178), M(  53, 167), M(  52, 139), M(  52,  92), M(  78,  45), M(  95,  23), M(  75,  32), M(   3,  53),
    M( 178, 238), M( 202, 230), M( 165, 210), M( 174, 158), M( 194,  89), M( 192,  42), M(  47,  82), M(  33,  98),
    M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0), M(   0,   0),
};

EvalScore psqt_knight_same_side_castle[64] = {
    M( -38, -62), M(  -5, -48), M(   2, -29), M(   5, -28), M(   3, -25), M( -10, -42), M(  -6, -72), M( -55, -65),
    M(  -3, -28), M(  -9, -12), M(   5, -24), M(   4,  -6), M(   6, -13), M( -16, -23), M( -28, -19), M( -36, -40),
    M(  -5, -29), M(   7,  -4), M(   9,  -1), M(  13,  12), M(   5,  16), M(  -9, -12), M( -13, -22), M( -26, -56),
    M(   6,  -1), M(  34,  18), M(   8,  39), M(  16,  38), M(   2,  34), M(  -4,  23), M( -15,   4), M( -17, -27),
    M(   8,   9), M(  -2,  34), M(  29,  45), M(  -2,  53), M(  14,  50), M(   8,  23), M(  -8,  -1), M( -17, -11),
    M( -14,   6), M(  13,  27), M(  70,  45), M(  53,  27), M(  21,  32), M(  23,  22), M( -10,   2), M( -50, -13),
    M(  11, -11), M( -50,  24), M(  64,  -1), M(  34,  21), M(  42,  24), M(  19,  -6), M( -32,  -8), M( -25, -30),
    M(-164, -53), M( -96,  17), M(-133,  58), M( -18,  15), M( -54,  18), M(-163,  43), M(-159,  14), M(-266, -28),
};

EvalScore psqt_knight_opposite_side_castle[64] = {
    M( -46, -69), M( -33, -13), M(   1, -24), M(   7,  -4), M(   3,   0), M( -13, -15), M( -11, -36), M( -61, -23),
    M( -12, -33), M( -17,  -6), M(   1, -13), M(   9,  -2), M(  13,   3), M(  -6,   1), M( -39,  15), M( -42, -17),
    M( -15, -27), M(  -3,   0), M(  12,   6), M(  12,  27), M(  19,  29), M(   2,  15), M(  -1,  -4), M( -26, -20),
    M(   6,  -2), M(  25,  12), M(  25,  32), M(  22,  44), M(  37,  42), M(  21,  38), M(  34,  18), M(   8,  -3),
    M(   1,   4), M(  15,  12), M(  40,  29), M(  48,  51), M(  25,  59), M(  41,  52), M(  15,  25), M(  17,   0),
    M( -29,  -9), M(  26,   7), M(  49,  30), M(  49,  40), M(  70,  36), M(  91,  37), M(  46,   4), M(  14, -14),
    M( -13, -22), M(  -4,  -3), M(  57, -10), M(  84,  19), M(  58,  15), M(  54,   4), M(  -8,   0), M(  19, -28),
    M(-204, -59), M(-153,  22), M(-130,  35), M( -39,  25), M(  -2,   1), M( -82,  26), M( -44,   8), M(-204, -23),
};

EvalScore psqt_bishop_same_side_castle[64] = {
    M( -16, -20), M( -35,  -3), M( -19,   0), M( -32,   2), M( -26,  -2), M( -23,  -4), M( -15,   1), M(  -4, -18),
    M( -15, -26), M(   9, -18), M(  -9, -10), M( -13,   5), M( -22,   4), M( -11,  -6), M(  -8, -16), M(  -9, -18),
    M(   9,   0), M(  -4,   5), M(  -2,   8), M( -11,  17), M(  -8,  17), M( -13,  11), M(  -4,   2), M( -21,   2),
    M( -15,   7), M( -15,  17), M( -26,  26), M(   6,  19), M(   1,  22), M( -10,  20), M( -21,   8), M( -34,  -1),
    M( -29,  25), M( -12,  32), M(  -1,  28), M(   1,  33), M(  24,  22), M( -16,  20), M(  -9,  13), M( -48,  15),
    M( -20,  26), M(  18,  37), M(  26,  36), M(  22,  26), M(  -1,  20), M(   9,  16), M( -27,  27), M( -33,  11),
    M( -55,  24), M( -24,  29), M(   6,  22), M( -37,  30), M( -47,  30), M( -38,  29), M( -39,  26), M( -88,  32),
    M( -67,  28), M( -23,  17), M(-151,  38), M(-115,  40), M(-134,  46), M(-147,  43), M(-117,  52), M( -89,  44),
};

EvalScore psqt_bishop_opposite_side_castle[64] = {
    M(   7,  11), M( -24,  15), M(  -6,  17), M( -24,  23), M( -21,  23), M( -27,  17), M( -24,  20), M( -21,  18),
    M(  12, -14), M(  43,   2), M(  13,  10), M(   5,  16), M( -15,  21), M( -13,  17), M( -18,   6), M( -11,   7),
    M(  14,  11), M(  28,  19), M(  35,  23), M(   6,  26), M(   4,  33), M( -13,  28), M(  -8,  21), M( -21,  25),
    M(  -9,  22), M(  10,  30), M(  10,  32), M(  37,  30), M(  12,  38), M(   2,  35), M(  -6,  25), M(  -5,  18),
    M( -16,  38), M(   5,  37), M(  19,  38), M(  31,  40), M(  50,  38), M(  18,  32), M(  20,  37), M(  -6,  24),
    M( -23,  37), M(  13,  41), M(  18,  39), M(  29,  39), M(  56,  35), M(  73,  37), M(  48,  29), M(   3,  25),
    M( -60,  44), M( -42,  53), M( -10,  47), M( -42,  48), M( -16,  45), M(  46,  27), M(   6,  32), M( -54,  26),
    M( -71,  69), M( -87,  50), M( -85,  48), M( -89,  58), M(-118,  51), M( -95,  49), M(   7,  35), M( -25,  48),
};

EvalScore psqt_rook_same_side_castle[64] = {
    M( -16, -10), M(  16,   3), M(  20,   6), M(  21,  -1), M(  21,   2), M(  11,   9), M(   3,   7), M(  -6,   2),
    M( -83,  10), M(   9,  -6), M(  20, -10), M(   5,   0), M(   3,   2), M(  -7,   8), M( -10,   0), M( -45,   6),
    M( -20,  -1), M(  22,  17), M(   5,  16), M(  -3,  13), M(  -3,  16), M( -18,  22), M( -10,  21), M( -25,  13),
    M( -11,  26), M(  21,  36), M(   3,  41), M(  -2,  34), M(  -4,  38), M( -14,  44), M( -17,  44), M( -18,  30),
    M(  29,  40), M(  47,  40), M(  49,  43), M(  36,  42), M(  45,  39), M(  17,  46), M(   7,  42), M(  -3,  44),
    M(  48,  45), M( 122,  27), M( 110,  42), M(  85,  29), M(  59,  38), M(  30,  50), M(  42,  40), M(  -7,  53),
    M(  51,  52), M(  61,  56), M( 127,  35), M(  42,  66), M(  43,  64), M(  24,  66), M(  -5,  70), M(   2,  62),
    M( 100,  46), M(  92,  53), M( 105,  50), M(  31,  61), M(  16,  66), M(   9,  70), M(  28,  63), M(  26,  60),
};

EvalScore psqt_rook_opposite_side_castle[64] = {
    M( -29,  18), M(   3,  15), M(  11,  16), M(  19,  11), M(  25,   6), M(  31,  12), M(  29,   8), M(   1,   9),
    M( -82,  20), M( -24,   9), M(  -3,   9), M(  -4,  17), M(   3,  15), M(  14,  12), M(  21,   2), M( -31,  15),
    M( -35,  16), M(   9,  20), M( -16,  27), M( -11,  29), M( -11,  29), M(  -2,  29), M(   9,  28), M(  -4,  15),
    M(  -9,  35), M(  26,  34), M(  -4,  49), M(  -4,  51), M(  -2,  45), M(   0,  53), M(   7,  45), M(  -1,  34),
    M(  25,  46), M(  34,  42), M(  49,  41), M(  55,  49), M(  45,  48), M(  44,  56), M(  45,  40), M(  48,  34),
    M(  39,  48), M(  89,  28), M(  69,  47), M(  82,  42), M(  93,  40), M( 102,  44), M( 121,  26), M(  59,  39),
    M(  47,  52), M(  56,  53), M(  63,  55), M(  55,  71), M(  65,  61), M( 137,  32), M(  68,  48), M(  58,  44),
    M( 120,  30), M(  91,  45), M(  76,  53), M(  52,  60), M(  70,  53), M( 149,  32), M( 105,  42), M(  61,  56),
};

EvalScore psqt_queen_same_side_castle[64] = {
    M( -32, -29), M( -23, -42), M( -13, -32), M(   8, -36), M(   1,   1), M(  -3, -25), M(  -6, -24), M( -13,  -9),
    M( -27, -23), M(  13, -85), M(  21, -80), M(   7, -18), M(   6,  -8), M(   8, -33), M(   1, -16), M( -11, -14),
    M(  15,  -9), M(  22,   0), M(   8,  16), M(   5, -10), M(   0,  -4), M(   2,   5), M(   6,  -1), M( -11,  -3),
    M(  24,  38), M(  22,  33), M(  11,  31), M(  -4,  39), M(  -9,  37), M(  -3,   2), M(  -5,  21), M( -15,  14),
    M(  18,  56), M(  25,  74), M(  12,  76), M(  -9,  65), M( -18,  56), M( -20,  19), M( -25,  45), M( -32,  41),
    M( -38, 113), M(  42,  76), M(  14,  86), M(  24,  51), M(  -5,  32), M( -31,  43), M( -31,  39), M( -47,  63),
    M(  24,  92), M( -49, 164), M(  98,  30), M( -62, 118), M( -59,  91), M( -34,  59), M( -87, 103), M( -45,  76),
    M(  58,  45), M(  23,  88), M(  43,  54), M( -17,  65), M( -17,  59), M( -27,  73), M( -30,  75), M( -45,  76),
};

EvalScore psqt_queen_opposite_side_castle[64] = {
    M( -42,   5), M( -22, -14), M(  -7, -32), M(  -1,   1), M(  -4,  16), M( -10,   9), M( -13,  43), M( -12,  26),
    M( -44, -12), M( -10, -17), M(  22, -48), M(  15,  12), M(  10,   2), M(  17,  -9), M(  21,  -2), M(   9,  20),
    M( -25,  34), M(   8,  23), M(   8,  40), M(   6,  31), M(   6,  24), M(  12,  34), M(  22,  24), M(  22,  29),
    M( -14,  51), M(   2,  61), M(  -9,  73), M(   1,  91), M(  -1,  75), M(  14,  64), M(  22,  54), M(  32,  36),
    M(  -6,  61), M( -19,  95), M( -13,  68), M( -10, 106), M(  12,  96), M(  32,  78), M(  47,  74), M(  32,  55),
    M( -20,  90), M(   3,  56), M(  15,  62), M(   4,  86), M(  41,  90), M(  18, 123), M(  77,  40), M(  12,  65),
    M( -13,  79), M( -53, 130), M(  -6,  94), M( -47, 136), M( -65, 164), M(  82,  63), M( -15, 116), M(  29,  54),
    M(   7,  73), M(  10,  88), M(  -1,  90), M( -22, 110), M( -13, 103), M( 110,  28), M(  19,  82), M(  34,  52),
};

EvalScore psqt_king[64] = {
    M( -32, -94), M( 125, -48), M(  75, -19), M( 120, -12), M(  83, -24), M(  50, -13), M(  41,  -8), M(-145, -64),
    M(  14, -30), M( 115,  12), M( 202, -14), M( 193, -20), M( 149, -13), M( 169,  -9), M(  52,  26), M(  20, -28),
    M(  32,   1), M( 129,  19), M( 191,   6), M( 170,   0), M( 164,  -2), M( 186,   6), M( 116,  18), M(  21,  -3),
    M( -24,  15), M(  85,   9), M( 122,  15), M(  91,  21), M( 107,  15), M( 118,  14), M(  69,   7), M( -57,  11),
    M( -26, -12), M(  72,  -3), M(  77,  16), M(  30,  29), M(  51,  28), M(  49,  19), M(  64,   3), M( -34, -11),
    M( -18, -11), M(   2,   2), M(  13,  15), M(   0,  27), M(  12,  26), M(   0,  23), M(   2,   6), M( -21,  -6),
    M(  38, -24), M(  13,  -6), M(   2,   8), M( -38,  17), M(  -9,  14), M( -22,  18), M(  18,  -5), M(  37, -28),
    M(  42, -72), M(  47, -37), M(  23, -20), M(   8, -34), M(  13, -42), M(  -9, -12), M(  56, -35), M(  63, -87),
};

EvalScore mobilityKnight[9] = {
    M(   20,  -50), M(   35,    0), M(   41,   28), M(   47,   37), M(   54,   44), M(   59,   53), M(   65,   57), M(   76,   51),
    M(   83,   39), };

EvalScore mobilityBishop[14] = {
    M(   20,  -54), M(   27,   11), M(   35,   44), M(   41,   61), M(   49,   73), M(   55,   88), M(   56,   99), M(   57,  107),
    M(   61,  111), M(   68,  112), M(   74,  111), M(   87,  109), M(   93,  127), M(  131,   88), };

EvalScore mobilityRook[15] = {
    M(   26,   22), M(   33,   40), M(   36,   58), M(   39,   74), M(   42,   87), M(   49,   93), M(   54,  101), M(   60,  102),
    M(   64,  108), M(   70,  113), M(   72,  117), M(   77,  120), M(   85,  120), M(   96,  109), M(  165,   75), };

EvalScore mobilityQueen[28] = {
    M(  -10, -164), M(    3,  -89), M(   10,   34), M(   15,   92), M(   18,  134), M(   20,  168), M(   21,  189), M(   25,  207),
    M(   29,  218), M(   32,  227), M(   34,  233), M(   37,  244), M(   38,  251), M(   38,  260), M(   37,  268), M(   34,  274),
    M(   33,  276), M(   32,  279), M(   33,  280), M(   40,  274), M(   43,  270), M(   42,  267), M(   31,  268), M(   46,  260),
    M(    1,  276), M(   58,  247), M(   13,  295), M(   24,  301), };

EvalScore hangingEval[5] {
    M(   -1,    2), M(   -3,   -5), M(   -9,  -10), M(  -13,   -8), M(  -14,  -12),
};

EvalScore pinnedEval[15] {
    M(    2,  -24), M(  -22,   16), M(   -6,   29), M(  -20,  -78), M(  -30,   -8),
    M(  -22,   23), M(   -3,   -8), M(  -40,   -6), M(  -15,    5), M(  -26,  -95),
    M(    0,   -3), M(    1,  -13), M(   22, -222), M(  -36, -339), M(   -9,    9),
};

EvalScore SIDE_TO_MOVE                  = M(   10,   18);
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
EvalScore ROOK_HALF_OPEN_FILE           = M(    0,   -4);
EvalScore ROOK_KING_LINE                = M(   24,    4);
EvalScore BISHOP_DOUBLED                = M(   40,   55);
EvalScore BISHOP_PAWN_SAME_SQUARE       = M(   -5,    5);
EvalScore BISHOP_FIANCHETTO             = M(   -3,   11);
EvalScore QUEEN_DISTANCE_ENEMY_KING     = M(    6,  -25);
EvalScore KING_CLOSE_OPPONENT           = M(  -15,   49);
EvalScore KING_PAWN_SHIELD              = M(   27,    5);
EvalScore CASTLING_RIGHTS               = M(   25,   -8);
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
    &BISHOP_PAWN_SAME_SQUARE,
    &BISHOP_FIANCHETTO,             // 16

    &QUEEN_DISTANCE_ENEMY_KING,     // 17

    &KING_CLOSE_OPPONENT,
    &KING_PAWN_SHIELD,              // 19

    &CASTLING_RIGHTS,               // 20
};



int mobEntryCount[6] {0, 9, 14, 15, 28, 0};

float* phaseValues = new float[6] {
    0, 1, 1, 2, 4, 0,
};

float kingSafetyTable[100] {0,   0,   1,   2,   3,   5,   7,   9,   12,  15,  18,  22,  26,  30,  35,  39,  44,
                            50,  56,  62,  68,  75,  82,  85,  89,  97,  105, 113, 122, 131, 140, 150, 169, 180,
                            191, 202, 213, 225, 237, 248, 260, 272, 283, 295, 307, 319, 330, 342, 354, 366, 377,
                            389, 401, 412, 424, 436, 448, 459, 471, 483, 494, 500, 500, 500, 500, 500, 500, 500,
                            500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
                            500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500};

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

    // values to scale early/lategame weights
    float earlyWeightScalar = (1 - phase);
    float lateWeightScalar  = (phase);

    // the pst are multiples of 100
    float earlyPSTScalar = earlyWeightScalar / 100;
    float latePSTScalar  = lateWeightScalar / 100;

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

        featureScore += BISHOP_PAWN_SAME_SQUARE
                        * bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
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
        featureScore -= BISHOP_PAWN_SAME_SQUARE
                        * bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));

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

    EvalScore hangingEval = computeHangingPieces(b);
    EvalScore pinnedEval  = computePinnedPieces(b);

    float kingSafetyTableScore =
        (kingSafetyTable[bkingSafety_valueOfAttacks] - kingSafetyTable[wkingSafety_valueOfAttacks]);
    float earlySum = kingSafetyTableScore * 3.7182693;
    float endSum   = kingSafetyTableScore * -1.4492081;

    // clang-format off
    featureScore += CASTLING_RIGHTS*(
            + b->getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
            + b->getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING));
    // clang-format on
    featureScore += SIDE_TO_MOVE * (b->getActivePlayer() == WHITE ? 1 : -1);

    EvalScore totalScore = evalScore + pinnedEval + hangingEval + featureScore + mobScore;

    res = earlySum * (1 - phase) + endSum * (phase);
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

    stringstream ss {};

    // String format = "%-30s | %-20s | %-20s %n";

    ss << std::setw(40) << std::left << "feature"
       << " | " << std::setw(20) << std::right << "difference"
       << " | " << std::setw(20) << "early weight"
       << " | " << std::setw(20) << "late weight"
       << " | " << std::setw(20) << "tapered weight"
       << " | " << std::setw(20) << "sum"
       << "\n";

    ss << "-----------------------------------------+----------------------+"
          "----------------------+----------------------+"
          "----------------------+----------------------+\n";
    ss << std::setw(40) << std::left << "PHASE"
       << " | " << std::setw(20) << std::right << ""
       << " | " << std::setw(20) << "0"
       << " | " << std::setw(20) << "1"
       << " | " << std::setw(20) << phase << " | " << std::setw(20) << phase << " | \n";

    ss << "-----------------------------------------+----------------------+"
          "----------------------+----------------------+"
          "----------------------+----------------------+\n";

    string names[] {
        "INDEX_PAWN_VALUE",
        "INDEX_PAWN_PSQT",
        "INDEX_PAWN_STRUCTURE",
        "INDEX_PAWN_PASSED",
        "INDEX_PAWN_ISOLATED",
        "INDEX_PAWN_DOUBLED",
        "INDEX_PAWN_DOUBLED_AND_ISOLATED",
        "INDEX_PAWN_BACKWARD",
        "INDEX_PAWN_OPEN",

        "INDEX_KNIGHT_VALUE",
        "INDEX_KNIGHT_PSQT",
        "INDEX_KNIGHT_MOBILITY",
        "INDEX_KNIGHT_OUTPOST",

        "INDEX_BISHOP_VALUE",
        "INDEX_BISHOP_PSQT",
        "INDEX_BISHOP_MOBILITY",
        "INDEX_BISHOP_DOUBLED",
        "INDEX_BISHOP_PAWN_SAME_SQUARE",
        "INDEX_BISHOP_FIANCHETTO",

        "INDEX_ROOK_VALUE",
        "INDEX_ROOK_PSQT",
        "INDEX_ROOK_MOBILITY",
        "INDEX_ROOK_OPEN_FILE",
        "INDEX_ROOK_HALF_OPEN_FILE",
        "INDEX_ROOK_KING_LINE",

        "INDEX_QUEEN_VALUE",
        "INDEX_QUEEN_PSQT",
        "INDEX_QUEEN_MOBILITY",

        "INDEX_KING_SAFETY",
        "INDEX_KING_PSQT",
        "INDEX_KING_CLOSE_OPPONENT",
        "INDEX_KING_PAWN_SHIELD",

        "INDEX_KNIGHT_DISTANCE_ENEMY_KING",
        "INDEX_QUEEN_DISTANCE_ENEMY_KING",

        "INDEX_PINNED_PAWN_BY_BISHOP",
        "INDEX_PINNED_PAWN_BY_ROOK",
        "INDEX_PINNED_PAWN_BY_QUEEN",
        "INDEX_PINNED_KNIGHT_BY_BISHOP",
        "INDEX_PINNED_KNIGHT_BY_ROOK",
        "INDEX_PINNED_KNIGHT_BY_QUEEN",
        "INDEX_PINNED_BISHOP_BY_BISHOP",
        "INDEX_PINNED_BISHOP_BY_ROOK",
        "INDEX_PINNED_BISHOP_BY_QUEEN",
        "INDEX_PINNED_ROOK_BY_BISHOP",
        "INDEX_PINNED_ROOK_BY_ROOK",
        "INDEX_PINNED_ROOK_BY_QUEEN",
        "INDEX_PINNED_QUEEN_BY_BISHOP",
        "INDEX_PINNED_QUEEN_BY_ROOK",
        "INDEX_PINNED_QUEEN_BY_QUEEN",

        "INDEX_PAWN_HANGING",
        "INDEX_KNIGHT_HANGING",
        "INDEX_BISHOP_HANGING",
        "INDEX_ROOK_HANGING",
        "INDEX_QUEEN_HANGING",

        // ignore this and place new values before here
        "-",
        "-",
        "-",
        "-",
    };

//    for (int i = 0; i < unusedVariable; i++) {
//
//        ss << std::setw(40) << std::left << names[i] << " | " << std::setw(20) << std::right << ev.getFeatures()[i]
//           << " | " << std::setw(20) << ev.getEarlyGameParams()[i] << " | " << std::setw(20)
//           << ev.getLateGameParams()[i] << " | " << std::setw(20)
//           << ev.getEarlyGameParams()[i] * (1 - phase) + ev.getLateGameParams()[i] * phase << " | " << std::setw(20)
//           << (ev.getEarlyGameParams()[i] * (1 - phase) + ev.getLateGameParams()[i] * phase) * ev.getFeatures()[i]
//           << " | \n";
//    }
    ss << "-----------------------------------------+----------------------+"
          "----------------------+----------------------+"
          "----------------------+----------------------+\n";

    ss << std::setw(40) << std::left << "TOTAL"
       << " | " << std::setw(20) << std::right << ""
       << " | " << std::setw(20) << ""
       << " | " << std::setw(20) << ""
       << " | " << std::setw(20) << ""
       << " | " << std::setw(20) << score << " | \n";

    ss << "-----------------------------------------+----------------------+"
          "----------------------+----------------------+"
          "----------------------+----------------------+\n";

    std::cout << ss.str() << std::endl;
}

float* Evaluator::getFeatures() { return nullptr; }

float Evaluator::getPhase() { return phase; }

float* Evaluator::getEarlyGameParams() { return nullptr; }

float* Evaluator::getLateGameParams() { return nullptr; }

int Evaluator::paramCount() { return 0; }

float* Evaluator::getPSQT(Piece piece, bool early) {
    switch (piece) {
        //         case PAWN: return early ? psqt_pawn : psqt_pawn_endgame;
        //        case KNIGHT: return early ? psqt_knight : psqt_knight_endgame;
        //        case BISHOP: return early ? psqt_bishop : psqt_bishop_endgame;
        //        case ROOK: return early ? psqt_rook : psqt_rook_endgame;
        //        case QUEEN: return early ? psqt_queen : psqt_queen_endgame;
        //        case KING: return early ? psqt_king : psqt_king_endgame;
    }
    return nullptr;
}
