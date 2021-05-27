
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

#include <iomanip>

EvalScore SIDE_TO_MOVE                  = M(   13,   14);
EvalScore PAWN_STRUCTURE                = M(    8,    2);
EvalScore PAWN_PASSED_AND_DOUBLED       = M(  -12,  -36);
EvalScore PAWN_PASSED_AND_BLOCKED       = M(    1,  -35);
EvalScore PAWN_PASSED_COVERED_PROMO     = M(   -2,    8);
EvalScore PAWN_PASSED_HELPER            = M(    2,    4);
EvalScore PAWN_PASSED_AND_DEFENDED      = M(   11,    1);
EvalScore PAWN_PASSED_SQUARE_RULE       = M(    3,   15);
EvalScore PAWN_PASSED_EDGE_DISTANCE     = M(   -4,   -4);
EvalScore PAWN_PASSED_KING_TROPISM      = M(   -2,   13);
EvalScore PAWN_ISOLATED                 = M(   -1,   -7);
EvalScore PAWN_DOUBLED                  = M(   -6,   -7);
EvalScore PAWN_DOUBLED_AND_ISOLATED     = M(   -5,  -21);
EvalScore PAWN_BACKWARD                 = M(   -9,   -2);
EvalScore PAWN_OPEN                     = M(   -9,  -10);
EvalScore PAWN_BLOCKED                  = M(   -3,   -9);
EvalScore PAWN_CONNECTED                = M(    8,    7);
EvalScore PAWN_CONNECTED_FAR            = M(   34,   65);
EvalScore KNIGHT_OUTPOST                = M(   22,   18);
EvalScore KNIGHT_DISTANCE_ENEMY_KING    = M(   -6,   -2);
EvalScore ROOK_OPEN_FILE                = M(   23,   -1);
EvalScore ROOK_HALF_OPEN_FILE           = M(    1,   -9);
EvalScore ROOK_KING_LINE                = M(   21,    2);
EvalScore BISHOP_DOUBLED                = M(   13,   74);
EvalScore BISHOP_FIANCHETTO             = M(   23,   28);
EvalScore BISHOP_PIECE_SAME_SQUARE_E    = M(    2,    3);
EvalScore QUEEN_DISTANCE_ENEMY_KING     = M(  -18,  -13);
EvalScore KING_CLOSE_OPPONENT           = M(  -13,   14);
EvalScore KING_PAWN_SHIELD              = M(   21,    7);
EvalScore CASTLING_RIGHTS               = M(   16,    1);
EvalScore MINOR_BEHIND_PAWN             = M(    5,   19);
EvalScore SAFE_QUEEN_CHECK              = M(    4,   25);
EvalScore SAFE_ROOK_CHECK               = M(    9,    5);
EvalScore SAFE_BISHOP_CHECK             = M(   10,    4);
EvalScore SAFE_KNIGHT_CHECK             = M(   11,    4);
EvalScore PAWN_ATTACK_MINOR             = M(   38,   66);
EvalScore PAWN_ATTACK_ROOK              = M(   40,   24);
EvalScore PAWN_ATTACK_QUEEN             = M(   29,   28);
EvalScore MINOR_ATTACK_ROOK             = M(   33,   25);
EvalScore MINOR_ATTACK_QUEEN            = M(   23,   37);
EvalScore ROOK_ATTACK_QUEEN             = M(   31,   15);

EvalScore mobilityKnight[9] = {
        M(  -67,   11), M(  -56,   56), M(  -52,   82), M(  -48,   97), M(  -43,  107),
        M(  -39,  116), M(  -33,  117), M(  -24,  112), M(  -11,   97), };

EvalScore mobilityBishop[14] = {
        M(  -21,  -12), M(  -11,   44), M(   -4,   71), M(   -1,   88), M(    4,  101),
        M(    8,  112), M(    9,  120), M(    8,  124), M(   10,  127), M(   13,  126),
        M(   20,  121), M(   35,  113), M(   45,  122), M(   66,   93), };

EvalScore mobilityRook[15] = {
        M(  -75,   78), M(  -69,  114), M(  -66,  147), M(  -65,  171), M(  -64,  185),
        M(  -59,  191), M(  -53,  197), M(  -46,  200), M(  -41,  206), M(  -37,  211),
        M(  -33,  216), M(  -29,  219), M(  -20,  217), M(    6,  200), M(   58,  170), };

EvalScore mobilityQueen[28] = {
        M( -196,  137), M( -180,  139), M( -169,  240), M( -165,  306), M( -163,  341),
        M( -163,  365), M( -162,  385), M( -159,  398), M( -158,  408), M( -155,  414),
        M( -153,  419), M( -151,  423), M( -149,  424), M( -149,  428), M( -149,  430),
        M( -150,  430), M( -150,  429), M( -151,  428), M( -149,  424), M( -142,  415),
        M( -134,  401), M( -131,  393), M( -136,  385), M( -117,  368), M( -178,  393),
        M(  -57,  304), M(  -99,  358), M( -190,  435), };

EvalScore hangingEval[5] = {
        M(   -3,   -2), M(   -4,   -1), M(   -5,   -6), M(   -5,   -4), M(   -4,   -7), };

EvalScore pinnedEval[15] = {
        M(    2,   -4), M(  -10,    8), M(  -11,   52), M(  -18,  -55), M(  -22,  -13),
        M(  -21,   46), M(    0,   -8), M(  -27,  -13), M(  -18,   35), M(  -10,  -11),
        M(    1,   -9), M(  -16,   38), M(   12,  -18), M(  -10,  -30), M(  -19,   53), };

EvalScore passer_rank_n[N_RANKS] = {
        M(    0,    0), M(  -13,    5), M(  -22,   19), M(  -15,   55),
        M(   12,   92), M(   32,  149), M(   14,   61), M(    0,    0), };

EvalScore candidate_passer[N_RANKS] = {
        M(    0,    0), M(  -21,    6), M(   -6,   14), M(   -3,   25),
        M(    2,   73), M(    5,   60), M(    0,    0), M(    0,    0), };

EvalScore bishop_pawn_same_color_table_o[9] = {
        M(  -49,   40), M(  -55,   42), M(  -53,   29),
        M(  -55,   20), M(  -58,   11), M(  -61,    0),
        M(  -63,  -15), M(  -62,  -28), M(  -68,  -66), };

EvalScore bishop_pawn_same_color_table_e[9] = {
        M(  -37,   26), M(  -54,   38), M(  -58,   31),
        M(  -63,   25), M(  -66,   17), M(  -69,    5),
        M(  -71,  -10), M(  -69,  -20), M(  -77,  -28), };

EvalScore kingSafetyTable[100] = {
        M(   -3,   -1), M(    0,    0), M(   -8,   -1), M(   -3,   -3), M(   -5,   -3),
        M(   20,   -5), M(    9,   -7), M(   28,   -5), M(   17,   -7), M(   44,  -16),
        M(   58,   -9), M(   78,  -20), M(   47,  -20), M(  103,  -19), M(  103,  -17),
        M(  112,  -14), M(  105,  -28), M(  163,  -16), M(  191,  -39), M(  209,  -55),
        M(  215,  -66), M(  177,  -27), M(  288,  -52), M(  228,  -13), M(  272,  -29),
        M(  259,   11), M(  356,   -9), M(  371,  -66), M(  300,   81), M(  396,   -3),
        M(  432, -156), M(  461,  -75), M(  575, -237), M(  651, -236), M(  494, -104),
        M( 1851,-3155), M(  -31, 1595), M( 1663,-2031), M(  216,  796), M( 1166,  881),
        M(  424, -116), M(  995, -694), M( 1651, 1347), M(  500,  500), M(  501,  500),
        M( 1785, 1506), M(  500,  500), M(  998,  711), M(  500,  500), M(-1669,-2054),
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
    &PAWN_PASSED_AND_DOUBLED,
    &PAWN_PASSED_AND_BLOCKED,
    &PAWN_PASSED_COVERED_PROMO,
    &PAWN_PASSED_HELPER,
    &PAWN_PASSED_AND_DEFENDED,
    &PAWN_PASSED_SQUARE_RULE,
    &PAWN_PASSED_EDGE_DISTANCE,
    &PAWN_PASSED_KING_TROPISM,
    &PAWN_ISOLATED,
    &PAWN_DOUBLED,
    &PAWN_DOUBLED_AND_ISOLATED,
    &PAWN_BACKWARD,
    &PAWN_OPEN,
    &PAWN_BLOCKED,
    &PAWN_CONNECTED,
    &PAWN_CONNECTED_FAR,
    
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

float phaseValues[N_PIECE_TYPES] {
    0, 1, 1, 2, 4, 0,
};


constexpr int lazyEvalAlphaBound = 803;
constexpr int lazyEvalBetaBound  = 392;

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
    return (((PASSED_PAWN_MASK[c][s] & ~FILES_BB[fileIndex(s)]) & opponentPawns) == 0 && (sq & pawnCover));
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

template<Color color>
EvalScore Evaluator::computePinnedPieces(Board* b) {
    UCI_ASSERT(b);

    EvalScore result = 0;

    constexpr Color us   = color;
    constexpr Color them = 1 - color;

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
        U64 inBetween = IN_BETWEEN_SQUARES[kingSq][pinnerSquare];

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
        if constexpr (us == WHITE) {
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

template<Color color>
EvalScore Evaluator::computePassedPawns(Board* b){
    
    EvalScore h = M(0,0);
    
    U64 pawns    = b->getPieceBB( color, PAWN);
    U64 oppPawns = b->getPieceBB(!color, PAWN);
    Square kingSq = bitscanForward(b->getPieceBB(color, KING));
    Square oppKingSq = bitscanForward(b->getPieceBB(!color, KING));

    
    U64 bb = pawns;
    
    while (bb) {
        Square s      = bitscanForward(bb);
        Rank   r      = color == WHITE ? rankIndex(s) : 7 - rankIndex(s);
        File   f      = fileIndex(s);
        U64    sqBB   = ONE << s;
        U64   advBB   = color == WHITE ? shiftNorth(sqBB) : shiftSouth(sqBB);
        Square adv    = bitscanForward(advBB);

        U64 passerMask = PASSED_PAWN_MASK[color][s];

        bool passed = !(passerMask & oppPawns);


        // check if passer
        if (passed) {
            U64    teleBB  = color == WHITE ? shiftNorth(sqBB) : shiftSouth(sqBB);
            U64    promBB  = FILES_BB[f] & (color == WHITE ? RANK_8_BB:RANK_1_BB);
            U64    promCBB = promBB & WHITE_SQUARES_BB ? WHITE_SQUARES_BB : BLACK_SQUARES_BB;
            
            h += passer_rank_n[r] + PAWN_PASSED_EDGE_DISTANCE * (f > 3 ? 7 - f : f);
            
            // check if doubled
            h += bitCount(teleBB & pawns) * PAWN_PASSED_AND_DOUBLED;
            
            // check if square in front is blocked
            h += bitCount(teleBB & b->getTeamOccupiedBB(!color)) * PAWN_PASSED_AND_BLOCKED;
            
            // check if promotion square can be covered
            h += (  bitCount(b->getPieceBB(color, BISHOP) & promCBB)
                  + bitCount(b->getPieceBB(color, QUEEN))
                  - bitCount(b->getPieceBB(!color, BISHOP) & promCBB)
                  - bitCount(b->getPieceBB(!color, QUEEN))) * PAWN_PASSED_COVERED_PROMO;
           
            // check if there is a helper
            h += (bitCount(pawns & (color == WHITE ? wAttackRearSpans(pawns) : bAttackRearSpans(pawns)))) * PAWN_PASSED_HELPER;
            
            // check if its defended
            h += (bitCount(sqBB & evalData.pawnWestAttacks[color]) + bitCount(sqBB & evalData.pawnEastAttacks[color])) * PAWN_PASSED_AND_DEFENDED;
            
            // check if can be caught by king
            h += ((7 - r + (color != b->getActivePlayer())) < manhattanDistance(
                      bitscanForward(promBB),
                      bitscanForward(b->getPieceBB(!color, KING)))) * PAWN_PASSED_SQUARE_RULE;

            h += PAWN_PASSED_KING_TROPISM 
                * std::clamp(chebyshevDistance(oppKingSq, adv) - chebyshevDistance(kingSq, adv), -4, 4);
        }

        if (!passed && (sqBB & evalData.semiOpen[color])) {
            U64 antiPassers = passerMask & oppPawns;                                 // pawns that make this NOT a passer
            U64 pawnAdvance = color == WHITE ? shiftNorth(sqBB) : shiftSouth(sqBB); // advance square
            U64 levers = oppPawns & (color == WHITE ?                                // levers are pawns in active tension
                (shiftNorthEast(sqBB) | shiftNorthWest(sqBB)) :                      // https://www.chessprogramming.org/Pawn_Levers_(Bitboards)
                (shiftSouthEast(sqBB) | shiftSouthWest(sqBB)));                      //
            U64 forwardLevers = oppPawns & (color == WHITE ?                         //
                (shiftNorthEast(pawnAdvance) | shiftNorthWest(pawnAdvance)) :        // levers that would apply if pawn was advanced
                (shiftSouthEast(pawnAdvance) | shiftSouthWest(pawnAdvance)));        //
            U64 helpers = (shiftEast(sqBB) | shiftWest(sqBB)) & pawns;               // friendly pawns on either side

            bool push = !(antiPassers ^ levers); // Are all the pawns currently levers, we can make this pawn passed by pushing it
            bool helped = !(antiPassers ^ forwardLevers) // Are all the pawns forward lever, we can push through with support
                && (bitCount(helpers) >= bitCount(forwardLevers)); // <-- supporters

            if (push || helped) {
                h += candidate_passer[r];
            }
        }
        
        bb = lsbReset(bb);
    }
    return h;
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

    // connceted pawns
    U64 whiteConnectedPawns = whitePawns & (shiftEast(whitePawns) | shiftWest(whitePawns)) 
        & (RANK_4_BB | RANK_5_BB | RANK_6_BB | RANK_7_BB);
    U64 blackConnectedPawns = blackPawns & (shiftEast(blackPawns) | shiftWest(blackPawns)) 
        & (RANK_5_BB | RANK_4_BB | RANK_3_BB | RANK_2_BB);
    
    evalData.semiOpen[WHITE] = ~fillFile(blackPawns);
    evalData.semiOpen[BLACK] = ~fillFile(whitePawns);
    
    U64 openFiles          = evalData.semiOpen[WHITE] & evalData.semiOpen[BLACK];

    Square square;
    U64    attacks;
    U64    k;
    
    U64 whitePawnEastCover = shiftNorthEast(whitePawns) & whitePawns;
    U64 whitePawnWestCover = shiftNorthWest(whitePawns) & whitePawns;
    U64 blackPawnEastCover = shiftSouthEast(blackPawns) & blackPawns;
    U64 blackPawnWestCover = shiftSouthWest(blackPawns) & blackPawns;

    evalData.pawnEastAttacks[WHITE] = shiftNorthEast(whitePawns);
    evalData.pawnEastAttacks[BLACK] = shiftSouthEast(blackPawns);
    evalData.pawnWestAttacks[WHITE] = shiftNorthWest(whitePawns);
    evalData.pawnWestAttacks[BLACK] = shiftSouthWest(blackPawns);
    
    evalData.attacks[WHITE][PAWN] = evalData.pawnEastAttacks[WHITE] | evalData.pawnWestAttacks[WHITE];
    evalData.attacks[BLACK][PAWN] = evalData.pawnEastAttacks[BLACK] | evalData.pawnWestAttacks[BLACK];

    U64 mobilitySquaresWhite = ~whiteTeam & ~(evalData.attacks[BLACK][PAWN]);
    U64 mobilitySquaresBlack = ~blackTeam & ~(evalData.attacks[WHITE][PAWN]);

    // clang-format off
    evalData.threats[WHITE] = PAWN_ATTACK_MINOR * bitCount(evalData.attacks[WHITE][PAWN] & (b->getPieceBB<BLACK>(KNIGHT) | b->getPieceBB<BLACK>(BISHOP)));
    evalData.threats[BLACK] = PAWN_ATTACK_MINOR * bitCount(evalData.attacks[BLACK][PAWN] & (b->getPieceBB<WHITE>(KNIGHT) | b->getPieceBB<WHITE>(BISHOP)));

    evalData.threats[WHITE] += PAWN_ATTACK_ROOK * bitCount(evalData.attacks[WHITE][PAWN] & b->getPieceBB<BLACK>(ROOK));
    evalData.threats[BLACK] += PAWN_ATTACK_ROOK * bitCount(evalData.attacks[BLACK][PAWN] & b->getPieceBB<WHITE>(ROOK));

    evalData.threats[WHITE] += PAWN_ATTACK_QUEEN * bitCount(evalData.attacks[WHITE][PAWN] & b->getPieceBB<BLACK>(QUEEN));
    evalData.threats[BLACK] += PAWN_ATTACK_QUEEN * bitCount(evalData.attacks[BLACK][PAWN] & b->getPieceBB<WHITE>(QUEEN));

    featureScore += PAWN_DOUBLED_AND_ISOLATED * (
            + bitCount(whiteIsolatedPawns & whiteDoubledPawns)
            - bitCount(blackIsolatedPawns & blackDoubledPawns));
    featureScore += PAWN_DOUBLED * (
            + bitCount(~whiteIsolatedPawns & whiteDoubledPawns)
            - bitCount(~blackIsolatedPawns & blackDoubledPawns));
    featureScore += PAWN_ISOLATED * (
            + bitCount(whiteIsolatedPawns & ~whiteDoubledPawns)
            - bitCount(blackIsolatedPawns & ~blackDoubledPawns));
    featureScore += PAWN_STRUCTURE * (
            + bitCount(whitePawnEastCover)
            + bitCount(whitePawnWestCover)
            - bitCount(blackPawnEastCover)
            - bitCount(blackPawnWestCover));
    featureScore += PAWN_OPEN * (
            + bitCount(whitePawns & ~evalData.attacks[WHITE][PAWN] & ~fillSouth(blackPawns))
            - bitCount(blackPawns & ~evalData.attacks[BLACK][PAWN] & ~fillNorth(whitePawns)));
    featureScore += PAWN_BACKWARD * (
            + bitCount(fillSouth(~wAttackFrontSpans(whitePawns) & evalData.attacks[BLACK][PAWN]) & whitePawns)
            - bitCount(fillNorth(~bAttackFrontSpans(blackPawns) & evalData.attacks[WHITE][PAWN]) & blackPawns));
    featureScore += PAWN_BLOCKED * (
            + bitCount(whiteBlockedPawns)
            - bitCount(blackBlockedPawns));
    
    featureScore += PAWN_CONNECTED * (
              bitCount(whiteConnectedPawns)
            - bitCount(blackConnectedPawns));

    featureScore += PAWN_CONNECTED_FAR * (
              bitCount(whiteConnectedPawns & (RANK_6_BB | RANK_7_BB))
            - bitCount(blackConnectedPawns & (RANK_3_BB | RANK_2_BB)));

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

        evalData.threats[WHITE] += MINOR_ATTACK_ROOK            * bitCount(attacks & b->getPieceBB<BLACK>(ROOK));
        evalData.threats[WHITE] += MINOR_ATTACK_QUEEN           * bitCount(attacks & b->getPieceBB<BLACK>(QUEEN));
        featureScore    += KNIGHT_OUTPOST               * isOutpost(square, WHITE, blackPawns, evalData.attacks[WHITE][PAWN]);
        featureScore    += KNIGHT_DISTANCE_ENEMY_KING   * manhattanDistance(square, blackKingSquare);
        featureScore    += SAFE_KNIGHT_CHECK            * bitCount(bKingKnightAttacks & attacks & ~evalData.attacks[BLACK][PAWN]);

        addToKingSafety(attacks, evalData.kingZone[BLACK], bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 2);

        k = lsbReset(k);
    }

    k = b->getPieceBB()[BLACK_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[square];
        evalData.attacks[BLACK][KNIGHT] |= attacks;
    
        mobScore        -= mobilityKnight[bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresBlack)];

        evalData.threats[BLACK] += MINOR_ATTACK_ROOK            * bitCount(attacks & b->getPieceBB<WHITE>(ROOK));
        evalData.threats[BLACK] += MINOR_ATTACK_QUEEN           * bitCount(attacks & b->getPieceBB<WHITE>(QUEEN));
        featureScore    -= KNIGHT_OUTPOST               * isOutpost(square, BLACK, whitePawns, evalData.attacks[BLACK][PAWN]);
        featureScore    -= KNIGHT_DISTANCE_ENEMY_KING   * manhattanDistance(square, whiteKingSquare);
        featureScore    -= SAFE_KNIGHT_CHECK            * bitCount(wKingKnightAttacks & attacks & ~evalData.attacks[WHITE][PAWN]);
    
        addToKingSafety(attacks, evalData.kingZone[WHITE], wkingSafety_attPiecesCount, wkingSafety_valueOfAttacks, 2);

        k = lsbReset(k);
    }
  
    /**********************************************************************************
     *                                  B I S H O P S                                 *
     **********************************************************************************/

    k = b->getPieceBB()[WHITE_BISHOP];
    while (k) {
        square  = bitscanForward(k);
        U64 sqBB = k & -k;

        attacks = lookUpBishopAttack(square, occupied & ~b->getPieceBB()[WHITE_QUEEN]);
        evalData.attacks[WHITE][BISHOP] |= attacks;
        
        mobScore        += mobilityBishop[bitCount(attacks & mobilitySquaresWhite)];

        evalData.threats[WHITE] += MINOR_ATTACK_ROOK            * bitCount(attacks & b->getPieceBB<BLACK>(ROOK));
        evalData.threats[WHITE] += MINOR_ATTACK_QUEEN           * bitCount(attacks & b->getPieceBB<BLACK>(QUEEN));
        featureScore    += bishop_pawn_same_color_table_e[bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB))];
        featureScore    += bishop_pawn_same_color_table_o[bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB))];
        featureScore    += BISHOP_PIECE_SAME_SQUARE_E
                           * bitCount(blackTeam & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB));

        if (!(CENTER_SQUARES_BB & sqBB) && bitCount(CENTER_SQUARES_BB & lookUpBishopAttack(square, (whitePawns | blackPawns))) > 1)
            featureScore += BISHOP_FIANCHETTO;
                        
        featureScore    += SAFE_BISHOP_CHECK * bitCount(bKingBishopAttacks & attacks & ~evalData.attacks[BLACK][PAWN]);
        addToKingSafety(attacks,  evalData.kingZone[BLACK], bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 2);

        k = lsbReset(k);
    }

    k = b->getPieceBB()[BLACK_BISHOP];
    while (k) {
        square  = bitscanForward(k);
        U64 sqBB = k & -k;

        attacks = lookUpBishopAttack(square, occupied & ~b->getPieceBB()[BLACK_QUEEN]);
        evalData.attacks[BLACK][BISHOP] |= attacks;
    
        mobScore        -= mobilityBishop[bitCount(attacks & mobilitySquaresBlack)];

        evalData.threats[BLACK] += MINOR_ATTACK_ROOK            * bitCount(attacks & b->getPieceBB<WHITE>(ROOK));
        evalData.threats[BLACK] += MINOR_ATTACK_QUEEN           * bitCount(attacks & b->getPieceBB<WHITE>(QUEEN));
        featureScore    -= bishop_pawn_same_color_table_e[bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB))];
        featureScore    -= bishop_pawn_same_color_table_o[bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB))];
        featureScore    -= BISHOP_PIECE_SAME_SQUARE_E
                           * bitCount(whiteTeam & (((ONE << square) & WHITE_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB));
                           
        if (!(CENTER_SQUARES_BB & sqBB) && bitCount(CENTER_SQUARES_BB & lookUpBishopAttack(square, (whitePawns | blackPawns))) > 1)
            featureScore -= BISHOP_FIANCHETTO;
                        
        featureScore    -= SAFE_BISHOP_CHECK * bitCount(wKingBishopAttacks & attacks & ~evalData.attacks[WHITE][PAWN]);
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

        evalData.threats[WHITE] += ROOK_ATTACK_QUEEN * bitCount(attacks & b->getPieceBB<BLACK>(QUEEN));

        featureScore += SAFE_ROOK_CHECK * bitCount(bKingRookAttacks & attacks & ~evalData.attacks[BLACK][PAWN]);

        addToKingSafety(attacks, evalData.kingZone[BLACK], bkingSafety_attPiecesCount, bkingSafety_valueOfAttacks, 3);

        k = lsbReset(k);
    }

    k = b->getPieceBB()[BLACK_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied & ~b->getPieceBB()[BLACK_ROOK] & ~b->getPieceBB()[BLACK_QUEEN]);
        evalData.attacks[BLACK][ROOK] |= attacks;

        mobScore -= mobilityRook[bitCount(attacks & mobilitySquaresBlack)];

        evalData.threats[BLACK] += ROOK_ATTACK_QUEEN * bitCount(attacks & b->getPieceBB<WHITE>(QUEEN));

        featureScore -= SAFE_ROOK_CHECK * bitCount(wKingRookAttacks & attacks & ~evalData.attacks[WHITE][PAWN]);

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
            + bitCount(evalData.semiOpen[WHITE] & ~openFiles & b->getPieceBB(WHITE, ROOK))
            - bitCount(evalData.semiOpen[BLACK] & ~openFiles & b->getPieceBB(BLACK, ROOK)));
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
        featureScore += SAFE_QUEEN_CHECK * bitCount((bKingRookAttacks | bKingBishopAttacks) & attacks & ~evalData.attacks[BLACK][PAWN]);

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
        featureScore -= SAFE_QUEEN_CHECK * bitCount((wKingRookAttacks | wKingBishopAttacks) & attacks & ~evalData.attacks[WHITE][PAWN]);

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
    EvalScore pinnedEvalScore  = computePinnedPieces<WHITE>(b) - computePinnedPieces<BLACK>(b);
    EvalScore passedScore      = computePassedPawns<WHITE>(b) - computePassedPawns<BLACK>(b);

    evalScore += kingSafetyTable[bkingSafety_valueOfAttacks] - kingSafetyTable[wkingSafety_valueOfAttacks];

    featureScore += CASTLING_RIGHTS
                    * (+b->getCastlingRights(WHITE_QUEENSIDE_CASTLING)
                       + b->getCastlingRights(WHITE_KINGSIDE_CASTLING)
                       - b->getCastlingRights(BLACK_QUEENSIDE_CASTLING)
                       - b->getCastlingRights(BLACK_KINGSIDE_CASTLING));
    featureScore += SIDE_TO_MOVE             * (b->getActivePlayer() == WHITE ? 1 : -1);

    EvalScore totalScore = evalScore + pinnedEvalScore + hangingEvalScore + featureScore + mobScore + passedScore + evalData.threats[WHITE] - evalData.threats[BLACK];
    res = (int) ((float) MgScore(totalScore + materialScore) * (1 - phase));
    Score eg = EgScore(totalScore + materialScore);
    eg = eg*(120-(8-bitCount(b->getPieceBB(eg > 0 ? WHITE : BLACK, PAWN)))*(8-bitCount(b->getPieceBB(eg > 0 ? WHITE : BLACK, PAWN)))) / 100;
    res += (int) ((float) eg * (phase));

    if (!hasMatingMaterial(b, res > 0 ? WHITE : BLACK))
        res = res / 10;
    return res;
}

void printEvaluation(Board* board) {
    UCI_ASSERT(board);

    using namespace std;

    Evaluator ev {};
    Score     score = ev.evaluate(board);
    float     phase = ev.phase;

    std::cout << setw(15) << right << "evaluation: " << left << setw(8) << score << setw(15) << right << "phase: " << left << setprecision(3) << setw(8) << phase << std::endl;
}

