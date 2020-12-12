
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
    L(   20,  -50,   20,  -50),
    L(   35,    0,   35,    0),
    L(   41,   28,   41,   28),
    L(   47,   37,   47,   37),
    L(   54,   44,   54,   44),
    L(   59,   53,   59,   53),
    L(   65,   57,   65,   57),
    L(   74,   51,   74,   51),
    L(   83,   39,   83,   39), };

EvalScore4i mobilityBishop[14] = {
    L(   20,  -52,   20,  -52),
    L(   27,   11,   27,   11),
    L(   35,   44,   35,   44),
    L(   41,   61,   41,   61),
    L(   49,   73,   49,   73),
    L(   55,   88,   55,   88),
    L(   56,   99,   56,   99),
    L(   57,  107,   57,  107),
    L(   61,  111,   61,  111),
    L(   66,  112,   66,  112),
    L(   74,  111,   74,  111),
    L(   87,  105,   87,  105),
    L(   91,  125,   91,  125),
    L(  133,   84,  133,   84), };

EvalScore4i mobilityRook[15] = {
    L(   28,   22,   28,   22),
    L(   33,   40,   33,   40),
    L(   38,   58,   38,   58),
    L(   39,   76,   39,   76),
    L(   42,   87,   42,   87),
    L(   49,   93,   49,   93),
    L(   54,  101,   54,  101),
    L(   60,  102,   60,  102),
    L(   66,  106,   66,  106),
    L(   70,  111,   70,  111),
    L(   74,  113,   74,  113),
    L(   79,  116,   79,  116),
    L(   87,  114,   87,  114),
    L(  102,  101,  102,  101),
    L(  171,   67,  171,   67), };

EvalScore4i mobilityQueen[28] = {
    L(  -10, -160,  -10, -160),
    L(    3,  -85,    3,  -85),
    L(   12,   36,   12,   36),
    L(   15,  108,   15,  108),
    L(   18,  150,   18,  150),
    L(   20,  176,   20,  176),
    L(   23,  201,   23,  201),
    L(   25,  219,   25,  219),
    L(   27,  234,   27,  234),
    L(   30,  241,   30,  241),
    L(   34,  247,   34,  247),
    L(   37,  254,   37,  254),
    L(   38,  261,   38,  261),
    L(   38,  268,   38,  268),
    L(   37,  276,   37,  276),
    L(   36,  278,   36,  278),
    L(   37,  282,   37,  282),
    L(   34,  283,   34,  283),
    L(   37,  284,   37,  284),
    L(   44,  278,   44,  278),
    L(   51,  270,   51,  270),
    L(   48,  269,   48,  269),
    L(   39,  264,   39,  264),
    L(   62,  252,   62,  252),
    L(  -15,  286,  -15,  286),
    L(   94,  223,   94,  223),
    L(   17,  291,   17,  291),
    L(   18,  299,   18,  299), };

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
    M(    0,    0), M(   -9,  -29), M(   -9,  -24), M(   -5,    2), M(   22,   23), M(   47,   66), M(    4,   27), M(    0,    0),
    M(    0,    0), M(    1,    2), M(  -44,  -49), M(  -45,  -11), M(  -29,  -14), M(   35,   -7), M(   91, -143), M(    0,    0),
};

EvalScore bishop_pawn_same_color_table_o[9]{
    M(    2,   11),
    M(   -4,   14),
    M(   -4,    4),
    M(   -9,   -4),
    M(  -13,  -14),
    M(  -18,  -30),
    M(  -25,  -42),
    M(  -30,  -51),
    M(  -44,  -71),
};

EvalScore bishop_pawn_same_color_table_e[9]{
    M(   11,    5),
    M(   -3,   11),
    M(   -7,    6),
    M(  -13,    3),
    M(  -16,   -3),
    M(  -19,  -17),
    M(  -25,  -23),
    M(  -26,  -22),
    M(  -33,  -21),
};



EvalScore4i SIDE_TO_MOVE                  = L(   10,   14,   10,   14);
EvalScore4i PAWN_STRUCTURE                = L(   10,    6,   10,    6);
EvalScore4i PAWN_PASSED                   = L(    3,   47,    3,   47);
EvalScore4i PAWN_ISOLATED                 = L(   -4,  -14,   -4,  -14);
EvalScore4i PAWN_DOUBLED                  = L(   -5,   -2,   -5,   -2);
EvalScore4i PAWN_DOUBLED_AND_ISOLATED     = L(   -7,  -23,   -7,  -23);
EvalScore4i PAWN_BACKWARD                 = L(  -12,    1,  -12,    1);
EvalScore4i PAWN_OPEN                     = L(   -5,   -3,   -5,   -3);
EvalScore4i PAWN_BLOCKED                  = L(   -6,  -20,   -6,  -20);
EvalScore4i KNIGHT_OUTPOST                = L(   24,   17,   24,   17);
EvalScore4i KNIGHT_DISTANCE_ENEMY_KING    = L(   -8,    2,   -8,    2);
EvalScore4i ROOK_OPEN_FILE                = L(   29,    0,   29,    0);
EvalScore4i ROOK_HALF_OPEN_FILE           = L(   -2,   -5,   -2,   -5);
EvalScore4i ROOK_KING_LINE                = L(   18,    4,   18,    4);
EvalScore4i BISHOP_DOUBLED                = L(   19,   78,   19,   78);
EvalScore4i BISHOP_FIANCHETTO             = L(   -7,   10,   -7,   10);
EvalScore4i BISHOP_PIECE_SAME_SQUARE_E    = L(    1,    4,    1,    4);
EvalScore4i QUEEN_DISTANCE_ENEMY_KING     = L(    3,  -26,    3,  -26);
EvalScore4i KING_CLOSE_OPPONENT           = L(  -18,   42,  -18,   42);
EvalScore4i KING_PAWN_SHIELD              = L(   27,    5,   27,    5);
EvalScore4i CASTLING_RIGHTS               = L(   25,   -7,   25,   -7);

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
    EvalScore4i featureScore4i  = L(0, 0, 0, 0);
    EvalScore4i mobScore        = L(0, 0, 0, 0);
    EvalScore materialScore     = M(0, 0);
    
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
    featureScore4i += PAWN_DOUBLED_AND_ISOLATED * (
            + bitCount(whiteIsolatedPawns & whiteDoubledPawns)
            - bitCount(blackIsolatedPawns & blackDoubledPawns));
    featureScore4i += PAWN_DOUBLED * (
            + bitCount(~whiteIsolatedPawns & whiteDoubledPawns)
            - bitCount(~blackIsolatedPawns & blackDoubledPawns));
    featureScore4i += PAWN_ISOLATED * (
            + bitCount(whiteIsolatedPawns & ~whiteDoubledPawns)
            - bitCount(blackIsolatedPawns & ~blackDoubledPawns));
    featureScore4i += PAWN_PASSED * (
            + bitCount(whitePassers)
            - bitCount(blackPassers));
    featureScore4i += PAWN_STRUCTURE * (
            + bitCount(whitePawnEastCover)
            + bitCount(whitePawnWestCover)
            - bitCount(blackPawnEastCover)
            - bitCount(blackPawnWestCover));
    featureScore4i += PAWN_OPEN * (
            + bitCount(whitePawns & ~fillSouth(blackPawns))
            - bitCount(blackPawns & ~fillNorth(whitePawns)));
    featureScore4i += PAWN_BACKWARD * (
            + bitCount(fillSouth(~wAttackFrontSpans(whitePawns) & blackPawnCover) & whitePawns)
            - bitCount(fillNorth(~bAttackFrontSpans(blackPawns) & whitePawnCover) & blackPawns));
    featureScore4i += PAWN_BLOCKED * (
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
    
        featureScore4i += KNIGHT_OUTPOST * isOutpost(square, WHITE, blackPawns, whitePawnCover);
        featureScore4i += KNIGHT_DISTANCE_ENEMY_KING * manhattanDistance(square, blackKingSquare);

        addToKingSafety(attacks, blackKingZone, bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 2);

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[square];
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_KNIGHT][square];
        
        mobScore -= mobilityKnight[bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresBlack)];

        featureScore4i -= KNIGHT_OUTPOST * isOutpost(square, BLACK, whitePawns, blackPawnCover);
        featureScore4i -= KNIGHT_DISTANCE_ENEMY_KING * manhattanDistance(square, whiteKingSquare);

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
        featureScore4i += BISHOP_PIECE_SAME_SQUARE_E
                        * bitCount(blackTeam & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
        featureScore4i += BISHOP_FIANCHETTO
                        * (square == G2 && whitePawns & ONE << F2 && whitePawns & ONE << H2
                           && whitePawns & (ONE << G3 | ONE << G4));
        featureScore4i += BISHOP_FIANCHETTO
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
        featureScore4i -= BISHOP_PIECE_SAME_SQUARE_E
                        * bitCount(whiteTeam & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
        featureScore4i -= BISHOP_FIANCHETTO
                        * (square == G7 && blackPawns & ONE << F7 && blackPawns & ONE << H7
                           && blackPawns & (ONE << G6 | ONE << G5));
        featureScore4i -= BISHOP_FIANCHETTO
                        * (square == B2 && blackPawns & ONE << A7 && blackPawns & ONE << C7
                           && blackPawns & (ONE << B6 | ONE << B5));
        addToKingSafety(attacks, whiteKingZone, wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, 2);

        k = lsbReset(k);
    }
    // clang-format off
    featureScore4i += BISHOP_DOUBLED * (
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
    featureScore4i += ROOK_KING_LINE * (
            + bitCount(lookUpRookAttack(blackKingSquare, occupied) & b->getPieces(WHITE, ROOK))
            - bitCount(lookUpRookAttack(whiteKingSquare, occupied) & b->getPieces(BLACK, ROOK)));
    featureScore4i += ROOK_OPEN_FILE * (
            + bitCount(openFiles & b->getPieces(WHITE, ROOK))
            - bitCount(openFiles & b->getPieces(BLACK, ROOK)));
    featureScore4i += ROOK_HALF_OPEN_FILE * (
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
        featureScore4i += QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, blackKingSquare);

        addToKingSafety(attacks, blackKingZone, bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 4);

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied) | lookUpBishopAttack(square, occupied);
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_QUEEN][square];
        mobScore -= mobilityQueen[bitCount(attacks & mobilitySquaresBlack)];
        featureScore4i -= QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, whiteKingSquare);

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
    
    
    
        featureScore4i += KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & whitePawns);
        featureScore4i += KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & blackTeam);

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_KING];
    while (k) {
        square = bitscanForward(k);
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_KING][square];
        featureScore4i -= KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & blackPawns);
        featureScore4i -= KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & whiteTeam);

        k = lsbReset(k);
    }
    
    
    EvalScore4i hangingEvalScore = computeHangingPieces(b);
    EvalScore4i pinnedEvalScore  = computePinnedPieces(b, WHITE) - computePinnedPieces(b, BLACK);

    evalScore += kingSafetyTable[bkingSafety_valueOfAttacks] - kingSafetyTable[wkingSafety_valueOfAttacks];
   
    // clang-format off
    featureScore4i += CASTLING_RIGHTS*(
            + b->getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
            + b->getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING));
    // clang-format on
    featureScore4i += SIDE_TO_MOVE * (b->getActivePlayer() == WHITE ? 1 : -1);
    
    
    EvalScore totalScore = evalScore
                           + M(S1(mobScore), S2(mobScore))
                           + materialScore
                           + M(S1(pinnedEvalScore), S2(pinnedEvalScore))
                           + M(S1(hangingEvalScore), S2(hangingEvalScore))
                           + M(S1(featureScore4i), S2(featureScore4i));

    res += MgScore(totalScore) * (1 - phase) + EgScore(totalScore) * (phase);

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



