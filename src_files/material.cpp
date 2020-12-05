//
// Created by Luecx on 04.12.2020.
//
#include "material.h"

using namespace bb;

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

EvalScore fast_pawn_psqt    [2][4][64];
EvalScore fast_knight_psqt  [2][4][64];
EvalScore fast_bishop_psqt  [2][4][64];
EvalScore fast_rook_psqt    [2][4][64];
EvalScore fast_queen_psqt   [2][4][64];
EvalScore fast_king_psqt    [2]   [64];

EvalScore fast_psqt[5][2][4][64]{};


void psqt_init() {
    for (int i = 0; i < 64; i++) {
        for (int kside = 0; kside < 2; kside++) {
            fast_pawn_psqt[WHITE][psqt_kingside_indexing(kside, kside)][i] =
                psqt_pawn_same_side_castle[pst_index_white(i, kside)] + pieceScores[PAWN];
            fast_pawn_psqt[BLACK][psqt_kingside_indexing(kside, kside)][i] = -
                psqt_pawn_same_side_castle[pst_index_black(i, kside)] - pieceScores[PAWN];
            fast_pawn_psqt[WHITE][psqt_kingside_indexing(kside, 1 - kside)][i] =
                psqt_pawn_opposite_side_castle[pst_index_white(i, kside)] + pieceScores[PAWN];
            fast_pawn_psqt[BLACK][psqt_kingside_indexing(1 - kside, kside)][i] = -
                psqt_pawn_opposite_side_castle[pst_index_black(i, kside)] - pieceScores[PAWN];
            
            fast_knight_psqt[WHITE][psqt_kingside_indexing(kside, kside)][i] =
                psqt_knight_same_side_castle[pst_index_white(i, kside)] + pieceScores[KNIGHT];
            fast_knight_psqt[BLACK][psqt_kingside_indexing(kside, kside)][i] = -
                psqt_knight_same_side_castle[pst_index_black(i, kside)] - pieceScores[KNIGHT];
            fast_knight_psqt[WHITE][psqt_kingside_indexing(kside, 1 - kside)][i] =
                psqt_knight_opposite_side_castle[pst_index_white(i, kside)] + pieceScores[KNIGHT];
            fast_knight_psqt[BLACK][psqt_kingside_indexing(1 - kside, kside)][i] = -
                psqt_knight_opposite_side_castle[pst_index_black(i, kside)] - pieceScores[KNIGHT];
            
            fast_bishop_psqt[WHITE][psqt_kingside_indexing(kside, kside)][i] =
                psqt_bishop_same_side_castle[pst_index_white(i, kside)] + pieceScores[BISHOP];
            fast_bishop_psqt[BLACK][psqt_kingside_indexing(kside, kside)][i] = -
                psqt_bishop_same_side_castle[pst_index_black(i, kside)] - pieceScores[BISHOP];
            fast_bishop_psqt[WHITE][psqt_kingside_indexing(kside, 1 - kside)][i] =
                psqt_bishop_opposite_side_castle[pst_index_white(i, kside)] + pieceScores[BISHOP];
            fast_bishop_psqt[BLACK][psqt_kingside_indexing(1 - kside, kside)][i] = -
                psqt_bishop_opposite_side_castle[pst_index_black(i, kside)] - pieceScores[BISHOP];
            
            fast_rook_psqt[WHITE][psqt_kingside_indexing(kside, kside)][i] =
                psqt_rook_same_side_castle[pst_index_white(i, kside)] + pieceScores[ROOK];
            fast_rook_psqt[BLACK][psqt_kingside_indexing(kside, kside)][i] = -
                psqt_rook_same_side_castle[pst_index_black(i, kside)] - pieceScores[ROOK];
            fast_rook_psqt[WHITE][psqt_kingside_indexing(kside, 1 - kside)][i] =
                psqt_rook_opposite_side_castle[pst_index_white(i, kside)] + pieceScores[ROOK];
            fast_rook_psqt[BLACK][psqt_kingside_indexing(1 - kside, kside)][i] = -
                psqt_rook_opposite_side_castle[pst_index_black(i, kside)] - pieceScores[ROOK];
            
            fast_queen_psqt[WHITE][psqt_kingside_indexing(kside, kside)][i] =
                psqt_queen_same_side_castle[pst_index_white(i, kside)] + pieceScores[QUEEN];
            fast_queen_psqt[BLACK][psqt_kingside_indexing(kside, kside)][i] = -
                psqt_queen_same_side_castle[pst_index_black(i, kside)] - pieceScores[QUEEN];
            fast_queen_psqt[WHITE][psqt_kingside_indexing(kside, 1 - kside)][i] =
                psqt_queen_opposite_side_castle[pst_index_white(i, kside)] + pieceScores[QUEEN];
            fast_queen_psqt[BLACK][psqt_kingside_indexing(1 - kside, kside)][i] = -
                psqt_queen_opposite_side_castle[pst_index_black(i, kside)] - pieceScores[QUEEN];
        }
        
        fast_king_psqt[WHITE][i] =  psqt_king[pst_index_white_s(i)];
        fast_king_psqt[BLACK][i] = -psqt_king[pst_index_black_s(i)];
    }
    
    for (int i = 0; i < 64; i++){
        for(int n = 0; n < 2; n++){
            for(int k = 0; k < 4; k++){
                fast_psqt[PAWN][n][k][i] = fast_pawn_psqt[n][k][i];
                fast_psqt[KNIGHT][n][k][i] = fast_knight_psqt[n][k][i];
                fast_psqt[BISHOP][n][k][i] = fast_bishop_psqt[n][k][i];
                fast_psqt[ROOK][n][k][i] = fast_rook_psqt[n][k][i];
                fast_psqt[QUEEN][n][k][i] = fast_queen_psqt[n][k][i];
            }
        }
    }
    
}