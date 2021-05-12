
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                                   by. Kim Kahre and Finn Eggers                                  *
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
#include "UCIAssert.h"

#include <immintrin.h>
#include <iomanip>

EvalScore SIDE_TO_MOVE                  = M(   12,   16);
EvalScore PAWN_STRUCTURE                = M(   10,    3);
EvalScore PAWN_PASSED                   = M(    8,   49);
EvalScore PAWN_ISOLATED                 = M(   -2,  -14);
EvalScore PAWN_DOUBLED                  = M(   -7,   -5);
EvalScore PAWN_DOUBLED_AND_ISOLATED     = M(   -7,  -26);
EvalScore PAWN_BACKWARD                 = M(  -11,    1);
EvalScore PAWN_OPEN                     = M(   -8,   -7);
EvalScore PAWN_BLOCKED                  = M(   -4,  -15);
EvalScore KNIGHT_OUTPOST                = M(   24,   18);
EvalScore KNIGHT_DISTANCE_ENEMY_KING    = M(   -5,   -1);
EvalScore ROOK_OPEN_FILE                = M(   23,   -2);
EvalScore ROOK_HALF_OPEN_FILE           = M(    1,  -12);
EvalScore ROOK_KING_LINE                = M(   15,    1);
EvalScore BISHOP_DOUBLED                = M(   16,   79);
EvalScore BISHOP_FIANCHETTO             = M(   -4,    4);
EvalScore BISHOP_PIECE_SAME_SQUARE_E    = M(    3,    3);
EvalScore QUEEN_DISTANCE_ENEMY_KING     = M(    2,  -20);
EvalScore KING_CLOSE_OPPONENT           = M(  -19,   15);
EvalScore KING_PAWN_SHIELD              = M(   30,   15);
EvalScore CASTLING_RIGHTS               = M(   20,   -4);
EvalScore MINOR_BEHIND_PAWN             = M(    6,   24);
EvalScore SAFE_QUEEN_CHECK              = M(    6,   20);
EvalScore SAFE_ROOK_CHECK               = M(   12,    4);
EvalScore SAFE_BISHOP_CHECK             = M(   10,    3);
EvalScore SAFE_KNIGHT_CHECK             = M(   12,    4);
EvalScore PAWN_ATTACK_MINOR             = M(   41,   75);
EvalScore PAWN_ATTACK_ROOK              = M(   42,   30);
EvalScore PAWN_ATTACK_QUEEN             = M(   33,   26);
EvalScore MINOR_ATTACK_ROOK             = M(   37,   24);
EvalScore MINOR_ATTACK_QUEEN            = M(   26,   39);
EvalScore ROOK_ATTACK_QUEEN             = M(   34,   13);

EvalScore mobilityKnight[9] = {
    M(  -50,   -5), M(  -38,   49), M(  -33,   80), M(  -29,   98), M(  -24,  110),
    M(  -20,  121), M(  -13,  122), M(   -4,  116), M(   10,  101), };

EvalScore mobilityBishop[14] = {
    M(  -10,  -37), M(    1,   33), M(    9,   67), M(   14,   88), M(   20,  106),
    M(   25,  120), M(   26,  128), M(   26,  133), M(   28,  138), M(   32,  137),
    M(   39,  132), M(   55,  125), M(   65,  134), M(   89,  106), };

EvalScore mobilityRook[15] = {
    M(  -45,   61), M(  -38,  105), M(  -34,  143), M(  -33,  172), M(  -32,  188),
    M(  -26,  195), M(  -20,  203), M(  -12,  206), M(   -6,  211), M(    0,  217),
    M(    6,  221), M(   10,  225), M(   20,  223), M(   44,  207), M(  102,  176), };

EvalScore mobilityQueen[28] = {
    M( -188,   94), M( -171,   97), M( -158,  225), M( -154,  307), M( -152,  352),
    M( -151,  383), M( -150,  407), M( -147,  424), M( -146,  437), M( -143,  444),
    M( -141,  452), M( -138,  457), M( -137,  460), M( -137,  465), M( -137,  469),
    M( -138,  470), M( -138,  470), M( -139,  469), M( -139,  467), M( -132,  458),
    M( -124,  446), M( -121,  437), M( -127,  431), M( -110,  416), M( -173,  439),
    M(  -45,  361), M(  -81,  408), M( -122,  432), };

EvalScore hangingEval[5] = {
    M(   -3,   -1), M(   -4,   -1), M(   -6,   -7), M(   -5,   -5), M(   -5,   -6), };

EvalScore pinnedEval[15] = {
    M(    1,   -6), M(   -7,    8), M(   -6,   23), M(  -20,  -61), M(  -23,  -13),
    M(  -16,   10), M(   -0,   -6), M(  -29,  -14), M(  -16,   -5), M(  -10,  -11),
    M(    5,  -10), M(  -13,   -2), M(   13,  -26), M(  -10,  -39), M(  -12,    6), };

EvalScore passer_rank_n[16] = {
    M(    0,    0), M(  -16,  -32), M(  -20,  -28), M(  -15,    1),
    M(   15,   27), M(   37,   84), M(   31,   47), M(    0,    0),
    M(    0,    0), M(    1,    2), M(  -38,  -60), M(  -35,  -16),
    M(  -23,  -13), M(   27,   12), M(  110, -116), M(    0,    0), };

EvalScore bishop_pawn_same_color_table_o[9] = {
    M(  -35,   42), M(  -41,   43), M(  -41,   29),
    M(  -44,   19), M(  -48,    8), M(  -53,   -5),
    M(  -56,  -23), M(  -57,  -42), M(  -67,  -94), };

EvalScore bishop_pawn_same_color_table_e[9] = {
    M(  -24,   37), M(  -40,   47), M(  -45,   40),
    M(  -50,   33), M(  -54,   24), M(  -58,   10),
    M(  -60,   -8), M(  -59,  -22), M(  -66,  -36), };

EvalScore kingSafetyTable[100] = {
    M(   -7,   -6), M(    0,    0), M(  -11,   -7), M(   -4,  -10), M(   -8,   -8),
    M(   21,  -12), M(    7,  -13), M(   30,  -14), M(   13,  -14), M(   43,  -23),
    M(   59,  -17), M(   76,  -27), M(   37,  -30), M(  106,  -27), M(  101,  -26),
    M(  103,  -19), M(   94,  -52), M(  168,  -24), M(  187,  -48), M(  202,  -66),
    M(  222, -123), M(  182,  -45), M(  285,  -74), M(  237,  -47), M(  266,  -59),
    M(  272,  -20), M(  364,  -42), M(  378,  -94), M(  309,   63), M(  423,  -74),
    M(  445, -182), M(  505, -157), M(  573, -293), M(  660, -263), M(  518,  -91),
    M( 2034,-3714), M(   61, 1087), M( 1858,-2614), M(  412,  343), M( 1076,  851),
    M( 1280,-1913), M( 1028, -760), M( 1573, 1324), M(  500,  500), M(  501,  500),
    M( 1712, 1486), M(  500,  500), M(  908,  687), M(  500,  500), M( -509, -513),
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
    &SAFE_QUEEN_CHECK,
    &SAFE_ROOK_CHECK,
    &SAFE_BISHOP_CHECK,
    &SAFE_KNIGHT_CHECK,
    
    &PAWN_ATTACK_MINOR,
    &PAWN_ATTACK_ROOK,
    &PAWN_ATTACK_QUEEN,

    &MINOR_ATTACK_ROOK,
    &MINOR_ATTACK_QUEEN,
    &ROOK_ATTACK_QUEEN,
};

int mobEntryCount[N_PIECE_TYPES] {0, 9, 14, 15, 28, 0};

float* phaseValues = new float[6] {
    0, 1, 1, 2, 4, 0,
};


int lazyEvalAlphaBound = 803;
int lazyEvalBetaBound  = 392;

EvalScore* mobilities[N_PIECE_TYPES] {nullptr, mobilityKnight, mobilityBishop, mobilityRook, mobilityQueen, nullptr};

/**
 * adds the factor to value of attacks if the piece attacks the kingzone
 * @param attacks
 * @param kingZone
 * @param pieceCount
 * @param valueOfAttacks
 * @param factor
 */

bool hasMatingMaterial(Board* b, bool side) {
    UCI_ASSERT(b);

    if ((b->getPieceBB()[QUEEN + side * 8] | b->getPieceBB()[ROOK + side * 8] | b->getPieceBB()[PAWN + side * 8])
        || (bitCount(b->getPieceBB()[BISHOP + side * 8] | b->getPieceBB()[KNIGHT + side * 8]) > 1 && b->getPieceBB()[BISHOP + side * 8]))
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
        if (((whitePassedPawnMask[s] & ~FILES_BB[fileIndex(s)]) & opponentPawns) == 0 && (sq & pawnCover)) {
            return true;
        }
    } else {
        if (((blackPassedPawnMask[s] & ~FILES_BB[fileIndex(s)]) & opponentPawns) == 0 && (sq & pawnCover)) {
            return true;
        }
    }
    return false;
}

bb::Score Evaluator::evaluateTempo(Board* b) {
    UCI_ASSERT(b);

    phase = (24.0f + phaseValues[5] - phaseValues[0] * bitCount(b->getPieceBB()[WHITE_PAWN] | b->getPieceBB()[BLACK_PAWN])
             - phaseValues[1] * bitCount(b->getPieceBB()[WHITE_KNIGHT] | b->getPieceBB()[BLACK_KNIGHT])
             - phaseValues[2] * bitCount(b->getPieceBB()[WHITE_BISHOP] | b->getPieceBB()[BLACK_BISHOP])
             - phaseValues[3] * bitCount(b->getPieceBB()[WHITE_ROOK] | b->getPieceBB()[BLACK_ROOK])
             - phaseValues[4] * bitCount(b->getPieceBB()[WHITE_QUEEN] | b->getPieceBB()[BLACK_QUEEN]))
            / 24.0f;

    if (phase > 1)
        phase = 1;
    if (phase < 0)
        phase = 0;

    return MgScore(SIDE_TO_MOVE) * (1 - phase) + EgScore(SIDE_TO_MOVE) * (phase);
}

EvalScore Evaluator::computeHangingPieces(Board* b) {
    UCI_ASSERT(b);


    U64 WnotAttacked = ~evalData.allAttacks[WHITE];
    U64 BnotAttacked = ~evalData.allAttacks[BLACK];

    EvalScore res = M(0, 0);

    for (int i = PAWN; i <= QUEEN; i++) {
        res += hangingEval[i] * (+ bitCount(b->getPieceBB(WHITE, i) & WnotAttacked)
                                 - bitCount(b->getPieceBB(BLACK, i) & BnotAttacked));
    }
    return res;
}

EvalScore Evaluator::computePinnedPieces(Board* b, Color color) {
    UCI_ASSERT(b);

    EvalScore result = 0;

    Color us   = color;
    Color them = 1 - color;

    // figure out where the opponent has pieces
    U64 opponentOcc = b->getTeamOccupiedBB()[them];
    U64 ourOcc      = b->getTeamOccupiedBB()[us];

    // get the pieces which can pin our king
    U64 bishops = b->getPieceBB(them, BISHOP);
    U64 rooks   = b->getPieceBB(them, ROOK);
    U64 queens  = b->getPieceBB(them, QUEEN);

    // get the king positions
    Square kingSq = bitscanForward(b->getPieceBB(us, KING));

    // get the potential pinners for rook/bishop attacks
    U64 rookAttacks   = lookUpRookAttack(kingSq, opponentOcc) & (rooks | queens);
    U64 bishopAttacks = lookUpBishopAttack(kingSq, opponentOcc) & (bishops | queens);

    // get all pinners (either rook or bishop attackers)
    U64 potentialPinners = (rookAttacks | bishopAttacks);

    while (potentialPinners) {

        Square pinnerSquare = bitscanForward(potentialPinners);

        // get all the squares in between the king and the potential pinner
        U64 inBetween = inBetweenSquares[kingSq][pinnerSquare];

        // if there is exactly one of our pieces in the way, consider it pinned. Otherwise, continue
        U64 potentialPinned = ourOcc & inBetween;
        if (potentialPinned == 0 || lsbIsolation(potentialPinned) != potentialPinned) {
            potentialPinners = lsbReset(potentialPinners);
            continue;
        }

        // extract the pinner pieces and the piece that pins
        Piece pinnedPiece = b->getPiece(bitscanForward(potentialPinned));
        Piece pinnerPiece = b->getPiece(pinnerSquare) - BISHOP;

        // normalise the values (black pieces will be made to white pieces)
        if (us == WHITE) {
            pinnerPiece -= 8;
        } else {
            pinnedPiece -= 8;
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
bb::Score Evaluator::evaluate(Board* b, Score alpha, Score beta) {
    UCI_ASSERT(b);

    Score res = 0;

    EvalScore evalScore     = M(0, 0);
    EvalScore featureScore  = M(0, 0);
    EvalScore mobScore      = M(0, 0);
    EvalScore materialScore = b->getBoardStatus()->material();

    phase = (24.0f + phaseValues[5] - phaseValues[0] * bitCount(b->getPieceBB()[WHITE_PAWN] | b->getPieceBB()[BLACK_PAWN])
             - phaseValues[1] * bitCount(b->getPieceBB()[WHITE_KNIGHT] | b->getPieceBB()[BLACK_KNIGHT])
             - phaseValues[2] * bitCount(b->getPieceBB()[WHITE_BISHOP] | b->getPieceBB()[BLACK_BISHOP])
             - phaseValues[3] * bitCount(b->getPieceBB()[WHITE_ROOK] | b->getPieceBB()[BLACK_ROOK])
             - phaseValues[4] * bitCount(b->getPieceBB()[WHITE_QUEEN] | b->getPieceBB()[BLACK_QUEEN]))
            / 24.0f;

    if (phase > 1)
        phase = 1;
    if (phase < 0)
        phase = 0;

    // lazy eval
    res += (int) ((float) MgScore(materialScore) * (1 - phase));
    res += (int) ((float) EgScore(materialScore) * (phase));
    
    Score lazyScore = res * ((b->getActivePlayer() == WHITE) ? 1 : -1);
    if(lazyScore < alpha - lazyEvalAlphaBound){
        return res;
    }
    if(lazyScore > beta + lazyEvalBetaBound){
        return res;
    }
    

    U64 whiteTeam = b->getTeamOccupiedBB()[WHITE];
    U64 blackTeam = b->getTeamOccupiedBB()[BLACK];
    U64 occupied  = *b->getOccupiedBB();

    Square whiteKingSquare = bitscanForward(b->getPieceBB()[WHITE_KING]);
    Square blackKingSquare = bitscanForward(b->getPieceBB()[BLACK_KING]);

    evalData = {};
    evalData.kingZone[WHITE] = KING_ATTACKS[whiteKingSquare];
    evalData.kingZone[BLACK] = KING_ATTACKS[blackKingSquare];

    int wkingSafety_attPiecesCount = 0;
    int wkingSafety_valueOfAttacks = 0;

    int bkingSafety_attPiecesCount = 0;
    int bkingSafety_valueOfAttacks = 0;

    U64 wKingBishopAttacks = lookUpBishopAttack(whiteKingSquare, occupied) & ~blackTeam;
    U64 bKingBishopAttacks = lookUpBishopAttack(blackKingSquare, occupied) & ~whiteTeam;
    U64 wKingRookAttacks   = lookUpRookAttack(whiteKingSquare, occupied) & ~blackTeam;
    U64 bKingRookAttacks   = lookUpRookAttack(blackKingSquare, occupied) & ~whiteTeam;
    U64 wKingKnightAttacks = KNIGHT_ATTACKS[whiteKingSquare] & ~blackTeam;
    U64 bKingKnightAttacks = KNIGHT_ATTACKS[blackKingSquare] & ~whiteTeam;

    /**********************************************************************************
     *                                  P A W N S                                     *
     **********************************************************************************/

    U64 whitePawns = b->getPieceBB()[WHITE_PAWN];
    U64 blackPawns = b->getPieceBB()[BLACK_PAWN];

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

    Square square;
    U64    attacks;
    U64    k;
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

    evalData.attacks[WHITE][PAWN] = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
    evalData.attacks[BLACK][PAWN] = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);
    U64 whitePawnCover            = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
    U64 blackPawnCover            = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);

    U64 mobilitySquaresWhite = ~whiteTeam & ~(blackPawnCover);
    U64 mobilitySquaresBlack = ~blackTeam & ~(whitePawnCover);

    // clang-format off
    featureScore += PAWN_ATTACK_MINOR * (
            + bitCount(evalData.attacks[WHITE][PAWN] & (b->getPieceBB<BLACK>(KNIGHT) | b->getPieceBB<BLACK>(BISHOP)))
            - bitCount(evalData.attacks[BLACK][PAWN] & (b->getPieceBB<WHITE>(KNIGHT) | b->getPieceBB<WHITE>(BISHOP))));
    featureScore += PAWN_ATTACK_ROOK  * (
            + bitCount(evalData.attacks[WHITE][PAWN] & b->getPieceBB<BLACK>(ROOK))
            - bitCount(evalData.attacks[BLACK][PAWN] & b->getPieceBB<WHITE>(ROOK)));
    featureScore += PAWN_ATTACK_QUEEN * (
            + bitCount(evalData.attacks[WHITE][PAWN] & b->getPieceBB<BLACK>(QUEEN))
            - bitCount(evalData.attacks[BLACK][PAWN] & b->getPieceBB<WHITE>(QUEEN)));
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
            + bitCount(shiftNorth(b->getPieceBB()[WHITE_KNIGHT]|b->getPieceBB()[WHITE_BISHOP])&(b->getPieceBB()[WHITE_PAWN]|b->getPieceBB()[BLACK_PAWN]))
            - bitCount(shiftSouth(b->getPieceBB()[BLACK_KNIGHT]|b->getPieceBB()[BLACK_BISHOP])&(b->getPieceBB()[WHITE_PAWN]|b->getPieceBB()[BLACK_PAWN])));
    

    
   
    /**********************************************************************************
     *                                  K N I G H T S                                 *
     **********************************************************************************/
    k = b->getPieceBB()[WHITE_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[square];
        evalData.attacks[WHITE][KNIGHT] |= attacks;
     
        
        mobScore        += mobilityKnight[bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresWhite)];

        featureScore    += MINOR_ATTACK_ROOK            * bitCount(attacks & b->getPieceBB<BLACK>(ROOK));
        featureScore    += MINOR_ATTACK_QUEEN           * bitCount(attacks & b->getPieceBB<BLACK>(QUEEN));
        featureScore    += KNIGHT_OUTPOST               * isOutpost(square, WHITE, blackPawns, whitePawnCover);
        featureScore    += KNIGHT_DISTANCE_ENEMY_KING   * manhattanDistance(square, blackKingSquare);
        featureScore    += SAFE_KNIGHT_CHECK            * bitCount(bKingKnightAttacks & attacks & ~blackPawnCover);

        addToKingSafety(attacks, evalData.kingZone[BLACK], bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 2);

        k = lsbReset(k);
    }

    k = b->getPieceBB()[BLACK_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[square];
        evalData.attacks[BLACK][KNIGHT] |= attacks;
    
        mobScore        -= mobilityKnight[bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresBlack)];

        featureScore    -= MINOR_ATTACK_ROOK            * bitCount(attacks & b->getPieceBB<WHITE>(ROOK));
        featureScore    -= MINOR_ATTACK_QUEEN           * bitCount(attacks & b->getPieceBB<WHITE>(QUEEN));
        featureScore    -= KNIGHT_OUTPOST               * isOutpost(square, BLACK, whitePawns, blackPawnCover);
        featureScore    -= KNIGHT_DISTANCE_ENEMY_KING   * manhattanDistance(square, whiteKingSquare);
        featureScore    -= SAFE_KNIGHT_CHECK            * bitCount(wKingKnightAttacks & attacks & ~whitePawnCover);
    
        addToKingSafety(attacks, evalData.kingZone[WHITE], wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, 2);

        k = lsbReset(k);
    }
  
    /**********************************************************************************
     *                                  B I S H O P S                                 *
     **********************************************************************************/

    k = b->getPieceBB()[WHITE_BISHOP];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpBishopAttack(square, occupied & ~b->getPieceBB()[WHITE_QUEEN]);
        evalData.attacks[WHITE][BISHOP] |= attacks;
        
        mobScore        += mobilityBishop[bitCount(attacks & mobilitySquaresWhite)];

        featureScore    += MINOR_ATTACK_ROOK    * bitCount(attacks & b->getPieceBB<BLACK>(ROOK));
        featureScore    += MINOR_ATTACK_QUEEN   * bitCount(attacks & b->getPieceBB<BLACK>(QUEEN));
        featureScore    += bishop_pawn_same_color_table_e[bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB))];
        featureScore    += bishop_pawn_same_color_table_o[bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB))];
        featureScore    += BISHOP_PIECE_SAME_SQUARE_E
                           * bitCount(blackTeam & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB));
        featureScore    += BISHOP_FIANCHETTO
                           * (square == G2 && whitePawns & ONE << F2 && whitePawns & ONE << H2
                              && whitePawns & (ONE << G3 | ONE << G4));
        featureScore    += BISHOP_FIANCHETTO
                           * (square == B2 && whitePawns & ONE << A2 && whitePawns & ONE << C2
                              && whitePawns & (ONE << B3 | ONE << B4));
                        
        featureScore    += SAFE_BISHOP_CHECK * bitCount(bKingBishopAttacks & attacks & ~blackPawnCover);
        addToKingSafety(attacks,  evalData.kingZone[BLACK], bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 2);

        k = lsbReset(k);
    }

    k = b->getPieceBB()[BLACK_BISHOP];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpBishopAttack(square, occupied & ~b->getPieceBB()[BLACK_QUEEN]);
        evalData.attacks[BLACK][BISHOP] |= attacks;
    
        mobScore        -= mobilityBishop[bitCount(attacks & mobilitySquaresBlack)];

        featureScore    -= MINOR_ATTACK_ROOK * bitCount(attacks & b->getPieceBB<WHITE>(ROOK));
        featureScore    -= MINOR_ATTACK_QUEEN * bitCount(attacks & b->getPieceBB<WHITE>(QUEEN));
        featureScore    -= bishop_pawn_same_color_table_e[bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB))];
        featureScore    -= bishop_pawn_same_color_table_o[bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB))];
        featureScore    -= BISHOP_PIECE_SAME_SQUARE_E
                           * bitCount(whiteTeam & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB));
        featureScore    -= BISHOP_FIANCHETTO
                           * (square == G7 && blackPawns & ONE << F7 && blackPawns & ONE << H7
                              && blackPawns & (ONE << G6 | ONE << G5));
        featureScore    -= BISHOP_FIANCHETTO
                           * (square == B2 && blackPawns & ONE << A7 && blackPawns & ONE << C7
                              && blackPawns & (ONE << B6 | ONE << B5));
                        
        featureScore    -= SAFE_BISHOP_CHECK * bitCount(wKingBishopAttacks & attacks & ~whitePawnCover);
        addToKingSafety(attacks, evalData.kingZone[WHITE], wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, 2);

        k = lsbReset(k);
    }
    // clang-format off
    featureScore += BISHOP_DOUBLED * (
            + (bitCount(b->getPieceBB()[WHITE_BISHOP]) == 2)
            - (bitCount(b->getPieceBB()[BLACK_BISHOP]) == 2));
    // clang-format on

    /**********************************************************************************
     *                                  R O O K S                                     *
     **********************************************************************************/

    k = b->getPieceBB()[WHITE_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied & ~b->getPieceBB()[WHITE_ROOK] & ~b->getPieceBB()[WHITE_QUEEN]);
        evalData.attacks[WHITE][ROOK] |= attacks;

        mobScore += mobilityRook[bitCount(attacks & mobilitySquaresWhite)];

        featureScore += ROOK_ATTACK_QUEEN * bitCount(attacks & b->getPieceBB<BLACK>(QUEEN));
        featureScore += SAFE_ROOK_CHECK * bitCount(bKingRookAttacks & attacks & ~blackPawnCover);

        addToKingSafety(attacks, evalData.kingZone[BLACK], bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 3);

        k = lsbReset(k);
    }

    k = b->getPieceBB()[BLACK_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied & ~b->getPieceBB()[BLACK_ROOK] & ~b->getPieceBB()[BLACK_QUEEN]);
        evalData.attacks[BLACK][ROOK] |= attacks;

        mobScore -= mobilityRook[bitCount(attacks & mobilitySquaresBlack)];

        featureScore -= ROOK_ATTACK_QUEEN * bitCount(attacks & b->getPieceBB<WHITE>(QUEEN));
        featureScore -= SAFE_ROOK_CHECK * bitCount(wKingRookAttacks & attacks & ~whitePawnCover);

        addToKingSafety(attacks, evalData.kingZone[WHITE], wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, 3);

        k = lsbReset(k);
    }

    // clang-format off
    featureScore += ROOK_KING_LINE * (
            + bitCount(lookUpRookAttack(blackKingSquare, occupied) & b->getPieceBB(WHITE, ROOK))
            - bitCount(lookUpRookAttack(whiteKingSquare, occupied) & b->getPieceBB(BLACK, ROOK)));
    featureScore += ROOK_OPEN_FILE * (
            + bitCount(openFiles & b->getPieceBB(WHITE, ROOK))
            - bitCount(openFiles & b->getPieceBB(BLACK, ROOK)));
    featureScore += ROOK_HALF_OPEN_FILE * (
            + bitCount(openFilesBlack & ~openFiles & b->getPieceBB(WHITE, ROOK))
            - bitCount(openFilesWhite & ~openFiles & b->getPieceBB(BLACK, ROOK)));
    // clang-format on

    /**********************************************************************************
     *                                  Q U E E N S                                   *
     **********************************************************************************/

    k = b->getPieceBB()[WHITE_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied & ~b->getPieceBB()[WHITE_ROOK])
                  | lookUpBishopAttack(square, occupied & ~b->getPieceBB()[WHITE_BISHOP]);
        evalData.attacks[WHITE][QUEEN] |= attacks;

        mobScore += mobilityQueen[bitCount(attacks & mobilitySquaresWhite)];
        featureScore += QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, blackKingSquare);
        featureScore += SAFE_QUEEN_CHECK * bitCount((bKingRookAttacks | bKingBishopAttacks) & attacks & ~blackPawnCover);

        addToKingSafety(attacks, evalData.kingZone[BLACK], bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 4);

        k = lsbReset(k);
    }

    k = b->getPieceBB()[BLACK_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied & ~b->getPieceBB()[BLACK_ROOK])
                  | lookUpBishopAttack(square, occupied & ~b->getPieceBB()[BLACK_BISHOP]);
        evalData.attacks[BLACK][QUEEN] |= attacks;

        mobScore -= mobilityQueen[bitCount(attacks & mobilitySquaresBlack)];
        featureScore -= QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, whiteKingSquare);
        featureScore -= SAFE_QUEEN_CHECK * bitCount((wKingRookAttacks | wKingBishopAttacks) & attacks & ~whitePawnCover);

        addToKingSafety(attacks, evalData.kingZone[WHITE], wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, 4);

        k = lsbReset(k);
    }

    /**********************************************************************************
     *                                  K I N G S                                     *
     **********************************************************************************/
    k = b->getPieceBB()[WHITE_KING];

    while (k) {
        square                        = bitscanForward(k);
        evalData.attacks[WHITE][KING] = KING_ATTACKS[square];

        featureScore += KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & whitePawns);
        featureScore += KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & blackTeam);

        k = lsbReset(k);
    }

    k = b->getPieceBB()[BLACK_KING];
    while (k) {
        square                        = bitscanForward(k);
        evalData.attacks[BLACK][KING] = KING_ATTACKS[square];

        featureScore -= KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & blackPawns);
        featureScore -= KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & whiteTeam);

        k = lsbReset(k);
    }

    for(Piece p = 0; p < 6; p++){
        evalData.allAttacks[WHITE] |= evalData.attacks[WHITE][p];
        evalData.allAttacks[BLACK] |= evalData.attacks[BLACK][p];
    }

    EvalScore hangingEvalScore = computeHangingPieces(b);
    EvalScore pinnedEvalScore  = computePinnedPieces(b, WHITE) - computePinnedPieces(b, BLACK);

    evalScore += kingSafetyTable[bkingSafety_valueOfAttacks] - kingSafetyTable[wkingSafety_valueOfAttacks];

    featureScore += CASTLING_RIGHTS
                    * (+b->getCastlingRights(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
                       + b->getCastlingRights(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
                       - b->getCastlingRights(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
                       - b->getCastlingRights(STATUS_INDEX_BLACK_KINGSIDE_CASTLING));
    featureScore += SIDE_TO_MOVE             * (b->getActivePlayer() == WHITE ? 1 : -1);
    EvalScore totalScore = evalScore + pinnedEvalScore + hangingEvalScore + featureScore + mobScore;

    res += (int) ((float) MgScore(totalScore) * (1 - phase));
    res += (int) ((float) EgScore(totalScore) * (phase));

    if (!hasMatingMaterial(b, res > 0 ? WHITE : BLACK))
        res = res / 10;
    return res;
}

void printEvaluation(Board* board) {
    UCI_ASSERT(board);

    using namespace std;

    Evaluator ev {};
    Score     score = ev.evaluate(board);
    float     phase = ev.getPhase();

    std::cout << setw(15) << right << "evaluation: " << left << setw(8) << score << setw(15) << right << "phase: " << left << setprecision(3) << setw(8) << phase << std::endl;
}

float Evaluator::getPhase() { return phase; }
