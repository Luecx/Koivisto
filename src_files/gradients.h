
#include "eval.h"

#ifdef TUNE

#define CHANGE(midGrad, endGrad, lr) M(midGrad > 0 ? -lr : midGrad < 0 ? lr : 0, endGrad > 0 ? -lr : endGrad < 0 ? lr : 0)

namespace tuning {

// phase - psqt_index - square
float psqt_gradients[2][11][64] {};
// phase - piece - seen squares (28 at max for queens)
float mob_gradients[2][6][28] {};
// phase - piece
float hanging_gradients[2][5] {};
// phase - index
float pinned_gradients[2][15] {};
// phase - index
float feature_gradients[2][23] {};
// phase - index
float passer_gradients[2][16] {};
// phase - index
float kingSafety_gradients[2][100] {};

void collectGradients_psqt(Board* board, float evalGrad, float phase) {
    // target: incrementing psqt_gradients for the given board using the evaluation gradient
    // which is the same as the derivative of the evaluation output with respect to the loss

    bool wKSide = (fileIndex(bitscanForward(board->getPieces()[WHITE_KING])) > 3 ? 0 : 1);
    bool bKSide = (fileIndex(bitscanForward(board->getPieces()[BLACK_KING])) > 3 ? 0 : 1);

    bool opposingSide = wKSide != bKSide;

    // do pawns - queens first (kings are handled later)
    for (Piece p = PAWN; p <= QUEEN; p++) {

        // this has to match the field: psqt
        int psqt_index = p * 2 + opposingSide;

        // do it for both colors
        for (Color c = WHITE; c <= BLACK; c++) {

            U64 k = board->getPieces(c, p);
            while (k) {
                Square s = bitscanForward(k);

                // computing relative ranks/files
                Rank relativeRank = c == WHITE ? rankIndex(s) : 7 - rankIndex(s);
                File relativeFile = c == WHITE ? (wKSide ? fileIndex(s) : 7 - fileIndex(s))
                                               : (bKSide ? fileIndex(s) : 7 - fileIndex(s));

                psqt_gradients[0][psqt_index][squareIndex(relativeRank, relativeFile)] +=
                    (c == WHITE ? (1 - phase) : -(1 - phase)) * evalGrad;
                psqt_gradients[1][psqt_index][squareIndex(relativeRank, relativeFile)] +=
                    (c == WHITE ? (phase) : -(phase)) * evalGrad;

                k = lsbReset(k);
            }
        }
    }

    // doing the king psqt (note that they are independent of king relations)
    // do it for both colors
    for (Color c = WHITE; c <= BLACK; c++) {

        // the the piece occupancy of the king(s)
        U64 k = board->getPieces(c, KING);
        while (k) {
            Square s = bitscanForward(k);
    
            if(c == WHITE){
                psqt_gradients[0][10][pst_index_white_s(s)] += (1 - phase) * evalGrad;
                psqt_gradients[1][10][pst_index_white_s(s)] += phase * evalGrad;
            }else{
                psqt_gradients[0][10][pst_index_black_s(s)] += -(1 - phase) * evalGrad;
                psqt_gradients[1][10][pst_index_black_s(s)] += -phase * evalGrad;
            }
            
            k = lsbReset(k);
        }
    }
}
void collectGradients_mobility(Board* board, float evalGrad, float phase) {
    U64 whiteTeam      = board->getTeamOccupied()[WHITE];
    U64 blackTeam      = board->getTeamOccupied()[BLACK];
    U64 whitePawns     = board->getPieces()[WHITE_PAWN];
    U64 blackPawns     = board->getPieces()[BLACK_PAWN];
    U64 whitePawnCover = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
    U64 blackPawnCover = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);

    U64 mobilitySquaresWhite = ~whiteTeam & ~(blackPawnCover);
    U64 mobilitySquaresBlack = ~blackTeam & ~(whitePawnCover);

    U64 occupied = *board->getOccupied();

    for (Piece p = KNIGHT; p <= QUEEN; p++) {

        U64 w = board->getPieces(WHITE, p);
        U64 b = board->getPieces(BLACK, p);

        while (w) {
            Square s = bitscanForward(w);

            U64 attacks = ZERO;
            switch (p) {
                case KNIGHT: attacks = KNIGHT_ATTACKS[s]; break;
                case BISHOP: attacks = lookUpBishopAttack(s, occupied); break;
                case ROOK: attacks = lookUpRookAttack(s, occupied); break;
                case QUEEN: attacks = lookUpBishopAttack(s, occupied) | lookUpRookAttack(s, occupied); break;
            }
            attacks &= mobilitySquaresWhite;

            mob_gradients[0][p][bitCount(attacks)] += evalGrad * (1 - phase);
            mob_gradients[1][p][bitCount(attacks)] += evalGrad * (phase);

            w = lsbReset(w);
        }
        while (b) {
            Square s = bitscanForward(b);

            U64 attacks = ZERO;
            switch (p) {
                case KNIGHT: attacks = KNIGHT_ATTACKS[s]; break;
                case BISHOP: attacks = lookUpBishopAttack(s, occupied); break;
                case ROOK: attacks = lookUpRookAttack(s, occupied); break;
                case QUEEN: attacks = lookUpBishopAttack(s, occupied) | lookUpRookAttack(s, occupied); break;
            }
            attacks &= mobilitySquaresBlack;

            mob_gradients[0][p][bitCount(attacks)] -= evalGrad * (1 - phase);
            mob_gradients[1][p][bitCount(attacks)] -= evalGrad * (phase);

            b = lsbReset(b);
        }
    }
}
void collectGradients_pinners(Board* board, float evalGrad, float phase) {
    Square square;
    Square wkingSq = bitscanForward(board->getPieces(WHITE, KING));
    Square bkingSq = bitscanForward(board->getPieces(BLACK, KING));
    U64    pinner  = lookUpRookXRayAttack(wkingSq, *board->getOccupied(), board->getTeamOccupied()[WHITE])
                 & (board->getPieces(BLACK, ROOK) | board->getPieces(BLACK, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[wkingSq][square] & board->getTeamOccupied()[WHITE]);
        int    index       = 3 * (board->getPiece(pinnedPlace) % 6) + (board->getPiece(square) % 6 - BISHOP);
        pinned_gradients[0][index] += evalGrad * (1 - phase);
        pinned_gradients[1][index] += evalGrad * (phase);
        pinner = lsbReset(pinner);
    }
    pinner = lookUpBishopXRayAttack(wkingSq, *board->getOccupied(), board->getTeamOccupied()[WHITE])
             & (board->getPieces(BLACK, BISHOP) | board->getPieces(BLACK, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[wkingSq][square] & board->getTeamOccupied()[WHITE]);
        int    index       = 3 * (board->getPiece(pinnedPlace) % 6) + (board->getPiece(square) % 6 - BISHOP);
        pinned_gradients[0][index] += evalGrad * (1 - phase);
        pinned_gradients[1][index] += evalGrad * (phase);
        pinner = lsbReset(pinner);
    }
    pinner = lookUpRookXRayAttack(bkingSq, *board->getOccupied(), board->getTeamOccupied()[BLACK])
             & (board->getPieces(WHITE, ROOK) | board->getPieces(WHITE, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[bkingSq][square] & board->getTeamOccupied()[BLACK]);
        int    index       = 3 * (board->getPiece(pinnedPlace) % 6) + (board->getPiece(square) % 6 - BISHOP);
        pinned_gradients[0][index] -= evalGrad * (1 - phase);
        pinned_gradients[1][index] -= evalGrad * (phase);
        pinner = lsbReset(pinner);
    }
    pinner = lookUpBishopXRayAttack(bkingSq, *board->getOccupied(), board->getTeamOccupied()[BLACK])
             & (board->getPieces(WHITE, BISHOP) | board->getPieces(WHITE, QUEEN));
    while (pinner) {
        square             = bitscanForward(pinner);
        Square pinnedPlace = bitscanForward(inBetweenSquares[bkingSq][square] & board->getTeamOccupied()[BLACK]);
        int    index       = 3 * (board->getPiece(pinnedPlace) % 6) + (board->getPiece(square) % 6 - BISHOP);
        pinned_gradients[0][index] -= evalGrad * (1 - phase);
        pinned_gradients[1][index] -= evalGrad * (phase);
        pinner = lsbReset(pinner);
    }
}
void collectGradients_hanging(Board* board, float evalGrad, float phase) {
    U64 WnotAttacked = ~board->getAttackedSquares(WHITE);
    U64 BnotAttacked = ~board->getAttackedSquares(BLACK);

    EvalScore res = M(0, 0);

    for (int i = PAWN; i <= QUEEN; i++) {

        int whiteHanging = bitCount(board->getPieces(WHITE, i) & WnotAttacked);
        int blackHanging = bitCount(board->getPieces(BLACK, i) & BnotAttacked);

        hanging_gradients[0][i] += evalGrad * (whiteHanging - blackHanging) * (1 - phase);
        hanging_gradients[0][i] += evalGrad * (whiteHanging - blackHanging) * (phase);
    }
}
void collectGradients_features(Board* board, float evalGrad, float phase) {
    
    float evalGradMid = evalGrad * (1 - phase);
    float evalGradEnd = evalGrad * (    phase);
    
    
    
    U64 whiteTeam = board->getTeamOccupied()[WHITE];
    U64 blackTeam = board->getTeamOccupied()[BLACK];
    U64 whitePawns = board->getPieces()[WHITE_PAWN];
    U64 blackPawns = board->getPieces()[BLACK_PAWN];
    
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
    U64 occupied       = *board->getOccupied();
    
    U64 whitePawnEastCover = shiftNorthEast(whitePawns) & whitePawns;
    U64 whitePawnWestCover = shiftNorthWest(whitePawns) & whitePawns;
    U64 blackPawnEastCover = shiftSouthEast(blackPawns) & blackPawns;
    U64 blackPawnWestCover = shiftSouthWest(blackPawns) & blackPawns;
    
    U64 whitePawnCover = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
    U64 blackPawnCover = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);
    
    float pawn_doubled_and_isolated_grad = (
        + bitCount(whiteIsolatedPawns & whiteDoubledPawns)
        - bitCount(blackIsolatedPawns & blackDoubledPawns));
    float pawn_doubled_grad = (
        + bitCount(~whiteIsolatedPawns & whiteDoubledPawns)
        - bitCount(~blackIsolatedPawns & blackDoubledPawns));
    float pawn_isolated_grad = (
        + bitCount(whiteIsolatedPawns & ~whiteDoubledPawns)
        - bitCount(blackIsolatedPawns & ~blackDoubledPawns));
    float pawn_passed_grad = (
        + bitCount(whitePassers)
        - bitCount(blackPassers));
    float pawn_structure_grad = (
        + bitCount(whitePawnEastCover)
        + bitCount(whitePawnWestCover)
        - bitCount(blackPawnEastCover)
        - bitCount(blackPawnWestCover));
    float pawn_open_grad = (
        + bitCount(whitePawns & ~fillSouth(blackPawns))
        - bitCount(blackPawns & ~fillNorth(whitePawns)));
    float pawn_backward_grad = (
        + bitCount(fillSouth(~wAttackFrontSpans(whitePawns) & blackPawnCover) & whitePawns)
        - bitCount(fillNorth(~bAttackFrontSpans(blackPawns) & whitePawnCover) & blackPawns));
    float pawn_blocked_grad = (
        + bitCount(whiteBlockedPawns)
        - bitCount(blackBlockedPawns));
    
    feature_gradients[0][5] += pawn_doubled_and_isolated_grad * evalGradMid;
    feature_gradients[1][5] += pawn_doubled_and_isolated_grad * evalGradEnd;
    
    feature_gradients[0][4] += pawn_doubled_grad * evalGradMid;
    feature_gradients[1][4] += pawn_doubled_grad * evalGradEnd;
    
    feature_gradients[0][3] += pawn_isolated_grad * evalGradMid;
    feature_gradients[1][3] += pawn_isolated_grad * evalGradEnd;
    
    feature_gradients[0][2] += pawn_passed_grad * evalGradMid;
    feature_gradients[1][2] += pawn_passed_grad * evalGradEnd;
    
    feature_gradients[0][1] += pawn_structure_grad * evalGradMid;
    feature_gradients[1][1] += pawn_structure_grad * evalGradEnd;
    
    feature_gradients[0][7] += pawn_open_grad * evalGradMid;
    feature_gradients[1][7] += pawn_open_grad * evalGradEnd;
    
    feature_gradients[0][6] += pawn_backward_grad * evalGradMid;
    feature_gradients[1][6] += pawn_backward_grad * evalGradEnd;
    
    feature_gradients[0][8] += pawn_blocked_grad * evalGradMid;
    feature_gradients[1][8] += pawn_blocked_grad * evalGradEnd;
    
    
    
    
    
    
    
    
    Square whiteKingSquare = bitscanForward(board->getPieces(WHITE, KING));
    Square blackKingSquare = bitscanForward(board->getPieces(BLACK, KING));
    
    U64 k, attacks;
    Square square;
    
    
    
    
    /**********************************************************************************
     *                                  K N I G H T S                                 *
     **********************************************************************************/
    
    k = board->getPieces()[WHITE_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
    
        feature_gradients[0][9] += isOutpost(square, WHITE, blackPawns, whitePawnCover) * evalGradMid;
        feature_gradients[1][9] += isOutpost(square, WHITE, blackPawns, whitePawnCover) * evalGradEnd;
    
    
        feature_gradients[0][10] +=  manhattanDistance(square, blackKingSquare) * evalGradMid;
        feature_gradients[1][10] +=  manhattanDistance(square, blackKingSquare) * evalGradEnd;
//        featureScore += KNIGHT_OUTPOST * isOutpost(square, WHITE, blackPawns, whitePawnCover);
//        featureScore += KNIGHT_DISTANCE_ENEMY_KING * manhattanDistance(square, blackKingSquare);
        
        k = lsbReset(k);
    }
    
    k = board->getPieces()[BLACK_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
    
        feature_gradients[0][9] -= isOutpost(square, BLACK, whitePawns, blackPawnCover) * evalGradMid;
        feature_gradients[1][9] -= isOutpost(square, BLACK, whitePawns, blackPawnCover) * evalGradEnd;
        
        feature_gradients[0][10] -=  manhattanDistance(square, whiteKingSquare) * evalGradMid;
        feature_gradients[1][10] -=  manhattanDistance(square, whiteKingSquare) * evalGradEnd;
       
        k = lsbReset(k);
    }
    /**********************************************************************************
     *                                  B I S H O P S                                 *
     **********************************************************************************/
    
    k = board->getPieces()[WHITE_BISHOP];
    while (k) {
        square  = bitscanForward(k);
    
        feature_gradients[0][15] += bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES)) * evalGradMid;
        feature_gradients[1][15] += bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES)) * evalGradEnd;
    
        feature_gradients[0][16] += (square == G2 && whitePawns & ONE << F2 && whitePawns & ONE << H2
                                       && whitePawns & (ONE << G3 | ONE << G4)) * evalGradMid;
        feature_gradients[1][16] += (square == G2 && whitePawns & ONE << F2 && whitePawns & ONE << H2
                                       && whitePawns & (ONE << G3 | ONE << G4)) * evalGradEnd;
        feature_gradients[0][16] += (square == B2 && whitePawns & ONE << A2 && whitePawns & ONE << C2
                                     && whitePawns & (ONE << B3 | ONE << B4)) * evalGradMid;
        feature_gradients[1][16] += (square == B2 && whitePawns & ONE << A2 && whitePawns & ONE << C2
                                     && whitePawns & (ONE << B3 | ONE << B4)) * evalGradEnd;
        k = lsbReset(k);
    }
    
    k = board->getPieces()[BLACK_BISHOP];
    while (k) {
        square  = bitscanForward(k);
    
        feature_gradients[0][15] -= bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES)) * evalGradMid;
        feature_gradients[1][15] -= bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES)) * evalGradEnd;
    
        feature_gradients[0][16] -= (square == G7 && blackPawns & ONE << F7 && blackPawns & ONE << H7
                                     && blackPawns & (ONE << G6 | ONE << G5)) * evalGradMid;
        feature_gradients[1][16] -= (square == G7 && blackPawns & ONE << F7 && blackPawns & ONE << H7
                                     && blackPawns & (ONE << G6 | ONE << G5)) * evalGradEnd;
        feature_gradients[0][16] -= (square == B2 && blackPawns & ONE << A7 && blackPawns & ONE << C7
                                     && blackPawns & (ONE << B6 | ONE << B5)) * evalGradMid;
        feature_gradients[1][16] -= (square == B2 && blackPawns & ONE << A7 && blackPawns & ONE << C7
                                     && blackPawns & (ONE << B6 | ONE << B5)) * evalGradEnd;
        k = lsbReset(k);
    }
    feature_gradients[0][14] += (+ (bitCount(board->getPieces()[WHITE_BISHOP]) == 2)
                                 - (bitCount(board->getPieces()[BLACK_BISHOP]) == 2)) * evalGradMid;
    feature_gradients[1][14] += (+ (bitCount(board->getPieces()[WHITE_BISHOP]) == 2)
                                 - (bitCount(board->getPieces()[BLACK_BISHOP]) == 2)) * evalGradEnd;
    
    /**********************************************************************************
     *                                  R O O K S                                     *
     **********************************************************************************/
    
    feature_gradients[0][13] += evalGradMid * (
        + bitCount(lookUpRookAttack(blackKingSquare, occupied) & board->getPieces(WHITE, ROOK))
        - bitCount(lookUpRookAttack(whiteKingSquare, occupied) & board->getPieces(BLACK, ROOK)));
    feature_gradients[1][13] += evalGradEnd * (
        + bitCount(lookUpRookAttack(blackKingSquare, occupied) & board->getPieces(WHITE, ROOK))
        - bitCount(lookUpRookAttack(whiteKingSquare, occupied) & board->getPieces(BLACK, ROOK)));
    feature_gradients[0][11] += evalGradMid * (
        + bitCount(openFiles & board->getPieces(WHITE, ROOK))
        - bitCount(openFiles & board->getPieces(BLACK, ROOK)));
    feature_gradients[1][11] += evalGradEnd * (
        + bitCount(openFiles & board->getPieces(WHITE, ROOK))
        - bitCount(openFiles & board->getPieces(BLACK, ROOK)));
    feature_gradients[0][12] += evalGradMid * (
        + bitCount(openFilesBlack & ~openFiles & board->getPieces(WHITE, ROOK))
        - bitCount(openFilesWhite & ~openFiles & board->getPieces(BLACK, ROOK)));
    feature_gradients[1][12] += evalGradEnd * (
        + bitCount(openFilesBlack & ~openFiles & board->getPieces(WHITE, ROOK))
        - bitCount(openFilesWhite & ~openFiles & board->getPieces(BLACK, ROOK)));

    
    /**********************************************************************************
     *                                  Q U E E N S                                   *
     **********************************************************************************/
    
    k = board->getPieces()[WHITE_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        feature_gradients[0][17] += evalGradMid * manhattanDistance(square, blackKingSquare);
        feature_gradients[1][17] += evalGradEnd * manhattanDistance(square, blackKingSquare);
        k = lsbReset(k);
    }
    
    k = board->getPieces()[BLACK_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        
        feature_gradients[0][17]  -= evalGradMid * manhattanDistance(square, whiteKingSquare);
        feature_gradients[1][17]  -= evalGradEnd * manhattanDistance(square, whiteKingSquare);
        
        k = lsbReset(k);
    }
    
    /**********************************************************************************
     *                                  K I N G S                                     *
     **********************************************************************************/
    k = board->getPieces()[WHITE_KING];
    
    while (k) {
        square = bitscanForward(k);
    
        feature_gradients[0][19] += evalGradMid * bitCount(KING_ATTACKS[square] & whitePawns);
        feature_gradients[1][19] += evalGradEnd * bitCount(KING_ATTACKS[square] & whitePawns);
        
        feature_gradients[0][18] += evalGradMid * bitCount(KING_ATTACKS[square] & blackTeam);
        feature_gradients[1][18] += evalGradEnd * bitCount(KING_ATTACKS[square] & blackTeam);
        
//        featureScore += KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & whitePawns);
//        featureScore += KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & blackTeam);
        
        k = lsbReset(k);
    }
    
    k = board->getPieces()[BLACK_KING];
    while (k) {
        square = bitscanForward(k);
    
        feature_gradients[0][19] -= evalGradMid * bitCount(KING_ATTACKS[square] & blackTeam);
        feature_gradients[1][19] -= evalGradEnd * bitCount(KING_ATTACKS[square] & blackTeam);
    
        feature_gradients[0][18] -= evalGradMid * bitCount(KING_ATTACKS[square] & whitePawns);
        feature_gradients[1][18] -= evalGradEnd * bitCount(KING_ATTACKS[square] & whitePawns);
        
        k = lsbReset(k);
    }
    
    feature_gradients[0][20] += evalGradMid*(
        + board->getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
        + board->getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
        - board->getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
        - board->getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING));
    feature_gradients[1][20] += evalGradEnd*(
        + board->getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
        + board->getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
        - board->getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
        - board->getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING));
    
    feature_gradients[0][0] += evalGradMid * (board->getActivePlayer() == WHITE ? 1 : -1);
    feature_gradients[1][0] += evalGradEnd * (board->getActivePlayer() == WHITE ? 1 : -1);
}
void collectGradients_passers(Board* board, float evalGrad, float phase){
    U64 k;
    Square square;
    
    
    U64 whiteTeam = board->getTeamOccupied()[WHITE];
    U64 blackTeam = board->getTeamOccupied()[BLACK];
    U64 whitePawns = board->getPieces()[WHITE_PAWN];
    U64 blackPawns = board->getPieces()[BLACK_PAWN];
    
    // all passed pawns for white/black
    U64 whitePassers = wPassedPawns(whitePawns, blackPawns);
    U64 blackPassers = bPassedPawns(blackPawns, whitePawns);
    
    U64 whiteBlockedPawns = shiftNorth(whitePawns) & (whiteTeam | blackTeam);
    U64 blackBlockedPawns = shiftSouth(blackPawns) & (whiteTeam | blackTeam);
    
    k = whitePassers;
    while (k) {
        square = bitscanForward(k);
        passer_gradients[0][getBit(whiteBlockedPawns, square) * 8 + rankIndex(square)] += evalGrad * (1 - phase);
        passer_gradients[1][getBit(whiteBlockedPawns, square) * 8 + rankIndex(square)] += evalGrad * (    phase);
        k = lsbReset(k);
    }
    k = blackPassers;
    while (k) {
        square = bitscanForward(k);
        passer_gradients[0][getBit(blackBlockedPawns, square) * 8 + 7 - rankIndex(square)] -= evalGrad * (1 - phase);
        passer_gradients[1][getBit(blackBlockedPawns, square) * 8 + 7 - rankIndex(square)] -= evalGrad * (    phase);
        k = lsbReset(k);
    }
}
void collectGradients_kingSafety(Board* board, float evalGrad, float phase){
    int wkingSafety_attPiecesCount = 0;
    int wkingSafety_valueOfAttacks = 0;
    
    int bkingSafety_attPiecesCount = 0;
    int bkingSafety_valueOfAttacks = 0;
    
    Square whiteKingSquare = bitscanForward(board->getPieces(WHITE, KING));
    Square blackKingSquare = bitscanForward(board->getPieces(BLACK, KING));
    
    U64 occupied = *board->getOccupied();
    
    U64 whiteKingZone = KING_ATTACKS[whiteKingSquare];
    U64 blackKingZone = KING_ATTACKS[blackKingSquare];
    
    static const int safetyWeights[5] = {0,2,2,3,4};
    
    for (Piece p = KNIGHT; p <= QUEEN; p++) {
        
        U64 w = board->getPieces(WHITE, p);
        U64 b = board->getPieces(BLACK, p);
        
        while (w) {
            Square s = bitscanForward(w);
            
            U64 attacks = ZERO;
            switch (p) {
                case KNIGHT: attacks = KNIGHT_ATTACKS[s]; break;
                case BISHOP: attacks = lookUpBishopAttack(s, occupied); break;
                case ROOK: attacks = lookUpRookAttack(s, occupied); break;
                case QUEEN: attacks = lookUpBishopAttack(s, occupied) | lookUpRookAttack(s, occupied); break;
            }
    
            addToKingSafety(attacks, blackKingZone, bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, safetyWeights[p]);
    
            w = lsbReset(w);
        }
        while (b) {
            Square s = bitscanForward(b);
            
            U64 attacks = ZERO;
            switch (p) {
                case KNIGHT: attacks = KNIGHT_ATTACKS[s]; break;
                case BISHOP: attacks = lookUpBishopAttack(s, occupied); break;
                case ROOK: attacks = lookUpRookAttack(s, occupied); break;
                case QUEEN: attacks = lookUpBishopAttack(s, occupied) | lookUpRookAttack(s, occupied); break;
            }
            addToKingSafety(attacks, whiteKingZone, wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks,  safetyWeights[p]);
           
            b = lsbReset(b);
        }
    }
    
//    std::cerr << wkingSafety_valueOfAttacks << "   " << bkingSafety_valueOfAttacks << std::endl;
//    std::cerr << kingSafety_gradients[0][wkingSafety_valueOfAttacks] << std::endl;
//    std::cerr << kingSafety_gradients[1][wkingSafety_valueOfAttacks] << std::endl;
    
    kingSafety_gradients[0][wkingSafety_valueOfAttacks] -= (1-phase) * evalGrad;
    kingSafety_gradients[1][wkingSafety_valueOfAttacks] -= (  phase) * evalGrad;
//    std::cerr << kingSafety_gradients[0][wkingSafety_valueOfAttacks] << std::endl;
//    std::cerr << kingSafety_gradients[1][wkingSafety_valueOfAttacks] << std::endl;
    
    kingSafety_gradients[0][bkingSafety_valueOfAttacks] += (1-phase) * evalGrad;
    kingSafety_gradients[1][bkingSafety_valueOfAttacks] += (  phase) * evalGrad;
//    std::cerr << kingSafety_gradients[0][bkingSafety_valueOfAttacks] << std::endl;
//    std::cerr << kingSafety_gradients[1][bkingSafety_valueOfAttacks] << std::endl;
}


void updateGradients(int lr=1) {
    // psqt
    for (int i = 0; i < 11; i++) {
        for (Square s = 0; s < 64; s++) {

            float midGrad = psqt_gradients[0][i][s];
            float endGrad = psqt_gradients[1][i][s];

            EvalScore change = CHANGE(midGrad, endGrad, lr);
            psqt[i][s] += change;
        }
    }

    // mobilities
    for (int i = PAWN; i <= QUEEN; i++) {
        for (Square s = 0; s < mobEntryCount[i]; s++) {

            float midGrad = mob_gradients[0][i][s];
            float endGrad = mob_gradients[1][i][s];

            EvalScore change= CHANGE(midGrad, endGrad, lr);
            mobilities[i][s] += change;
        }
    }

    // pinned evaluation
    for (int i = 0; i < 15; i++){
        float midGrad = pinned_gradients[0][i];
        float endGrad = pinned_gradients[1][i];
        EvalScore change = CHANGE(midGrad, endGrad, lr);
        pinnedEval[i] += change;
    }

    // hanging evaluation
    for (int i = 0; i < 5; i++){
        float midGrad = hanging_gradients[0][i];
        float endGrad = hanging_gradients[1][i];
        EvalScore change = CHANGE(midGrad, endGrad, lr);
        hangingEval[i] += change;
    }
    
    // features
    for(int i = 0; i < 21; i++){
        float midGrad = feature_gradients[0][i];
        float endGrad = feature_gradients[1][i];
        EvalScore change = CHANGE(midGrad, endGrad, lr);
        *evfeatures[i] += change;
        
    }
    
    // passers
    for(int i = 0; i < 16; i++){
        float midGrad = passer_gradients[0][i];
        float endGrad = passer_gradients[1][i];
        EvalScore change = CHANGE(midGrad, endGrad, lr);
        passer_rank_n[i] += change;
    }
    
    // king safety
    for(int i = 0; i < 100; i++){
        float midGrad = kingSafety_gradients[0][i];
        float endGrad = kingSafety_gradients[1][i];
        EvalScore change = CHANGE(midGrad, endGrad, lr);
        kingSafetyTable[i] += change;
    }
    
    eval_init();
}

void clearGradients() {
    for (int i = 0; i < 11; i++) {
        for (int s = 0; s < 64; s++) {
            psqt_gradients[0][i][s] = 0;
            psqt_gradients[1][i][s] = 0;
        }
    }

    for (int i = 0; i < 6; i++) {
        for (int s = 0; s < 28; s++) {
            mob_gradients[0][i][s] = 0;
            mob_gradients[1][i][s] = 0;
        }
    }

    for (int i = 0; i < 5; i++) {
        hanging_gradients[0][i] = 0;
        hanging_gradients[1][i] = 0;
    }
    
    for (int i = 0; i < 15; i++) {
        pinned_gradients[0][i] = 0;
        pinned_gradients[1][i] = 0;
    }
    
    for (int i = 0; i < 21; i++) {
        feature_gradients[0][i] = 0;
        feature_gradients[1][i] = 0;
    }
    
    for (int i = 0; i < 16; i++) {
        passer_gradients[0][i] = 0;
        passer_gradients[1][i] = 0;
    }
    
    for (int i = 0; i < 100; i++) {
        kingSafety_gradients[0][i] = 0;
        kingSafety_gradients[1][i] = 0;
    }
}

double collectGradients(std::vector<TrainingEntry>& entries, double K) {

    Evaluator evaluator {};
    
    double error = 0;
    
    for (TrainingEntry& en : entries) {

        bool matingMaterial = hasMatingMaterial(&en.board, en.board.getActivePlayer());

        // we need the phase to compute gradients for early and late phase
        float phase =
            (24.0f + phaseValues[5]
             - phaseValues[0] * bitCount(en.board.getPieces()[WHITE_PAWN] | en.board.getPieces()[BLACK_PAWN])
             - phaseValues[1] * bitCount(en.board.getPieces()[WHITE_KNIGHT] | en.board.getPieces()[BLACK_KNIGHT])
             - phaseValues[2] * bitCount(en.board.getPieces()[WHITE_BISHOP] | en.board.getPieces()[BLACK_BISHOP])
             - phaseValues[3] * bitCount(en.board.getPieces()[WHITE_ROOK] | en.board.getPieces()[BLACK_ROOK])
             - phaseValues[4] * bitCount(en.board.getPieces()[WHITE_QUEEN] | en.board.getPieces()[BLACK_QUEEN]))
            / 24.0f;
        if (phase > 1)
            phase = 1;
        if (phase < 0)
            phase = 0;

        // compute the gradient of the loss function with respect to the output
        Score  q_i      = evaluator.evaluate(&en.board);
        
        double expected = en.target;

        double sig       = sigmoid(q_i, K);
        double sigPrime  = sigmoidPrime(q_i, K);
        double lossPrime = -2 * (expected - sig);
        
        error += (expected - sig) * (expected - sig);
        
        if (matingMaterial) {
            lossPrime /= 10;
        }

        collectGradients_psqt(&en.board, lossPrime * sigPrime, phase);
        collectGradients_mobility(&en.board, lossPrime * sigPrime, phase);
        collectGradients_pinners(&en.board, lossPrime * sigPrime, phase);
        collectGradients_hanging(&en.board, lossPrime * sigPrime, phase);
        collectGradients_features(&en.board, lossPrime * sigPrime, phase);
        collectGradients_passers(&en.board, lossPrime * sigPrime, phase);
        collectGradients_kingSafety(&en.board, lossPrime * sigPrime, phase);
    }
    
    error /= entries.size();
    return error;
}

double optimiseGradients(double K, int lr=1) {
    clearGradients();
    double error = collectGradients(training_entries, K);
    updateGradients(lr);
    return error;
}

}    // namespace tuning

#endif
