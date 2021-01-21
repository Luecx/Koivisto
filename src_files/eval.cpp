
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

EvalScore mobilityKnight[9] = {
    M(  -48,   -6), M(  -37,   48), M(  -32,   80), M(  -28,   98), M(  -24,  110),
    M(  -20,  120), M(  -14,  122), M(   -4,  116), M(   10,  101), };

EvalScore mobilityBishop[14] = {
    M(  -14,  -37), M(   -4,   31), M(    3,   64), M(    8,   86), M(   14,  102),
    M(   19,  116), M(   20,  124), M(   21,  128), M(   23,  132), M(   27,  131),
    M(   35,  126), M(   50,  118), M(   61,  126), M(   85,   98), };

EvalScore mobilityRook[15] = {
    M(  -49,   63), M(  -42,  108), M(  -39,  144), M(  -39,  173), M(  -37,  188),
    M(  -31,  195), M(  -26,  203), M(  -18,  205), M(  -12,  210), M(   -6,  215),
    M(   -1,  219), M(    4,  222), M(   13,  220), M(   37,  204), M(   94,  173), };

EvalScore mobilityQueen[28] = {
    M( -191,  116), M( -175,  120), M( -163,  248), M( -159,  330), M( -157,  375),
    M( -156,  405), M( -155,  430), M( -153,  447), M( -151,  461), M( -149,  470),
    M( -147,  479), M( -145,  486), M( -144,  491), M( -144,  499), M( -145,  505),
    M( -147,  509), M( -147,  512), M( -149,  514), M( -149,  515), M( -143,  510),
    M( -137,  502), M( -135,  498), M( -142,  496), M( -128,  488), M( -189,  514),
    M(  -68,  446), M( -115,  499), M( -175,  534), };

EvalScore hangingEval[5] = {
    M(   -3,   -1), M(   -4,   -0), M(   -6,   -6), M(   -5,   -6), M(   -4,   -8), };

EvalScore pinnedEval[15] = {
    M(   -3,   -6), M(  -12,    8), M(   -8,   33), M(  -23,  -59), M(  -28,  -12),
    M(  -19,   27), M(   -5,   -7), M(  -34,  -13), M(  -17,   15), M(  -48,  -34),
    M(   -3,   -9), M(  -16,   18), M(  -11,  -65), M(  -45,  -48), M(  -16,   17), };

EvalScore passer_rank_n[16] = {
    M(    0,    0), M(  -10,  -30), M(  -15,  -26), M(  -10,    0),
    M(   20,   27), M(   42,   88), M(   14,   45), M(    0,    0),
    M(    0,    0), M(    1,    2), M(  -32,  -62), M(  -29,  -19),
    M(  -15,  -14), M(   38,   20), M(   95, -110), M(    0,    0), };

EvalScore bishop_pawn_same_color_table_o[9] = {
    M(  -34,   42), M(  -41,   42), M(  -41,   29),
    M(  -44,   19), M(  -49,    9), M(  -54,   -3),
    M(  -57,  -22), M(  -58,  -39), M(  -67,  -88), };

EvalScore bishop_pawn_same_color_table_e[9] = {
    M(  -23,   41), M(  -40,   49), M(  -44,   39),
    M(  -50,   29), M(  -54,   17), M(  -58,    1),
    M(  -60,  -20), M(  -59,  -37), M(  -66,  -55), };


EvalScore SIDE_TO_MOVE                  = M(   10,   15);
EvalScore PAWN_PASSED                   = M(    4,   47);
EvalScore PAWN_ISOLATED                 = M(   -3,  -13);
EvalScore PAWN_DOUBLED                  = M(   -7,   -5);
EvalScore PAWN_DOUBLED_AND_ISOLATED     = M(   -8,  -26);
EvalScore PAWN_BACKWARD                 = M(  -10,    0);
EvalScore PAWN_OPEN                     = M(   -9,   -5);
EvalScore PAWN_BLOCKED                  = M(   -4,  -14);
EvalScore KNIGHT_OUTPOST                = M(   25,   18);
EvalScore KNIGHT_DISTANCE_ENEMY_KING    = M(   -6,   -1);
EvalScore ROOK_OPEN_FILE                = M(   24,   -2);
EvalScore ROOK_HALF_OPEN_FILE           = M(    1,  -11);
EvalScore ROOK_KING_LINE                = M(   20,    3);
EvalScore BISHOP_DOUBLED                = M(   16,   78);
EvalScore BISHOP_FIANCHETTO             = M(   -4,    1);
EvalScore BISHOP_PIECE_SAME_SQUARE_E    = M(    3,    6);
EvalScore QUEEN_DISTANCE_ENEMY_KING     = M(    2,  -30);
EvalScore KING_CLOSE_OPPONENT           = M(  -10,   14);
EvalScore KING_PAWN_SHIELD              = M(   28,    9);
EvalScore CASTLING_RIGHTS               = M(   20,   -4);
EvalScore MINOR_BEHIND_PAWN             = M(    5,   24);

EvalScore kingSafetyTable[100] = {
    M(  -16,   -7), M(    0,    0), M(  -16,   -6), M(  -14,   -9), M(  -11,   -9),
    M(   15,   -9), M(    7,  -12), M(   27,  -14), M(   19,  -11), M(   45,  -22),
    M(   67,  -17), M(   81,  -23), M(   52,  -20), M(  115,  -26), M(  118,  -17),
    M(  120,  -18), M(  115,  -25), M(  190,  -26), M(  219,  -49), M(  227,  -60),
    M(  241,  -68), M(  212,  -38), M(  324,  -62), M(  270,  -27), M(  306,  -33),
    M(  313,   -7), M(  401,  -13), M(  408,  -50), M(  360,   66), M(  466,  -22),
    M(  488, -174), M(  574, -140), M(  624, -259), M(  728, -239), M(  569,  -52),
    M( 2097,-3685), M(  113, 1094), M( 1879,-2404), M(  788, -360), M(  973,  824),
    M( 1315,-1886), M(  992, -481), M( 1504, 1316), M(  500,  500), M(  501,  500),
    M( 1660, 1536), M(  500,  500), M(  879,  682), M(  500,  500), M( -510, -513),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500),
    M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), M(  500,  500), };

EvalScore* evfeatures[] {
    &SIDE_TO_MOVE,
//    &PAWN_STRUCTURE,
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
    bool   sameSideCastle    = wKSide == bKSide;
    
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


    EvalScore evalScore    = M(0, 0);
    EvalScore featureScore = M(0, 0);
    EvalScore mobScore     = M(0, 0);
    EvalScore materialScore= M(0, 0);
    
    k = whitePawns;
    while (k) {
        square = bitscanForward(k);
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_PAWN][square];
        k = lsbReset(k);
    }
    k = blackPawns;
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
    
    
    // defended pawns from east and west for both colors
    k = whitePawnEastCover;
    while(k){
        square = bitscanForward(k);
        evalScore += pawn_structure_table[0][sameSideCastle][pst_index_white(square, wKSide)];
        k = lsbReset(k);
    } k = whitePawnWestCover;
    while(k){
        square = bitscanForward(k);
        evalScore += pawn_structure_table[1][sameSideCastle][pst_index_white(square, wKSide)];
        k = lsbReset(k);
    } k = blackPawnEastCover;
    while(k){
        square = bitscanForward(k);
        evalScore -= pawn_structure_table[0][sameSideCastle][pst_index_black(square, bKSide)];
        k = lsbReset(k);
    } k = blackPawnWestCover;
    while(k){
        square = bitscanForward(k);
        evalScore -= pawn_structure_table[1][sameSideCastle][pst_index_black(square, bKSide)];
        k = lsbReset(k);
    }
    
    
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

        addToKingSafety(attacks, blackKingZone, bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 4);

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

        addToKingSafety(attacks, whiteKingZone, wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, 4);

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



