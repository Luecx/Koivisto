
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

EvalScore SIDE_TO_MOVE                  = M(   14,   12);
EvalScore PAWN_PASSED_AND_DOUBLED       = M(  -12,  -40);
EvalScore PAWN_PASSED_AND_BLOCKED       = M(    4,  -44);
EvalScore PAWN_PASSED_COVERED_PROMO     = M(   -2,    8);
EvalScore PAWN_PASSED_HELPER            = M(    1,    3);
EvalScore PAWN_PASSED_AND_DEFENDED      = M(   14,    4);
EvalScore PAWN_PASSED_SQUARE_RULE       = M(    1,   38);
EvalScore PAWN_PASSED_EDGE_DISTANCE     = M(   -5,   -2);
EvalScore PAWN_ISOLATED                 = M(   -2,   -6);
EvalScore PAWN_DOUBLED                  = M(   -6,   -6);
EvalScore PAWN_DOUBLED_AND_ISOLATED     = M(   -6,  -20);
EvalScore PAWN_BACKWARD                 = M(   -6,   -5);
EvalScore PAWN_OPEN                     = M(   -6,   -5);
EvalScore PAWN_BLOCKED                  = M(   -3,   -7);
EvalScore KNIGHT_OUTPOST                = M(   22,   19);
EvalScore KNIGHT_DISTANCE_ENEMY_KING    = M(   -6,   -2);
EvalScore ROOK_OPEN_FILE                = M(   22,   -1);
EvalScore ROOK_HALF_OPEN_FILE           = M(    1,  -10);
EvalScore ROOK_KING_LINE                = M(   21,    1);
EvalScore BISHOP_DOUBLED                = M(   14,   73);
EvalScore BISHOP_FIANCHETTO             = M(   24,   27);
EvalScore BISHOP_PIECE_SAME_SQUARE_E    = M(    2,    3);
EvalScore QUEEN_DISTANCE_ENEMY_KING     = M(  -17,  -17);
EvalScore KING_CLOSE_OPPONENT           = M(  -13,   14);
EvalScore KING_PAWN_SHIELD              = M(   20,    9);
EvalScore CASTLING_RIGHTS               = M(   16,    1);
EvalScore MINOR_BEHIND_PAWN             = M(    5,   19);
EvalScore SAFE_QUEEN_CHECK              = M(    4,   25);
EvalScore SAFE_ROOK_CHECK               = M(    9,    5);
EvalScore SAFE_BISHOP_CHECK             = M(    9,    3);
EvalScore SAFE_KNIGHT_CHECK             = M(   11,    4);
EvalScore PAWN_ATTACK_MINOR             = M(   39,   64);
EvalScore PAWN_ATTACK_ROOK              = M(   39,   24);
EvalScore PAWN_ATTACK_QUEEN             = M(   30,   27);
EvalScore MINOR_ATTACK_ROOK             = M(   33,   24);
EvalScore MINOR_ATTACK_QUEEN            = M(   23,   36);
EvalScore ROOK_ATTACK_QUEEN             = M(   31,   14);

EvalScore mobilityKnight[9] = {
        M(  -68,   11), M(  -57,   56), M(  -53,   82), M(  -49,   97), M(  -44,  106), 
        M(  -41,  116), M(  -35,  117), M(  -26,  112), M(  -14,   96), };

EvalScore mobilityBishop[14] = {
        M(  -17,  -12), M(   -8,   41), M(   -1,   67), M(    3,   84), M(    8,   97),
        M(   11,  108), M(   12,  116), M(   12,  119), M(   13,  123), M(   17,  122),
        M(   24,  117), M(   38,  109), M(   48,  117), M(   69,   89), };

EvalScore mobilityRook[15] = {
        M(  -72,   72), M(  -66,  108), M(  -63,  140), M(  -62,  163), M(  -61,  177),
        M(  -56,  184), M(  -50,  190), M(  -43,  193), M(  -38,  198), M(  -33,  204),
        M(  -29,  208), M(  -25,  211), M(  -16,  209), M(    9,  192), M(   60,  162), };

EvalScore mobilityQueen[28] = {
        M( -198,  136), M( -182,  137), M( -170,  237), M( -167,  303), M( -165,  338),
        M( -165,  362), M( -163,  381), M( -161,  394), M( -159,  405), M( -157,  410), 
        M( -155,  416), M( -153,  419), M( -151,  421), M( -151,  425), M( -151,  427),
        M( -152,  427), M( -152,  426), M( -153,  425), M( -151,  421), M( -144,  412),
        M( -137,  399), M( -134,  390), M( -138,  382), M( -120,  366), M( -180,  391),
        M(  -62,  305), M(  -95,  352), M( -192,  433), };

EvalScore hangingEval[5] = {
        M(   -3,   -2), M(   -3,   -1), M(   -5,   -6), M(   -5,   -4), M(   -4,   -7), };

EvalScore pinnedEval[15] = {
        M(    3,   -4), M(   -9,    9), M(  -11,   51), M(  -19,  -54), M(  -21,  -14),
        M(  -21,   46), M(   -0,   -8), M(  -27,  -13), M(  -18,   35), M(   -9,  -11),
        M(    0,   -9), M(  -16,   39), M(   12,  -19), M(  -10,  -33), M(  -19,   54), };

EvalScore passer_rank_n[N_RANKS] = {
        M(    0,    0), M(  -10,   18), M(  -16,   20), M(  -12,   44),
        M(   18,   69), M(   39,  120), M(    8,   36), M(    0,    0), };

EvalScore candidate_passer[N_RANKS] = {
        M(    0,    0), M(  -27,    2), M(  -14,    9), M(   -7,   29),
        M(    2,   53), M(   11,   61), M(    0,    0), M(    0,    0), };

EvalScore connected_pawn[N_RANKS] = {
        M(    0,    0), M(    3,   -1), M(   12,   -1), M(   12,    1),
        M(   24,   22), M(   46,   77), M(   37,  117), M(    0,    0), };

EvalScore defense_connected_pawn[N_RANKS] = {
        M(    0,    0), M(    0,    0), M(   16,    2), M(   11,   -1),
        M(   19,    6), M(   42,   45), M(  167,   43), M(    0,    0), };

EvalScore bishop_pawn_same_color_table_o[9] = {
        M(  -51,   37), M(  -58,   39), M(  -57,   27),
        M(  -59,   18), M(  -62,    9), M(  -65,   -1),
        M(  -67,  -16), M(  -66,  -30), M(  -73,  -70), };

EvalScore bishop_pawn_same_color_table_e[9] = {
        M(  -35,   29), M(  -50,   41), M(  -53,   34),
        M(  -58,   28), M(  -61,   19), M(  -64,    7),
        M(  -66,  -10), M(  -66,  -22), M(  -76,  -34), };

EvalScore kingSafetyTable[100] = {
        M(   -4,   -1), M(    0,    0), M(   -9,   -1), M(   -1,   -4), M(   -5,   -4),
        M(   21,   -7), M(    9,   -8), M(   30,   -7), M(   17,   -8), M(   44,  -17),
        M(   59,  -10), M(   78,  -22), M(   47,  -21), M(  103,  -21), M(  104,  -19),
        M(  113,  -16), M(  105,  -31), M(  165,  -20), M(  192,  -42), M(  210,  -59),
        M(  215,  -69), M(  176,  -30), M(  288,  -56), M(  230,  -19), M(  272,  -34),
        M(  260,    5), M(  357,  -16), M(  370,  -73), M(  303,   73), M(  398,  -11), 
        M(  428, -157), M(  470, -101), M(  578, -246), M(  650, -240), M(  494, -104),
        M( 1873,-3218), M(  -31, 1620), M( 1659,-1951), M(  225,  770), M( 1155,  878),
        M(  448, -180), M(  992, -701), M( 1644, 1345), M(  500,  500), M(  501,  500),
        M( 1782, 1505), M(  500,  500), M(  993,  710), M(  500,  500), M( -995,-1151),
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
    &PAWN_PASSED_AND_DOUBLED,
    &PAWN_PASSED_AND_BLOCKED,
    &PAWN_PASSED_COVERED_PROMO,
    &PAWN_PASSED_HELPER,
    &PAWN_PASSED_AND_DEFENDED,
    &PAWN_PASSED_SQUARE_RULE,
    &PAWN_PASSED_EDGE_DISTANCE,
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
    
    U64 bb = pawns;
    
    while (bb) {
        Square s      = bitscanForward(bb);
        Rank   r      = color == WHITE ? rankIndex(s) : 7 - rankIndex(s);
        File   f      = fileIndex(s);
        U64    sqBB   = ONE << s;

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

template<Color color>
EvalScore Evaluator::computePawns(Board* b) {
    EvalScore h = M( 0, 0);
    
    U64 pawns = b->getPieceBB( color, PAWN);
    U64 pawnAttacks = color == WHITE ? 
        (shiftNorthEast(pawns) | shiftNorthWest(pawns)) : 
        (shiftSouthEast(pawns) | shiftSouthWest(pawns));
    U64 connected = pawns & (shiftEast(pawns) | shiftWest(pawns) | pawnAttacks);

    U64 bb = connected;
    while (bb) {
        U64 sqBB = bb & -bb;
        Square sq = bitscanForward(bb);
        Rank r = color == WHITE ? rankIndex(sq) : 7 - rankIndex(sq);
        bool rankConnected = pawns & (shiftWest(sqBB) | shiftEast(sqBB));
        
        if (rankConnected) {
            h += connected_pawn[r];
        } else {
            h += defense_connected_pawn[r];
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

    evalData.semiOpen[WHITE] = ~fillFile(blackPawns);
    evalData.semiOpen[BLACK] = ~fillFile(whitePawns);
    
    U64 openFiles          = evalData.semiOpen[WHITE] & evalData.semiOpen[BLACK];

    Square square;
    U64    attacks;
    U64    k;
    
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
    featureScore += PAWN_OPEN * (
            + bitCount(whitePawns & ~fillSouth(blackPawns))
            - bitCount(blackPawns & ~fillNorth(whitePawns)));
    featureScore += PAWN_BACKWARD * (
            + bitCount(fillSouth(~wAttackFrontSpans(whitePawns) & evalData.attacks[BLACK][PAWN]) & whitePawns)
            - bitCount(fillNorth(~bAttackFrontSpans(blackPawns) & evalData.attacks[WHITE][PAWN]) & blackPawns));
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
    EvalScore pawnsScore       = computePawns<WHITE>(b) - computePawns<BLACK>(b);
    EvalScore passedScore      = computePassedPawns<WHITE>(b) - computePassedPawns<BLACK>(b);

    evalScore += kingSafetyTable[bkingSafety_valueOfAttacks] - kingSafetyTable[wkingSafety_valueOfAttacks];

    featureScore += CASTLING_RIGHTS
                    * (+b->getCastlingRights(WHITE_QUEENSIDE_CASTLING)
                       + b->getCastlingRights(WHITE_KINGSIDE_CASTLING)
                       - b->getCastlingRights(BLACK_QUEENSIDE_CASTLING)
                       - b->getCastlingRights(BLACK_KINGSIDE_CASTLING));
    featureScore += SIDE_TO_MOVE             * (b->getActivePlayer() == WHITE ? 1 : -1);

    EvalScore totalScore = evalScore + pinnedEvalScore + hangingEvalScore + featureScore + mobScore + passedScore + pawnsScore + evalData.threats[WHITE] - evalData.threats[BLACK];
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

