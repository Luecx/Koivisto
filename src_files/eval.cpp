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


EvalScore passer_rank_n[16] = {
        M(    0,    0), M(   -9,  -31), M(   -7,  -24), M(    1,    4), M(   10,   33),
        M(   13,   42), M(    8,   29), M(    0,    0), M(    0,    0), M(    1,    2),
        M(  -22,  -73), M(   -9,  -29), M(   -7,  -24), M(  -15,  -51), M(  -17,  -55),
        M(    0,    0),
};

EvalScore psqt_pawn_same_side_castle[64] = {M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    4,   -6), M(   35,   12), M(   17,   33), M(  -10,   46), M(   -7,   14), M(  -13,   20), M(  -20,   21), M(  -17,   29), M(   15,    2), M(   23,    6), M(   15,   19), M(    3,   19), M(   -2,    8), M(   -5,    9), M(  -26,   15), M(  -19,   24), M(    9,   12), M(   16,   19), M(   13,   15), M(   15,   10), M(    7,    0), M(    2,    4), M(  -15,   26), M(  -17,   29), M(   15,   26), M(   30,   33), M(   46,   16), M(   33,   12), M(   20,   -2), M(    3,   22), M(   -5,   39), M(  -10,   50), M(   20,   70), M(   84,   58), M(  120,   37), M(   79,   46), M(   61,   52), M(   53,   83), M(   25,  109), M(   -8,  120), M(   -4,  152), M(  -48,  133), M(  189,   62), M(  192,   85), M(  193,  106), M(  166,  158), M(  175,  170), M(  202,  181), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), 
};
EvalScore psqt_pawn_opposite_side_castle[64] = {M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(   21,   24), M(   37,   15), M(   29,   19), M(   -6,   31), M(  -34,   42), M(  -27,   37), M(  -53,   48), M(  -51,   41), M(   27,   16), M(   34,   11), M(    9,   22), M(  -10,   21), M(  -14,   27), M(  -17,   28), M(  -77,   39), M(  -53,   32), M(   20,   37), M(   15,   35), M(   14,   27), M(   11,   22), M(    9,   17), M(   -4,   24), M(  -41,   37), M(  -49,   35), M(   30,   68), M(   18,   62), M(   21,   49), M(   10,   36), M(   28,   23), M(   29,   17), M(  -15,   39), M(  -36,   34), M(   12,  178), M(   61,  167), M(   24,  145), M(   76,   92), M(  110,   41), M(   91,   35), M(   49,   44), M(   11,   57), M(  158,  252), M(  222,  224), M(  121,  238), M(  180,  164), M(  158,  129), M(  176,   60), M(  -23,  106), M(   15,  114), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), M(    0,    0), 
};
EvalScore psqt_knight_same_side_castle[64] = {};
EvalScore psqt_knight_opposite_side_castle[64] = {};

EvalScore psqt_bishop_same_side_castle[64] = {};
EvalScore psqt_bishop_opposite_side_castle[64] = {};
EvalScore psqt_rook_same_side_castle[64] = {};
EvalScore psqt_rook_opposite_side_castle[64] = {};
EvalScore psqt_queen_same_side_castle[64] = {};
EvalScore psqt_queen_opposite_side_castle[64] = {};


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

EvalScore* psqt[11]{
        psqt_pawn_same_side_castle, psqt_pawn_opposite_side_castle, psqt_knight_same_side_castle, psqt_knight_opposite_side_castle, psqt_bishop_same_side_castle, psqt_bishop_opposite_side_castle, psqt_rook_same_side_castle, psqt_rook_opposite_side_castle, psqt_queen_same_side_castle, psqt_queen_opposite_side_castle, psqt_king_n
};


EvalScore pieceScores[6] = {
        M(   90,  104),
        M(  463,  326),
        M(  474,  288),
        M(  577,  594),
        M( 1359, 1121),
        M(    0,    0),
};

EvalScore SIDE_TO_MOVE               = M(   24,   26);
EvalScore PAWN_STRUCTURE             = M(    7,   12);
EvalScore PAWN_PASSED                = M(    4,   42);
EvalScore PAWN_ISOLATED              = M(   -9,   -8);
EvalScore PAWN_DOUBLED               = M(    8,   -4);
EvalScore PAWN_DOUBLED_AND_ISOLATED  = M(    0,  -17);
EvalScore PAWN_BACKWARD              = M(  -13,    2);
EvalScore PAWN_OPEN                  = M(   15,   -7);
EvalScore PAWN_BLOCKED               = M(   -9,  -12);
EvalScore KNIGHT_OUTPOST             = M(   23,   15);
EvalScore KNIGHT_DISTANCE_ENEMY_KING = M(   -6,    1);
EvalScore ROOK_OPEN_FILE             = M(   58,  -22);
EvalScore ROOK_HALF_OPEN_FILE        = M(    4,   -2);
EvalScore ROOK_KING_LINE             = M(   36,   32);
EvalScore BISHOP_DOUBLED             = M(   42,   55);
EvalScore BISHOP_PAWN_SAME_SQUARE    = M(   -5,    5);
EvalScore BISHOP_FIANCHETTO          = M(    5,    7);
EvalScore QUEEN_DISTANCE_ENEMY_KING  = M(    6,  -25);
EvalScore KING_CLOSE_OPPONENT        = M(  -37,   57);
EvalScore KING_PAWN_SHIELD           = M(   13,    3);
EvalScore CASTLING_RIGHTS            = M(13, -4);

EvalScore fast_pawn_psqt[2][2][2][64];
EvalScore fast_knight_psqt[2][2][2][64];
EvalScore fast_bishop_psqt[2][2][2][64];
EvalScore fast_rook_psqt[2][2][2][64];
EvalScore fast_queen_psqt[2][2][2][64];
EvalScore fast_king_psqt[2][64];

EvalScore* evfeatures[]{
        &SIDE_TO_MOVE,
        &PAWN_STRUCTURE,
        &PAWN_PASSED,
        &PAWN_ISOLATED,
        &PAWN_DOUBLED,
        &PAWN_DOUBLED_AND_ISOLATED,
        &PAWN_BACKWARD,
        &PAWN_OPEN,
        &PAWN_BLOCKED,
        
        &KNIGHT_OUTPOST,
        &KNIGHT_DISTANCE_ENEMY_KING,
        
        &ROOK_OPEN_FILE,
        &ROOK_HALF_OPEN_FILE,
        &ROOK_KING_LINE,
        
        &BISHOP_DOUBLED,
        &BISHOP_PAWN_SAME_SQUARE,
        &BISHOP_FIANCHETTO,
        
        &QUEEN_DISTANCE_ENEMY_KING,
        
        &KING_CLOSE_OPPONENT,
        &KING_PAWN_SHIELD,
        
        &CASTLING_RIGHTS,
};

EvalScore hangingEval[5]{
        M(   -7,    2),
        M(  -19,   -5),
        M(  -11,  -10),
        M(   -9,   -8),
        M(  -10,  -12),
};

EvalScore pinnedEval[15]{
        M(   20,  -40),
        M(  -20,   12),
        M(    2,   27),
        M(  -36, -142),
        M(  -48,  -58),
        M(  -34,    1),
        M(  -13,  -10),
        M(  -72,  -72),
        M(   15,  -83),
        M(  -68, -395),
        M(  -26,   -3),
        M(  -13,  -73),
        M( -176, -522),
        M( -336, -639),
        M(  -17,   -9),
};

EvalScore mobilityKnight[9]{
        M(    0,    0), M(   27,   20), M(   39,   28), M(   47,   35), M(   54,   40),
        M(   61,   45), M(   67,   49), M(   72,   53), M(   77,   57),
};
EvalScore mobilityBishop[15]{
        M(    0,    0), M(   25,   37), M(   35,   52), M(   43,   63), M(   49,   73),
        M(   55,   82), M(   60,   89), M(   65,   97), M(   69,  103), M(   74,  110),
        M(   78,  115), M(   81,  121), M(   85,  127), M(   89,  132), M(   92,  137),
};
EvalScore mobilityRook[15]{
        M(    0,    0), M(   23,   32), M(   32,   46), M(   39,   56), M(   46,   65),
        M(   51,   73), M(   56,   79), M(   60,   86), M(   64,   92), M(   68,   97),
        M(   72,  103), M(   75,  108), M(   79,  112), M(   82,  117), M(   85,  121),
};
EvalScore mobilityQueen[29]{
        M(    0,    0), M(    9,   61), M(   12,   86), M(   15,  106), M(   18,  122),
        M(   20,  136), M(   21,  149), M(   23,  161), M(   25,  172), M(   26,  183),
        M(   28,  193), M(   29,  202), M(   30,  211), M(   32,  220), M(   33,  228),
        M(   34,  236), M(   35,  244), M(   36,  251), M(   37,  258), M(   38,  266),
        M(   39,  272), M(   40,  279), M(   41,  286), M(   42,  292), M(   43,  298),
        M(   44,  305), M(   45,  311), M(   46,  317), M(   46,  322),
};


int mobEntryCount[6]{
    0,9,15,15,29,0
};


EvalScore* mobilities[6]{
    nullptr, mobilityKnight, mobilityBishop, mobilityRook, mobilityQueen, nullptr
};

//float* _pieceValuesEarly = new float[6]{
//
//        371.82693,5.2139874
//};
//
//
//float* _pieceValuesLate = new float[6]{
//
//       -144.92081,17.488422
//};

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
            fast_pawn_psqt[WHITE][kside][0][i] = psqt_pawn_same_side_castle[squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))] + pieceScores[PAWN];
            fast_pawn_psqt[BLACK][kside][0][i] = psqt_pawn_same_side_castle[squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))]+ pieceScores[PAWN];
            fast_pawn_psqt[WHITE][kside][1][i] = psqt_pawn_opposite_side_castle[squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))] + pieceScores[PAWN];
            fast_pawn_psqt[BLACK][kside][1][i] = psqt_pawn_opposite_side_castle[squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))]+ pieceScores[PAWN];
        }
        for (int kside = 0; kside < 2; kside++) {
            fast_knight_psqt[WHITE][kside][0][i] = psqt_knight_same_side_castle[squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))] + pieceScores[KNIGHT];
            fast_knight_psqt[BLACK][kside][0][i] = psqt_knight_same_side_castle[squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))]+ pieceScores[KNIGHT];
            fast_knight_psqt[WHITE][kside][1][i] = psqt_knight_opposite_side_castle[squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))] + pieceScores[KNIGHT];
            fast_knight_psqt[BLACK][kside][1][i] = psqt_knight_opposite_side_castle[squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))]+ pieceScores[KNIGHT];
        }
        for (int kside = 0; kside < 2; kside++) {
            fast_bishop_psqt[WHITE][kside][0][i] = psqt_bishop_same_side_castle[squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))] + pieceScores[BISHOP];
            fast_bishop_psqt[BLACK][kside][0][i] = psqt_bishop_same_side_castle[squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))]+ pieceScores[BISHOP];
            fast_bishop_psqt[WHITE][kside][1][i] = psqt_bishop_opposite_side_castle[squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))] + pieceScores[BISHOP];
            fast_bishop_psqt[BLACK][kside][1][i] = psqt_bishop_opposite_side_castle[squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))]+ pieceScores[BISHOP];
        }
        for (int kside = 0; kside < 2; kside++) {
            fast_rook_psqt[WHITE][kside][0][i] = psqt_rook_same_side_castle[squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))] + pieceScores[ROOK];
            fast_rook_psqt[BLACK][kside][0][i] = psqt_rook_same_side_castle[squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))]+ pieceScores[ROOK];
            fast_rook_psqt[WHITE][kside][1][i] = psqt_rook_opposite_side_castle[squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))] + pieceScores[ROOK];
            fast_rook_psqt[BLACK][kside][1][i] = psqt_rook_opposite_side_castle[squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))]+ pieceScores[ROOK];
        }
        for (int kside = 0; kside < 2; kside++) {
            fast_queen_psqt[WHITE][kside][0][i] = psqt_queen_same_side_castle[squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))] + pieceScores[QUEEN];
            fast_queen_psqt[BLACK][kside][0][i] = psqt_queen_same_side_castle[squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))]+ pieceScores[QUEEN];
            fast_queen_psqt[WHITE][kside][1][i] = psqt_queen_opposite_side_castle[squareIndex(rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))] + pieceScores[QUEEN];
            fast_queen_psqt[BLACK][kside][1][i] = psqt_queen_opposite_side_castle[squareIndex(7 - rankIndex(i), (kside ? fileIndex(i) : 7 - fileIndex(i)))]+ pieceScores[QUEEN];
        }

        fast_king_psqt[WHITE][i] = psqt_king_n[pst_index_white_s(i)] + pieceScores[KING];
        fast_king_psqt[BLACK][i] = psqt_king_n[pst_index_black_s(i)] + pieceScores[KING];
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
    EvalScore mobScore = M(0,0);
    
    while (k) {
        square = bitscanForward(k);
        
        evalScore += fast_pawn_psqt[WHITE][wKSide][wKSide!=bKSide][square];
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_PAWN];
    while (k) {
        square = bitscanForward(k);

        evalScore -= fast_pawn_psqt[BLACK][bKSide][wKSide!=bKSide][square];
        
        
        k = lsbReset(k);
    }
    k = whitePassers;
    while(k){
        square = bitscanForward(k);
        featureScore += passer_rank_n[getBit(whiteBlockedPawns,square)*8+rankIndex(square)];
//        features[INDEX_PASSER_RANK] += passer_rank[getBit(whiteBlockedPawns,square)*8+rankIndex(square)]/10;
        k = lsbReset(k);
    }
    k=blackPassers;
    while (k){
        square = bitscanForward(k);
        featureScore -= passer_rank_n[getBit(blackBlockedPawns,square)*8+7-rankIndex(square)];
//        features[INDEX_PASSER_RANK] -= passer_rank[getBit(blackBlockedPawns,square)*8+7-rankIndex(square)]/10;
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
        
        evalScore += fast_knight_psqt[WHITE][wKSide][wKSide!=bKSide][square];
    
    
        mobScore += mobilityKnight[bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresWhite)];

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
        
        evalScore -= fast_knight_psqt[BLACK][bKSide][wKSide!=bKSide][square];
    
        mobScore -= mobilityKnight[bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresBlack)];

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
        
        
        evalScore += fast_bishop_psqt[WHITE][wKSide][wKSide!=bKSide][square];
    
        mobScore += mobilityBishop[bitCount(attacks & mobilitySquaresWhite)];
        
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
        
        evalScore -= fast_bishop_psqt[BLACK][bKSide][wKSide!=bKSide][square];
    
        mobScore -= mobilityBishop[bitCount(attacks & mobilitySquaresBlack)];
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
        
        evalScore += fast_rook_psqt[WHITE][wKSide][wKSide!=bKSide][square];
    
        mobScore += mobilityRook[bitCount(attacks & mobilitySquaresWhite)];
        
        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks,3);
        
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied);
        
        evalScore -= fast_rook_psqt[BLACK][bKSide][wKSide!=bKSide][square];
    
        mobScore -= mobilityRook[bitCount(attacks & mobilitySquaresBlack)];
        
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
        
        evalScore += fast_queen_psqt[WHITE][wKSide][wKSide!=bKSide][square];
    
        mobScore += mobilityQueen[bitCount(attacks & mobilitySquaresWhite)];
        featureScore += QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, blackKingSquare);
        
        addToKingSafety(attacks, blackKingZone, blackkingSafety_attackingPiecesCount, blackkingSafety_valueOfAttacks,
                        4);
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied) | lookUpBishopAttack(square, occupied);
        
        evalScore -= fast_queen_psqt[BLACK][bKSide][wKSide!=bKSide][square];
    
    
        mobScore -= mobilityQueen[bitCount(attacks & mobilitySquaresBlack)];
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
        
        evalScore += fast_king_psqt[WHITE_KING][square];
        
        featureScore += KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & whitePawns);
        featureScore += KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & blackTeam);
        
        
        k = lsbReset(k);
    }
    
    k = b->getPieces()[BLACK_KING];
    while (k) {
        square = bitscanForward(k);
        
        evalScore -= fast_king_psqt[BLACK_KING][square];
        
        featureScore -= KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & blackPawns);
        featureScore -= KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & whiteTeam);
        
        k = lsbReset(k);
    }
    
    EvalScore hangingEval = computeHangingPieces(b);
    EvalScore pinnedEval = computePinnedPieces(b);
    
    
    float kingSafetyTableScore = (kingSafetyTable[blackkingSafety_valueOfAttacks] - kingSafetyTable[whitekingSafety_valueOfAttacks]);
    float earlySum = kingSafetyTableScore * 3.7182693;
    float endSum = kingSafetyTableScore * -1.4492081;
  
//    features[INDEX_KING_SAFETY] =
//            (kingSafetyTable[blackkingSafety_valueOfAttacks] - kingSafetyTable[whitekingSafety_valueOfAttacks]) / 100;
    featureScore += CASTLING_RIGHTS*(
            + b->getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
            + b->getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING));
    featureScore += SIDE_TO_MOVE * (b->getActivePlayer() == WHITE ? 1:-1);
    
//    for(int i = 0; i < 6; i++){
//        earlySum += features[i] * _pieceValuesEarly[i];
//        endSum += features[i] * _pieceValuesLate[i];
//    }
    
    EvalScore totalScore = evalScore + pinnedEval + hangingEval + featureScore + mobScore;
    
    res = earlySum * (1 - phase) + endSum * (phase);
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

float* Evaluator::getFeatures() { return nullptr; }

float Evaluator::getPhase() { return phase; }

float* Evaluator::getEarlyGameParams() { return nullptr; }

float* Evaluator::getLateGameParams() { return nullptr; }

int Evaluator::paramCount() { return unusedVariable; }

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
float* Evaluator::getPhaseValues() { return passer_rank; }
#ifdef TUNE_PST
float* Evaluator::getTunablePST_MG() { return psqt_bishop; }

float* Evaluator::getTunablePST_EG() { return psqt_bishop_endgame; }

float* Evaluator::getTunablePST_MG_grad() { return tunablePST_MG_grad; }

float* Evaluator::getTunablePST_EG_grad() { return tunablePST_EG_grad; }
#endif
