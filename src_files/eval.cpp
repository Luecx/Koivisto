//
// Created by finne on 5/31/2020.
//

#include <iomanip>
#include <immintrin.h>
#include "eval.h"


#define pst_index_white(s) squareIndex(7-rankIndex(s), fileIndex(s))
#define pst_index_black(s) s


float *psqt_pawn_endgame = new float[64]{
        0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000,
        8.468, -5.743, 9.089, 4.721, 9.111, 0.484, -14.387, -7.112,
        -3.367, 0.189, -6.827, 0.655, -1.621, -2.004, -5.150, -7.745,
        8.127, 6.428, -3.006, -17.729, -12.550, -3.491, 0.343, 3.798,
        23.973, 20.263, 14.301, 2.387, 2.797, 8.021, 17.699, 15.459,
        84.682, 90.018, 80.790, 63.723, 61.787, 61.816, 77.160, 77.968,
        149.876, 129.031, 118.179, 102.717, 100.661, 104.561, 117.594, 131.497,
        0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000,
};

float *psqt_pawn = new float[64]{
        0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000,
        -4.819, 51.504, 32.302, -19.137, -26.091, -52.893, 6.069, -12.853,
        5.231, 18.449, -8.151, 1.120, -10.764, 7.481, -19.175, -3.865,
        -18.220, -18.375, -6.706, 28.162, 47.944, 4.643, -12.987, -13.113,
        -2.504, -2.743, 2.084, 20.448, 27.055, 2.174, 4.219, 0.870,
        1.288, 1.900, 3.693, 4.249, 3.415, 3.028, 0.642, 2.725,
        0.605, 1.247, 1.414, 1.158, 1.453, 1.024, 2.779, 1.624,
        0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000,
};


float *psqt_knight         = new float[64]{
        -233.5, -74.1, -75.4, -64.3, -12.3, -186.9, -71.1, -127.4,
        -196.4, -175.3, 114.8, -24.4, -31.9, 24.2, -39.7, -129.8,
        -141.5, 51.6, -20.3, 54.9, 93.6, 152.2, 59.4, 2.2,
        -35.5, -6.5, -41.3, 68.1, 14.3, 106.3, -14.5, 15.0,
        -35.7, -9.8, -6.4, 5.1, 41.1, 19.3, 37.0, -12.1,
        -81.2, -42.9, 14.8, 17.1, 46.2, 25.6, 30.2, -52.1,
        -38.2, -77.3, -29.4, 41.5, 38.4, 39.6, -8.8, -17.0,
        -115.5, -2.2, -87.3, -28.3, 28.4, -39.0, -0.3, -45.5,
};
float *psqt_knight_endgame = new float[64]{
        -63.1, -58.7, -6.9, -37.2, -29.3, -25.0, -85.6, -131.6,
        -3.2, 22.3, -53.1, 8.3, -1.9, -32.4, -19.2, -42.7,
        -15.5, -29.1, 16.5, -0.4, -18.6, -27.9, -33.5, -53.8,
        -10.1, -2.9, 30.2, 10.1, 22.3, -5.1, 4.6, -26.1,
        -14.9, -15.0, 13.8, 29.2, 10.2, 9.9, -5.5, -23.6,
        -8.8, 2.9, -14.6, 10.9, -2.9, -21.4, -35.6, -13.1,
        -38.9, -12.0, -6.8, -17.9, -10.0, -26.9, -23.7, -60.4,
        -17.3, -42.4, -11.6, -1.1, -30.7, -8.7, -48.0, -70.2,
};

float *psqt_bishop = new float[64]{
        -231.2, -115.6, -241.3, -147.8, -94.1, -174.1, -35.8, -69.9,
        -335.3, -133.3, -212.2, -210.7, -23.3, 101.8, -48.6, -401.0,
        -217.7, -96.8, 35.4, -43.2, -42.9, 69.9, 48.2, -149.8,
        -161.3, -69.1, -111.6, 83.0, 5.7, 25.6, -80.3, -88.0,
        -86.9, -88.8, -20.1, -17.1, 60.6, -78.9, -60.9, -61.4,
        -102.6, 26.0, -21.6, -8.7, -51.8, 54.8, -30.7, -81.2,
        22.0, -12.8, 32.7, -59.8, 11.2, -7.6, 42.8, -40.9,
        -179.5, 14.3, -86.3, -113.7, -109.5, -57.6, -148.6, -198.4,
};

float *psqt_bishop_endgame = new float[64]{
        -63.9, -27.6, -45.6, -30.4, -23.2, -28.5, -8.1, -49.6,
        -4.1, -19.0, 5.4, -45.4, 1.1, -0.5, -19.4, -19.9,
        8.0, 9.7, -26.7, -19.0, -14.2, 10.5, 0.1, 16.1,
        -1.4, 27.8, 50.2, 1.8, 17.0, 10.8, -2.7, -8.3,
        -12.2, 8.2, 39.7, 84.6, -32.4, 30.4, -17.6, 10.6,
        7.5, -2.8, 89.3, 52.8, 133.8, 15.7, 30.3, 20.8,
        -18.5, -14.7, -18.3, 31.4, 47.3, 12.9, 10.8, -27.0,
        6.4, 30.2, 28.6, 43.8, 46.9, 8.3, 44.6, 12.4,
};


float *psqt_rook = new float[64]{
        -2.2, 4.3, -12.4, 8.8, 9.4, -1.7, -3.2, -2.9,
        -16.6, -6.2, 17.0, 16.4, 24.6, 22.0, -8.5, -2.9,
        -9.2, 5.9, -1.9, 4.8, -4.0, 9.7, 18.9, 5.4,
        -30.8, -10.0, -0.4, 5.8, 5.4, 8.5, -11.7, -0.0,
        -28.6, -16.2, -9.5, -9.5, 4.6, -12.9, 0.9, 2.8,
        -42.0, -11.1, -5.4, -16.8, -3.9, -3.4, -8.6, -17.2,
        -38.6, -6.7, -18.2, -3.9, 8.7, 4.9, -18.7, -55.8,
        -10.6, -7.4, 7.6, 11.1, 13.1, 7.3, -28.2, 10.8,
};

float *psqt_rook_endgame = new float[64]{
        23.7, 16.4, 20.9, 17.7, 22.5, 22.1, 22.4, 15.2,
        26.2, 26.0, 21.5, 22.4, 11.2, 21.9, 26.3, 17.7,
        13.4, 14.8, 13.0, 15.8, 8.6, 10.4, 5.4, -1.8,
        13.6, 8.1, 23.4, 4.7, 12.5, 12.7, 4.4, 3.8,
        10.9, 15.7, 16.1, 10.6, 2.1, 4.9, 4.7, -10.3,
        9.1, 9.2, 1.4, 6.6, -0.8, -2.2, 2.9, -9.4,
        6.9, 6.0, 9.3, 7.5, -3.4, -1.6, -1.8, 4.4,
        3.1, 10.8, 7.3, 2.8, -0.2, -0.6, 3.3, -22.6,
};

float *psqt_queen = new float[64]{
        -32.5, -14.1, -9.1, -9.5, 6.7, -4.2, -4.5, 3.8,
        -46.4, -71.2, -22.4, -2.0, -22.4, 11.7, -3.5, 16.6,
        -20.8, -18.2, -5.8, -20.3, 9.4, 16.4, 10.8, 14.0,
        -28.4, -26.9, -24.2, -36.4, -6.1, -0.1, -1.7, -5.8,
        5.2, -30.6, 3.7, -11.7, 5.1, 3.7, 8.7, -0.3,
        -13.7, 16.1, 6.5, 11.5, 5.1, 12.7, 19.3, 10.4,
        -17.7, 3.4, 33.0, 31.3, 44.9, 26.1, -5.5, 1.1,
        3.8, 8.8, 16.6, 34.3, 6.2, -15.0, -16.6, -31.2,
};

float *psqt_queen_endgame = new float[64]{
        -32.2, -4.9, -6.0, -10.4, 0.5, -4.9, -5.7, -4.9,
        -26.7, -2.1, -1.6, 7.5, 3.9, 6.2, 2.8, -3.5,
        -20.4, -14.2, -12.7, 15.1, 14.5, 6.7, 4.8, -1.2,
        -0.8, 3.4, 0.5, 13.4, 18.0, 9.9, 19.1, 12.0,
        -11.6, 8.4, 0.4, 30.0, 10.2, 9.9, 20.5, 5.4,
        -8.0, -25.7, 0.1, -1.7, 4.8, 9.7, 9.1, -4.2,
        -12.4, -16.4, -33.0, -23.1, -22.5, -12.5, -14.3, -14.6,
        -19.5, -23.8, -22.8, -31.2, -3.4, -17.6, -15.4, -28.6,
    
};


float psqt_king[]         = {
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        20, 20, 0, 0, 0, 0, 20, 20,
        20, 30, 10, 0, 0, 10, 30, 20
};
float psqt_king_endgame[] = {
        -50, -40, -30, -20, -20, -30, -40, -50,
        -30, -20, -10, 0, 0, -10, -20, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -30, 0, 0, 0, 0, -30, -30,
        -50, -30, -30, -30, -30, -30, -30, -50
};


int unusedVariable = 0;

int INDEX_PAWN_VALUE                = unusedVariable++;
int INDEX_PAWN_PSQT                 = unusedVariable++;
int INDEX_PAWN_STRUCTURE            = unusedVariable++;
int INDEX_PAWN_PASSED               = unusedVariable++;
int INDEX_PAWN_ISOLATED             = unusedVariable++;
int INDEX_PAWN_DOUBLED              = unusedVariable++;
int INDEX_PAWN_DOUBLED_AND_ISOLATED = unusedVariable++;
int INDEX_PAWN_BACKWARD             = unusedVariable++;
int INDEX_PAWN_OPEN                 = unusedVariable++;

int INDEX_KNIGHT_VALUE    = unusedVariable++;
int INDEX_KNIGHT_PSQT     = unusedVariable++;
int INDEX_KNIGHT_MOBILITY = unusedVariable++;
int INDEX_KNIGHT_OUTPOST  = unusedVariable++;

int INDEX_BISHOP_VALUE            = unusedVariable++;
int INDEX_BISHOP_PSQT             = unusedVariable++;
int INDEX_BISHOP_MOBILITY         = unusedVariable++;
int INDEX_BISHOP_DOUBLED          = unusedVariable++;
int INDEX_BISHOP_PAWN_SAME_SQUARE = unusedVariable++;
int INDEX_BISHOP_FIANCHETTO       = unusedVariable++;

int INDEX_ROOK_VALUE          = unusedVariable++;
int INDEX_ROOK_PSQT           = unusedVariable++;
int INDEX_ROOK_MOBILITY       = unusedVariable++;
int INDEX_ROOK_OPEN_FILE      = unusedVariable++;
int INDEX_ROOK_HALF_OPEN_FILE = unusedVariable++;
int INDEX_ROOK_KING_LINE      = unusedVariable++;

int INDEX_QUEEN_VALUE    = unusedVariable++;
int INDEX_QUEEN_PSQT     = unusedVariable++;
int INDEX_QUEEN_MOBILITY = unusedVariable++;

int INDEX_KING_SAFETY         = unusedVariable++;
int INDEX_KING_PSQT           = unusedVariable++;
int INDEX_KING_CLOSE_OPPONENT = unusedVariable++;
int INDEX_KING_PAWN_SHIELD    = unusedVariable++;

int INDEX_KNIGHT_DISTANCE_ENEMY_KING = unusedVariable++;
int INDEX_QUEEN_DISTANCE_ENEMY_KING  = unusedVariable++;

int INDEX_PINNED_PAWN_BY_BISHOP   = unusedVariable++;
int INDEX_PINNED_PAWN_BY_ROOK     = unusedVariable++;
int INDEX_PINNED_PAWN_BY_QUEEN    = unusedVariable++;
int INDEX_PINNED_KNIGHT_BY_BISHOP = unusedVariable++;
int INDEX_PINNED_KNIGHT_BY_ROOK   = unusedVariable++;
int INDEX_PINNED_KNIGHT_BY_QUEEN  = unusedVariable++;
int INDEX_PINNED_BISHOP_BY_BISHOP = unusedVariable++;
int INDEX_PINNED_BISHOP_BY_ROOK   = unusedVariable++;
int INDEX_PINNED_BISHOP_BY_QUEEN  = unusedVariable++;
int INDEX_PINNED_ROOK_BY_BISHOP   = unusedVariable++;
int INDEX_PINNED_ROOK_BY_ROOK     = unusedVariable++;
int INDEX_PINNED_ROOK_BY_QUEEN    = unusedVariable++;
int INDEX_PINNED_QUEEN_BY_BISHOP  = unusedVariable++;
int INDEX_PINNED_QUEEN_BY_ROOK    = unusedVariable++;
int INDEX_PINNED_QUEEN_BY_QUEEN   = unusedVariable++;

int INDEX_PAWN_HANGING   = unusedVariable++;
int INDEX_KNIGHT_HANGING = unusedVariable++;
int INDEX_BISHOP_HANGING = unusedVariable++;
int INDEX_ROOK_HANGING   = unusedVariable++;
int INDEX_QUEEN_HANGING  = unusedVariable++;

/*
 * it is very important that unusuedVariable is a multiple of 4!!!
 */
int SPACER1 = unusedVariable += unusedVariable % 4 == 0 ? 0 : (4 - unusedVariable % 4);


#ifdef TUNE_PST
float * tunablePST_MG_grad = new float[64]{};
float * tunablePST_EG_grad = new float[64]{};
#endif

float *_pieceValuesEarly = new float[unusedVariable]{
        92.356491, 38.61282, 5.1149216, -5.7460375, -8.4308376, 8.4697132, -7.2890692, -13.922237, 14.421892, 460.70462,
        46.232033, 29.186869, 25.658445, 477.89893, 33.474796, 24.781307, 41.798512, -6.3010058, 11.997969, 584.98969,
        94.901245, 20.897881, 60.646118, 15.080976, 17.863449, 1344.5829, 118.19398, 7.8332171, 377.67072, 240.60492,
        -55.949089, 9.1129723, -6.7414474, -3.9745944, 21.693964, -7.9193916, 5.2397718, -32.676979, -41.000996,
        -24.028038, -14.886013, -64.022903, 18.608335, -49.408333, -9.1496172, -17.614901, -191.12534, -347.66696,
        -25.90111, -3.2204432, -16.162563, -5.6897264, -5.4419866, -14.898696, 0, 0,
};

float *_pieceValuesLate = new float[unusedVariable]{
        102.05948, 178.15341, 10.821315, 45.563454, -7.9744067, -4.9831324, -20.481129, 2.6967096, -3.6025305,
        326.50967, 91.260658, 19.195377, 19.30518, 292.87314, 9.4227409, 31.857086, 54.170273, 6.2003446, 29.931547,
        595.79254, 100.59767, 30.526958, -14.472153, 2.76579, 2.3643596, 1134.3336, 42.109905, 58.535522, -123.70769,
        42.605293, 51.641876, 0.46838874, -0.44353971, -14.546595, -44.644421, 18.968077, 25.257627, -156.0616,
        -50.780262, -27.907042, -14.829045, -65.330856, -104.16544, -375.89084, -19.760118, -73.758202, -484.84842,
        -574.42114, -26.89514, 2.1228392, -2.6465628, -11.620555, -11.112517, -5.267952, 0, 0,
};


//TODO tweak values
float _kingSafetyTable[100]{
        0, 0, 1, 2, 3, 5, 7, 9, 12, 15,
        18, 22, 26, 30, 35, 39, 44, 50, 56, 62,
        68, 75, 82, 85, 89, 97, 105, 113, 122, 131,
        140, 150, 169, 180, 191, 202, 213, 225, 237, 248,
        260, 272, 283, 295, 307, 319, 330, 342, 354, 366,
        377, 389, 401, 412, 424, 436, 448, 459, 471, 483,
        494, 500, 500, 500, 500, 500, 500, 500, 500, 500,
        500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
        500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
        500, 500, 500, 500, 500, 500, 500, 500, 500, 500
};


float *_features = new float[unusedVariable];

float _phase;

/**
 * adds the factor to value of attacks if the piece attacks the kingzone
 * @param attacks
 * @param kingZone
 * @param pieceCount
 * @param valueOfAttacks
 * @param factor
 */
void addToKingSafety(U64 attacks, U64 kingZone, int &pieceCount, int &valueOfAttacks, int factor) {
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

void computeHangingPieces(Board *b) {
    U64 WnotAttacked = ~b->getAttackedSquares(WHITE);
    U64 BnotAttacked = ~b->getAttackedSquares(BLACK);
    
    
    for (int i = PAWN; i <= QUEEN; i++) {
        _features[INDEX_PAWN_HANGING + i] =
                +bitCount(b->getPieces(WHITE, i) & WnotAttacked)
                - bitCount(b->getPieces(BLACK, i) & BnotAttacked);
    }
    
}

void computePinnedPieces(Board *b) {
    
    for (int i     = 0; i < 15; i++) {
        _features[INDEX_PINNED_PAWN_BY_BISHOP + i] = 0;
    }
    
    Square square;
    Square wkingSq = bitscanForward(b->getPieces(WHITE, KING));
    U64    pinner  = lookUpRookXRayAttack(wkingSq, *b->getOccupied(), b->getTeamOccupied()[WHITE]) &
                     (b->getPieces(BLACK, ROOK) | b->getPieces(BLACK, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[wkingSq][square] & b->getTeamOccupied()[WHITE]);
        _features[INDEX_PINNED_PAWN_BY_BISHOP + 3 * (b->getPiece(pinnedPlace) % 6) +
                  (b->getPiece(square) % 6 - BISHOP)] += 1;
        pinner = lsbReset(pinner);
    }
    
    pinner = lookUpBishopXRayAttack(wkingSq, *b->getOccupied(), b->getTeamOccupied()[WHITE]) &
             (b->getPieces(BLACK, BISHOP) | b->getPieces(BLACK, QUEEN));
    while (pinner) {
        square = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[wkingSq][square] & b->getTeamOccupied()[WHITE]);
        
        
        _features[INDEX_PINNED_PAWN_BY_BISHOP + 3 * (b->getPiece(pinnedPlace) % 6) +
                  (b->getPiece(square) % 6 - BISHOP)] += 1;
        pinner = lsbReset(pinner);
    }
    
    Square bkingSq = bitscanForward(b->getPieces(BLACK, KING));
    pinner = lookUpRookXRayAttack(bkingSq, *b->getOccupied(), b->getTeamOccupied()[BLACK]) &
             (b->getPieces(WHITE, ROOK) | b->getPieces(WHITE, QUEEN));
    while (pinner) {
        square = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[bkingSq][square] & b->getTeamOccupied()[BLACK]);
        _features[INDEX_PINNED_PAWN_BY_BISHOP + 3 * (b->getPiece(pinnedPlace) % 6) +
                  (b->getPiece(square) % 6 - BISHOP)] -= 1;
        pinner = lsbReset(pinner);
    }
    pinner = lookUpBishopXRayAttack(bkingSq, *b->getOccupied(), b->getTeamOccupied()[BLACK]) &
             (b->getPieces(WHITE, BISHOP) | b->getPieces(WHITE, QUEEN));
    while (pinner) {
        square = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[bkingSq][square] & b->getTeamOccupied()[BLACK]);
        
        
        _features[INDEX_PINNED_PAWN_BY_BISHOP + 3 * (b->getPiece(pinnedPlace) % 6) +
                  (b->getPiece(square) % 6 - BISHOP)] -= 1;
        pinner = lsbReset(pinner);
    }
    
    
}


/**
 * evaluates the board.
 * @param b
 * @return
 */
bb::Score Evaluator::evaluate(Board *b) {
    
    Score res = 0;


#ifdef TUNE_PST
    for(int i = 0; i < 64; i++){
        tunablePST_MG_grad[i] = 0;
        tunablePST_EG_grad[i] = 0;
    }
#endif
    
    
    memset(_features, 0, unusedVariable * sizeof(float));
    
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
    
    _phase =
            (24.0f
             - 1 * bitCount(
                    b->getPieces()[WHITE_BISHOP] |
                    b->getPieces()[BLACK_BISHOP] |
                    b->getPieces()[WHITE_KNIGHT] |
                    b->getPieces()[BLACK_KNIGHT])
             - 2 * bitCount(
                    b->getPieces()[WHITE_ROOK] |
                    b->getPieces()[BLACK_ROOK])
             - 4 * bitCount(
                    b->getPieces()[WHITE_QUEEN] |
                    b->getPieces()[BLACK_QUEEN])) / 24.0f;
    
    if (_phase > 1) _phase = 1;
    if (_phase < 0) _phase = 0;
    
    
    //values to scale early/lategame weights
    float earlyWeightScalar = (1 - _phase);
    float lateWeightScalar  = (_phase);
    
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
        
        _features[INDEX_PAWN_PSQT] +=
                psqt_pawn[squareIndex(rankIndex(square), (wKSide ? fileIndex(square) : 7 - fileIndex(square)))] *
                earlyPSTScalar;
        _features[INDEX_PAWN_PSQT] += psqt_pawn_endgame[square] * latePSTScalar;
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_PAWN];
    while (k) {
        square = bitscanForward(k);
        
        _features[INDEX_PAWN_PSQT] -=
                psqt_pawn[squareIndex(7 - rankIndex(square), (bKSide ? fileIndex(square) : 7 - fileIndex(square)))] * earlyPSTScalar;
        _features[INDEX_PAWN_PSQT] -=
                psqt_pawn_endgame[squareIndex(7 - rankIndex(square), fileIndex(square))] * latePSTScalar;
        
        k = lsbReset(k);
    }
    
    
    U64 whitePawnEastCover = shiftNorthEast(whitePawns) & whitePawns;
    U64 whitePawnWestCover = shiftNorthWest(whitePawns) & whitePawns;
    U64 blackPawnEastCover = shiftSouthEast(blackPawns) & blackPawns;
    U64 blackPawnWestCover = shiftSouthWest(blackPawns) & blackPawns;
    
    U64 whitePawnCover = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
    U64 blackPawnCover = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);
    
    _features[INDEX_PAWN_DOUBLED_AND_ISOLATED] =
            +bitCount(whiteIsolatedPawns & whiteDoubledPawns)
            - bitCount(blackIsolatedPawns & blackDoubledPawns);
    _features[INDEX_PAWN_DOUBLED]              =
            +bitCount(~whiteIsolatedPawns & whiteDoubledPawns)
            - bitCount(~blackIsolatedPawns & blackDoubledPawns);
    _features[INDEX_PAWN_ISOLATED]             =
            +bitCount(whiteIsolatedPawns & ~whiteDoubledPawns)
            - bitCount(blackIsolatedPawns & ~blackDoubledPawns);
    _features[INDEX_PAWN_PASSED]               =
            +bitCount(whitePassers)
            - bitCount(blackPassers);
    _features[INDEX_PAWN_VALUE]                =
            +bitCount(b->getPieces()[WHITE_PAWN])
            - bitCount(b->getPieces()[BLACK_PAWN]);
    _features[INDEX_PAWN_STRUCTURE]            =
            +bitCount(whitePawnEastCover)
            + bitCount(whitePawnWestCover)
            - bitCount(blackPawnEastCover)
            - bitCount(blackPawnWestCover);
    _features[INDEX_PAWN_OPEN]                 =
            +bitCount(whitePawns & ~fillSouth(blackPawns))
            - bitCount(blackPawns & ~fillNorth(whitePawns));
    _features[INDEX_PAWN_BACKWARD]             =
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
        
        
        _features[INDEX_KNIGHT_PSQT] += psqt_knight[pst_index_white(square)] * earlyPSTScalar;
        _features[INDEX_KNIGHT_PSQT] += psqt_knight_endgame[pst_index_white(square)] * latePSTScalar;
        
        
        _features[INDEX_KNIGHT_MOBILITY] += sqrt(bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresWhite));
        _features[INDEX_KNIGHT_OUTPOST] += isOutpost(square, WHITE, blackPawns, whitePawnCover);
        _features[INDEX_KNIGHT_DISTANCE_ENEMY_KING] += manhattanDistance(square, blackKingSquare);
        
        
        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks,
                        2);
        
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[square];
        
        
        _features[INDEX_KNIGHT_PSQT] -= psqt_knight[pst_index_black(square)] * earlyPSTScalar;
        _features[INDEX_KNIGHT_PSQT] -= psqt_knight_endgame[pst_index_black(square)] * latePSTScalar;;
        
        
        _features[INDEX_KNIGHT_MOBILITY] -= sqrt(bitCount(attacks & mobilitySquaresBlack));
        _features[INDEX_KNIGHT_OUTPOST] -= isOutpost(square, BLACK, whitePawns, blackPawnCover);
        _features[INDEX_KNIGHT_DISTANCE_ENEMY_KING] -= manhattanDistance(square, whiteKingSquare);
        
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks,
                        2);
        
        k = lsbReset(k);
    }
    _features[INDEX_KNIGHT_VALUE]            = (bitCount(b->getPieces()[WHITE_KNIGHT]) -
                                                bitCount(b->getPieces()[BLACK_KNIGHT]));
    /**********************************************************************************
     *                                  B I S H O P S                                 *
     **********************************************************************************/

    k = b->getPieces()[WHITE_BISHOP];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpBishopAttack(square, occupied);

#ifdef TUNE_PST
        tunablePST_MG_grad[pst_index_white(square)] += _pieceValuesEarly[INDEX_BISHOP_PSQT] * (1-_phase) / 100;
        tunablePST_EG_grad[pst_index_white(square)] += _pieceValuesLate [INDEX_BISHOP_PSQT] * _phase     / 100;
#endif
        
        _features[INDEX_BISHOP_PSQT] += psqt_bishop[pst_index_white(square)] * earlyPSTScalar;
        _features[INDEX_BISHOP_PSQT] += psqt_bishop_endgame[pst_index_white(square)] * (_phase) / 100.0;
        
        _features[INDEX_BISHOP_MOBILITY] += sqrt(bitCount(attacks & mobilitySquaresWhite));
        _features[INDEX_BISHOP_PAWN_SAME_SQUARE] += bitCount(
                blackPawns & ((ONE << square) & WHITE_SQUARES ? WHITE_SQUARES : BLACK_SQUARES));
        
        
        _features[INDEX_BISHOP_FIANCHETTO] +=
                (square == G2 &&
                 whitePawns & ONE << F2 &&
                 whitePawns & ONE << H2 &&
                 whitePawns & (ONE << G3 | ONE << G4));
        _features[INDEX_BISHOP_FIANCHETTO] +=
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
        tunablePST_MG_grad[pst_index_black(square)] -= _pieceValuesEarly[INDEX_BISHOP_PSQT] * (1-_phase) / 100;
        tunablePST_EG_grad[pst_index_black(square)] -= _pieceValuesLate [INDEX_BISHOP_PSQT] * _phase     / 100;
#endif
        
        _features[INDEX_BISHOP_PSQT] -= psqt_bishop[pst_index_black(square)] * earlyPSTScalar;
        _features[INDEX_BISHOP_PSQT] -= psqt_bishop_endgame[pst_index_black(square)] * latePSTScalar;
        
        _features[INDEX_BISHOP_MOBILITY] -= sqrt(bitCount(attacks & mobilitySquaresBlack));
        _features[INDEX_BISHOP_PAWN_SAME_SQUARE] -= bitCount(
                whitePawns & ((ONE << square) & WHITE_SQUARES ? WHITE_SQUARES : BLACK_SQUARES));
        
        
        _features[INDEX_BISHOP_FIANCHETTO] -=
                (square == G7 &&
                 blackPawns & ONE << F7 &&
                 blackPawns & ONE << H7 &&
                 blackPawns & (ONE << G6 | ONE << G5));
        _features[INDEX_BISHOP_FIANCHETTO] -=
                (square == B2 &&
                 blackPawns & ONE << A7 &&
                 blackPawns & ONE << C7 &&
                 blackPawns & (ONE << B6 | ONE << B5));
        
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks,
                        2);
        
        k = lsbReset(k);
    }
    _features[INDEX_BISHOP_VALUE] = (bitCount(b->getPieces()[WHITE_BISHOP]) - bitCount(b->getPieces()[BLACK_BISHOP]));
    _features[INDEX_BISHOP_DOUBLED] =
            (bitCount(b->getPieces()[WHITE_BISHOP]) == 2) - (bitCount(b->getPieces()[BLACK_BISHOP]) == 2);
    /**********************************************************************************
     *                                  R O O K S                                     *
     **********************************************************************************/

    k = b->getPieces()[WHITE_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied);
        
        
        _features[INDEX_ROOK_PSQT] += psqt_rook[pst_index_white(square)] * earlyPSTScalar;
        _features[INDEX_ROOK_PSQT] += psqt_rook_endgame[pst_index_white(square)] * (_phase) / 100.0;
        
        
        _features[INDEX_ROOK_MOBILITY] += sqrt(bitCount(attacks & mobilitySquaresWhite));
        
        if (lookUpRookAttack(square, ZERO) & b->getPieces()[BLACK_KING]) {
            //rook on same file or rank as king
            _features[INDEX_ROOK_KING_LINE]++;
        }
        if ((whitePawns & FILES[fileIndex(square)]) == 0) {
            if ((blackPawns & FILES[fileIndex(square)]) == 0) {
                //open
                _features[INDEX_ROOK_OPEN_FILE]++;
            } else {
                //half open
                _features[INDEX_ROOK_HALF_OPEN_FILE]++;
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
        
        
        _features[INDEX_ROOK_PSQT] -= psqt_rook[pst_index_black(square)] * earlyPSTScalar;
        _features[INDEX_ROOK_PSQT] -= psqt_rook_endgame[pst_index_black(square)] * (_phase) / 100.0;
        
        _features[INDEX_ROOK_MOBILITY] -= sqrt(bitCount(attacks & mobilitySquaresBlack));
        
        if (lookUpRookAttack(square, ZERO) & b->getPieces()[WHITE_KING]) {
            //rook on same file or rank as king
            _features[INDEX_ROOK_KING_LINE]--;
        }
        
        if ((whitePawns & FILES[fileIndex(square)]) == 0) {
            if ((blackPawns & FILES[fileIndex(square)]) == 0) {
                //open
                _features[INDEX_ROOK_OPEN_FILE]--;
            } else {
                //half open
                _features[INDEX_ROOK_HALF_OPEN_FILE]--;
            }
        }
        
        
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks,
                        3);
        
        k = lsbReset(k);
    }
    _features[INDEX_ROOK_VALUE] = (bitCount(b->getPieces()[WHITE_ROOK]) - bitCount(b->getPieces()[BLACK_ROOK]));
    
    /**********************************************************************************
     *                                  Q U E E N S                                   *
     **********************************************************************************/

    
    k = b->getPieces()[WHITE_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied) | lookUpBishopAttack(square, occupied);
        
        
        _features[INDEX_QUEEN_PSQT] += psqt_queen[pst_index_white(square)] * earlyPSTScalar;
        _features[INDEX_QUEEN_PSQT] += psqt_queen_endgame[pst_index_white(square)] * latePSTScalar;
        
        
        _features[INDEX_QUEEN_MOBILITY] += sqrt(bitCount(attacks & mobilitySquaresWhite));
        _features[INDEX_QUEEN_DISTANCE_ENEMY_KING] += manhattanDistance(square, blackKingSquare);
        
        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks,
                        4);
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied) | lookUpBishopAttack(square, occupied);
        
        
        _features[INDEX_QUEEN_PSQT] -= psqt_queen[pst_index_black(square)] * earlyPSTScalar;
        _features[INDEX_QUEEN_PSQT] -= psqt_queen_endgame[pst_index_black(square)] * latePSTScalar;
        
        
        _features[INDEX_QUEEN_MOBILITY] -= sqrt(bitCount(attacks & mobilitySquaresBlack));
        _features[INDEX_QUEEN_DISTANCE_ENEMY_KING] -= manhattanDistance(square, whiteKingSquare);
        
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks,
                        4);
        
        k = lsbReset(k);
    }
    _features[INDEX_QUEEN_VALUE] = bitCount(b->getPieces()[WHITE_QUEEN]) - bitCount(b->getPieces()[BLACK_QUEEN]);
    
    /**********************************************************************************
     *                                  K I N G S                                     *
     **********************************************************************************/
    k = b->getPieces()[WHITE_KING];

    
    while (k) {
        square = bitscanForward(k);
        
        _features[INDEX_KING_PSQT] += psqt_king[63 - square] * earlyPSTScalar;
        _features[INDEX_KING_PSQT] += psqt_king_endgame[63 - square] * latePSTScalar;
        
        _features[INDEX_KING_PAWN_SHIELD] += bitCount(KING_ATTACKS[square] & whitePawns);
        _features[INDEX_KING_CLOSE_OPPONENT] += bitCount(KING_ATTACKS[square] & blackTeam);
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_KING];
    while (k) {
        square = bitscanForward(k);
        
        _features[INDEX_KING_PSQT] -= psqt_king[square] * earlyPSTScalar;
        _features[INDEX_KING_PSQT] -= psqt_king_endgame[square] * latePSTScalar;
        
        _features[INDEX_KING_PAWN_SHIELD] -= bitCount(KING_ATTACKS[square] & blackPawns);
        _features[INDEX_KING_CLOSE_OPPONENT] -= bitCount(KING_ATTACKS[square] & whiteTeam);
        
        k = lsbReset(k);
    }
    
    computeHangingPieces(b);
    computePinnedPieces(b);
    
    _features[INDEX_KING_SAFETY] =
            (_kingSafetyTable[blackkingSafety_valueOfAttacks] - _kingSafetyTable[whitekingSafety_valueOfAttacks]) / 100;
    
    
    
    
    
    
    
    __m128 earlyRes{};
    __m128 lateRes{};
    
    for (int i = 0; i < unusedVariable; i += 4) {
        __m128 *feat = (__m128 *) (_features + (i));
        
        __m128 *w1 = (__m128 *) (_pieceValuesEarly + (i));
        __m128 *w2 = (__m128 *) (_pieceValuesLate + (i));
        
        earlyRes = _mm_add_ps(earlyRes, _mm_mul_ps(*w1, *feat));
        lateRes  = _mm_add_ps(lateRes, _mm_mul_ps(*w2, *feat));
    }
    
    
    const __m128 tE   = _mm_add_ps(earlyRes, _mm_movehl_ps(earlyRes, earlyRes));
    const __m128 sumE = _mm_add_ss(tE, _mm_shuffle_ps(tE, tE, 1));
    const __m128 tL   = _mm_add_ps(lateRes, _mm_movehl_ps(lateRes, lateRes));
    const __m128 sumL = _mm_add_ss(tL, _mm_shuffle_ps(tL, tL, 1));
    
    res = sumE[0] * (1 - _phase) + sumL[0] * (_phase);
    return res;
}

void printEvaluation(Board *board) {
    
    using namespace std;
    
    Evaluator ev{};
    Score     score = ev.evaluate(board);
    float     phase = ev.getPhase();
    
    
    stringstream ss{};
    
    
    //String format = "%-30s | %-20s | %-20s %n";
    
    ss << std::setw(40) << std::left << "feature" << " | "
       << std::setw(20) << std::right << "difference" << " | "
       << std::setw(20) << "early weight" << " | "
       << std::setw(20) << "late weight" << " | "
       << std::setw(20) << "tapered weight" << " | "
       << std::setw(20) << "sum" << "\n";
    
    
    ss << "-----------------------------------------+----------------------+"
          "----------------------+----------------------+"
          "----------------------+----------------------+\n";
    ss << std::setw(40) << std::left << "PHASE" << " | "
       << std::setw(20) << std::right << "" << " | "
       << std::setw(20) << "0" << " | "
       << std::setw(20) << "1" << " | "
       << std::setw(20) << phase << " | "
       << std::setw(20) << phase << " | \n";
    
    ss << "-----------------------------------------+----------------------+"
          "----------------------+----------------------+"
          "----------------------+----------------------+\n";
    
    string names[]{
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
            
            //ignore this and place new values before here
            "-",
            "-",
            "-",
            "-",
    };
    
    for (int i = 0; i < unusedVariable; i++) {
        
        ss << std::setw(40) << std::left << names[i] << " | "
           << std::setw(20) << std::right << ev.getFeatures()[i] << " | "
           << std::setw(20) << ev.getEarlyGameParams()[i] << " | "
           << std::setw(20) << ev.getLateGameParams()[i] << " | "
           << std::setw(20) << ev.getEarlyGameParams()[i] * (1 - phase) + ev.getLateGameParams()[i] * phase << " | "
           << std::setw(20)
           << (ev.getEarlyGameParams()[i] * (1 - phase) + ev.getLateGameParams()[i] * phase) * ev.getFeatures()[i]
           << " | \n";
    }
    ss << "-----------------------------------------+----------------------+"
          "----------------------+----------------------+"
          "----------------------+----------------------+\n";
    
    
    ss << std::setw(40) << std::left << "TOTAL" << " | "
       << std::setw(20) << std::right << "" << " | "
       << std::setw(20) << "" << " | "
       << std::setw(20) << "" << " | "
       << std::setw(20) << "" << " | "
       << std::setw(20) << score << " | \n";
    
    ss << "-----------------------------------------+----------------------+"
          "----------------------+----------------------+"
          "----------------------+----------------------+\n";
    
    std::cout << ss.str() << std::endl;
}

float *Evaluator::getFeatures() {
    return _features;
}

float Evaluator::getPhase() {
    return _phase;
}

float *Evaluator::getEarlyGameParams() {
    return _pieceValuesEarly;
}

float *Evaluator::getLateGameParams() {
    return _pieceValuesLate;
}

int Evaluator::paramCount() {
    return unusedVariable;
}

float *Evaluator::getPSQT(Piece piece, bool early) {
    switch (piece) {
        case PAWN:
            return early ? psqt_pawn : psqt_pawn_endgame;
        case KNIGHT:
            return early ? psqt_knight : psqt_knight_endgame;
        case BISHOP:
            return early ? psqt_bishop : psqt_bishop_endgame;
        case ROOK:
            return early ? psqt_rook : psqt_rook_endgame;
        case QUEEN:
            return early ? psqt_queen : psqt_queen_endgame;
        case KING:
            return early ? psqt_king : psqt_king_endgame;
    }
}

#ifdef TUNE_PST
float *Evaluator::getTunablePST_MG() {
    return psqt_bishop;
}

float *Evaluator::getTunablePST_EG() {
    return psqt_bishop_endgame;
}

float *Evaluator::getTunablePST_MG_grad() {
    return tunablePST_MG_grad;
}

float *Evaluator::getTunablePST_EG_grad() {
    return tunablePST_EG_grad;
}
#endif
