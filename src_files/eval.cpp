
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

EvalScore4i mobilityKnight[9] = {
    L( -16, -68,  32, -59), L(  14, -12,  41,  -9), L(  32,  31,  44,  13), L(  47,  49,  47,  28),
    L(  54,  47,  51,  47), L(  65,  53,  53,  59), L(  71,  48,  59,  69), L(  77,  42,  68,  69),
    L(  95,  30,  74,  57),
};

EvalScore4i mobilityBishop[14] = {
    L( -25, -88,  29, -34), L(   6,   5,  33,  11), L(  23,  47,  38,  38), L(  38,  73,  41,  55),
    L(  49,  85,  46,  73), L(  61,  91,  49,  91), L(  65,  96,  50, 102), L(  66, 101,  51, 107),
    L(  70, 105,  52, 114), L(  75, 100,  54, 124), L(  77,  99,  62, 120), L(  81,  87,  81, 126),
    L(  82, 110, 100, 119), L( 109,  72, 121, 111),
};

EvalScore4i mobilityRook[15] = {
    L(  13,  -8,  31,  31), L(  27,  10,  36,  40), L(  38,  40,  38,  58), L(  39,  82,  42,  64),
    L(  48,  90,  42,  78), L(  52,  96,  49,  87), L(  63, 104,  51,  95), L(  63, 102,  60,  99),
    L(  66, 106,  66, 106), L(  64, 108,  70, 120), L(  68, 113,  71, 122), L(  70, 116,  79, 122),
    L(  84, 114,  81, 123), L( 105, 101,  90, 107), L( 162,  73, 141,  88)
};

EvalScore4i mobilityQueen[28] = {
    L( -40,-181,  -4,-139), L( -24,-109,   6, -58), L(  -3,  15,  12,  57), L(  -6,  93,  18, 117),
    L(   6, 135,  18, 165), L(  17, 176,  20, 179), L(  20, 198,  23, 201), L(  25, 216,  25, 219),
    L(  27, 234,  27, 234), L(  36, 232,  30, 238), L(  37, 244,  34, 244), L(  37, 254,  37, 254),
    L(  41, 264,  38, 255), L(  41, 265,  35, 271), L(  37, 273,  37, 276), L(  36, 278,  33, 281),
    L(  37, 276,  34, 285), L(  34, 283,  28, 289), L(  34, 284,  34, 284), L(  41, 278,  35, 287),
    L(  48, 276,  42, 267), L(  57, 275,  27, 266), L(  45, 273,  27, 252), L(  83, 252,  41, 240),
    L(  -9, 292, -24, 277), L( 103, 232,  64, 211), L(  17, 291,  17, 291), L(  18, 302,  15, 299)
};

EvalScore4i hangingEval[5] {
    L(   -1,    2,   -1,    2),
    L(   -1,   -5,   -1,   -5),
    L(   -9,  -10,   -9,  -10),
    L(  -13,   -8,  -13,   -8),
    L(  -14,  -12,  -14,  -12),
};

EvalScore4i pinnedEval[15] {
    L(    0,  -20,    0,  -20),
    L(  -22,   16,  -22,   16),
    L(   -6,   33,   -6,   33),
    L(  -22,  -76,  -22,  -76),
    L(  -32,   -6,  -32,   -6),
    L(  -20,   25,  -20,   25),
    L(   -5,   -6,   -5,   -6),
    L(  -44,   -2,  -44,   -2),
    L(  -25,   19,  -25,   19),
    L(  -46,  -51,  -46,  -51),
    L(    0,   -7,    0,   -7),
    L(   -5,    5,   -5,    5),
    L(   -8,  -76,   -8,  -76),
    L(  -56,  -33,  -56,  -33),
    L(  -17,   21,  -17,   21),
};

EvalScore4i passer_rank_n[16] {
    L(    0,    0,    0,    0),
    L(   -9,  -29,   -9,  -29),
    L(   -9,  -24,   -9,  -24),
    L(   -5,    2,   -5,    2),
    L(   22,   23,   22,   23),
    L(   47,   66,   47,   66),
    L(    4,   27,    4,   27),
    L(    0,    0,    0,    0),
    L(    0,    0,    0,    0),
    L(    1,    2,    1,    2),
    L(  -44,  -49,  -44,  -49),
    L(  -45,  -11,  -45,  -11),
    L(  -29,  -14,  -29,  -14),
    L(   35,   -7,   35,   -7),
    L(   91, -143,   91, -143),
    L(    0,    0,    0,    0),
};

EvalScore4i bishop_pawn_same_color_table_o[9]{
    L(    2,   11,    2,   11),
    L(   -4,   14,   -4,   14),
    L(   -4,    4,   -4,    4),
    L(   -9,   -4,   -9,   -4),
    L(  -13,  -14,  -13,  -14),
    L(  -18,  -30,  -18,  -30),
    L(  -25,  -42,  -25,  -42),
    L(  -30,  -51,  -30,  -51),
    L(  -44,  -71,  -44,  -71),
};

EvalScore4i bishop_pawn_same_color_table_e[9]{
    L(   11,    5,   11,    5),
    L(   -3,   11,   -3,   11),
    L(   -7,    6,   -7,    6),
    L(  -13,    3,  -13,    3),
    L(  -16,   -3,  -16,   -3),
    L(  -19,  -17,  -19,  -17),
    L(  -25,  -23,  -25,  -23),
    L(  -26,  -22,  -26,  -22),
    L(  -33,  -21,  -33,  -21),
};



EvalScore4i SIDE_TO_MOVE                  = L(  15,  19,  10,   8);
EvalScore4i PAWN_STRUCTURE                = L(  27, -10,   6,  13);
EvalScore4i PAWN_PASSED                   = L(   4,  67,   6,  40);
EvalScore4i PAWN_ISOLATED                 = L( -13, -23,  -1,  -3);
EvalScore4i PAWN_DOUBLED                  = L(  -5,  -7,  -6,   1);
EvalScore4i PAWN_DOUBLED_AND_ISOLATED     = L(  -1, -41, -10, -11);
EvalScore4i PAWN_BACKWARD                 = L(  -4,  13, -12, -15);
EvalScore4i PAWN_OPEN                     = L( -12, -22,  -8,  13);
EvalScore4i PAWN_BLOCKED                  = L(  -7, -28,  -7, -14);
EvalScore4i KNIGHT_OUTPOST                = L(  40,   0,  17,  35);
EvalScore4i KNIGHT_DISTANCE_ENEMY_KING    = L( -18,   1,  -5,   4);
EvalScore4i ROOK_OPEN_FILE                = L(  48, -31,  16,  31);
EvalScore4i ROOK_HALF_OPEN_FILE           = L(  -3, -14,  -1,  -2);
EvalScore4i ROOK_KING_LINE                = L(  67,  -2, -27,  16);
EvalScore4i BISHOP_DOUBLED                = L(  -3,  50,  22, 113);
EvalScore4i BISHOP_FIANCHETTO             = L(  22,  57,  -8, -23);
EvalScore4i BISHOP_PIECE_SAME_SQUARE_E    = L(   1,   3,   1,   5);
EvalScore4i QUEEN_DISTANCE_ENEMY_KING     = L( -11, -29,   5, -16);
EvalScore4i KING_CLOSE_OPPONENT           = L( -58,  58,  22,  19);
EvalScore4i KING_PAWN_SHIELD              = L(  56,  16,  22, -11);
EvalScore4i CASTLING_RIGHTS               = L(   6,  -6,  29, -11);

EvalScore4i kingSafetyTable[100] {
    L( -18,  -6, -18,  -6), L(   0,   0,   0,   0), L( -22,  -4, -22,  -4), L( -12, -10, -12, -10), L( -14,  -6, -14,  -6),
    L(  14,  -8,  14,  -8), L(  10, -16,  10, -16), L(  26,  -4,  26,  -4), L(  14,  -2,  14,  -2), L(  34, -18,  34, -18),
    L(  68, -14,  68, -14), L(  78, -16,  78, -16), L(  36,   8,  36,   8), L(  96, -14,  96, -14), L( 122,   2, 122,   2),
    L( 120,  -6, 120,  -6), L(  94,   6,  94,   6), L( 180, -10, 180, -10), L( 234, -46, 234, -46), L( 214, -26, 214, -26),
    L( 232, -54, 232, -54), L( 256, -36, 256, -36), L( 320, -38, 320, -38), L( 304, -40, 304, -40), L( 298,  10, 298,  10),
    L( 346,  10, 346,  10), L( 410,   0, 410,   0), L( 434, 124, 434, 124), L( 476,  -2, 476,  -2), L( 464,  16, 464,  16),
    L( 500, 500, 500, 500), L( 500, 294, 500, 294), L( 500,  36, 500,  36), L( 500, 500, 500, 500), L( 500, 500, 500, 500),
    L( 500, 352, 500, 352), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 494, 416, 494, 416), L( 500, 500, 500, 500),
    L( 314,  90, 314,  90), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500),
    L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L(-500,-500,-500,-500),
    L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500),
    L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500),
    L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500),
    L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500),
    L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500),
    L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500),
    L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500),
    L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500),
    L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500),
    L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500), L( 500, 500, 500, 500),
};


EvalScore4i * evfeatures[] {
    &SIDE_TO_MOVE,
    &PAWN_STRUCTURE,
    &PAWN_PASSED,
    &PAWN_ISOLATED,
    &PAWN_DOUBLED,
    &PAWN_DOUBLED_AND_ISOLATED,
    &PAWN_BACKWARD,
    &PAWN_OPEN,
    &PAWN_BLOCKED,                  // 8
    
    &KNIGHT_OUTPOST,
    &KNIGHT_DISTANCE_ENEMY_KING,    // 10
    
    &ROOK_OPEN_FILE,
    &ROOK_HALF_OPEN_FILE,
    &ROOK_KING_LINE,                // 13
    
    &BISHOP_DOUBLED,
    &BISHOP_FIANCHETTO,
    &BISHOP_PIECE_SAME_SQUARE_E,    // 17
    
    &QUEEN_DISTANCE_ENEMY_KING,     // 18
    
    &KING_CLOSE_OPPONENT,
    &KING_PAWN_SHIELD,              // 20
    
    &CASTLING_RIGHTS,               // 21
    
    
};



int mobEntryCount[6] {0, 9, 14, 15, 28, 0};

float* phaseValues = new float[6] {
    0, 1, 1, 2, 4, 0,
};


EvalScore4i* mobilities[6] {nullptr, mobilityKnight, mobilityBishop, mobilityRook, mobilityQueen, nullptr};




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
 
    return interpolateD2(SIDE_TO_MOVE, 2 * phase - 1, 0);
}

EvalScore4i Evaluator::computeHangingPieces(Board* b) {
    U64 WnotAttacked = ~b->getAttackedSquares(WHITE);
    U64 BnotAttacked = ~b->getAttackedSquares(BLACK);

    EvalScore4i res = L(0, 0, 0, 0);

    for (int i = PAWN; i <= QUEEN; i++) {
        res += hangingEval[i]
               * (+bitCount(b->getPieces(WHITE, i) & WnotAttacked) - bitCount(b->getPieces(BLACK, i) & BnotAttacked));
    }
    return res;
}

EvalScore4i Evaluator::computePinnedPieces(Board* b, Color color) {
    
    
    EvalScore4i result = 0;
    
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

    U64 whiteKingZone = KING_ATTACKS[whiteKingSquare];
    U64 blackKingZone = KING_ATTACKS[blackKingSquare];

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
    
    int wkingSafety_attPiecesCount = 0;
    int wkingSafety_valueOfAttacks = 0;

    int bkingSafety_attPiecesCount = 0;
    int bkingSafety_valueOfAttacks = 0;
    /**********************************************************************************
     *                                  P A W N S                                     *
     **********************************************************************************/

    U64 whitePawns = b->getPieces()[WHITE_PAWN];
    U64 blackPawns = b->getPieces()[BLACK_PAWN];
    
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
    
    
    EvalScore4i evalScore       = L(0, 0, 0, 0);
    EvalScore4i featureScore    = L(0, 0, 0, 0);
    EvalScore4i mobScore        = L(0, 0, 0, 0);
    EvalScore4i materialScore   = L(0, 0, 0, 0);
    
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
        materialScore += passer_rank_n[getBit(whiteBlockedPawns, square) * 8 + rankIndex(square)];
        k = lsbReset(k);
    }
    k = blackPassers;
    while (k) {
        square = bitscanForward(k);
        materialScore -= passer_rank_n[getBit(blackBlockedPawns, square) * 8 + 7 - rankIndex(square)];
        k = lsbReset(k);
    }

    U64 whitePawnEastCover = shiftNorthEast(whitePawns) & whitePawns;
    U64 whitePawnWestCover = shiftNorthWest(whitePawns) & whitePawns;
    U64 blackPawnEastCover = shiftSouthEast(blackPawns) & blackPawns;
    U64 blackPawnWestCover = shiftSouthWest(blackPawns) & blackPawns;

    U64 whitePawnCover = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
    U64 blackPawnCover = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);

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
    
    
    /*
     * only these squares are counted for mobility
     */
    U64 mobilitySquaresWhite = ~whiteTeam & ~(blackPawnCover);
    U64 mobilitySquaresBlack = ~blackTeam & ~(whitePawnCover);
    // clang-format on
    
   
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
        attacks = lookUpBishopAttack(square, occupied);
        
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_BISHOP][square];
        mobScore += mobilityBishop[bitCount(attacks & mobilitySquaresWhite)];
    
        materialScore += bishop_pawn_same_color_table_e[bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
        materialScore += bishop_pawn_same_color_table_o[bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
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
        attacks = lookUpBishopAttack(square, occupied);
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_BISHOP][square];
    
        mobScore -= mobilityBishop[bitCount(attacks & mobilitySquaresBlack)];
        materialScore -= bishop_pawn_same_color_table_e[bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
        materialScore -= bishop_pawn_same_color_table_o[bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
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
        attacks = lookUpRookAttack(square, occupied);
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_ROOK][square];
        mobScore += mobilityRook[bitCount(attacks & mobilitySquaresWhite)];

        addToKingSafety(attacks, blackKingZone, bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 3);

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied);
    
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
        attacks = lookUpRookAttack(square, occupied) | lookUpBishopAttack(square, occupied);
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_QUEEN][square];
        mobScore += mobilityQueen[bitCount(attacks & mobilitySquaresWhite)];
        featureScore += QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, blackKingSquare);

        addToKingSafety(attacks, blackKingZone, bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 4);

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied) | lookUpBishopAttack(square, occupied);
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_QUEEN][square];
        mobScore -= mobilityQueen[bitCount(attacks & mobilitySquaresBlack)];
        featureScore -= QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, whiteKingSquare);

        addToKingSafety(attacks, whiteKingZone, wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, 4);

        k = lsbReset(k);
    }
    
    //showScore(materialScore);
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
    
    
    EvalScore4i hangingEvalScore = computeHangingPieces(b);
    EvalScore4i pinnedEvalScore  = computePinnedPieces(b, WHITE) - computePinnedPieces(b, BLACK);

    evalScore += kingSafetyTable[bkingSafety_valueOfAttacks] - kingSafetyTable[wkingSafety_valueOfAttacks];
   
    // clang-format off
    featureScore += CASTLING_RIGHTS*(
            + b->getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
            + b->getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING));
    // clang-format on
    featureScore += SIDE_TO_MOVE * (b->getActivePlayer() == WHITE ? 1 : -1);

    
    EvalScore4i totalScore2 = evalScore
                            + mobScore
                            + materialScore
                            + pinnedEvalScore
                            + hangingEvalScore
                            + featureScore;
    
    double interpolationScalar2 = bitCount(whitePawns | blackPawns) / 8.0 - 1.0;
    
    double v2 =  interpolateD2(totalScore2, 2 * phase-1, interpolationScalar2);
    res += v2;
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



