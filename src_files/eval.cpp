
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
EvalScore SIDE_TO_MOVE                  = M(   10,   15);
EvalScore PAWN_STRUCTURE                = M(   10,    4);
EvalScore PAWN_PASSED                   = M(    4,   49);
EvalScore PAWN_ISOLATED                 = M(   -2,  -15);
EvalScore PAWN_DOUBLED                  = M(   -7,   -5);
EvalScore PAWN_DOUBLED_AND_ISOLATED     = M(   -8,  -27);
EvalScore PAWN_BACKWARD                 = M(  -11,    1);
EvalScore PAWN_OPEN                     = M(  -10,   -6);
EvalScore PAWN_BLOCKED                  = M(   -4,  -14);
EvalScore KNIGHT_OUTPOST                = M(   25,   19);
EvalScore KNIGHT_DISTANCE_ENEMY_KING    = M(   -6,   -1);
EvalScore ROOK_OPEN_FILE                = M(   26,   -2);
EvalScore ROOK_HALF_OPEN_FILE           = M(    1,  -12);
EvalScore ROOK_KING_LINE                = M(   11,    5);
EvalScore BISHOP_DOUBLED                = M(   16,   81);
EvalScore BISHOP_FIANCHETTO             = M(   -4,    2);
EvalScore BISHOP_PIECE_SAME_SQUARE_E    = M(    3,    6);
EvalScore QUEEN_DISTANCE_ENEMY_KING     = M(    3,  -28);
EvalScore KING_CLOSE_OPPONENT           = M(  -16,   16);
EvalScore KING_PAWN_SHIELD              = M(   29,   10);
EvalScore CASTLING_RIGHTS               = M(   21,   -3);
EvalScore MINOR_BEHIND_PAWN             = M(    5,   25);

EvalScore mobilityKnight[9] = {
    M(  -46,   -2), M(  -35,   55), M(  -30,   87), M(  -26,  106), M(  -22,  118),
    M(  -18,  129), M(  -11,  130), M(   -2,  124), M(   13,  109), };

EvalScore mobilityBishop[14] = {
    M(  -13,  -39), M(   -2,   34), M(    5,   69), M(    9,   91), M(   16,  108),
    M(   20,  122), M(   21,  130), M(   21,  134), M(   24,  138), M(   28,  137),
    M(   35,  132), M(   51,  125), M(   59,  134), M(   83,  105), };

EvalScore mobilityRook[15] = {
    M(  -45,   74), M(  -39,  119), M(  -35,  157), M(  -34,  187), M(  -33,  203),
    M(  -27,  210), M(  -21,  218), M(  -13,  220), M(   -6,  225), M(    0,  231),
    M(    6,  234), M(   12,  238), M(   22,  235), M(   49,  218), M(  108,  186), };

EvalScore mobilityQueen[28] = {
    M( -185,  138), M( -168,  143), M( -155,  274), M( -151,  357), M( -149,  404),
    M( -149,  436), M( -148,  462), M( -146,  479), M( -144,  494), M( -142,  503),
    M( -140,  512), M( -138,  519), M( -137,  524), M( -137,  532), M( -138,  538),
    M( -140,  542), M( -140,  544), M( -142,  547), M( -142,  548), M( -136,  542),
    M( -130,  535), M( -128,  530), M( -136,  528), M( -123,  520), M( -186,  548),
    M(  -66,  480), M( -118,  538), M( -165,  567), };

EvalScore hangingEval[5] = {
    M(   -3,   -0), M(   -4,   -0), M(   -6,   -6), M(   -5,   -6), M(   -5,   -8), };

EvalScore pinnedEval[15] = {
    M(   -4,   -6), M(  -11,    8), M(   -7,   32), M(  -25,  -61), M(  -24,  -14),
    M(  -19,   26), M(   -6,   -6), M(  -31,  -15), M(  -17,   13), M(  -49,  -36),
    M(    6,  -12), M(  -15,   15), M(  -12,  -67), M(  -44,  -45), M(  -15,   14), };

EvalScore passer_rank_n[16] = {
    M(    0,    0), M(  -10,  -31), M(  -16,  -27), M(  -10,    2),
    M(   19,   29), M(   41,   87), M(   18,   51), M(    0,    0),
    M(    0,    0), M(    1,    2), M(  -33,  -62), M(  -30,  -17),
    M(  -17,  -14), M(   33,   13), M(  100, -116), M(    0,    0), };

EvalScore bishop_pawn_same_color_table_o[9] = {
    M(  -33,   43), M(  -40,   44), M(  -40,   30),
    M(  -43,   20), M(  -47,   10), M(  -52,   -3),
    M(  -56,  -22), M(  -57,  -39), M(  -67,  -91), };

EvalScore bishop_pawn_same_color_table_e[9] = {
    M(  -21,   46), M(  -38,   53), M(  -43,   43),
    M(  -48,   33), M(  -53,   21), M(  -57,    4),
    M(  -59,  -18), M(  -58,  -35), M(  -65,  -54), };

EvalScore kingSafetyTable[100] = {
    M(   13,   -5), M(    0,    0), M(   14,   -4), M(   13,   -6), M(   22,   -6),
    M(   44,   -6), M(   22,   -6), M(   70,  -15), M(   39,    2), M(   54,  -14),
    M(   66,    0), M(  101,  -16), M(   51,   -8), M(  133,  -19), M(   97,   12),
    M(  105,    4), M(  115,   13), M(  177,    0), M(   71,   11), M(  197,    4),
    M(  150,   18), M(  151,    0), M(  176,   27), M(  246,    2), M(  149,  -17),
    M(  292,    6), M(  279,  -14), M(  234,  -41), M(  312,  -23), M(  291,   31),
    M(  280,  -65), M(  416,  -60), M(  381,  -44), M(  325,    2), M(  366,  -33),
    M( 2530,-3031), M(  261,  258), M( 1536,-1613), M( 1316, -954), M(  338,  199),
    M( 1682,-1563), M(  707, -128), M(  354,  141), M(  917, -305), M(  277,  390),
    M( 1127,  708), M(  496,  718), M( 1150,  -59), M(  215,  719), M(   71,   63),
    M(  768,  500), M(  769,  163), M(  844,  843), M(  500,  500), M(  559,  569),
    M(  919,  919), M(  500,  500), M(  890,  888), M(  502,  500), M(  500,  500),
    M(  301,  295), M(  500,  500), M(  521,  513), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  501,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), };


EvalScore* evfeatures[] {
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
    &BISHOP_FIANCHETTO,
    &BISHOP_PIECE_SAME_SQUARE_E,
    
    &QUEEN_DISTANCE_ENEMY_KING,
    
    &KING_CLOSE_OPPONENT,
    &KING_PAWN_SHIELD,
    
    &CASTLING_RIGHTS,

    &MINOR_BEHIND_PAWN,
    
};



int mobEntryCount[6] {0, 9, 14, 15, 28, 0};

float* phaseValues = new float[6] {
    0, 1, 1, 2, 4, 0,
};


EvalScore* mobilities[6] {nullptr, mobilityKnight, mobilityBishop, mobilityRook, mobilityQueen, nullptr};




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

EvalScore Evaluator::computePinnedPieces(Board* b, Color color) {
    
    
    EvalScore result = 0;
    
    Color us = color;
    Color them = 1 - color;
    
    // figure out where the opponent has pieces
    U64 opponentOcc = b->getTeamOccupied()[them];
    U64      ourOcc = b->getTeamOccupied()[us];
    
    // get the pieces which can pin our king
    U64 bishops = b->getPieces(them, BISHOP);
    U64   rooks = b->getPieces(them, ROOK);
    U64  queens = b->getPieces(them, QUEEN);
    
    // get the king positions
    Square kingSq = bitscanForward(b->getPieces(us, KING));
    
    // get the potential pinners for rook/bishop attacks
    U64 rookAttacks   = lookUpRookAttack  (kingSq, opponentOcc) & (rooks   | queens);
    U64 bishopAttacks = lookUpBishopAttack(kingSq, opponentOcc) & (bishops | queens);
    
    // get all pinners (either rook or bishop attackers)
    U64 potentialPinners = (rookAttacks | bishopAttacks);
    
    while(potentialPinners){

        Square pinnerSquare = bitscanForward(potentialPinners);

        // get all the squares in between the king and the potential pinner
        U64 inBetween = inBetweenSquares[kingSq][pinnerSquare];

        // if there is exactly one of our pieces in the way, consider it pinned. Otherwise, continue
        U64 potentialPinned = ourOcc & inBetween;
        if (potentialPinned == 0 || lsbIsolation(potentialPinned) != potentialPinned){
            potentialPinners = lsbReset(potentialPinners);
            continue;
        }

        // extract the pinner pieces and the piece that pins
        Piece pinnedPiece = b->getPiece(bitscanForward(potentialPinned));
        Piece pinnerPiece = b->getPiece(pinnerSquare) - BISHOP;

        // normalise the values (black pieces will be made to white pieces)
        if(us == WHITE){
            pinnerPiece -= 6;
        }else{
            pinnedPiece -= 6;
        }

        // add to the result indexing using pinnedPiece for which there are 5 different pieces and the pinner
        result += pinnedEval[pinnedPiece * 3 + pinnerPiece];
        
        // reset the lsb
        potentialPinners = lsbReset(potentialPinners);
    }
    
    return result;
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

    U64 whiteKingZone = KING_ATTACKS[whiteKingSquare];
    U64 blackKingZone = KING_ATTACKS[blackKingSquare];

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
    EvalScore materialScore= M(0, 0);

    while (k) {
        square = bitscanForward(k);
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_PAWN][square];
     
        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_PAWN];
    while (k) {
        square = bitscanForward(k);
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_PAWN][square];
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

    U64 mobilitySquaresWhite = ~whiteTeam & ~(blackPawnCover);
    U64 mobilitySquaresBlack = ~blackTeam & ~(whitePawnCover);

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
    featureScore += MINOR_BEHIND_PAWN * (
            + bitCount(shiftNorth(b->getPieces()[WHITE_KNIGHT]|b->getPieces()[WHITE_BISHOP])&(b->getPieces()[WHITE_PAWN]|b->getPieces()[BLACK_PAWN]))
            - bitCount(shiftSouth(b->getPieces()[BLACK_KNIGHT]|b->getPieces()[BLACK_BISHOP])&(b->getPieces()[WHITE_PAWN]|b->getPieces()[BLACK_PAWN])));
    

    
   
    /**********************************************************************************
     *                                  K N I G H T S                                 *
     **********************************************************************************/

    k = b->getPieces()[WHITE_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[square];
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_KNIGHT][square];
        
        
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
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_KNIGHT][square];
        
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
        attacks = lookUpBishopAttack(square, occupied & ~b->getPieces()[WHITE_QUEEN]);
        
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_BISHOP][square];
        
        
        mobScore += mobilityBishop[bitCount(attacks & mobilitySquaresWhite)];

        featureScore += bishop_pawn_same_color_table_e[bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
        featureScore += bishop_pawn_same_color_table_o[bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
        featureScore += BISHOP_PIECE_SAME_SQUARE_E 
                        * bitCount(blackTeam & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
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
        attacks = lookUpBishopAttack(square, occupied & ~b->getPieces()[BLACK_QUEEN]);
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_BISHOP][square];
    
        mobScore -= mobilityBishop[bitCount(attacks & mobilitySquaresBlack)];
        featureScore -= bishop_pawn_same_color_table_e[bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
        featureScore -= bishop_pawn_same_color_table_o[bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
        featureScore -= BISHOP_PIECE_SAME_SQUARE_E 
                        * bitCount(whiteTeam & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
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
        attacks = lookUpRookAttack(square, occupied & ~b->getPieces()[WHITE_ROOK] & ~b->getPieces()[WHITE_QUEEN]);
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_ROOK][square];
        mobScore += mobilityRook[bitCount(attacks & mobilitySquaresWhite)];

        addToKingSafety(attacks, blackKingZone, bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 3);

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied & ~b->getPieces()[BLACK_ROOK] & ~b->getPieces()[BLACK_QUEEN]);
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_ROOK][square];
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
        attacks = lookUpRookAttack  (square,    occupied & ~b->getPieces()[WHITE_ROOK])
                | lookUpBishopAttack(square,    occupied & ~b->getPieces()[WHITE_BISHOP]);
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_QUEEN][square];
        mobScore += mobilityQueen[bitCount(attacks & mobilitySquaresWhite)];
        featureScore += QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, blackKingSquare);

        addToKingSafety(attacks, blackKingZone, bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 6);

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack  (square,    occupied & ~b->getPieces()[BLACK_ROOK])
                | lookUpBishopAttack(square,    occupied & ~b->getPieces()[BLACK_BISHOP]);
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_QUEEN][square];
        mobScore -= mobilityQueen[bitCount(attacks & mobilitySquaresBlack)];
        featureScore -= QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, whiteKingSquare);

        addToKingSafety(attacks, whiteKingZone, wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, 6);

        k = lsbReset(k);
    }
    
    /**********************************************************************************
     *                                  K I N G S                                     *
     **********************************************************************************/
    k = b->getPieces()[WHITE_KING];

    while (k) {
        square = bitscanForward(k);
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_KING][square];

        featureScore += KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & whitePawns);
        featureScore += KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & blackTeam);

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_KING];
    while (k) {
        square = bitscanForward(k);
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_KING][square];

        featureScore -= KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & blackPawns);
        featureScore -= KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & whiteTeam);

        k = lsbReset(k);
    }
    
    
    EvalScore hangingEvalScore = computeHangingPieces(b);
    EvalScore pinnedEvalScore  = computePinnedPieces(b, WHITE) - computePinnedPieces(b, BLACK);

    evalScore += kingSafetyTable[bkingSafety_valueOfAttacks] - kingSafetyTable[wkingSafety_valueOfAttacks];
   
    // clang-format off
    featureScore += CASTLING_RIGHTS*(
            + b->getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
            + b->getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING));
    // clang-format on
    featureScore += SIDE_TO_MOVE * (b->getActivePlayer() == WHITE ? 1 : -1);

    EvalScore totalScore = evalScore + pinnedEvalScore + hangingEvalScore + featureScore + mobScore + materialScore;

    res += (int) ((float) MgScore(totalScore) * (1 - phase));
    res += (int) ((float) EgScore(totalScore) * (phase));

    if (!hasMatingMaterial(b, res > 0 ? WHITE : BLACK))
        res = res / 10;
    return res;
}

void printEvaluation(Board* board) {

    using namespace std;

    Evaluator ev {};
    Score     score = ev.evaluate(board);
    float     phase = ev.getPhase();

    std::cout <<
        setw(15) << right << "evaluation: " << left << setw(8) << score <<
        setw(15) << right << "phase: "      << left << setprecision(3) << setw(8) << phase << std::endl;
}

float Evaluator::getPhase() { return phase; }



