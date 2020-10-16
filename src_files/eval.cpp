//
// Created by finne on 5/31/2020.
//

#include "eval.h"

#include <immintrin.h>
#include <iomanip>




float sqrts[28] = {
    0,       1,       1.41421, 1.73205, 2,       2.23607, 2.44949, 2.64575, 2.82843, 3,
    3.16228, 3.31662, 3.4641,  3.60555, 3.74166, 3.87298, 4,       4.12311, 4.24264, 4.3589,
    4.47214, 4.58258, 4.69042, 4.79583, 4.89898, 5,       5.09902, 5.19615,
};

float passer_rank[16] = {             0,    -18.000015,           -14,           2.3,     18.599987,     23.999985,     16.299986,             0,             0,    0.99999994,    -41.700001,    -16.700014,           -14,    -29.000015,    -31.700001,             0,
};


EvalScore psqt_pawn_n[64] = {
    M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0),
    M(  -11,   -9), M(   35,  -13), M(   30,   -3), M(   -4,   16), M(   -6,   11), M(  -20,   15), M(  -13,    4), M(  -22,   14),
    M(   -8,   -7), M(   22,  -13), M(   13,   -1), M(    2,    3), M(   -4,    1), M(   -6,   -8), M(  -32,    5), M(  -25,    3),
    M(  -26,   10), M(   -2,    7), M(   16,   -3), M(   26,   -7), M(   19,  -12), M(   -1,   -6), M(  -25,    9), M(  -30,   11),
    M(  -19,   31), M(    6,   28), M(   18,   16), M(   27,   -4), M(   22,   -7), M(    1,    3), M(   -8,   16), M(  -17,   25),
    M(   -6,  100), M(   40,   99), M(   78,   67), M(   51,   54), M(   26,   48), M(   28,   59), M(   -2,   83), M(    4,   88),
    M(    0,  247), M(   23,  218), M(   76,  199), M(   70,  192), M(   73,  163), M(   76,  164), M(   77,  216), M(   46,  221),
    M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0),
};

EvalScore psqt_knight_n[64] = {
    M( -136,  -72), M(  -56,  -53), M(  -57,  -13), M(  -52,  -26), M(   18,  -33), M( -111,  -23), M(  -55,  -66), M(  -84, -116),
    M( -111,  -15), M(  -79,    3), M(   66,  -33), M(   -9,   15), M(  -11,    2), M(   22,  -17), M(  -21,  -21), M(  -62,  -50),
    M(  -79,  -19), M(   24,  -22), M(   -6,   13), M(   18,   12), M(   60,    0), M(   94,  -15), M(   31,  -24), M(   20,  -55),
    M(  -11,  -14), M(   -4,   -4), M(  -19,   22), M(   32,   20), M(    1,   27), M(   53,    8), M(   -9,    4), M(   15,  -21),
    M(  -20,  -15), M(  -13,  -12), M(   -5,   11), M(   -6,   22), M(   19,   12), M(    9,    8), M(   21,   -3), M(   -5,  -19),
    M(  -35,  -21), M(  -22,   -4), M(    3,  -16), M(    1,   11), M(   19,   -3), M(   14,  -13), M(   18,  -32), M(  -21,  -25),
    M(  -26,  -38), M(  -52,  -17), M(  -19,  -11), M(   16,  -15), M(   15,  -11), M(   14,  -30), M(  -10,  -28), M(  -10,  -51),
    M(  -79,  -22), M(    0,  -49), M(  -48,  -19), M(  -22,   -9), M(   12,  -26), M(  -26,  -17), M(    1,  -51), M(  -25,  -62)
};

EvalScore psqt_bishop_n[64] = {
    M(  -74,    2), M(  -64,    1), M( -117,    5), M(  -86,   10), M(  -61,   14), M(  -91,    7), M(  -44,    6), M(  -22,   -9),
    M(  -78,   21), M(  -24,    2), M(  -67,   21), M(  -81,    3), M(  -23,    9), M(   25,    3), M(  -25,   11), M( -109,   14),
    M(  -65,   18), M(  -17,    3), M(   -2,   -1), M(  -25,    2), M(  -16,    2), M(   -2,    6), M(   -9,    6), M(  -47,   22),
    M(  -38,    6), M(  -27,    5), M(  -20,   10), M(   20,    0), M(   -5,    5), M(   -2,    4), M(  -22,   -3), M(  -28,    9),
    M(  -33,    0), M(  -18,    0), M(  -10,    3), M(   -7,   12), M(   10,   -5), M(  -21,    5), M(  -12,   -7), M(  -18,    7),
    M(  -23,    3), M(   -2,    0), M(   -4,    6), M(   -8,   10), M(  -10,   13), M(   20,   -8), M(   -7,   -1), M(  -10,   -5),
    M(   -5,   -7), M(   -3,  -10), M(   -3,   -4), M(  -19,    8), M(    0,    0), M(   -8,   -2), M(   13,  -15), M(  -17,  -14),
    M(  -60,    3), M(    0,    5), M(  -24,    5), M(  -31,    9), M(  -22,    9), M(  -27,    9), M(  -68,   18), M(  -48,    3),
};

EvalScore psqt_rook_n[64] = {
    M(  -17,   26), M(    1,   17), M(  -44,   33), M(    6,   20), M(    8,   25), M(  -15,   27), M(   -9,   24), M(  -14,   21),
    M(  -10,   29), M(   -3,   29), M(   31,   21), M(   27,   22), M(   47,    8), M(   58,   14), M(  -12,   33), M(    2,   20),
    M(  -22,   24), M(   -5,   25), M(   -2,   21), M(    6,   24), M(  -16,   25), M(   31,   10), M(   56,    6), M(    9,    5),
    M(  -34,   23), M(  -26,   22), M(   -9,   30), M(    0,   17), M(   -1,   21), M(   24,   18), M(  -16,   18), M(  -19,   21),
    M(  -39,   23), M(  -27,   22), M(  -18,   24), M(  -16,   19), M(    0,    9), M(   -1,    7), M(    8,    5), M(  -14,    5),
    M(  -42,   13), M(  -19,   13), M(  -10,    2), M(  -18,   10), M(   -2,    1), M(   11,   -6), M(    4,    0), M(  -19,   -5),
    M(  -35,    9), M(  -12,    4), M(  -23,   12), M(    2,    7), M(   11,   -5), M(   20,  -12), M(   -4,   -8), M(  -51,    8),
    M(  -13,    4), M(   -2,    5), M(    7,    4), M(   18,   -3), M(   21,   -9), M(   21,  -11), M(  -12,   -3), M(    3,  -22),
};

EvalScore psqt_queen_n[64] = {
    M(  -23,  -13), M(  -23,   24), M(  -11,   12), M(  -29,    4), M(   29,  -12), M(   17,   -4), M(    5,   -1), M(   30,   10),
    M(  -34,    5), M(  -38,   -1), M(  -24,   18), M(  -13,   18), M(  -22,   15), M(   22,  -10), M(   -4,   14), M(   30,    5),
    M(  -15,   10), M(  -18,    4), M(    9,  -29), M(  -34,   32), M(    2,   22), M(   27,   -6), M(   15,    2), M(   17,    0),
    M(  -39,   40), M(  -38,   26), M(  -33,    6), M(  -34,   17), M(   -4,   10), M(   -4,    7), M(   -4,   35), M(   -9,   34),
    M(  -10,    0), M(  -41,   37), M(   -8,    4), M(   -9,   18), M(   -2,    3), M(    3,    4), M(    3,   31), M(    3,   20),
    M(  -29,   26), M(    9,  -32), M(   -6,    3), M(   -1,   -8), M(   -2,   -3), M(    8,    6), M(   18,   18), M(    9,   21),
    M(  -39,   22), M(   -8,  -11), M(    9,  -13), M(   12,  -16), M(   24,  -16), M(   21,  -27), M(   -4,  -36), M(    7,  -10),
    M(  -13,    9), M(   -8,   -7), M(   -2,    1), M(    5,    0), M(   -1,   15), M(  -20,    0), M(  -19,   -8), M(  -32,  -36),
};

EvalScore psqt_king_n[64] = {
    M( -152,  -60), M(  -11,  -38), M(   23,  -25), M(  -70,  -18), M(  -99,   -4), M(  -58,   15), M(   -3,  -10), M(   11,  -40),
    M(   20,  -18), M(   -5,   16), M(   10,   12), M(  -19,   24), M(  -31,   17), M(   33,   27), M(    4,    4), M(  -58,    4),
    M(   40,   -7), M(   33,    9), M(   19,   18), M(  -62,   22), M(  -44,   20), M(   14,   38), M(   36,   24), M(  -19,   -1),
    M(  -22,  -13), M(  -21,   15), M(  -50,   29), M(  -63,   35), M(  -85,   31), M(  -50,   30), M(   -7,    7), M(  -95,    3),
    M(  -76,  -10), M(  -42,   -1), M(  -73,   32), M( -138,   47), M( -125,   48), M(  -85,   33), M(  -34,    3), M(  -70,  -11),
    M(  -10,  -19), M(   10,   -4), M(  -39,   23), M(  -72,   35), M(  -70,   40), M(  -34,   29), M(   12,    6), M(  -25,   -8),
    M(   44,  -48), M(   49,  -18), M(    8,   10), M(  -34,   21), M(  -25,   24), M(  -12,   14), M(   44,  -11), M(   45,  -28),
    M(   34,  -70), M(   57,  -45), M(   35,  -22), M(  -24,  -10), M(   15,  -22), M(   -7,   -6), M(   68,  -37), M(   53,  -67),
};

EvalScore* psqt[6]{
    psqt_pawn_n, psqt_knight_n, psqt_bishop_n, psqt_rook_n, psqt_queen_n, psqt_king_n
};

EvalScore pieceScores[6] = {
    M( 90, 105),M( 463, 328), M( 474, 287),M( 574, 590),M(1350,1121),M(0,0)
};

EvalScore   PAWN_STRUCTURE           = M(7,13);
EvalScore  PAWN_PASSED               = M(3,43);
EvalScore  PAWN_ISOLATED             = M(-8,-7);
EvalScore  PAWN_DOUBLED              = M(9,-3);
EvalScore PAWN_DOUBLED_AND_ISOLATED  = M(-4, -16);
EvalScore  PAWN_BACKWARD             = M(-12, 1);
EvalScore  PAWN_OPEN                 = M(16,-8);
EvalScore  PAWN_BLOCKED              = M(-7,-10);

EvalScore  KNIGHT_OUTPOST            = M(22,17);
EvalScore  KNIGHT_DISTANCE_ENEMY_KING= M(-6,1);

EvalScore ROOK_OPEN_FILE              = M(61, -17);
EvalScore ROOK_HALF_OPEN_FILE         = M(16, 2);
EvalScore ROOK_KING_LINE              = M(12, 8);

EvalScore BISHOP_DOUBLED          = M(44,56);
EvalScore BISHOP_PAWN_SAME_SQUARE = M(-5,6);
EvalScore BISHOP_FIANCHETTO       = M(13,-4);

EvalScore QUEEN_DISTANCE_ENEMY_KING = M(7,-24);

EvalScore KING_CLOSE_OPPONENT = M(-37,60);
EvalScore KING_PAWN_SHIELD    = M(12,0);

EvalScore CASTLING_RIGHTS     = M(13,-4);

EvalScore fast_pawn_psqt[2][2][64];
EvalScore fast_psqt[12][64];



EvalScore hangingEval[5]{
    M(-7,4),
    M(-20,-9),
    M(-11,-12),
    M(-10,-10),
    M(-10,-16)
};

EvalScore pinnedEval[15]{
    M(20,-43),
    M(-9,22),
    M(0,27),
    M(-39, -140),
    M(-35, -46),
    M(-35, 15),
    M(-18,-15),
    M(-63, -61),
    M(10,-86),
    M(-59, -383),
    M(-20, 5),
    M(-13, -71),
    M(-181, -524),
    M(-339, -640),
    M(-13, -10)
};




float* _pieceValuesEarly = new float[6]{
        
        27.236883,  24.555048,  22.751461,  8.7743282, 371.82693,5.2139874
};


float* _pieceValuesLate = new float[6]{
        
        20.023214,  36.51939,32.432838,  60.920574, -144.92081,17.488422
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


void eval_init() {
    for (int i = 0; i < 64; i++) {
        for (int kside = 0; kside < 2; kside++) {
            fast_pawn_psqt[WHITE][kside][i] =
                +psqt_pawn_n[squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))] + pieceScores[PAWN];
            fast_pawn_psqt[BLACK][kside][i] =
                -psqt_pawn_n[squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))]- pieceScores[PAWN];
        }
        
        for(Piece p = KNIGHT; p <= KING; p++){
            fast_psqt[p  ][i] = +psqt[p][pst_index_white_s(i)] + pieceScores[p];
            fast_psqt[p+6][i] = -psqt[p][pst_index_black_s(i)] - pieceScores[p];
        }
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

bool hasMatingMaterial(Board *b, bool side)
{
    if ((b->getPieces()[QUEEN+side*6]|b->getPieces()[ROOK+side*6]|b->getPieces()[PAWN+side*6])||(bitCount(b->getPieces()[BISHOP+side*6]|b->getPieces()[KNIGHT+side*6])>1&&b->getPieces()[BISHOP+side*6])) return true;
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

EvalScore Evaluator::computeHangingPieces(Board* b) {
    U64 WnotAttacked = ~b->getAttackedSquares(WHITE);
    U64 BnotAttacked = ~b->getAttackedSquares(BLACK);
    
    EvalScore res = M(0,0);
    
    for (int i = PAWN; i <= QUEEN; i++) {
        res += hangingEval[i] *(
                + bitCount(b->getPieces(WHITE, i) & WnotAttacked)
                - bitCount(b->getPieces(BLACK, i) & BnotAttacked));
        
    }
    return res;
}

EvalScore Evaluator::computePinnedPieces(Board* b) {

    
    EvalScore res = M(0,0);
    
    Square square;
    Square wkingSq = bitscanForward(b->getPieces(WHITE, KING));
    U64    pinner  = lookUpRookXRayAttack(wkingSq, *b->getOccupied(), b->getTeamOccupied()[WHITE])
                 & (b->getPieces(BLACK, ROOK) | b->getPieces(BLACK, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[wkingSq][square] & b->getTeamOccupied()[WHITE]);
        res += pinnedEval[3 * (b->getPiece(pinnedPlace) % 6)+ (b->getPiece(square) % 6 - BISHOP)];
        
        
        pinner = lsbReset(pinner);
    }

    pinner = lookUpBishopXRayAttack(wkingSq, *b->getOccupied(), b->getTeamOccupied()[WHITE])
             & (b->getPieces(BLACK, BISHOP) | b->getPieces(BLACK, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[wkingSq][square] & b->getTeamOccupied()[WHITE]);
    
        res += pinnedEval[3 * (b->getPiece(pinnedPlace) % 6)+ (b->getPiece(square) % 6 - BISHOP)];
        pinner = lsbReset(pinner);
    }

    Square bkingSq = bitscanForward(b->getPieces(BLACK, KING));
    pinner         = lookUpRookXRayAttack(bkingSq, *b->getOccupied(), b->getTeamOccupied()[BLACK])
             & (b->getPieces(WHITE, ROOK) | b->getPieces(WHITE, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[bkingSq][square] & b->getTeamOccupied()[BLACK]);
        res -= pinnedEval[3 * (b->getPiece(pinnedPlace) % 6)+ (b->getPiece(square) % 6 - BISHOP)];
        pinner = lsbReset(pinner);
    }
    pinner = lookUpBishopXRayAttack(bkingSq, *b->getOccupied(), b->getTeamOccupied()[BLACK])
             & (b->getPieces(WHITE, BISHOP) | b->getPieces(WHITE, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[bkingSq][square] & b->getTeamOccupied()[BLACK]);
    
        res-= pinnedEval[3 * (b->getPiece(pinnedPlace) % 6)+ (b->getPiece(square) % 6 - BISHOP)];
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



    memset(features, 0, 50 * sizeof(float));

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
    
    U64 whiteBlockedPawns = shiftNorth(whitePawns)&(whiteTeam|blackTeam);
    U64 blackBlockedPawns = shiftSouth(blackPawns)&(whiteTeam|blackTeam);
    
    U64 openFilesWhite = ~fillFile(whitePawns);
    U64 openFilesBlack = ~fillFile(blackPawns);
    U64 openFiles      = openFilesBlack & openFilesWhite;
    
    k = whitePawns;
    
    EvalScore evalScore = M(0,0);
    EvalScore featureScore = M(0,0);

    while (k) {
        square = bitscanForward(k);

        evalScore += fast_pawn_psqt[WHITE][wKSide][square];
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_PAWN];
    while (k) {
        square = bitscanForward(k);
        evalScore += fast_pawn_psqt[BLACK][bKSide][square];

        k = lsbReset(k);
    }
    k = whitePassers;
    while(k){
        square = bitscanForward(k);
        features[INDEX_PASSER_RANK] += passer_rank[getBit(whiteBlockedPawns,square)*8+rankIndex(square)]/10;
        k = lsbReset(k);
    }
    k=blackPassers;
    while (k){
        square = bitscanForward(k);
        features[INDEX_PASSER_RANK] -= passer_rank[getBit(blackBlockedPawns,square)*8+7-rankIndex(square)]/10;
        k = lsbReset(k);
    }
    
    U64 whitePawnEastCover = shiftNorthEast(whitePawns) & whitePawns;
    U64 whitePawnWestCover = shiftNorthWest(whitePawns) & whitePawns;
    U64 blackPawnEastCover = shiftSouthEast(blackPawns) & blackPawns;
    U64 blackPawnWestCover = shiftSouthWest(blackPawns) & blackPawns;
    
    U64 whitePawnCover = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
    U64 blackPawnCover = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);
    
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
    
    /**********************************************************************************
     *                                  K N I G H T S                                 *
     **********************************************************************************/

    
    
    k = b->getPieces()[WHITE_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[square];
    
        evalScore += fast_psqt[WHITE_KNIGHT][square];
    
        features[INDEX_KNIGHT_MOBILITY] += sqrts[bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresWhite)];
        featureScore += KNIGHT_OUTPOST * isOutpost(square, WHITE, blackPawns, whitePawnCover);
        featureScore += KNIGHT_DISTANCE_ENEMY_KING * manhattanDistance(square, blackKingSquare);
        
        
        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks,
                        2);
        
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[square];
    
        evalScore += fast_psqt[BLACK_KNIGHT][square];
    
        features[INDEX_KNIGHT_MOBILITY] -= sqrts[bitCount(attacks & mobilitySquaresBlack)];
        featureScore -= KNIGHT_OUTPOST * isOutpost(square, BLACK, whitePawns, blackPawnCover);
        featureScore -= KNIGHT_DISTANCE_ENEMY_KING * manhattanDistance(square, whiteKingSquare);
        
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks,
                        2);
        
        k = lsbReset(k);
    }
    /**********************************************************************************
     *                                  B I S H O P S                                 *
     **********************************************************************************/

    k = b->getPieces()[WHITE_BISHOP];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpBishopAttack(square, occupied);
    
    
        evalScore += fast_psqt[WHITE_BISHOP][square];
    
        features[INDEX_BISHOP_MOBILITY] += sqrts[bitCount(attacks & mobilitySquaresWhite)];
    
        featureScore += BISHOP_PAWN_SAME_SQUARE *
                bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
        featureScore += BISHOP_FIANCHETTO *
                (square == G2 &&
                 whitePawns & ONE << F2 &&
                 whitePawns & ONE << H2 &&
                 whitePawns & (ONE << G3 | ONE << G4));
        featureScore += BISHOP_FIANCHETTO *
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
        
        evalScore += fast_psqt[BLACK_BISHOP][square];
        
        features[INDEX_BISHOP_MOBILITY] -= sqrts[bitCount(attacks & mobilitySquaresBlack)];
        featureScore -= BISHOP_PAWN_SAME_SQUARE *
                        bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
        
        featureScore -= BISHOP_FIANCHETTO *
                (square == G7 &&
                 blackPawns & ONE << F7 &&
                 blackPawns & ONE << H7 &&
                 blackPawns & (ONE << G6 | ONE << G5));
        featureScore -= BISHOP_FIANCHETTO *
                (square == B2 &&
                 blackPawns & ONE << A7 &&
                 blackPawns & ONE << C7 &&
                 blackPawns & (ONE << B6 | ONE << B5));
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks,
                        2);
        
        k = lsbReset(k);
    }
    featureScore += BISHOP_DOUBLED * (
            + (bitCount(b->getPieces()[WHITE_BISHOP]) == 2)
            - (bitCount(b->getPieces()[BLACK_BISHOP]) == 2));
    /**********************************************************************************
     *                                  R O O K S                                     *
     **********************************************************************************/

    k = b->getPieces()[WHITE_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied);
    
        evalScore += fast_psqt[WHITE_ROOK][square];
    
        features[INDEX_ROOK_MOBILITY] += sqrts[bitCount(attacks & mobilitySquaresWhite)];
        
        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks,3);
        
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied);
    
        evalScore += fast_psqt[BLACK_ROOK][square];
    
        features[INDEX_ROOK_MOBILITY] -= sqrts[bitCount(attacks & mobilitySquaresBlack)];
        
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks,
                        3);
        
        k = lsbReset(k);
    }
    
    featureScore += ROOK_KING_LINE * (
            + bitCount(lookUpRookAttack(blackKingSquare, occupied) & b->getPieces(WHITE, ROOK))
            - bitCount(lookUpRookAttack(whiteKingSquare, occupied) & b->getPieces(BLACK, ROOK)));
    featureScore += ROOK_OPEN_FILE * (
            + bitCount(openFiles & b->getPieces(WHITE, ROOK))
            - bitCount(openFiles & b->getPieces(BLACK, ROOK)));
    featureScore += ROOK_HALF_OPEN_FILE * (
            + bitCount(openFilesBlack & ~openFiles & b->getPieces(WHITE, ROOK))
            - bitCount(openFilesWhite & ~openFiles & b->getPieces(BLACK, ROOK)));

    /**********************************************************************************
     *                                  Q U E E N S                                   *
     **********************************************************************************/

    
    k = b->getPieces()[WHITE_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied) | lookUpBishopAttack(square, occupied);
    
        evalScore += fast_psqt[WHITE_QUEEN][square];
    
        features[INDEX_QUEEN_MOBILITY] += sqrts[bitCount(attacks & mobilitySquaresWhite)];
        featureScore += QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, blackKingSquare);

        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks,
                        4);
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied) | lookUpBishopAttack(square, occupied);
    
        evalScore += fast_psqt[BLACK_QUEEN][square];
    
    
        features[INDEX_QUEEN_MOBILITY] -= sqrts[bitCount(attacks & mobilitySquaresBlack)];
        featureScore -= QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, whiteKingSquare);
        
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks,
                        4);
        
        k = lsbReset(k);
    }
    
    /**********************************************************************************
     *                                  K I N G S                                     *
     **********************************************************************************/
    k = b->getPieces()[WHITE_KING];

    
    while (k) {
        square = bitscanForward(k);
    
        evalScore += fast_psqt[WHITE_KING][square];
        
        featureScore += KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & whitePawns);
        featureScore += KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & blackTeam);
        
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_KING];
    while (k) {
        square = bitscanForward(k);
    
        evalScore += fast_psqt[BLACK_KING][square];
        
        featureScore -= KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & blackPawns);
        featureScore -= KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & whiteTeam);
        
        k = lsbReset(k);
    }
    
    EvalScore hangingEval = computeHangingPieces(b);
    EvalScore pinnedEval = computePinnedPieces(b);
    
    features[INDEX_KING_SAFETY] =
            (kingSafetyTable[blackkingSafety_valueOfAttacks] - kingSafetyTable[whitekingSafety_valueOfAttacks]) / 100;
    featureScore += CASTLING_RIGHTS*(
            + b->getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
            + b->getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING));

    
    float earlySum = 0;
    float endSum = 0;
    for(int i = 0; i < 6; i++){
        earlySum += features[i] * _pieceValuesEarly[i];
        endSum += features[i] * _pieceValuesLate[i];
    }

    EvalScore totalScore = evalScore + pinnedEval + hangingEval + featureScore;
    
    res = earlySum * (1 - phase) + endSum * (phase);
    res += (b->getActivePlayer() == WHITE ? 15 : -15);
    res += MgScore(totalScore) * (1 - phase) + EgScore(totalScore) * (phase);
    
    if (!hasMatingMaterial(b, res>0?WHITE:BLACK))res=res/10;
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
//        case PAWN: return early ? psqt_pawn : psqt_pawn_endgame;
//        case KNIGHT: return early ? psqt_knight : psqt_knight_endgame;
//        case BISHOP: return early ? psqt_bishop : psqt_bishop_endgame;
//        case ROOK: return early ? psqt_rook : psqt_rook_endgame;
//        case QUEEN: return early ? psqt_queen : psqt_queen_endgame;
//        case KING: return early ? psqt_king : psqt_king_endgame;
    }
    return nullptr;
}
float* Evaluator::getPhaseValues() { return passer_rank; }
#ifdef TUNE_PST
float* Evaluator::getTunablePST_MG() { return psqt_bishop; }

float* Evaluator::getTunablePST_EG() { return psqt_bishop_endgame; }

float* Evaluator::getTunablePST_MG_grad() { return tunablePST_MG_grad; }

float* Evaluator::getTunablePST_EG_grad() { return tunablePST_EG_grad; }
#endif
