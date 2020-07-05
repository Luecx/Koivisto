//
// Created by finne on 5/31/2020.
//

#include <iomanip>
#include "eval.h"


#define pst_index_white(s) squareIndex(7-rankIndex(s), fileIndex(s))
#define pst_index_black(s) s

double * psqt_pawn_endgame = new double[64]{
     0.000,     0.000,     0.000,     0.000,     0.000,     0.000,     0.000,     0.000,
     8.468,    -5.743,     9.089,     4.721,     9.111,     0.484,   -14.387,    -7.112,
    -3.367,     0.189,    -6.827,     0.655,    -1.621,    -2.004,    -5.150,    -7.745,
     8.127,     6.428,    -3.006,   -17.729,   -12.550,    -3.491,     0.343,     3.798,
    23.973,    20.263,    14.301,     2.387,     2.797,     8.021,    17.699,    15.459,
    84.682,    90.018,    80.790,    63.723,    61.787,    61.816,    77.160,    77.968,
   149.876,   129.031,   118.179,   102.717,   100.661,   104.561,   117.594,   131.497,
     0.000,     0.000,     0.000,     0.000,     0.000,     0.000,     0.000,     0.000,
};

double * psqt_pawn = new double[64]{
          0.000,     0.000,     0.000,     0.000,     0.000,     0.000,     0.000,     0.000,
    -4.819,    51.504,    32.302,   -19.137,   -26.091,   -52.893,     6.069,   -12.853,
     5.231,    18.449,    -8.151,     1.120,   -10.764,     7.481,   -19.175,    -3.865,
   -18.220,   -18.375,    -6.706,    28.162,    47.944,     4.643,   -12.987,   -13.113,
    -2.504,    -2.743,     2.084,    20.448,    27.055,     2.174,     4.219,     0.870,
     1.288,     1.900,     3.693,     4.249,     3.415,     3.028,     0.642,     2.725,
     0.605,     1.247,     1.414,     1.158,     1.453,     1.024,     2.779,     1.624,
     0.000,     0.000,     0.000,     0.000,     0.000,     0.000,     0.000,     0.000,
};


double* psqt_knight = new double[64]{
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50,
};
double* psqt_knight_endgame = new double[64]{
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50,
};

double * psqt_bishop = new double[64]{
        -231.445,  -113.944,  -238.573,  -146.317,   -93.138,  -172.036,   -35.634,   -69.548,
        -336.083,  -133.202,  -210.561,  -208.590,   -22.389,   100.960,   -45.850,  -400.840,
        -219.501,   -98.473,    36.832,   -42.520,   -42.634,    70.228,    47.584,  -148.553,
        -163.391,   -59.191,  -112.249,    82.143,     4.871,    25.918,   -82.413,   -88.737,
         -85.815,   -88.367,   -12.882,   -21.046,    61.966,   -86.123,   -63.157,   -61.738,
        -108.750,    31.933,   -19.343,    10.250,   -56.049,    51.839,   -25.637,   -96.362,
          22.542,   -25.105,    41.936,   -53.528,     7.838,    -6.833,    12.715,   -37.420,
        -177.770,    15.745,   -96.949,  -113.831,  -111.571,   -44.741,  -147.119,  -202.108
};

double * psqt_bishop_endgame = new double[64]{
        -63.803,   -26.753,   -45.054,   -30.422,   -23.665,   -28.748,    -8.041,   -49.229,
         -5.346,   -19.950,     3.591,   -45.005,     0.341,    -1.064,   -19.871,   -19.884,
          6.314,     9.385,   -27.568,   -19.494,   -14.257,     8.968,    -0.978,    14.444,
         -2.681,    27.947,    48.461,     0.643,    14.817,     9.852,    -2.028,    -8.850,
        -11.990,     8.421,    40.236,    82.356,   -33.350,    28.875,   -16.337,    10.150,
          7.806,    -1.855,    91.171,    52.317,   135.674,    18.454,    31.272,    21.261,
        -18.252,   -12.854,   -17.464,    31.653,    50.292,    13.326,    13.911,   -25.495,
          7.250,    30.103,    28.035,    44.119,    47.047,     8.251,    44.781,    12.708,
};




double * psqt_rook = new double[64]{
         -2.2,       4.3,     -12.4,       8.8,       9.4,      -1.7,      -3.2,      -2.9,
        -16.6,      -6.2,      17.0,      16.4,      24.6,      22.0,      -8.5,      -2.9,
         -9.2,       5.9,      -1.9,       4.8,      -4.0,       9.7,      18.9,       5.4,
        -30.8,     -10.0,      -0.4,       5.8,       5.4,       8.5,     -11.7,      -0.0,
        -28.6,     -16.2,      -9.5,      -9.5,       4.6,     -12.9,       0.9,       2.8,
        -42.0,     -11.1,      -5.4,     -16.8,      -3.9,      -3.4,      -8.6,     -17.2,
        -38.6,      -6.7,     -18.2,      -3.9,       8.7,       4.9,     -18.7,     -55.8,
        -10.6,      -7.4,       7.6,      11.1,      13.1,       7.3,     -28.2,      10.8,
};

double * psqt_rook_endgame = new double[64]{
        23.7,      16.4,      20.9,      17.7,      22.5,      22.1,      22.4,      15.2,
        26.2,      26.0,      21.5,      22.4,      11.2,      21.9,      26.3,      17.7,
        13.4,      14.8,      13.0,      15.8,       8.6,      10.4,       5.4,      -1.8,
        13.6,       8.1,      23.4,       4.7,      12.5,      12.7,       4.4,       3.8,
        10.9,      15.7,      16.1,      10.6,       2.1,       4.9,       4.7,     -10.3,
         9.1,       9.2,       1.4,       6.6,      -0.8,      -2.2,       2.9,      -9.4,
         6.9,       6.0,       9.3,       7.5,      -3.4,      -1.6,      -1.8,       4.4,
         3.1,      10.8,       7.3,       2.8,      -0.2,      -0.6,       3.3,     -22.6,
};

double * psqt_queen = new double[64]{
        -32.7,     -14.0,      -8.9,      -8.9,       6.5,      -4.4,      -4.6,       2.7,
        -47.2,     -70.6,     -21.9,      -1.4,     -20.8,      11.5,      -2.9,      16.0,
        -22.3,     -18.5,      -5.1,     -18.7,      10.2,      16.6,      10.4,      15.0,
        -28.6,     -25.7,     -22.8,     -35.2,      -5.5,       0.7,      -1.0,      -6.5,
          5.0,     -30.2,       6.1,     -16.4,       6.2,       4.2,       8.5,      -1.2,
        -13.7,      23.4,       8.8,      11.7,       4.6,      13.4,      17.3,       9.6,
        -17.8,       2.3,      36.7,      32.0,      44.2,      24.7,      -6.9,       0.4,
          2.4,       5.8,      13.0,      33.2,       1.6,     -16.0,     -16.7,     -30.9,
};

double * psqt_queen_endgame = new double[64]{
        -32.2,      -5.2,      -6.1,     -10.3,       0.6,      -4.9,      -5.7,      -5.1,
        -26.9,      -2.4,      -1.7,       7.5,       3.8,       6.3,       2.8,      -3.6,
        -20.6,     -14.2,     -12.3,      15.0,      14.5,       6.9,       4.8,      -1.0,
         -1.3,       3.4,       0.7,      13.5,      18.1,      10.0,      19.0,      12.0,
        -11.6,       8.2,       0.6,      30.0,      10.5,      10.1,      20.4,       5.4,
         -8.1,     -25.3,       0.4,      -1.5,       5.1,       9.8,       8.9,      -4.3,
        -12.5,     -16.2,     -32.8,     -23.0,     -22.4,     -12.5,     -14.2,     -14.5,
        -19.7,     -23.8,     -22.9,     -31.5,      -3.7,     -17.6,     -15.4,     -28.5,
};

double psqt_king[] = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        20, 20,  0,  0,  0,  0, 20, 20,
        20, 30, 10,  0,  0, 10, 30, 20
};
double psqt_king_endgame[] = {
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
};


int unusedVariable = 0;

int INDEX_PAWN_VALUE = unusedVariable++;
int INDEX_PAWN_PSQT = unusedVariable++;
int INDEX_PAWN_STRUCTURE = unusedVariable++;
int INDEX_PAWN_PASSED = unusedVariable++;
int INDEX_PAWN_ISOLATED = unusedVariable++;
int INDEX_PAWN_DOUBLED = unusedVariable++;
int INDEX_PAWN_DOUBLED_AND_ISOLATED = unusedVariable++;

int INDEX_KNIGHT_VALUE = unusedVariable++;
int INDEX_KNIGHT_PSQT = unusedVariable++;
int INDEX_KNIGHT_MOBILITY = unusedVariable++;
int INDEX_KNIGHT_OUTPOST = unusedVariable++;

int INDEX_BISHOP_VALUE = unusedVariable++;
int INDEX_BISHOP_PSQT = unusedVariable++;
int INDEX_BISHOP_MOBILITY = unusedVariable++;
int INDEX_BISHOP_DOUBLED = unusedVariable++;
int INDEX_BISHOP_PAWN_SAME_SQUARE = unusedVariable++;
int INDEX_BISHOP_FIANCHETTO = unusedVariable++;

int INDEX_ROOK_VALUE = unusedVariable++;
int INDEX_ROOK_PSQT = unusedVariable++;
int INDEX_ROOK_MOBILITY = unusedVariable++;
int INDEX_ROOK_OPEN_FILE = unusedVariable++;
int INDEX_ROOK_HALF_OPEN_FILE = unusedVariable++;
int INDEX_ROOK_KING_LINE = unusedVariable++;

int INDEX_QUEEN_VALUE = unusedVariable++;
int INDEX_QUEEN_PSQT = unusedVariable++;
int INDEX_QUEEN_MOBILITY = unusedVariable++;

int INDEX_KING_SAFETY = unusedVariable++;
int INDEX_KING_PSQT = unusedVariable++;
int INDEX_KING_CLOSE_OPPONENT = unusedVariable++;
int INDEX_KING_PAWN_SHIELD = unusedVariable++;


#ifdef TUNE_PST
double * tunablePST_MG_grad = new double[64]{};
double * tunablePST_EG_grad = new double[64]{};
#endif

double* _pieceValuesEarly = new double[unusedVariable]{
        98.6423,       100,       2.99681,      -14.4001,      -12.5356,      0.171409,      -23.6882,       376.415,       61.2558,       29.3162,       41.5542,       424.944,       34.5504,       23.6178,       27.7169,      -4.37584,       11.9598,       558.332,       100,       12.1858,       47.2119,        18.385,       23.3907,       1165.71,      100,       7.46494,       366.888,       239.123,       -64.153,       13.2171,
    
};

double* _pieceValuesLate = new double[unusedVariable]{
        118.648,        100 ,       3.24607,       42.0323,      -10.9291,      -9.90538,      -24.8109,       358.821,       107.184,       23.4736,       33.4458,        312.52,       3.10072,       40.8573,       62.9856,       6.44621,         10.15,       639.243,        100,       28.3109,       3.98184,      -1.75178,       4.39457,       1181.54,       100,       62.2153,      -23.3872,       50.5677,       51.2386,      -2.42515,
    
    
    
};

//TODO tweak values
double _kingSafetyTable[100] {
        0  ,   0,   1,   2,   3,   5,   7,   9,  12,  15,
        18 ,  22,  26,  30,  35,  39,  44,  50,  56,  62,
        68 ,  75,  82,  85,  89,  97, 105, 113, 122, 131,
        140, 150, 169, 180, 191, 202, 213, 225, 237, 248,
        260, 272, 283, 295, 307, 319, 330, 342, 354, 366,
        377, 389, 401, 412, 424, 436, 448, 459, 471, 483,
        494, 500, 500, 500, 500, 500, 500, 500, 500, 500,
        500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
        500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
        500, 500, 500, 500, 500, 500, 500, 500, 500, 500
};



double* _features          = new double[unusedVariable];

double  _phase;

/**
 * adds the factor to value of attacks if the piece attacks the kingzone
 * @param attacks
 * @param kingZone
 * @param pieceCount
 * @param valueOfAttacks
 * @param factor
 */
void addToKingSafety(U64 attacks, U64 kingZone, int &pieceCount, int &valueOfAttacks, int factor){
    if(attacks & kingZone){
        pieceCount ++;
        valueOfAttacks += factor * bitCount(attacks & kingZone);
    }
}

/**
 * checks if the given square is an outpost given the color and a bitboard of the opponent pawns
 */
bool isOutpost(Square s, Color c, U64 opponentPawns, U64 pawnCover){
    U64 sq = ONE << s;
    
    if(c == WHITE){
        if(((whitePassedPawnMask[s] & ~FILES[fileIndex(s)]) & opponentPawns) == 0 && (sq & pawnCover)){
            return true;
        }
    }else{
        if(((blackPassedPawnMask[s] & ~FILES[fileIndex(s)]) & opponentPawns) == 0 && (sq & pawnCover)){
            return true;
        }
    }
    return false;
    
}


/**
 * evaluates the board.
 * @param b
 * @return
 */
bb::Score Evaluator::evaluate(Board *b) {
    
    Score res = 0;
    
    U64 k;
    int phase = 0;

    #ifdef TUNE_PST
    for(int i = 0; i < 64; i++){
        tunablePST_MG_grad[i] = 0;
        tunablePST_EG_grad[i] = 0;
    }
    #endif
    
    U64 whiteTeam = b->getTeamOccupied()[WHITE];
    U64 blackTeam = b->getTeamOccupied()[BLACK];
    U64 occupied = *b->getOccupied();
    
    U64 whiteKingZone = KING_ATTACKS[bitscanForward(b->getPieces()[WHITE_KING])];
    U64 blackKingZone = KING_ATTACKS[bitscanForward(b->getPieces()[BLACK_KING])];
    
    Square s;
    U64 attacks;
    
    
    
    _phase =
            (18 - bitCount(
                    b->getPieces()[WHITE_BISHOP] |
                    b->getPieces()[BLACK_BISHOP] |
                    b->getPieces()[WHITE_KNIGHT] |
                    b->getPieces()[BLACK_KNIGHT] |
                    b->getPieces()[WHITE_ROOK] |
                    b->getPieces()[BLACK_ROOK]) -
             3*bitCount(
                     b->getPieces()[WHITE_QUEEN] |
                     b->getPieces()[BLACK_QUEEN])) / 18.0;
    
    
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


    k = whitePawns;
    _features[INDEX_PAWN_PASSED] = 0;
    _features[INDEX_PAWN_ISOLATED] = 0;
    _features[INDEX_PAWN_DOUBLED] = 0;
    _features[INDEX_PAWN_DOUBLED_AND_ISOLATED] = 0;
    _features[INDEX_PAWN_PSQT] = 0;
    while(k){
        Square s = bitscanForward(k);
        
        bool isolated = (FILES_NEIGHBOUR[fileIndex(s)] & whitePawns) == 0;
        bool doubled = (FILES[fileIndex(s)] & (whitePawns & ~(ONE << s))) != 0;
        
        if(isolated && doubled){
            _features[INDEX_PAWN_DOUBLED_AND_ISOLATED] += 1;
        }else if(isolated){
            _features[INDEX_PAWN_ISOLATED] += 1;
        }else if(doubled){
            _features[INDEX_PAWN_DOUBLED] += 1;
        }
        
        
        //passed pawn
        if((whitePassedPawnMask[s] & blackPawns) == 0){
            _features[INDEX_PAWN_PASSED] += 1;
        }



        
        _features[INDEX_PAWN_PSQT]  += psqt_pawn            [squareIndex(rankIndex(s), (wKSide ? fileIndex(s) : 7 - fileIndex(s)))] * (1-_phase)    / 100.0;
        _features[INDEX_PAWN_PSQT]  += psqt_pawn_endgame    [s] * (  _phase)    / 100.0;
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_PAWN];
    while(k){
        Square s = bitscanForward(k);
        
        
        
        bool isolated = (FILES_NEIGHBOUR[fileIndex(s)] & blackPawns) == 0;
        bool doubled = (FILES[fileIndex(s)] & (blackPawns & ~(ONE << s))) != 0;
        
        if(isolated && doubled){
            _features[INDEX_PAWN_DOUBLED_AND_ISOLATED] -= 1;
        }else if(isolated){
            _features[INDEX_PAWN_ISOLATED] -= 1;
        }else if(doubled){
            _features[INDEX_PAWN_DOUBLED] -= 1;
        }
        
        
        //passed pawn
        if((blackPassedPawnMask[s] & whitePawns) == 0){
            _features[INDEX_PAWN_PASSED] -= 1;
        }


        
        _features[INDEX_PAWN_PSQT] -= psqt_pawn         [squareIndex(7-rankIndex(s), (bKSide ? fileIndex(s) : 7 - fileIndex(s)))] * (1-_phase) / 100.0;
        _features[INDEX_PAWN_PSQT] -= psqt_pawn_endgame [squareIndex(7-rankIndex(s), fileIndex(s))] * _phase     / 100.0;
        
        
        k = lsbReset(k);
    }
    
    
    
    U64 whitePawnEastCover = shiftNorthEast(whitePawns) & whitePawns;
    U64 whitePawnWestCover = shiftNorthEast(whitePawns) & whitePawns;
    U64 blackPawnEastCover = shiftNorthEast(blackPawns) & blackPawns;
    U64 blackPawnWestCover = shiftNorthEast(blackPawns) & blackPawns;
    
    _features[INDEX_PAWN_VALUE] =
            + bitCount(b->getPieces()[WHITE_PAWN])
            - bitCount(b->getPieces()[BLACK_PAWN]);
    _features[INDEX_PAWN_STRUCTURE] =
            + bitCount(whitePawnEastCover)
            + bitCount(whitePawnWestCover)
            - bitCount(blackPawnEastCover)
            - bitCount(blackPawnWestCover);
    
    
    U64 whitePawnCover = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
    U64 blackPawnCover = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);
    
    /*
     * only these squares are counted for mobility
     */
    U64 mobilitySquaresWhite = ~whiteTeam & ~(blackPawnCover);
    U64 mobilitySquaresBlack = ~blackTeam & ~(whitePawnCover);
    
    /**********************************************************************************
     *                                  K N I G H T S                                 *
     **********************************************************************************/
    
    _features[INDEX_KNIGHT_MOBILITY] = 0;
    _features[INDEX_KNIGHT_PSQT] = 0;
    _features[INDEX_KNIGHT_OUTPOST] = 0;
    
    k = b->getPieces()[WHITE_KNIGHT];
    while(k){
        s = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[s];


#ifdef TUNE_PST
        tunablePST_MG_grad[pst_index_white(s)] += _pieceValuesEarly[INDEX_KNIGHT_PSQT] * (1-_phase) / 100;
        tunablePST_EG_grad[pst_index_white(s)] += _pieceValuesLate [INDEX_KNIGHT_PSQT] * _phase     / 100;
#endif
        _features[INDEX_KNIGHT_PSQT]  += psqt_knight            [pst_index_white(s)] * (1-_phase)    / 100.0;
        _features[INDEX_KNIGHT_PSQT]  += psqt_knight_endgame    [pst_index_white(s)] * (  _phase)    / 100.0;
        
        
        
        _features[INDEX_KNIGHT_MOBILITY] += sqrt(bitCount(KNIGHT_ATTACKS[s] & mobilitySquaresWhite));
        
        
        _features[INDEX_KNIGHT_OUTPOST] += isOutpost(s, WHITE, blackPawns, whitePawnCover);
        
        
        
        phase++;
        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks, 2);
        
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_KNIGHT];
    while(k){
        s = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[s];


        
#ifdef TUNE_PST
        tunablePST_MG_grad[pst_index_white(s)] -= _pieceValuesEarly[INDEX_KNIGHT_PSQT] * (1-_phase) / 100;
        tunablePST_EG_grad[pst_index_white(s)] -= _pieceValuesLate [INDEX_KNIGHT_PSQT] * _phase     / 100;
#endif
        _features[INDEX_KNIGHT_PSQT]  -= psqt_knight            [pst_index_black(s)] * (1-_phase)    / 100.0;
        _features[INDEX_KNIGHT_PSQT]  -= psqt_knight_endgame    [pst_index_black(s)] * (  _phase)    / 100.0;
        
        
        
        
        _features[INDEX_KNIGHT_MOBILITY] -= sqrt(bitCount(attacks & mobilitySquaresBlack));
        
        _features[INDEX_KNIGHT_OUTPOST] -= isOutpost(s, BLACK, whitePawns, blackPawnCover);
        
        phase++;
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks, 2);
        
        k = lsbReset(k);
    }
    _features[INDEX_KNIGHT_VALUE] = (bitCount(b->getPieces()[WHITE_KNIGHT]) - bitCount(b->getPieces()[BLACK_KNIGHT]));
    /**********************************************************************************
     *                                  B I S H O P S                                 *
     **********************************************************************************/
    _features[INDEX_BISHOP_MOBILITY] = 0;
    _features[INDEX_BISHOP_PAWN_SAME_SQUARE] = 0;
    _features[INDEX_BISHOP_PSQT] = 0;
    _features[INDEX_BISHOP_FIANCHETTO] = 0;
    
    k = b->getPieces()[WHITE_BISHOP];
    while(k){
        Square s = bitscanForward(k);
        attacks = lookUpBishopAttack(s, occupied);


  
        
        _features[INDEX_BISHOP_PSQT]  += psqt_bishop            [pst_index_white(s)] * (1-_phase)    / 100.0;
        _features[INDEX_BISHOP_PSQT]  += psqt_bishop_endgame    [pst_index_white(s)] * (  _phase)    / 100.0;
        
        _features[INDEX_BISHOP_MOBILITY] += sqrt(bitCount(attacks & mobilitySquaresWhite));
        _features[INDEX_BISHOP_PAWN_SAME_SQUARE] += bitCount(blackPawns & ((ONE << s) & WHITE_SQUARES ? WHITE_SQUARES:BLACK_SQUARES));
        
        
        _features[INDEX_BISHOP_FIANCHETTO] +=
                (s == G2 &&
                 whitePawns & ONE<<F2 &&
                 whitePawns & ONE<<H2 &&
                 whitePawns & (ONE<<G3 | ONE<<G4));
        _features[INDEX_BISHOP_FIANCHETTO] +=
                (s == B2 &&
                 whitePawns & ONE<<A2 &&
                 whitePawns & ONE<<C2 &&
                 whitePawns & (ONE<<B3 | ONE<<B4));
        
        
        phase++;
        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks, 2);
        
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_BISHOP];
    while(k){
        Square s = bitscanForward(k);
        attacks = lookUpBishopAttack(s, occupied);

        
        
        _features[INDEX_BISHOP_PSQT] -= psqt_bishop         [pst_index_black(s)] * (1-_phase) / 100.0;
        _features[INDEX_BISHOP_PSQT] -= psqt_bishop_endgame [pst_index_black(s)] * _phase     / 100.0;
        
        _features[INDEX_BISHOP_MOBILITY] -= sqrt(bitCount(attacks & mobilitySquaresBlack));
        _features[INDEX_BISHOP_PAWN_SAME_SQUARE] -= bitCount(whitePawns & ((ONE << s) & WHITE_SQUARES ? WHITE_SQUARES:BLACK_SQUARES));
        
        
        _features[INDEX_BISHOP_FIANCHETTO] -=
                (s == G7 &&
                 blackPawns & ONE<<F7 &&
                 blackPawns & ONE<<H7 &&
                 blackPawns & (ONE<<G6 | ONE<<G5));
        _features[INDEX_BISHOP_FIANCHETTO] -=
                (s == B2 &&
                 blackPawns & ONE<<A7 &&
                 blackPawns & ONE<<C7 &&
                 blackPawns & (ONE<<B6 | ONE<<B5));
        
        phase++;
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks, 2);
        
        k = lsbReset(k);
    }
    _features[INDEX_BISHOP_VALUE] = (bitCount(b->getPieces()[WHITE_BISHOP]) - bitCount(b->getPieces()[BLACK_BISHOP]));
    _features[INDEX_BISHOP_DOUBLED] = (bitCount(b->getPieces()[WHITE_BISHOP])==2) - (bitCount(b->getPieces()[BLACK_BISHOP])==2);
    /**********************************************************************************
     *                                  R O O K S                                     *
     **********************************************************************************/
    
    _features[INDEX_ROOK_MOBILITY] = 0;
    _features[INDEX_ROOK_OPEN_FILE] = 0;
    _features[INDEX_ROOK_HALF_OPEN_FILE] = 0;
    _features[INDEX_ROOK_KING_LINE]  = 0;
    _features[INDEX_ROOK_PSQT] = 0;
    
    k = b->getPieces()[WHITE_ROOK];
    while(k){
        Square s = bitscanForward(k);
        attacks = lookUpRookAttack(s,occupied);


        _features[INDEX_ROOK_PSQT]  += psqt_rook            [pst_index_white(s)] * (1-_phase)    / 100.0;
        _features[INDEX_ROOK_PSQT]  += psqt_rook_endgame    [pst_index_white(s)] * (  _phase)    / 100.0;
        
        
        
        _features[INDEX_ROOK_MOBILITY] += sqrt(bitCount(attacks & mobilitySquaresWhite));
        
        if(lookUpRookAttack(s, ZERO) & b->getPieces()[BLACK_KING]){
            //rook on same file or rank as king
            _features[INDEX_ROOK_KING_LINE] ++;
        }
        if((whitePawns & FILES[fileIndex(s)]) == 0){
            if((blackPawns & FILES[fileIndex(s)]) == 0){
                //open
                _features[INDEX_ROOK_OPEN_FILE] ++;
            }else{
                //half open
                _features[INDEX_ROOK_HALF_OPEN_FILE] ++;
            }
        }
        
        phase++;
        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks, 3);
        
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_ROOK];
    while(k){
        Square s = bitscanForward(k);
        attacks = lookUpRookAttack(s,occupied);
        

        _features[INDEX_ROOK_PSQT]  -= psqt_rook            [pst_index_black(s)] * (1-_phase)    / 100.0;
        _features[INDEX_ROOK_PSQT]  -= psqt_rook_endgame    [pst_index_black(s)] * (  _phase)    / 100.0;
        
        _features[INDEX_ROOK_MOBILITY] -= sqrt(bitCount(attacks & mobilitySquaresBlack));
        
        if(lookUpRookAttack(s, ZERO) & b->getPieces()[WHITE_KING]){
            //rook on same file or rank as king
            _features[INDEX_ROOK_KING_LINE] --;
        }
        
        if((whitePawns & FILES[fileIndex(s)]) == 0){
            if((blackPawns & FILES[fileIndex(s)]) == 0){
                //open
                _features[INDEX_ROOK_OPEN_FILE] --;
            }else{
                //half open
                _features[INDEX_ROOK_HALF_OPEN_FILE] --;
            }
        }
        
        
        phase++;
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks, 3);
        
        k = lsbReset(k);
    }
    _features[INDEX_ROOK_VALUE] = (bitCount(b->getPieces()[WHITE_ROOK])   - bitCount(b->getPieces()[BLACK_ROOK]));
    
    /**********************************************************************************
     *                                  Q U E E N S                                   *
     **********************************************************************************/
    _features[INDEX_QUEEN_MOBILITY] = 0;
    _features[INDEX_QUEEN_PSQT] = 0;
    
    k = b->getPieces()[WHITE_QUEEN];
    while(k){
        Square s = bitscanForward(k);
        attacks = lookUpRookAttack(s, occupied) | lookUpBishopAttack(s, occupied);

        _features[INDEX_QUEEN_PSQT]  += psqt_queen            [pst_index_white(s)] * (1-_phase)    / 100.0;
        _features[INDEX_QUEEN_PSQT]  += psqt_queen_endgame    [pst_index_white(s)] * (  _phase)    / 100.0;
    
        
        _features[INDEX_QUEEN_MOBILITY] += sqrt(bitCount(attacks & mobilitySquaresWhite));

        
        phase+=3;
        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks, 4);
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_QUEEN];
    while(k){
        Square s = bitscanForward(k);
        attacks = lookUpRookAttack(s, occupied) | lookUpBishopAttack(s, occupied);


        _features[INDEX_QUEEN_PSQT]  -= psqt_queen            [pst_index_black(s)] * (1-_phase)    / 100.0;
        _features[INDEX_QUEEN_PSQT]  -= psqt_queen_endgame    [pst_index_black(s)] * (  _phase)    / 100.0;
        
        
        
        
        _features[INDEX_QUEEN_MOBILITY] -= sqrt(bitCount(attacks & mobilitySquaresBlack));

        
        phase+=3;
        addToKingSafety(attacks, whiteKingZone, whitekingSafety_attackingPiecesCount, whitekingSafety_valueOfAttacks, 4);
        
        k = lsbReset(k);
    }
    _features[INDEX_QUEEN_VALUE] = (bitCount(b->getPieces()[WHITE_QUEEN])  - bitCount(b->getPieces()[BLACK_QUEEN]));
    
    /**********************************************************************************
     *                                  K I N G S                                     *
     **********************************************************************************/
    k = b->getPieces()[WHITE_KING];
    _features[INDEX_KING_PSQT] = 0;
    _features[INDEX_KING_PAWN_SHIELD] = 0;
    _features[INDEX_KING_CLOSE_OPPONENT] = 0;
    
    while(k){
        Square s = bitscanForward(k);
        
        _features[INDEX_KING_PSQT] += psqt_king[63 - s]*(1-_phase) / 100.0;
        _features[INDEX_KING_PSQT] += psqt_king_endgame[63 - s]*_phase / 100.0;
        
        _features[INDEX_KING_PAWN_SHIELD]       += bitCount(KING_ATTACKS[s] & whitePawns);
        _features[INDEX_KING_CLOSE_OPPONENT]    += bitCount(KING_ATTACKS[s] & blackTeam);
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_KING];
    while(k){
        Square s = bitscanForward(k);
        
        _features[INDEX_KING_PSQT] -= psqt_king[s]*(1-_phase) / 100.0;
        _features[INDEX_KING_PSQT] -= psqt_king_endgame[s]*_phase / 100.0;
        
        
        _features[INDEX_KING_PAWN_SHIELD]       -= bitCount(KING_ATTACKS[s] & blackPawns);
        _features[INDEX_KING_CLOSE_OPPONENT]    -= bitCount(KING_ATTACKS[s] & whiteTeam);
        
        k = lsbReset(k);
    }
    
    _features[INDEX_KING_SAFETY] = (_kingSafetyTable[blackkingSafety_valueOfAttacks] - _kingSafetyTable[whitekingSafety_valueOfAttacks]) / 100;
    
    
    
    
    
    
    
    
    for(int i = 0; i < unusedVariable; i++){
        res += _features[i] * (_phase * _pieceValuesLate[i] + (1-_phase) * _pieceValuesEarly[i]);
    }
    
    return res;
}

void printEvaluation(Board *board){
    
    using namespace std;
    
    Evaluator ev{};
    Score s = ev.evaluate(board);
    double phase = ev.getPhase();
    
    
    
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
       << std::setw(20) << phase<< " | "
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
            "INDEX_KING_PAWN_SHIELD"
    };
    
    for(int i = 0; i < unusedVariable; i++){
        
        ss << std::setw(40) << std::left << names[i] << " | "
           << std::setw(20) << std::right << ev.getFeatures()[i] << " | "
           << std::setw(20) << ev.getEarlyGameParams()[i] << " | "
           << std::setw(20) << ev.getLateGameParams()[i] << " | "
           << std::setw(20) << ev.getEarlyGameParams()[i] * (1-phase) + ev.getLateGameParams()[i] * phase<< " | "
           << std::setw(20) << (ev.getEarlyGameParams()[i] * (1-phase) + ev.getLateGameParams()[i] * phase) * ev.getFeatures()[i] << " | \n";
    }
    ss << "-----------------------------------------+----------------------+"
          "----------------------+----------------------+"
          "----------------------+----------------------+\n";
    
    
    ss << std::setw(40) << std::left << "TOTAL" << " | "
       << std::setw(20) << std::right << "" << " | "
       << std::setw(20) << "" << " | "
       << std::setw(20) << "" << " | "
       << std::setw(20) << ""<< " | "
       << std::setw(20) << s << " | \n";
    
    ss << "-----------------------------------------+----------------------+"
          "----------------------+----------------------+"
          "----------------------+----------------------+\n";
    
    std::cout << ss.str() << std::endl;
}

double *Evaluator::getFeatures() {
    return _features;
}

double Evaluator::getPhase() {
    return _phase;
}

double *Evaluator::getEarlyGameParams() {
    return _pieceValuesEarly;
}

double *Evaluator::getLateGameParams() {
    return _pieceValuesLate;
}

int Evaluator::paramCount(){
    return unusedVariable;
}

#ifdef TUNE_PST
double *Evaluator::getTunablePST_MG() {
    return psqt_knight;
}

double *Evaluator::getTunablePST_EG() {
    return psqt_knight_endgame;
}

double *Evaluator::getTunablePST_MG_grad() {
    return tunablePST_MG_grad;
}

double *Evaluator::getTunablePST_EG_grad() {
    return tunablePST_EG_grad;
}
#endif
