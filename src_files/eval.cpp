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


double* _pieceValuesEarly = new double[5]{
        106.497,
        273.137,
        293.337,
        522.083,
        967.347};
double* _pieceValuesLate = new double[5]{
        75.0143,
        357.231,
        377.351,
        452.35,
        941.283};
double* _features          = new double[5];

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

    k = b->getPieces()[WHITE_PAWN];
    while(k){
        res += psqt_pawn[63 - bitscanForward(k)];
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_PAWN];
    while(k){
        res -= psqt_pawn[bitscanForward(k)];
        k = lsbReset(k);
    }
    
    k = b->getPieces()[WHITE_KNIGHT];
    while(k){
        res += psqt_knight[63 - bitscanForward(k)];
        k = lsbReset(k);
        phase++;
    }
    
    k = b->getPieces()[BLACK_KNIGHT];
    while(k){
        res -= psqt_knight[bitscanForward(k)];
        k = lsbReset(k);
        phase++;
    }
    
    k = b->getPieces()[WHITE_BISHOP];
    while(k){
        res += psqt_bishop[63 - bitscanForward(k)];
        k = lsbReset(k);
        phase++;
    }
    
    k = b->getPieces()[BLACK_BISHOP];
    while(k){
        res -= psqt_bishop[bitscanForward(k)];
        k = lsbReset(k);
        phase++;
    }
    
    k = b->getPieces()[WHITE_ROOK];
    while(k){
        res += psqt_rook[63 - bitscanForward(k)];
        k = lsbReset(k);
        phase++;
    }
    
    k = b->getPieces()[BLACK_ROOK];
    while(k){
        res -= psqt_rook[bitscanForward(k)];
        k = lsbReset(k);
        phase++;
    }
    
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
    
    
    _features[0] = (bitCount(b->getPieces()[WHITE_PAWN])   - bitCount(b->getPieces()[BLACK_PAWN]));
    _features[1] = (bitCount(b->getPieces()[WHITE_KNIGHT]) - bitCount(b->getPieces()[BLACK_KNIGHT]));
    _features[2] = (bitCount(b->getPieces()[WHITE_BISHOP]) - bitCount(b->getPieces()[BLACK_BISHOP]));
    _features[3] = (bitCount(b->getPieces()[WHITE_ROOK])   - bitCount(b->getPieces()[BLACK_ROOK]));
    _features[4] = (bitCount(b->getPieces()[WHITE_QUEEN])  - bitCount(b->getPieces()[BLACK_QUEEN]));

    _phase = (double)(18 - phase) / 18;


    for(int i = 0; i < 5; i++){
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
    return 5;
}
