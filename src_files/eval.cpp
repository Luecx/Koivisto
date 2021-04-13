
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


EvalScore SIDE_TO_MOVE                  = M(   16,   19);
EvalScore PAWN_STRUCTURE                = M(   10,    4);
EvalScore PAWN_PASSED                   = M(    9,   50);
EvalScore PAWN_ISOLATED                 = M(   -1,  -14);
EvalScore PAWN_DOUBLED                  = M(   -7,   -5);
EvalScore PAWN_DOUBLED_AND_ISOLATED     = M(   -7,  -26);
EvalScore PAWN_BACKWARD                 = M(  -11,    1);
EvalScore PAWN_OPEN                     = M(   -7,   -6);
EvalScore PAWN_BLOCKED                  = M(   -4,  -15);
EvalScore KNIGHT_OUTPOST                = M(   24,   19);
EvalScore KNIGHT_DISTANCE_ENEMY_KING    = M(   -5,   -2);
EvalScore ROOK_OPEN_FILE                = M(   23,    1);
EvalScore ROOK_HALF_OPEN_FILE           = M(    1,   -9);
EvalScore ROOK_KING_LINE                = M(   17,   11);
EvalScore BISHOP_DOUBLED                = M(   17,   78);
EvalScore BISHOP_FIANCHETTO             = M(   -4,    4);
EvalScore BISHOP_PIECE_SAME_SQUARE_E    = M(    3,    3);
EvalScore QUEEN_DISTANCE_ENEMY_KING     = M(    3,  -22);
EvalScore KING_CLOSE_OPPONENT           = M(  -20,   16);
EvalScore KING_PAWN_SHIELD              = M(   30,   16);
EvalScore CASTLING_RIGHTS               = M(   20,   -4);
EvalScore MINOR_BEHIND_PAWN             = M(    6,   25);
EvalScore SAFE_QUEEN_CHECK              = M(    6,   20);
EvalScore SAFE_ROOK_CHECK               = M(   12,    3);
EvalScore SAFE_BISHOP_CHECK             = M(   10,    3);
EvalScore SAFE_KNIGHT_CHECK             = M(   13,    3);
EvalScore PAWN_ATTACK_MINOR             = M(   43,   78);
EvalScore PAWN_ATTACK_ROOK              = M(   43,   33);
EvalScore PAWN_ATTACK_QUEEN             = M(   33,   31);
EvalScore MINOR_ATTACK_ROOK             = M(   37,   26);
EvalScore MINOR_ATTACK_QUEEN            = M(   26,   43);
EvalScore ROOK_ATTACK_QUEEN             = M(   34,   17);
EvalScore MINOR_ATTACK_HANING_MINOR     = M(   21,   29);
EvalScore MAJOR_ATTACK_HANING_MINOR     = M(   26,   37);
EvalScore PIECE_ATTACK_HANGING_PAWN     = M(    8,   37);

EvalScore mobilityKnight[9] = {
        M(  -51,   -7), M(  -39,   48), M(  -34,   79), M(  -30,   97), M(  -25,  108),
        M(  -21,  118), M(  -14,  120), M(   -4,  113), M(   10,   99), };

EvalScore mobilityBishop[14] = {
        M(  -10,  -36), M(    1,   33), M(    8,   67), M(   13,   87), M(   20,  104),
        M(   24,  118), M(   25,  126), M(   25,  131), M(   28,  136), M(   32,  135),
        M(   39,  130), M(   56,  123), M(   67,  132), M(   92,  104), };

EvalScore mobilityRook[15] = {
        M(  -43,   63), M(  -36,  107), M(  -33,  145), M(  -31,  173), M(  -30,  187),
        M(  -24,  193), M(  -18,  201), M(  -10,  203), M(   -4,  207), M(    2,  212),
        M(    8,  216), M(   13,  220), M(   22,  218), M(   46,  203), M(  103,  173), };

EvalScore mobilityQueen[28] = {
        M( -185,  101), M( -168,  103), M( -156,  229), M( -151,  310), M( -149,  354),
        M( -148,  384), M( -147,  408), M( -145,  424), M( -143,  436), M( -141,  443),
        M( -138,  450), M( -136,  455), M( -135,  457), M( -135,  463), M( -135,  466),
        M( -136,  468), M( -137,  468), M( -137,  468), M( -137,  467), M( -130,  459),
        M( -122,  447), M( -119,  439), M( -125,  433), M( -108,  418), M( -171,  442),
        M(  -41,  364), M(  -73,  410), M( -120,  439), };

EvalScore hangingEval[5] = {
        M(   -2,    3), M(   -2,    1), M(   -5,   -5), M(   -5,   -7), M(   -5,   -7), };

EvalScore pinnedEval[15] = {
        M(    1,   -7), M(   -7,    7), M(   -6,   24), M(  -20,  -63), M(  -24,  -13),
        M(  -16,   11), M(   -0,   -6), M(  -29,  -15), M(  -16,   -3), M(  -10,  -17),
        M(    2,   -8), M(  -14,   -3), M(   12,  -27), M(  -13,  -41), M(  -13,   10), };

EvalScore passer_rank_n[16] = {
        M(    0,    0), M(  -17,  -32), M(  -22,  -28), M(  -16,    0),
        M(   14,   26), M(   36,   83), M(   32,   46), M(    0,    0),
        M(    0,    0), M(    1,    2), M(  -39,  -62), M(  -36,  -18),
        M(  -23,  -15), M(   27,    9), M(  111, -121), M(    0,    0), };

EvalScore bishop_pawn_same_color_table_o[9] = {
        M(  -34,   39), M(  -41,   41), M(  -40,   28),
        M(  -44,   18), M(  -48,    9), M(  -53,   -4),
        M(  -56,  -21), M(  -57,  -39), M(  -68,  -88), };

EvalScore bishop_pawn_same_color_table_e[9] = {
        M(  -23,   37), M(  -39,   47), M(  -44,   39),
        M(  -49,   32), M(  -53,   23), M(  -57,    9),
        M(  -59,  -10), M(  -58,  -23), M(  -66,  -36), };

EvalScore kingSafetyTable[100] = {
        M(   -9,   -7), M(    0,    0), M(  -13,   -8), M(   -7,   -7), M(  -10,   -9),
        M(   18,   -9), M(    5,  -13), M(   28,  -11), M(   11,  -13), M(   41,  -22),
        M(   58,  -15), M(   75,  -26), M(   35,  -28), M(  105,  -26), M(  100,  -24),
        M(  103,  -18), M(   93,  -52), M(  168,  -23), M(  186,  -46), M(  203,  -63),
        M(  224, -125), M(  183,  -42), M(  287,  -74), M(  238,  -45), M(  267,  -58),
        M(  273,  -18), M(  368,  -45), M(  377,  -87), M(  308,   70), M(  429,  -80),
        M(  447, -189), M(  497, -145), M(  572, -291), M(  679, -295), M(  525, -101),
        M( 2048,-3760), M(   90, 1002), M( 1918,-2745), M(  393,  390), M( 1087,  855),
        M( 1321,-1924), M( 1003, -705), M( 1582, 1327), M(  500,  500), M(  501,  500),
        M( 1722, 1489), M(  500,  500), M(  921,  690), M(  500,  500), M( -509, -514),
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

    &MINOR_ATTACK_HANING_MINOR,
    &MAJOR_ATTACK_HANING_MINOR,
    &PIECE_ATTACK_HANGING_PAWN,
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

EvalScore Evaluator::computeHangingPieces(Board* b) {
    UCI_ASSERT(b);

    U64 WnotAttacked = ZERO;
    U64 BnotAttacked = ZERO;
    for(int i = PAWN; i <= KING; i++){
        WnotAttacked |= evalData.attacks[WHITE][i];
        BnotAttacked |= evalData.attacks[BLACK][i];
    }

    evalData.allAttacks[WHITE] = WnotAttacked;
    evalData.allAttacks[BLACK] = BnotAttacked;
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
    
    evalData = EvalData{};
    
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
    
    
    EvalScore hangingEvalScore = computeHangingPieces(b);

    //Score additional threats
    featureScore += MINOR_ATTACK_HANING_MINOR *(
            + bitCount(~evalData.allAttacks[BLACK] & ((evalData.attacks[WHITE][KNIGHT] & b->getPieceBB<BLACK>(BISHOP)) |
                                                      (evalData.attacks[WHITE][BISHOP] & b->getPieceBB<BLACK>(KNIGHT))))
            - bitCount(~evalData.allAttacks[WHITE] & ((evalData.attacks[BLACK][KNIGHT] & b->getPieceBB<WHITE>(BISHOP)) |
                                                      (evalData.attacks[BLACK][BISHOP] & b->getPieceBB<WHITE>(KNIGHT)))));

    featureScore += MAJOR_ATTACK_HANING_MINOR *(
            + bitCount(~evalData.allAttacks[BLACK] & (evalData.attacks[WHITE][QUEEN]|evalData.attacks[WHITE][ROOK]) & (b->getPieceBB<BLACK>(KNIGHT) | b->getPieceBB<BLACK>(BISHOP)))
            - bitCount(~evalData.allAttacks[WHITE] & (evalData.attacks[BLACK][QUEEN]|evalData.attacks[BLACK][ROOK]) & (b->getPieceBB<WHITE>(KNIGHT) | b->getPieceBB<WHITE>(BISHOP))));

    featureScore += PIECE_ATTACK_HANGING_PAWN* (
            + bitCount(~evalData.allAttacks[BLACK] & evalData.allAttacks[WHITE] & b->getPieceBB<BLACK>(PAWN))
            - bitCount(~evalData.allAttacks[WHITE] & evalData.allAttacks[BLACK] & b->getPieceBB<WHITE>(PAWN))
    );

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
EvalData* Evaluator::getEvalData() { return &evalData; }
