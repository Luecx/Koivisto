//
// Created by finne on 5/31/2020.
//

#include "eval.h"


Score psqt_pawn[] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
};

Score psqt_knight[] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50,
};

Score psqt_bishop[] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20,
};


Score psqt_rook[] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  5,  5,  0,  0,  0
};


Score psqt_queen[] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  5,  5,  5,  0, -5,
        0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
};

Score psqt_king[] = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        20, 20,  0,  0,  0,  0, 20, 20,
        20, 30, 10,  0,  0, 10, 30, 20
};
Score psqt_king_endgame[] = {
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
int INDEX_PAWN_STRUCTURE = unusedVariable++;
int INDEX_PAWN_PASSED = unusedVariable++;
int INDEX_PAWN_ISOLATED = unusedVariable++;

int INDEX_KNIGHT_VALUE = unusedVariable++;
int INDEX_KNIGHT_MOBILITY = unusedVariable++;

int INDEX_BISHOP_VALUE = unusedVariable++;
int INDEX_BISHOP_MOBILITY = unusedVariable++;
int INDEX_BISHOP_DOUBLED = unusedVariable++;

int INDEX_ROOK_VALUE = unusedVariable++;
int INDEX_ROOK_MOBILITY = unusedVariable++;

int INDEX_QUEEN_VALUE = unusedVariable++;


double* _pieceValuesEarly = new double[unusedVariable]{
         103.34,       2.73169,       47.4446,      -13.3571,       314.486,       28.0681,
        312.048,       30.8024,        56.437,       586.638,       26.4492,       1153.09
};
double* _pieceValuesLate = new double[unusedVariable]{
         79.626,       5.04308,      -10.5165,      -19.3154,       324.666,      -3.50834,
         300.89,       13.6052,       28.7572,        421.83,       12.2131,       1061.02
};
double* _features          = new double[unusedVariable];

double  _phase;

/**
 * evaluates the board.
 * @param b
 * @return
 */
bb::Score Evaluator::evaluate(Board *b) {
    
    Score res = 0;
    
    U64 k;
    int phase = 0;
    
    U64 whiteTeam = b->getTeamOccupied()[WHITE];
    U64 blackTeam = b->getTeamOccupied()[BLACK];
    U64 occupied = *b->getOccupied();
    
    /**********************************************************************************
     *                                  P A W N S                                     *
     **********************************************************************************/
    
    
    U64 whitePawns = b->getPieces()[WHITE_PAWN];
    U64 blackPawns = b->getPieces()[BLACK_PAWN];
    
    k = whitePawns;
    _features[INDEX_PAWN_PASSED] = 0;
    _features[INDEX_PAWN_ISOLATED] = 0;
    while(k){
        Square s = bitscanForward(k);
        
        //isolated pawns
        if((FILES_NEIGHBOUR[fileIndex(s)] & whitePawns) == 0){
            _features[INDEX_PAWN_ISOLATED] += 1;
        }
        
        //passed pawn
        if((whitePassedPawnMask[s] & blackPawns) == 0){
            _features[INDEX_PAWN_PASSED] += 1;
        }
        
        res += psqt_pawn[63 - s];
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_PAWN];
    while(k){
        Square s = bitscanForward(k);
    
        //isolated pawns
        if((FILES_NEIGHBOUR[fileIndex(s)] & blackPawns) == 0){
            _features[INDEX_PAWN_ISOLATED] -= 1;
        }
        //passed pawn
        if((blackPassedPawnMask[s] & whitePawns) == 0){
            _features[INDEX_PAWN_PASSED] -= 1;
        }
        
        res -= psqt_pawn[s];
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
    
    /**********************************************************************************
     *                                  K N I G H T S                                 *
     **********************************************************************************/
    
    _features[INDEX_KNIGHT_MOBILITY] = 0;
    
    k = b->getPieces()[WHITE_KNIGHT];
    while(k){
        Square s = bitscanForward(k);
        
        res += psqt_knight[63 - s];
    
        _features[INDEX_KNIGHT_MOBILITY] += sqrt(bitCount(KNIGHT_ATTACKS[s] & ~whiteTeam));
        
        k = lsbReset(k);
        phase++;
    }
    
    k = b->getPieces()[BLACK_KNIGHT];
    while(k){
    
        Square s = bitscanForward(k);
        
        res -= psqt_knight[s];
    
        _features[INDEX_KNIGHT_MOBILITY] -= sqrt(bitCount(KNIGHT_ATTACKS[s] & ~blackTeam));
        
        k = lsbReset(k);
        phase++;
    }
    _features[INDEX_KNIGHT_VALUE] = (bitCount(b->getPieces()[WHITE_KNIGHT]) - bitCount(b->getPieces()[BLACK_KNIGHT]));
    /**********************************************************************************
     *                                  B I S H O P S                                 *
     **********************************************************************************/
    _features[INDEX_BISHOP_MOBILITY] = 0;
    k = b->getPieces()[WHITE_BISHOP];
    while(k){
        Square s = bitscanForward(k);
        res += psqt_bishop[63 -s ];
        
        _features[INDEX_BISHOP_MOBILITY] += sqrt(bitCount(lookUpBishopAttack(s, occupied) & ~blackTeam));
        k = lsbReset(k);
        phase++;
    }
    
    k = b->getPieces()[BLACK_BISHOP];
    while(k){
        Square s = bitscanForward(k);
        res -= psqt_bishop[s];
        
        _features[INDEX_BISHOP_MOBILITY] -= sqrt(bitCount(lookUpBishopAttack(s, occupied) & ~blackTeam));
    
        k = lsbReset(k);
        phase++;
    }
    _features[INDEX_BISHOP_VALUE] = (bitCount(b->getPieces()[WHITE_BISHOP]) - bitCount(b->getPieces()[BLACK_BISHOP]));
    _features[INDEX_BISHOP_DOUBLED] = (bitCount(b->getPieces()[WHITE_BISHOP])==2) - (bitCount(b->getPieces()[BLACK_BISHOP])==2);
    /**********************************************************************************
     *                                  R O O K S                                     *
     **********************************************************************************/
    _features[INDEX_ROOK_MOBILITY] = 0;
    k = b->getPieces()[WHITE_ROOK];
    while(k){
        Square s = bitscanForward(k);
        
        
        res += psqt_rook[63 - s];
    
        _features[INDEX_ROOK_MOBILITY] += sqrt(bitCount(lookUpRookAttack(s, occupied) & ~whiteTeam));
        
        k = lsbReset(k);
        phase++;
    }
    
    k = b->getPieces()[BLACK_ROOK];
    while(k){
        Square s = bitscanForward(k);
        
        
        res -= psqt_rook[s];
        _features[INDEX_ROOK_MOBILITY] -= sqrt(bitCount(lookUpRookAttack(s, occupied) & ~blackTeam));
        
        
        k = lsbReset(k);
        phase++;
    }
    _features[INDEX_ROOK_VALUE] = (bitCount(b->getPieces()[WHITE_ROOK])   - bitCount(b->getPieces()[BLACK_ROOK]));
    /**********************************************************************************
     *                                  Q U E E N S                                   *
     **********************************************************************************/
    k = b->getPieces()[WHITE_QUEEN];
    while(k){
        res += psqt_queen[63 - bitscanForward(k)];
        k = lsbReset(k);
        phase+=3;
    }
    
    k = b->getPieces()[BLACK_QUEEN];
    while(k){
        res -= psqt_queen[bitscanForward(k)];
        k = lsbReset(k);
        phase+=3;
    }
    _features[INDEX_QUEEN_VALUE] = (bitCount(b->getPieces()[WHITE_QUEEN])  - bitCount(b->getPieces()[BLACK_QUEEN]));
    
    /**********************************************************************************
     *                                  K I N G S                                     *
     **********************************************************************************/
    k = b->getPieces()[WHITE_KING];
    while(k){
        res += psqt_king[63 - bitscanForward(k)]*phase/18;
        res += psqt_king_endgame[63 - bitscanForward(k)]*(18-phase)/18;
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_KING];
    while(k){
        res -= psqt_king[bitscanForward(k)]*phase/18;
        res -= psqt_king_endgame[bitscanForward(k)]*(18-phase)/18;
        k = lsbReset(k);
    }
    
    

    _phase = (double)(18 - phase) / 18;


    for(int i = 0; i < unusedVariable; i++){
        res += _features[i] * (_phase * _pieceValuesEarly[i] + (1-_phase) * _pieceValuesLate[i]);
    }
    
//    res += (bitCount(b->getPieces()[WHITE_BISHOP]) - bitCount(b->getPieces()[BLACK_BISHOP])) * 325;
//    res += (bitCount(b->getPieces()[WHITE_KNIGHT]) - bitCount(b->getPieces()[BLACK_KNIGHT])) * 310;
//    res += (bitCount(b->getPieces()[WHITE_ROOK])   - bitCount(b->getPieces()[BLACK_ROOK]))   * 500;
//    res += (bitCount(b->getPieces()[WHITE_QUEEN])  - bitCount(b->getPieces()[BLACK_QUEEN]))  * 920;
//    res += (bitCount(b->getPieces()[WHITE_PAWN])   - bitCount(b->getPieces()[BLACK_PAWN]))   * 100;
    
    return res;
    

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
