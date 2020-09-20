//
// Created by finne on 5/31/2020.
//

#include "eval.h"

#include <immintrin.h>
#include <iomanip>

#define pst_index_white(s) squareIndex(7 - rankIndex(s), fileIndex(s))
#define pst_index_black(s) s

float sqrts[28] = {
    0,       1,       1.41421, 1.73205, 2,       2.23607, 2.44949, 2.64575, 2.82843, 3,
    3.16228, 3.31662, 3.4641,  3.60555, 3.74166, 3.87298, 4,       4.12311, 4.24264, 4.3589,
    4.47214, 4.58258, 4.69042, 4.79583, 4.89898, 5,       5.09902, 5.19615,
};

float psqt_pawn_endgame[64] = {
    0,  0,  0,  0,  0,   0,   0,   0,   12,  -1,  3,   6,   9,  -3, -11, -1, 2,  2,  -6, -3, -4, -4,
    -8, -5, 15, 8,  -3,  -15, -12, -5,  0,   4,   29,  22,  13, -1, -1,  7,  15, 17, 84, 88, 77, 58,
    51, 55, 73, 75, 172, 163, 144, 132, 135, 131, 150, 163, 0,  0,  0,   0,  0,  0,  0,  0,
};

float psqt_pawn[64] = {
    0,   0,   0,   0,  0,  0,  0,  0,  -21, 38,  39, -2, -13, -30, -7, -20, -12, 11,  7,  8,  -7, -9,
    -29, -23, -25, -8, 7,  33, 23, -2, -28, -31, -2, 14, 27,  22,  23, -4,  -12, -12, 26, 37, 39, 39,
    34,  15,  -3,  7,  35, 34, 36, 36, 36,  36,  38, 37, 0,   0,   0,  0,   0,   0,   0,  0,
};

float psqt_knight[64] = {
    -243, -75, -77, -67, -7,  -190, -72, -134, -196, -173, 115, -21, -30,  29, -39, -131, -141, 52,  -19, 52,  96, 154,
    60,   5,   -36, -4,  -41, 68,   16,  106,  -14,  20,   -36, -10, -5,   7,  42,  20,   39,   -12, -80, -42, 14, 16,
    47,   24,  32,  -52, -37, -79,  -28, 40,   39,   37,   -10, -15, -124, 0,  -89, -28,  27,   -37, 4,   -47,
};
float psqt_knight_endgame[64] = {
    -63, -58, -5,  -36, -26, -25, -85, -132, -1,  23,  -52, 8,   -2,  -33, -19, -43, -17, -28, 17,  0,   -19, -26,
    -34, -54, -11, -2,  31,  11,  23,  -5,   6,   -26, -15, -15, 14,  31,  12,  11,  -4,  -24, -8,  3,   -16, 11,
    -4,  -20, -36, -13, -40, -11, -7,  -19,  -11, -27, -24, -59, -17, -42, -13, -1,  -32, -10, -49, -70,
};

float psqt_bishop[64] = {
    -233, -118, -254, -158, -98, -175, -36, -71,  -332, -132, -210, -219, -27,  100, -51,  -401,
    -214, -93,  33,   -43,  -43, 71,   50,  -148, -159, -68,  -110, 82,   4,    25,  -79,  -88,
    -86,  -85,  -22,  -15,  62,  -78,  -58, -59,  -102, 22,   -22,  -9,   -52,  54,  -32,  -69,
    22,   -11,  30,   -60,  11,  -8,   43,  -44,  -180, 10,   -86,  -116, -107, -59, -150, -198,
};

float psqt_bishop_endgame[64] = {
    -62, -25, -52, -28, -21, -30, -7,  -49, 3,  -15, 6,   -44, 2,  0,  -17, -21, 11,  12, -23, -18, -14, 11,
    4,   17,  -4,  28,  50,  5,   25,  13,  -3, -7,  -11, 10,  38, 87, -30, 32,  -21, 13, 8,   -9,  86,  55,
    135, 17,  28,  23,  -18, -19, -19, 31,  44, 14,  9,   -35, 2,  27, 27,  44,  43,  7,  41,  11,
};

float psqt_rook[64] = {
    -5, 4,  -17, 9,   5,   -3,  -3,  -8, -16, -6, 19,  18,  29,  28, -9, -1,  -13, 7,  1,   6,   -10, 13,
    26, 6,  -31, -11, 0,   6,   5,   12, -10, -1, -33, -18, -12, -7, 3,  -13, 2,   -2, -42, -13, -7,  -15,
    -5, -4, -8,  -19, -38, -12, -20, -1, 8,   8,  -16, -59, -10, -5, 4,  12,  12,  9,  -21, 9,
};

float psqt_rook_endgame[64] = {
    20, 14, 24, 17,  25, 21, 21, 15, 29, 29, 24, 25, 11, 20, 30, 20, 15, 16, 15, 21,  16, 13,
    9,  2,  17, 10,  24, 8,  15, 15, 9,  6,  14, 16, 18, 13, 3,  7,  5,  -7, 10, 7,   -1, 7,
    1,  -3, 3,  -11, 6,  4,  9,  10, -5, 0,  1,  8,  1,  8,  4,  0,  -3, -2, 6,  -25,
};

float psqt_queen[64] = {
    -5, 4,  -17, 9,   5,   -3,  -3,  -8, -16, -6, 19,  18,  29,  28, -9, -1,  -13, 7,  1,   6,   -10, 13,
    26, 6,  -31, -11, 0,   6,   5,   12, -10, -1, -33, -18, -12, -7, 3,  -13, 2,   -2, -42, -13, -7,  -15,
    -5, -4, -8,  -19, -38, -12, -20, -1, 8,   8,  -16, -59, -10, -5, 4,  12,  12,  9,  -21, 9,
};

float psqt_queen_endgame[64] = {
    20, 14, 24, 17,  25, 21, 21, 15, 29, 29, 24, 25, 11, 20, 30, 20, 15, 16, 15, 21,  16, 13,
    9,  2,  17, 10,  24, 8,  15, 15, 9,  6,  14, 16, 18, 13, 3,  7,  5,  -7, 10, 7,   -1, 7,
    1,  -3, 3,  -11, 6,  4,  9,  10, -5, 0,  1,  8,  1,  8,  4,  0,  -3, -2, 6,  -25,

};

float psqt_king[64] = {
    -46, -8,  -3,  -42, -43, -15, -12, -3,  -3,  -14, -1,  -22, -23, 0,   -1,  -10, 9,   0,   -7, -39, -32, -9,
    0,   -17, -15, -18, -34, -37, -48, -32, -10, -46, -41, -31, -42, -69, -64, -47, -15, -28, -9, -7,  -30, -43,
    -41, -22, 0,   -17, 26,  28,  -9,  -23, -22, -12, 21,  18,  28,  30,  1,   5,   5,   1,   32, 33,
};
float psqt_king_endgame[64] = {
    -59, -13, 7,   -22, -11, -16, -17, -68, -8, 10,  30,  29,  28,  30,  19,  -20, -10, 30,  51,  33,  27, 50,
    30,  -11, -18, 16,  34,  35,  40,  38,  17, -22, -33, -3,  28,  42,  38,  29,  -8,  -36, -26, 6,   20, 34,
    33,  21,  2,   -27, -44, -24, 10,  15,  15, 7,   -20, -39, -84, -45, -32, -40, -40, -31, -46, -84,
};

#ifdef TUNE_PST
float* tunablePST_MG_grad = new float[64] {};
float* tunablePST_EG_grad = new float[64] {};
#endif

float* _pieceValuesEarly = new float[unusedVariable] {
    90.612625,  52.992596,  5.3940973,  -2.5785639, -8.8384104, 8.0322876,  -5.2987285, -12.69003,
    13.774286,  461.8082,   48.244282,  27.408546,  25.548733,  477.50644,  32.163502,  24.40115,
    41.309193,  -5.9049501, 12.095577,  582.96118,  96.298126,  21.105064,  61.523682,  17.76198,
    14.738617,  1344.8832,  105.14661,  12.180225,  377.4198,   242.59882,  -54.686356, 11.756104,
    -5.9028187, 3.9798086,  20.540142,  -8.871707,  1.9888093,  -38.071625, -41.082283, -40.033123,
    -17.044693, -65.15358,  15.851129,  -61.761784, -6.1620493, -12.029309, -203.16486, -342.46075,
    -13.027541, -7.6117043, -18.215183, -11.174079, -9.8015833, -8.0599508, 12.237782,  0,

};

float* _pieceValuesLate = new float[unusedVariable] {
    102.09441,  174.62131,  12.113035,  43.921768,  -8.8293295, -5.7698684, -20.551733, 2.1095219,
    -4.4272146, 327.82663,  89.724663,  19.159079,  18.673559,  292.28979,  8.8539467,  31.280064,
    53.719395,  5.7388086,  28.595366,  595.83856,  100.94516,  29.340246,  -15.963839, 2.4505064,
    5.2854605,  1131.0789,  39.692463,  55.545258,  -124.80446, 44.435101,  54.995464,  -2.1891308,
    -1.3747581, -24.002651, -39.86771,  19.803225,  25.614729,  -153.57431, -51.70187,  -19.080151,
    -10.514104, -65.737129, -93.600891, -387.21597, -10.381309, -71.168625, -488.58844, -595.0282,
    -14.574704, 1.8221486,  -2.3508673, -9.2667027, -8.6066637, -9.1349344, -2.7119722, 0,

};

float* phaseValues = new float[6] {
    0, 1, 1, 2, 4, 0,
};

// TODO tweak values
float kingSafetyTable[100] {0,   0,   1,   2,   3,   5,   7,   9,   12,  15,  18,  22,  26,  30,  35,  39,  44,
                            50,  56,  62,  68,  75,  82,  85,  89,  97,  105, 113, 122, 131, 140, 150, 169, 180,
                            191, 202, 213, 225, 237, 248, 260, 272, 283, 295, 307, 319, 330, 342, 354, 366, 377,
                            389, 401, 412, 424, 436, 448, 459, 471, 483, 494, 500, 500, 500, 500, 500, 500, 500,
                            500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
                            500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500};

/**
 * adds the factor to value of attacks if the piece attacks the kingzone
 * @param attacks
 * @param kingZone
 * @param pieceCount
 * @param valueOfAttacks
 * @param factor
 */
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

void Evaluator::computeHangingPieces(Board* b) {
    U64 WnotAttacked = ~b->getAttackedSquares(WHITE);
    U64 BnotAttacked = ~b->getAttackedSquares(BLACK);

    for (int i = PAWN; i <= QUEEN; i++) {
        features[INDEX_PAWN_HANGING + i] =
            +bitCount(b->getPieces(WHITE, i) & WnotAttacked) - bitCount(b->getPieces(BLACK, i) & BnotAttacked);
    }
}

void Evaluator::computePinnedPieces(Board* b) {

    for (int i = 0; i < 15; i++) {
        features[INDEX_PINNED_PAWN_BY_BISHOP + i] = 0;
    }

    Square square;
    Square wkingSq = bitscanForward(b->getPieces(WHITE, KING));
    U64    pinner  = lookUpRookXRayAttack(wkingSq, *b->getOccupied(), b->getTeamOccupied()[WHITE])
                 & (b->getPieces(BLACK, ROOK) | b->getPieces(BLACK, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[wkingSq][square] & b->getTeamOccupied()[WHITE]);
        features[INDEX_PINNED_PAWN_BY_BISHOP + 3 * (b->getPiece(pinnedPlace) % 6)
                 + (b->getPiece(square) % 6 - BISHOP)] += 1;
        pinner = lsbReset(pinner);
    }

    pinner = lookUpBishopXRayAttack(wkingSq, *b->getOccupied(), b->getTeamOccupied()[WHITE])
             & (b->getPieces(BLACK, BISHOP) | b->getPieces(BLACK, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[wkingSq][square] & b->getTeamOccupied()[WHITE]);

        features[INDEX_PINNED_PAWN_BY_BISHOP + 3 * (b->getPiece(pinnedPlace) % 6)
                 + (b->getPiece(square) % 6 - BISHOP)] += 1;
        pinner = lsbReset(pinner);
    }

    Square bkingSq = bitscanForward(b->getPieces(BLACK, KING));
    pinner         = lookUpRookXRayAttack(bkingSq, *b->getOccupied(), b->getTeamOccupied()[BLACK])
             & (b->getPieces(WHITE, ROOK) | b->getPieces(WHITE, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[bkingSq][square] & b->getTeamOccupied()[BLACK]);
        features[INDEX_PINNED_PAWN_BY_BISHOP + 3 * (b->getPiece(pinnedPlace) % 6)
                 + (b->getPiece(square) % 6 - BISHOP)] -= 1;
        pinner = lsbReset(pinner);
    }
    pinner = lookUpBishopXRayAttack(bkingSq, *b->getOccupied(), b->getTeamOccupied()[BLACK])
             & (b->getPieces(WHITE, BISHOP) | b->getPieces(WHITE, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[bkingSq][square] & b->getTeamOccupied()[BLACK]);

        features[INDEX_PINNED_PAWN_BY_BISHOP + 3 * (b->getPiece(pinnedPlace) % 6)
                 + (b->getPiece(square) % 6 - BISHOP)] -= 1;
        pinner = lsbReset(pinner);
    }
}

/**
 * evaluates the board.
 * @param b
 * @return
 */
bb::Score Evaluator::evaluate(Board* b) {

    Score res = 0;

#ifdef TUNE_PST
    for (int i = 0; i < 64; i++) {
        tunablePST_MG_grad[i] = 0;
        tunablePST_EG_grad[i] = 0;
    }
#endif

    memset(features, 0, unusedVariable * sizeof(float));

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

    // clang-format off
    phase =
            (24.0f + phaseValues[5]
             - phaseValues[0] * bitCount(
                    b->getPieces()[WHITE_PAWN] |
                    b->getPieces()[BLACK_PAWN])
             - phaseValues[1] * bitCount(
                    b->getPieces()[WHITE_KNIGHT] |
                    b->getPieces()[BLACK_KNIGHT])
             - phaseValues[2] * bitCount(
                    b->getPieces()[WHITE_BISHOP] |
                    b->getPieces()[BLACK_BISHOP])
             - phaseValues[3] * bitCount(
                    b->getPieces()[WHITE_ROOK] |
                    b->getPieces()[BLACK_ROOK])
             - phaseValues[4] * bitCount(
                    b->getPieces()[WHITE_QUEEN] |
                    b->getPieces()[BLACK_QUEEN])) / 24.0f;
    
    
    if (phase > 1) phase = 1;
    if (phase < 0) phase = 0;
    
    
    //values to scale early/lategame weights
    float earlyWeightScalar = (1 - phase);
    float lateWeightScalar  = (phase);
    
    //the pst are multiples of 100
    float earlyPSTScalar = earlyWeightScalar / 100;
    float latePSTScalar  = lateWeightScalar / 100;
    
    
    int whitekingSafety_attackingPiecesCount = 0;
    int whitekingSafety_valueOfAttacks       = 0;
    
    int blackkingSafety_attackingPiecesCount = 0;
    int blackkingSafety_valueOfAttacks       = 0;
    /**********************************************************************************
     *                                  P A W N S                                     *
     **********************************************************************************/
    
    
    U64 whitePawns = b->getPieces()[WHITE_PAWN];
    U64 blackPawns = b->getPieces()[BLACK_PAWN];
    
    bool wKSide = (fileIndex(bitscanForward(b->getPieces()[WHITE_KING])) > 3 ? 0 : 1);
    bool bKSide = (fileIndex(bitscanForward(b->getPieces()[BLACK_KING])) > 3 ? 0 : 1);
    
    
    //all passed pawns for white/black
    U64 whitePassers = wPassedPawns(whitePawns, blackPawns);
    U64 blackPassers = bPassedPawns(blackPawns, whitePawns);
    
    //doubled pawns without the pawn least developed
    U64 whiteDoubledWithoutFirst = wFrontSpans(whitePawns) & whitePawns;
    U64 blackDoubledWithoutFirst = bFrontSpans(blackPawns) & blackPawns;
    
    //all doubled pawns
    U64 whiteDoubledPawns = whiteDoubledWithoutFirst | (wRearSpans(whiteDoubledWithoutFirst) & whitePawns);
    U64 blackDoubledPawns = blackDoubledWithoutFirst | (bRearSpans(blackDoubledWithoutFirst) & blackPawns);
    
    //all isolated pawns
    U64 whiteIsolatedPawns = whitePawns & ~(fillFile(shiftWest(whitePawns) | shiftEast(whitePawns)));
    U64 blackIsolatedPawns = blackPawns & ~(fillFile(shiftWest(blackPawns) | shiftEast(blackPawns)));
    
    
    k = whitePawns;
    while (k) {
        square = bitscanForward(k);
    
        features[INDEX_PAWN_PSQT] += psqt_pawn[squareIndex(rankIndex(square), (wKSide ? fileIndex(square) : 7 - fileIndex(square)))] * earlyPSTScalar;
        features[INDEX_PAWN_PSQT] += psqt_pawn_endgame[square] * latePSTScalar;
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_PAWN];
    while (k) {
        square = bitscanForward(k);
    
        features[INDEX_PAWN_PSQT] -=
                psqt_pawn[squareIndex(7 - rankIndex(square), (bKSide ? fileIndex(square) : 7 - fileIndex(square)))] * earlyPSTScalar;
        features[INDEX_PAWN_PSQT] -=
                psqt_pawn_endgame[squareIndex(7 - rankIndex(square), fileIndex(square))] * latePSTScalar;
        
        k = lsbReset(k);
    }
    
    
    U64 whitePawnEastCover = shiftNorthEast(whitePawns) & whitePawns;
    U64 whitePawnWestCover = shiftNorthWest(whitePawns) & whitePawns;
    U64 blackPawnEastCover = shiftSouthEast(blackPawns) & blackPawns;
    U64 blackPawnWestCover = shiftSouthWest(blackPawns) & blackPawns;
    
    U64 whitePawnCover = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
    U64 blackPawnCover = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);
    
    features[INDEX_PAWN_DOUBLED_AND_ISOLATED] =
            +bitCount(whiteIsolatedPawns & whiteDoubledPawns)
            - bitCount(blackIsolatedPawns & blackDoubledPawns);
    features[INDEX_PAWN_DOUBLED]              =
            +bitCount(~whiteIsolatedPawns & whiteDoubledPawns)
            - bitCount(~blackIsolatedPawns & blackDoubledPawns);
    features[INDEX_PAWN_ISOLATED]             =
            +bitCount(whiteIsolatedPawns & ~whiteDoubledPawns)
            - bitCount(blackIsolatedPawns & ~blackDoubledPawns);
    features[INDEX_PAWN_PASSED]               =
            +bitCount(whitePassers)
            - bitCount(blackPassers);
    features[INDEX_PAWN_VALUE]                =
            +bitCount(b->getPieces()[WHITE_PAWN])
            - bitCount(b->getPieces()[BLACK_PAWN]);
    features[INDEX_PAWN_STRUCTURE]            =
            +bitCount(whitePawnEastCover)
            + bitCount(whitePawnWestCover)
            - bitCount(blackPawnEastCover)
            - bitCount(blackPawnWestCover);
    features[INDEX_PAWN_OPEN]                 =
            +bitCount(whitePawns & ~fillSouth(blackPawns))
            - bitCount(blackPawns & ~fillNorth(whitePawns));
    features[INDEX_PAWN_BACKWARD]             =
            +bitCount(fillSouth(~wAttackFrontSpans(whitePawns) & blackPawnCover) & whitePawns)
            - bitCount(fillNorth(~bAttackFrontSpans(blackPawns) & whitePawnCover) & blackPawns);
    
    
    
    /*
     * only these squares are counted for mobility
     */
    U64 mobilitySquaresWhite = ~whiteTeam & ~(blackPawnCover);
    U64 mobilitySquaresBlack = ~blackTeam & ~(whitePawnCover);
    
    /**********************************************************************************
     *                                  K N I G H T S                                 *
     **********************************************************************************/

    
    
    k = b->getPieces()[WHITE_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[square];
    
    
        features[INDEX_KNIGHT_PSQT] += psqt_knight[pst_index_white(square)] * earlyPSTScalar;
        features[INDEX_KNIGHT_PSQT] += psqt_knight_endgame[pst_index_white(square)] * latePSTScalar;
    
    
        features[INDEX_KNIGHT_MOBILITY] += sqrts[bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresWhite)];
        features[INDEX_KNIGHT_OUTPOST] += isOutpost(square, WHITE, blackPawns, whitePawnCover);
        features[INDEX_KNIGHT_DISTANCE_ENEMY_KING] += manhattanDistance(square, blackKingSquare);
        
        
        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks,
                        2);
        
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[square];
    
    
        features[INDEX_KNIGHT_PSQT] -= psqt_knight[pst_index_black(square)] * earlyPSTScalar;
        features[INDEX_KNIGHT_PSQT] -= psqt_knight_endgame[pst_index_black(square)] * latePSTScalar;;
    
    
        features[INDEX_KNIGHT_MOBILITY] -= sqrts[bitCount(attacks & mobilitySquaresBlack)];
        features[INDEX_KNIGHT_OUTPOST] -= isOutpost(square, BLACK, whitePawns, blackPawnCover);
        features[INDEX_KNIGHT_DISTANCE_ENEMY_KING] -= manhattanDistance(square, whiteKingSquare);
        
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks,
                        2);
        
        k = lsbReset(k);
    }
    features[INDEX_KNIGHT_VALUE] = (bitCount(b->getPieces()[WHITE_KNIGHT]) -
                                    bitCount(b->getPieces()[BLACK_KNIGHT]));
    /**********************************************************************************
     *                                  B I S H O P S                                 *
     **********************************************************************************/

    k = b->getPieces()[WHITE_BISHOP];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpBishopAttack(square, occupied);

#ifdef TUNE_PST
        tunablePST_MG_grad[pst_index_white(square)] += _pieceValuesEarly[INDEX_BISHOP_PSQT] * (1-phase) / 100;
        tunablePST_EG_grad[pst_index_white(square)] += _pieceValuesLate [INDEX_BISHOP_PSQT] * phase     / 100;
#endif
    
        features[INDEX_BISHOP_PSQT] += psqt_bishop[pst_index_white(square)] * earlyPSTScalar;
        features[INDEX_BISHOP_PSQT] += psqt_bishop_endgame[pst_index_white(square)] * (phase) / 100.0;
    
        features[INDEX_BISHOP_MOBILITY] += sqrts[bitCount(attacks & mobilitySquaresWhite)];
        features[INDEX_BISHOP_PAWN_SAME_SQUARE] += bitCount(
                blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
    
    
        features[INDEX_BISHOP_FIANCHETTO] +=
                (square == G2 &&
                 whitePawns & ONE << F2 &&
                 whitePawns & ONE << H2 &&
                 whitePawns & (ONE << G3 | ONE << G4));
        features[INDEX_BISHOP_FIANCHETTO] +=
                (square == B2 &&
                 whitePawns & ONE << A2 &&
                 whitePawns & ONE << C2 &&
                 whitePawns & (ONE << B3 | ONE << B4));
        
        
        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks,
                        2);
        
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_BISHOP];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpBishopAttack(square, occupied);

#ifdef TUNE_PST
        tunablePST_MG_grad[pst_index_black(square)] -= _pieceValuesEarly[INDEX_BISHOP_PSQT] * (1-phase) / 100;
        tunablePST_EG_grad[pst_index_black(square)] -= _pieceValuesLate [INDEX_BISHOP_PSQT] * phase     / 100;
#endif
    
        features[INDEX_BISHOP_PSQT] -= psqt_bishop[pst_index_black(square)] * earlyPSTScalar;
        features[INDEX_BISHOP_PSQT] -= psqt_bishop_endgame[pst_index_black(square)] * latePSTScalar;
    
        features[INDEX_BISHOP_MOBILITY] -= sqrts[bitCount(attacks & mobilitySquaresBlack)];
        features[INDEX_BISHOP_PAWN_SAME_SQUARE] -= bitCount(
                whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
    
    
        features[INDEX_BISHOP_FIANCHETTO] -=
                (square == G7 &&
                 blackPawns & ONE << F7 &&
                 blackPawns & ONE << H7 &&
                 blackPawns & (ONE << G6 | ONE << G5));
        features[INDEX_BISHOP_FIANCHETTO] -=
                (square == B2 &&
                 blackPawns & ONE << A7 &&
                 blackPawns & ONE << C7 &&
                 blackPawns & (ONE << B6 | ONE << B5));
        
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks,
                        2);
        
        k = lsbReset(k);
    }
    features[INDEX_BISHOP_VALUE] = (bitCount(b->getPieces()[WHITE_BISHOP]) - bitCount(b->getPieces()[BLACK_BISHOP]));
    features[INDEX_BISHOP_DOUBLED] =
            (bitCount(b->getPieces()[WHITE_BISHOP]) == 2) - (bitCount(b->getPieces()[BLACK_BISHOP]) == 2);
    /**********************************************************************************
     *                                  R O O K S                                     *
     **********************************************************************************/

    k = b->getPieces()[WHITE_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied);
    
    
        features[INDEX_ROOK_PSQT] += psqt_rook[pst_index_white(square)] * earlyPSTScalar;
        features[INDEX_ROOK_PSQT] += psqt_rook_endgame[pst_index_white(square)] * (phase) / 100.0;
    
    
        features[INDEX_ROOK_MOBILITY] += sqrts[bitCount(attacks & mobilitySquaresWhite)];
        
        if (lookUpRookAttack(square, ZERO) & b->getPieces()[BLACK_KING]) {
            //rook on same file or rank as king
            features[INDEX_ROOK_KING_LINE]++;
        }
        if ((whitePawns & FILES[fileIndex(square)]) == 0) {
            if ((blackPawns & FILES[fileIndex(square)]) == 0) {
                //open
                features[INDEX_ROOK_OPEN_FILE]++;
            } else {
                //half open
                features[INDEX_ROOK_HALF_OPEN_FILE]++;
            }
        }
        
        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks,
                        3);
        
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied);
    
    
        features[INDEX_ROOK_PSQT] -= psqt_rook[pst_index_black(square)] * earlyPSTScalar;
        features[INDEX_ROOK_PSQT] -= psqt_rook_endgame[pst_index_black(square)] * (phase) / 100.0;
    
        features[INDEX_ROOK_MOBILITY] -= sqrts[bitCount(attacks & mobilitySquaresBlack)];
        
        if (lookUpRookAttack(square, ZERO) & b->getPieces()[WHITE_KING]) {
            //rook on same file or rank as king
            features[INDEX_ROOK_KING_LINE]--;
        }
        
        if ((whitePawns & FILES[fileIndex(square)]) == 0) {
            if ((blackPawns & FILES[fileIndex(square)]) == 0) {
                //open
                features[INDEX_ROOK_OPEN_FILE]--;
            } else {
                //half open
                features[INDEX_ROOK_HALF_OPEN_FILE]--;
            }
        }
        
        
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks,
                        3);
        
        k = lsbReset(k);
    }
    features[INDEX_ROOK_VALUE] = (bitCount(b->getPieces()[WHITE_ROOK]) - bitCount(b->getPieces()[BLACK_ROOK]));
    
    /**********************************************************************************
     *                                  Q U E E N S                                   *
     **********************************************************************************/

    
    k = b->getPieces()[WHITE_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied) | lookUpBishopAttack(square, occupied);
    
    
        features[INDEX_QUEEN_PSQT] += psqt_queen[pst_index_white(square)] * earlyPSTScalar;
        features[INDEX_QUEEN_PSQT] += psqt_queen_endgame[pst_index_white(square)] * latePSTScalar;
    
    
        features[INDEX_QUEEN_MOBILITY] += sqrts[bitCount(attacks & mobilitySquaresWhite)];
        features[INDEX_QUEEN_DISTANCE_ENEMY_KING] += manhattanDistance(square, blackKingSquare);
        
        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks,
                        4);
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied) | lookUpBishopAttack(square, occupied);
    
    
        features[INDEX_QUEEN_PSQT] -= psqt_queen[pst_index_black(square)] * earlyPSTScalar;
        features[INDEX_QUEEN_PSQT] -= psqt_queen_endgame[pst_index_black(square)] * latePSTScalar;
    
    
        features[INDEX_QUEEN_MOBILITY] -= sqrts[bitCount(attacks & mobilitySquaresBlack)];
        features[INDEX_QUEEN_DISTANCE_ENEMY_KING] -= manhattanDistance(square, whiteKingSquare);
        
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks,
                        4);
        
        k = lsbReset(k);
    }
    features[INDEX_QUEEN_VALUE] = bitCount(b->getPieces()[WHITE_QUEEN]) - bitCount(b->getPieces()[BLACK_QUEEN]);
    
    /**********************************************************************************
     *                                  K I N G S                                     *
     **********************************************************************************/
    k = b->getPieces()[WHITE_KING];

    
    while (k) {
        square = bitscanForward(k);
    
        features[INDEX_KING_PSQT] += psqt_king[63 - square] * earlyPSTScalar;
        features[INDEX_KING_PSQT] += psqt_king_endgame[63 - square] * latePSTScalar;
    
        features[INDEX_KING_PAWN_SHIELD] += bitCount(KING_ATTACKS[square] & whitePawns);
        features[INDEX_KING_CLOSE_OPPONENT] += bitCount(KING_ATTACKS[square] & blackTeam);
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_KING];
    while (k) {
        square = bitscanForward(k);
    
        features[INDEX_KING_PSQT] -= psqt_king[square] * earlyPSTScalar;
        features[INDEX_KING_PSQT] -= psqt_king_endgame[square] * latePSTScalar;
    
        features[INDEX_KING_PAWN_SHIELD] -= bitCount(KING_ATTACKS[square] & blackPawns);
        features[INDEX_KING_CLOSE_OPPONENT] -= bitCount(KING_ATTACKS[square] & whiteTeam);
        
        k = lsbReset(k);
    }
    
    computeHangingPieces(b);
    computePinnedPieces(b);
    
    features[INDEX_KING_SAFETY] =
            (kingSafetyTable[blackkingSafety_valueOfAttacks] - kingSafetyTable[whitekingSafety_valueOfAttacks]) / 100;
    features[INDEX_CASTLING_RIGHTS] =
            + b->getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
            + b->getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING);
    
    
    
    
    
    
    __m128 earlyRes{};
    __m128 lateRes{};
    
    for (int i = 0; i < unusedVariable; i += 4) {
        __m128 *feat = (__m128 *) (features + (i));
        
        __m128 *w1 = (__m128 *) (_pieceValuesEarly + (i));
        __m128 *w2 = (__m128 *) (_pieceValuesLate + (i));
        
        earlyRes = _mm_add_ps(earlyRes, _mm_mul_ps(*w1, *feat));
        lateRes  = _mm_add_ps(lateRes, _mm_mul_ps(*w2, *feat));
    }
    
    
    const __m128 tE   = _mm_add_ps(earlyRes, _mm_movehl_ps(earlyRes, earlyRes));
    const __m128 sumE = _mm_add_ss(tE, _mm_shuffle_ps(tE, tE, 1));
    const __m128 tL   = _mm_add_ps(lateRes, _mm_movehl_ps(lateRes, lateRes));
    const __m128 sumL = _mm_add_ss(tL, _mm_shuffle_ps(tL, tL, 1));
    
    res = sumE[0] * (1 - phase) + sumL[0] * (phase);
    res += (b->getActivePlayer() == WHITE ? 15 : -15);
    return res;
    // clang-format on
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

    for (int i = 0; i < unusedVariable; i++) {

        ss << std::setw(40) << std::left << names[i] << " | " << std::setw(20) << std::right << ev.getFeatures()[i]
           << " | " << std::setw(20) << ev.getEarlyGameParams()[i] << " | " << std::setw(20)
           << ev.getLateGameParams()[i] << " | " << std::setw(20)
           << ev.getEarlyGameParams()[i] * (1 - phase) + ev.getLateGameParams()[i] * phase << " | " << std::setw(20)
           << (ev.getEarlyGameParams()[i] * (1 - phase) + ev.getLateGameParams()[i] * phase) * ev.getFeatures()[i]
           << " | \n";
    }
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

float* Evaluator::getFeatures() { return features; }

float Evaluator::getPhase() { return phase; }

float* Evaluator::getEarlyGameParams() { return _pieceValuesEarly; }

float* Evaluator::getLateGameParams() { return _pieceValuesLate; }

int Evaluator::paramCount() { return unusedVariable; }

float* Evaluator::getPSQT(Piece piece, bool early) {
    switch (piece) {
        case PAWN: return early ? psqt_pawn : psqt_pawn_endgame;
        case KNIGHT: return early ? psqt_knight : psqt_knight_endgame;
        case BISHOP: return early ? psqt_bishop : psqt_bishop_endgame;
        case ROOK: return early ? psqt_rook : psqt_rook_endgame;
        case QUEEN: return early ? psqt_queen : psqt_queen_endgame;
        case KING: return early ? psqt_king : psqt_king_endgame;
    }
    return nullptr;
}
float* Evaluator::getPhaseValues() { return phaseValues; }
#ifdef TUNE_PST
float* Evaluator::getTunablePST_MG() { return psqt_bishop; }

float* Evaluator::getTunablePST_EG() { return psqt_bishop_endgame; }

float* Evaluator::getTunablePST_MG_grad() { return tunablePST_MG_grad; }

float* Evaluator::getTunablePST_EG_grad() { return tunablePST_EG_grad; }
#endif
