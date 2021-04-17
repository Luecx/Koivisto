
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


EvalScore SIDE_TO_MOVE                  = M(   15,   15);
EvalScore PAWN_STRUCTURE                = M(   10,    3);
EvalScore PAWN_PASSED                   = M(    9,   50);
EvalScore PAWN_ISOLATED                 = M(   -1,  -14);
EvalScore PAWN_DOUBLED                  = M(   -7,   -5);
EvalScore PAWN_DOUBLED_AND_ISOLATED     = M(   -7,  -27);
EvalScore PAWN_BACKWARD                 = M(  -12,    1);
EvalScore PAWN_OPEN                     = M(   -8,   -7);
EvalScore PAWN_BLOCKED                  = M(   -4,  -14);
EvalScore KNIGHT_OUTPOST                = M(   24,   17);
EvalScore KNIGHT_DISTANCE_ENEMY_KING    = M(   -5,   -2);
EvalScore ROOK_OPEN_FILE                = M(   24,   -2);
EvalScore ROOK_HALF_OPEN_FILE           = M(    1,  -12);
EvalScore ROOK_KING_LINE                = M(   17,    0);
EvalScore BISHOP_DOUBLED                = M(   16,   79);
EvalScore BISHOP_FIANCHETTO             = M(   -4,    4);
EvalScore BISHOP_PIECE_SAME_SQUARE_E    = M(    3,    3);
EvalScore QUEEN_DISTANCE_ENEMY_KING     = M(    2,  -21);
EvalScore KING_CLOSE_OPPONENT           = M(  -19,   15);
EvalScore KING_PAWN_SHIELD              = M(   30,   15);
EvalScore CASTLING_RIGHTS               = M(   21,   -4);
EvalScore MINOR_BEHIND_PAWN             = M(    5,   24);
EvalScore SAFE_QUEEN_CHECK              = M(    6,   20);
EvalScore SAFE_ROOK_CHECK               = M(   11,    4);
EvalScore SAFE_BISHOP_CHECK             = M(    9,    3);
EvalScore SAFE_KNIGHT_CHECK             = M(   12,    4);
EvalScore PAWN_ATTACK_MINOR             = M(   41,   74);
EvalScore PAWN_ATTACK_ROOK              = M(   41,   30);
EvalScore PAWN_ATTACK_QUEEN             = M(   32,   27);
EvalScore MINOR_ATTACK_ROOK             = M(   37,   24);
EvalScore MINOR_ATTACK_QUEEN            = M(   25,   40);
EvalScore ROOK_ATTACK_QUEEN             = M(   33,   14);
EvalScore SLOW_ATTACK_BUILDUP           = M(    2,    3);
EvalScore SLOW_ATTACK_BUILDUP_NORM      = M(   12,    2);

EvalScore mobilityKnight[9] = {
        M(  -52,   -8), M(  -40,   47), M(  -35,   78), M(  -31,   96), M(  -26,  108),
        M(  -21,  118), M(  -15,  120), M(   -5,  114), M(    9,   99), };

EvalScore mobilityBishop[14] = {
        M(  -10,  -36), M(    1,   34), M(    8,   67), M(   13,   88), M(   19,  104),
        M(   23,  118), M(   24,  126), M(   24,  131), M(   26,  134), M(   30,  133),
        M(   37,  128), M(   53,  121), M(   64,  129), M(   87,  101), };

EvalScore mobilityRook[15] = {
        M(  -43,   63), M(  -36,  107), M(  -33,  144), M(  -33,  173), M(  -31,  188),
        M(  -26,  194), M(  -20,  202), M(  -13,  204), M(   -7,  209), M(   -1,  214),
        M(    4,  217), M(    9,  220), M(   18,  218), M(   43,  202), M(  100,  170), };

EvalScore mobilityQueen[28] = {
        M( -184,   96), M( -168,   98), M( -155,  225), M( -151,  307), M( -150,  352),
        M( -149,  382), M( -148,  406), M( -146,  422), M( -145,  434), M( -142,  441),
        M( -140,  448), M( -138,  452), M( -137,  455), M( -137,  459), M( -137,  462),
        M( -139,  463), M( -139,  462), M( -140,  461), M( -140,  458), M( -133,  448),
        M( -125,  435), M( -122,  425), M( -128,  419), M( -111,  403), M( -174,  426),
        M(  -46,  348), M(  -83,  395), M( -123,  419), };

EvalScore hangingEval[5] = {
        M(   -3,   -1), M(   -4,   -1), M(   -6,   -6), M(   -5,   -6), M(   -4,   -6), };

EvalScore pinnedEval[15] = {
        M(    0,   -6), M(   -7,    8), M(   -5,   23), M(  -20,  -61), M(  -24,  -13),
        M(  -16,   10), M(   -0,   -7), M(  -30,  -14), M(  -16,   -5), M(  -10,  -12),
        M(    4,  -11), M(  -13,   -0), M(   13,  -25), M(  -12,  -38), M(  -12,    7), };

EvalScore passer_rank_n[16] = {
        M(    0,    0), M(  -17,  -33), M(  -22,  -28), M(  -17,    0),
        M(   14,   26), M(   36,   83), M(   31,   45), M(    0,    0),
        M(    0,    0), M(    1,    2), M(  -39,  -61), M(  -36,  -17),
        M(  -24,  -14), M(   27,   11), M(  109, -118), M(    0,    0), };

EvalScore bishop_pawn_same_color_table_o[9] = {
        M(  -35,   40), M(  -41,   41), M(  -41,   28),
        M(  -45,   18), M(  -49,    8), M(  -53,   -5),
        M(  -57,  -23), M(  -57,  -40), M(  -68,  -90), };

EvalScore bishop_pawn_same_color_table_e[9] = {
        M(  -23,   36), M(  -40,   47), M(  -44,   39),
        M(  -50,   32), M(  -54,   23), M(  -58,    9),
        M(  -60,  -10), M(  -59,  -24), M(  -67,  -38), };

EvalScore kingSafetyTable[100] = {
        M(   -7,   -7), M(    0,    0), M(  -12,   -7), M(   -6,  -10), M(  -10,   -9),
        M(   18,  -12), M(    4,  -14), M(   25,  -13), M(   10,  -15), M(   38,  -23),
        M(   55,  -17), M(   71,  -26), M(   33,  -30), M(  101,  -26), M(   97,  -26),
        M(   98,  -18), M(   90,  -52), M(  162,  -25), M(  181,  -48), M(  197,  -66),
        M(  217, -122), M(  176,  -43), M(  281,  -75), M(  231,  -47), M(  260,  -57),
        M(  267,  -20), M(  358,  -42), M(  373,  -95), M(  304,   63), M(  421,  -78),
        M(  445, -199), M(  506, -170), M(  571, -296), M(  666, -282), M(  513,  -94),
        M( 2055,-3770), M(   68, 1077), M( 1866,-2626), M(  381,  398), M( 1086,  854),
        M( 1274,-1916), M( 1039, -798), M( 1580, 1326), M(  500,  500), M(  501,  500),
        M( 1724, 1490), M(  500,  500), M(  912,  688), M(  500,  500), M( -509, -514),
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

    &SLOW_ATTACK_BUILDUP,
    &SLOW_ATTACK_BUILDUP_NORM,
};



int mobEntryCount[N_PIECE_TYPES] {0, 9, 14, 15, 28, 0};

float* phaseValues = new float[6] {
    0, 1, 1, 2, 4, 0,
};


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
        || (bitCount(b->getPieceBB()[BISHOP + side * 8] | b->getPieceBB()[KNIGHT + side * 8]) > 1
            && b->getPieceBB()[BISHOP + side * 8]))
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


bb::Score Evaluator::evaluateTempo(Board* b){
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

EvalScore Evaluator::computeHangingPieces(Board* b, EvalData* evalData) {
    UCI_ASSERT(b);

    U64 WnotAttacked = ZERO;
    U64 BnotAttacked = ZERO;
    for(int i = PAWN; i <= KING; i++){
        WnotAttacked |= evalData->attacks[WHITE][i];
        BnotAttacked |= evalData->attacks[BLACK][i];
    }
    WnotAttacked = ~WnotAttacked;
    BnotAttacked = ~BnotAttacked;
    
    
   
    EvalScore res = M(0, 0);

    for (int i = PAWN; i <= QUEEN; i++) {
        res += hangingEval[i]
               * (+ bitCount(b->getPieceBB(WHITE, i) & WnotAttacked)
                  - bitCount(b->getPieceBB(BLACK, i) & BnotAttacked));
    }
    return res;
}

EvalScore Evaluator::computePinnedPieces(Board* b, Color color) {
    UCI_ASSERT(b);
    
    EvalScore result = 0;
    
    Color us = color;
    Color them = 1 - color;
    
    // figure out where the opponent has pieces
    U64 opponentOcc = b->getTeamOccupiedBB()[them];
    U64      ourOcc = b->getTeamOccupiedBB()[us];
    
    // get the pieces which can pin our king
    U64 bishops = b->getPieceBB(them, BISHOP);
    U64   rooks = b->getPieceBB(them, ROOK);
    U64  queens = b->getPieceBB(them, QUEEN);
    
    // get the king positions
    Square kingSq = bitscanForward(b->getPieceBB(us, KING));
    
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
            pinnerPiece -= 8;
        }else{
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
bb::Score Evaluator::evaluate(Board* b) {
    UCI_ASSERT(b);

    Score res = 0;

    U64 whiteTeam = b->getTeamOccupiedBB()[WHITE];
    U64 blackTeam = b->getTeamOccupiedBB()[BLACK];
    U64 occupied  = *b->getOccupiedBB();

    Square whiteKingSquare = bitscanForward(b->getPieceBB()[WHITE_KING]);
    Square blackKingSquare = bitscanForward(b->getPieceBB()[BLACK_KING]);
    
    EvalData evalData{};


    Square square;
    U64    attacks;
    U64    k;

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

    evalData.kingZone[WHITE] = KING_ATTACKS[whiteKingSquare];
    evalData.kingZone[BLACK] = KING_ATTACKS[blackKingSquare];
    
    int wkingSafety_attPiecesCount = 0;
    int wkingSafety_valueOfAttacks = 0;

    int bkingSafety_attPiecesCount = 0;
    int bkingSafety_valueOfAttacks = 0;
    
    U64 wKingBishopAttacks = lookUpBishopAttack(whiteKingSquare, occupied)  & ~blackTeam;
    U64 bKingBishopAttacks = lookUpBishopAttack(blackKingSquare, occupied)  & ~whiteTeam;
    U64 wKingRookAttacks   = lookUpRookAttack  (whiteKingSquare, occupied)  & ~blackTeam;
    U64 bKingRookAttacks   = lookUpRookAttack  (blackKingSquare, occupied)  & ~whiteTeam;
    U64 wKingKnightAttacks = KNIGHT_ATTACKS    [whiteKingSquare]            & ~blackTeam;
    U64 bKingKnightAttacks = KNIGHT_ATTACKS    [blackKingSquare]            & ~whiteTeam;
    
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

    int centerClosed = bitCount((whiteBlockedPawns | blackBlockedPawns) & CENTER_SQUARES_EXTENDED_BB);

    U64 wKsideMask = fillFile(b->getPieceBB<WHITE>(KING));
        wKsideMask |= shiftEast(wKsideMask);
        wKsideMask |= shiftEast(wKsideMask);
        wKsideMask |= shiftWest(wKsideMask);
        wKsideMask |= shiftWest(wKsideMask);

    U64 bKsideMask = fillFile(b->getPieceBB<BLACK>(KING));
        bKsideMask |= shiftEast(bKsideMask);
        bKsideMask |= shiftEast(bKsideMask);
        bKsideMask |= shiftWest(bKsideMask);
        bKsideMask |= shiftWest(bKsideMask);

    int wKsideAttacks = 0;
    int bKsideAttacks = 0;

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
    
    evalData.attacks[WHITE][PAWN] = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
    evalData.attacks[BLACK][PAWN] = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);
    U64 whitePawnCover = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
    U64 blackPawnCover = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);

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
    
        materialScore   += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_KNIGHT][square];
        
        
        mobScore        += mobilityKnight[bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresWhite)];

        wKsideAttacks   -= bitCount(attacks & wKsideMask);
        bKsideAttacks   += bitCount(attacks & bKsideMask);

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
        materialScore   += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_KNIGHT][square];

        mobScore        -= mobilityKnight[bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresBlack)];

        wKsideAttacks   += bitCount(attacks & wKsideMask);
        bKsideAttacks   -= bitCount(attacks & bKsideMask);

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
        
        materialScore   += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_BISHOP][square];
        mobScore        += mobilityBishop[bitCount(attacks & mobilitySquaresWhite)];

        wKsideAttacks   -= bitCount(attacks & wKsideMask);
        bKsideAttacks   += bitCount(attacks & bKsideMask);

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
    
        materialScore   += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_BISHOP][square];
        mobScore        -= mobilityBishop[bitCount(attacks & mobilitySquaresBlack)];

        wKsideAttacks   += bitCount(attacks & wKsideMask);
        bKsideAttacks   -= bitCount(attacks & bKsideMask);

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
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_ROOK][square];
        mobScore      += mobilityRook[bitCount(attacks & mobilitySquaresWhite)];

        wKsideAttacks   -= bitCount(attacks & wKsideMask);
        bKsideAttacks   += bitCount(attacks & bKsideMask);

        featureScore  += ROOK_ATTACK_QUEEN * bitCount(attacks & b->getPieceBB<BLACK>(QUEEN));
        featureScore  += SAFE_ROOK_CHECK * bitCount(bKingRookAttacks & attacks & ~blackPawnCover);

        addToKingSafety(attacks,  evalData.kingZone[BLACK], bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 3);

        k = lsbReset(k);
    }

    k = b->getPieceBB()[BLACK_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied & ~b->getPieceBB()[BLACK_ROOK] & ~b->getPieceBB()[BLACK_QUEEN]);
        evalData.attacks[BLACK][ROOK] |= attacks;
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_ROOK][square];
        mobScore      -= mobilityRook[bitCount(attacks & mobilitySquaresBlack)];

        wKsideAttacks   += bitCount(attacks & wKsideMask);
        bKsideAttacks   -= bitCount(attacks & bKsideMask);

        featureScore  -= ROOK_ATTACK_QUEEN * bitCount(attacks & b->getPieceBB<WHITE>(QUEEN));
        featureScore  -= SAFE_ROOK_CHECK * bitCount(wKingRookAttacks & attacks & ~whitePawnCover);

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
        attacks = lookUpRookAttack  (square,    occupied & ~b->getPieceBB()[WHITE_ROOK])
                | lookUpBishopAttack(square,    occupied & ~b->getPieceBB()[WHITE_BISHOP]);
        evalData.attacks[WHITE][QUEEN] |= attacks;
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_QUEEN][square];
        mobScore      += mobilityQueen[bitCount(attacks & mobilitySquaresWhite)];

        wKsideAttacks   -= bitCount(attacks & wKsideMask);
        bKsideAttacks   += bitCount(attacks & bKsideMask);

        featureScore  += QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, blackKingSquare);
        featureScore  += SAFE_QUEEN_CHECK * bitCount((bKingRookAttacks | bKingBishopAttacks) & attacks & ~blackPawnCover);

        addToKingSafety(attacks,  evalData.kingZone[BLACK], bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 4);

        k = lsbReset(k);
    }

    k = b->getPieceBB()[BLACK_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack  (square,    occupied & ~b->getPieceBB()[BLACK_ROOK])
                | lookUpBishopAttack(square,    occupied & ~b->getPieceBB()[BLACK_BISHOP]);
        evalData.attacks[BLACK][QUEEN] |= attacks;
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_QUEEN][square];
        mobScore      -= mobilityQueen[bitCount(attacks & mobilitySquaresBlack)];

        wKsideAttacks   += bitCount(attacks & wKsideMask);
        bKsideAttacks   -= bitCount(attacks & bKsideMask);

        featureScore  -= QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, whiteKingSquare);
        featureScore  -= SAFE_QUEEN_CHECK * bitCount((wKingRookAttacks | wKingBishopAttacks) & attacks & ~whitePawnCover);
        
        addToKingSafety(attacks,  evalData.kingZone[WHITE], wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, 4);

        k = lsbReset(k);
    }
    
    /**********************************************************************************
     *                                  K I N G S                                     *
     **********************************************************************************/
    k = b->getPieceBB()[WHITE_KING];

    while (k) {
        square = bitscanForward(k);
        evalData.attacks[WHITE][KING] = KING_ATTACKS[square];
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][WHITE_KING][square];

        featureScore += KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & whitePawns);
        featureScore += KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & blackTeam);

        k = lsbReset(k);
    }

    k = b->getPieceBB()[BLACK_KING];
    while (k) {
        square = bitscanForward(k);
        evalData.attacks[BLACK][KING] = KING_ATTACKS[square];
    
        materialScore += piece_kk_square_tables[whiteKingSquare][blackKingSquare][BLACK_KING][square];

        featureScore -= KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & blackPawns);
        featureScore -= KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & whiteTeam);

        k = lsbReset(k);
    }
    
    mobScore -= wKsideAttacks > 0 ? SLOW_ATTACK_BUILDUP * centerClosed * wKsideAttacks/10 : 0;
    mobScore += bKsideAttacks > 0 ? SLOW_ATTACK_BUILDUP * centerClosed * bKsideAttacks/10 : 0;
    mobScore -= wKsideAttacks > 0 ? SLOW_ATTACK_BUILDUP_NORM * wKsideAttacks / 10 : 0;
    mobScore += bKsideAttacks > 0 ? SLOW_ATTACK_BUILDUP_NORM * bKsideAttacks / 10 : 0;

    EvalScore hangingEvalScore = computeHangingPieces(b, &evalData);
    EvalScore pinnedEvalScore  = computePinnedPieces(b, WHITE) - computePinnedPieces(b, BLACK);
    
    evalScore += kingSafetyTable[bkingSafety_valueOfAttacks] - kingSafetyTable[wkingSafety_valueOfAttacks];
    
    // clang-format off
    featureScore += CASTLING_RIGHTS*(
            + b->getCastlingRights(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
            + b->getCastlingRights(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
            - b->getCastlingRights(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
            - b->getCastlingRights(STATUS_INDEX_BLACK_KINGSIDE_CASTLING));
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
    UCI_ASSERT(board);

    using namespace std;

    Evaluator ev {};
    Score     score = ev.evaluate(board);
    float     phase = ev.getPhase();

    std::cout <<
        setw(15) << right << "evaluation: " << left << setw(8) << score <<
        setw(15) << right << "phase: "      << left << setprecision(3) << setw(8) << phase << std::endl;
}

float Evaluator::getPhase() { return phase; }



