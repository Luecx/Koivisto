
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

EvalScore SIDE_TO_MOVE                  = M(   14,   14);
EvalScore PAWN_STRUCTURE                = M(    8,    2);
EvalScore PAWN_PASSED_AND_DOUBLED       = M(  -12,  -35);
EvalScore PAWN_PASSED_AND_BLOCKED       = M(    1,  -35);
EvalScore PAWN_PASSED_COVERED_PROMO     = M(   -2,    8);
EvalScore PAWN_PASSED_HELPER            = M(    2,    4);
EvalScore PAWN_PASSED_AND_DEFENDED      = M(   11,    0);
EvalScore PAWN_PASSED_SQUARE_RULE       = M(    4,   15);
EvalScore PAWN_PASSED_EDGE_DISTANCE     = M(   -4,   -4);
EvalScore PAWN_PASSED_KING_TROPISM      = M(   -2,   13);
EvalScore PAWN_ISOLATED                 = M(   -0,   -7);
EvalScore PAWN_DOUBLED                  = M(   -6,   -7);
EvalScore PAWN_DOUBLED_AND_ISOLATED     = M(   -5,  -21);
EvalScore PAWN_BACKWARD                 = M(   -9,   -2);
EvalScore PAWN_OPEN                     = M(   -9,  -10);
EvalScore PAWN_BLOCKED                  = M(   -4,   -9);
EvalScore PAWN_CONNECTED                = M(    8,    8);
EvalScore KNIGHT_OUTPOST                = M(   22,   18);
EvalScore KNIGHT_DISTANCE_ENEMY_KING    = M(   -5,   -2);
EvalScore ROOK_OPEN_FILE                = M(   23,   -1);
EvalScore ROOK_HALF_OPEN_FILE           = M(    1,   -9);
EvalScore ROOK_KING_LINE                = M(   22,    2);
EvalScore BISHOP_DOUBLED                = M(   13,   74);
EvalScore BISHOP_FIANCHETTO             = M(   23,   29);
EvalScore BISHOP_STUNTED                = M(   -6,  -10);
EvalScore BISHOP_PIECE_SAME_SQUARE_E    = M(    3,    2);
EvalScore QUEEN_DISTANCE_ENEMY_KING     = M(  -19,  -13);
EvalScore KING_CLOSE_OPPONENT           = M(  -13,   14);
EvalScore KING_PAWN_SHIELD              = M(   25,    7);
EvalScore CASTLING_RIGHTS               = M(   16,    1);
EvalScore MINOR_BEHIND_PAWN             = M(    5,   19);
EvalScore SAFE_QUEEN_CHECK              = M(    4,   25);
EvalScore SAFE_ROOK_CHECK               = M(    9,    5);
EvalScore SAFE_BISHOP_CHECK             = M(    8,    4);
EvalScore SAFE_KNIGHT_CHECK             = M(   11,    4);
EvalScore PAWN_ATTACK_MINOR             = M(   38,   65);
EvalScore PAWN_ATTACK_ROOK              = M(   40,   25);
EvalScore PAWN_ATTACK_QUEEN             = M(   29,   28);
EvalScore MINOR_ATTACK_ROOK             = M(   34,   24);
EvalScore MINOR_ATTACK_QUEEN            = M(   23,   37);
EvalScore ROOK_ATTACK_QUEEN             = M(   31,   15);

EvalScore mobilityKnight[9] = {
    M(  -69,   11), M(  -59,   56), M(  -54,   82), M(  -50,   97), M(  -46,  106),
    M(  -42,  115), M(  -36,  117), M(  -27,  112), M(  -15,   97), };

EvalScore mobilityBishop[14] = {
    M(  -20,  -12), M(  -11,   44), M(   -4,   71), M(   -0,   88), M(    4,  100),
    M(    8,  111), M(    8,  118), M(    8,  122), M(    9,  125), M(   12,  123),
    M(   19,  118), M(   34,  110), M(   44,  118), M(   64,   91), };

EvalScore mobilityRook[15] = {
    M(  -73,   81), M(  -67,  118), M(  -65,  151), M(  -64,  174), M(  -63,  188),
    M(  -57,  194), M(  -52,  201), M(  -45,  204), M(  -40,  209), M(  -35,  215),
    M(  -31,  219), M(  -27,  222), M(  -18,  220), M(    8,  202), M(   61,  172), };

EvalScore mobilityQueen[28] = {
    M( -197,  136), M( -181,  139), M( -170,  240), M( -166,  306), M( -164,  341),
    M( -163,  365), M( -162,  386), M( -160,  399), M( -158,  409), M( -156,  415),
    M( -154,  420), M( -152,  424), M( -150,  425), M( -150,  429), M( -150,  431),
    M( -151,  431), M( -151,  430), M( -152,  429), M( -150,  425), M( -143,  416),
    M( -135,  403), M( -133,  394), M( -137,  386), M( -118,  369), M( -179,  394),
    M(  -60,  306), M(  -98,  357), M( -193,  438), };

EvalScore hangingEval[5] = {
    M(   -3,   -2), M(   -4,   -1), M(   -5,   -6), M(   -4,   -5), M(   -4,   -7), };

EvalScore pinnedEval[15] = {
    M(    2,   -3), M(   -9,    8), M(  -11,   52), M(  -18,  -54), M(  -21,  -14),
    M(  -21,   46), M(   -1,   -9), M(  -26,  -14), M(  -18,   36), M(  -10,  -10),
    M(    2,  -10), M(  -17,   38), M(   12,  -18), M(  -10,  -31), M(  -19,   53), };

EvalScore passer_rank_n[N_RANKS] = {
    M(    0,    0), M(  -13,    5), M(  -22,   19), M(  -15,   55),
    M(   11,   91), M(   31,  152), M(   12,   62), M(    0,    0), };

EvalScore candidate_passer[N_RANKS] = {
    M(    0,    0), M(  -21,    6), M(   -6,   13), M(   -3,   23),
    M(    3,   70), M(    5,   65), M(    0,    0), M(    0,    0), };

EvalScore bishop_pawn_same_color_table_o[9] = {
    M(  -48,   44), M(  -53,   45), M(  -52,   32),
    M(  -54,   23), M(  -57,   14), M(  -60,    3),
    M(  -62,  -12), M(  -61,  -26), M(  -68,  -63), };

EvalScore bishop_pawn_same_color_table_e[9] = {
    M(  -37,   26), M(  -54,   38), M(  -59,   32),
    M(  -63,   28), M(  -67,   21), M(  -70,   11),
    M(  -71,   -2), M(  -69,  -10), M(  -78,  -17), };

EvalScore kingSafetyTable[100] = {
    M(   -5,   -4), M(    0,    0), M(  -10,   -4), M(   -4,   -6), M(   -6,   -6),
    M(   18,   -9), M(    7,  -10), M(   27,   -9), M(   15,  -10), M(   43,  -20),
    M(   57,  -13), M(   76,  -24), M(   45,  -22), M(  102,  -23), M(  101,  -20),
    M(  108,  -14), M(  103,  -31), M(  162,  -21), M(  187,  -38), M(  206,  -58),
    M(  213,  -69), M(  170,  -25), M(  286,  -54), M(  226,  -16), M(  268,  -28),
    M(  256,    8), M(  355,  -14), M(  368,  -68), M(  299,   76), M(  394,   -9),
    M(  430, -162), M(  457,  -75), M(  575, -244), M(  651, -245), M(  493, -109),
    M( 1843,-3146), M(  -41, 1609), M( 1656,-2032), M(  214,  788), M( 1170,  882),
    M(  377,   13), M(  988, -690), M( 1654, 1348), M(  500,  500), M(  501,  500),
    M( 1787, 1507), M(  500,  500), M( 1001,  712), M(  500,  500), M(-1493,-2700),
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
    
    &KNIGHT_OUTPOST,
    &KNIGHT_DISTANCE_ENEMY_KING,
    
    &ROOK_OPEN_FILE,
    &ROOK_HALF_OPEN_FILE,
    &ROOK_KING_LINE,
    
    &BISHOP_DOUBLED,
    &BISHOP_FIANCHETTO,
    &BISHOP_STUNTED,
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

int kingSafetyAttackWeights[6]{
    0,2,2,3,4,0
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

            // https://www.chessprogramming.org/King_Pawn_Tropism
            // looking to the advanced square is more important
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

EvalScore Evaluator::computePawns(Board* b){
    
    EvalScore res = 0;
    
    
    // clang-format off
    U64 whiteTeam = b->getTeamOccupiedBB()[WHITE];
    U64 blackTeam = b->getTeamOccupiedBB()[BLACK];
    
    
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
    evalData.openFiles = openFiles;
    
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
    evalData.allAttacks[WHITE] |= evalData.attacks[WHITE][PAWN];
    evalData.allAttacks[BLACK] |= evalData.attacks[BLACK][PAWN];
    
    evalData.mobilitySquares[WHITE] = ~whiteTeam & ~(evalData.attacks[BLACK][PAWN]);
    evalData.mobilitySquares[BLACK] = ~blackTeam & ~(evalData.attacks[WHITE][PAWN]);
    
    evalData.threats[WHITE] = PAWN_ATTACK_MINOR * bitCount(evalData.attacks[WHITE][PAWN] & (b->getPieceBB<BLACK>(KNIGHT) | b->getPieceBB<BLACK>(BISHOP)));
    evalData.threats[BLACK] = PAWN_ATTACK_MINOR * bitCount(evalData.attacks[BLACK][PAWN] & (b->getPieceBB<WHITE>(KNIGHT) | b->getPieceBB<WHITE>(BISHOP)));
    
    evalData.threats[WHITE] += PAWN_ATTACK_ROOK * bitCount(evalData.attacks[WHITE][PAWN] & b->getPieceBB<BLACK>(ROOK));
    evalData.threats[BLACK] += PAWN_ATTACK_ROOK * bitCount(evalData.attacks[BLACK][PAWN] & b->getPieceBB<WHITE>(ROOK));
    
    evalData.threats[WHITE] += PAWN_ATTACK_QUEEN * bitCount(evalData.attacks[WHITE][PAWN] & b->getPieceBB<BLACK>(QUEEN));
    evalData.threats[BLACK] += PAWN_ATTACK_QUEEN * bitCount(evalData.attacks[BLACK][PAWN] & b->getPieceBB<WHITE>(QUEEN));
    
    res += PAWN_DOUBLED_AND_ISOLATED * (
        + bitCount(whiteIsolatedPawns & whiteDoubledPawns)
        - bitCount(blackIsolatedPawns & blackDoubledPawns));
    res += PAWN_DOUBLED * (
        + bitCount(~whiteIsolatedPawns & whiteDoubledPawns)
        - bitCount(~blackIsolatedPawns & blackDoubledPawns));
    res += PAWN_ISOLATED * (
        + bitCount(whiteIsolatedPawns & ~whiteDoubledPawns)
        - bitCount(blackIsolatedPawns & ~blackDoubledPawns));
    res += PAWN_STRUCTURE * (
        + bitCount(whitePawnEastCover)
        + bitCount(whitePawnWestCover)
        - bitCount(blackPawnEastCover)
        - bitCount(blackPawnWestCover));
    res += PAWN_OPEN * (
        + bitCount(whitePawns & ~evalData.attacks[WHITE][PAWN] & ~fillSouth(blackPawns))
        - bitCount(blackPawns & ~evalData.attacks[BLACK][PAWN] & ~fillNorth(whitePawns)));
    res += PAWN_BACKWARD * (
        + bitCount(fillSouth(~wAttackFrontSpans(whitePawns) & evalData.attacks[BLACK][PAWN]) & whitePawns)
        - bitCount(fillNorth(~bAttackFrontSpans(blackPawns) & evalData.attacks[WHITE][PAWN]) & blackPawns));
    res += PAWN_BLOCKED * (
        + bitCount(whiteBlockedPawns)
        - bitCount(blackBlockedPawns));
    res += PAWN_CONNECTED * (
        bitCount(whiteConnectedPawns)
        - bitCount(blackConnectedPawns));
    res += MINOR_BEHIND_PAWN * (
        + bitCount(shiftNorth(b->getPieceBB()[WHITE_KNIGHT]|b->getPieceBB()[WHITE_BISHOP])&(b->getPieceBB()[WHITE_PAWN]|b->getPieceBB()[BLACK_PAWN]))
        - bitCount(shiftSouth(b->getPieceBB()[BLACK_KNIGHT]|b->getPieceBB()[BLACK_BISHOP])&(b->getPieceBB()[WHITE_PAWN]|b->getPieceBB()[BLACK_PAWN])));
    // clang-format on
    return res;
}

template<Color color, PieceType pieceType>
EvalScore Evaluator::computePieces(Board* b){
    EvalScore score = 0;
    
    
    U64 k = b->getPieceBB<color>(pieceType);
    while (k) {
        
        // get the square
        Square square  = bitscanForward(k);
        U64 attacks;
        U64 squareBB = ONE << square;
        
        // compute the attacks
        if constexpr (pieceType == KNIGHT){
            attacks = KNIGHT_ATTACKS[square];
        } else if constexpr (pieceType == BISHOP){
            attacks = lookUpBishopAttack(square, b->getOccupiedBB()
                                                   & ~b->getPieceBB<color>(QUEEN));
        } else if constexpr (pieceType == ROOK){
            attacks = lookUpRookAttack(square, b->getOccupiedBB()
                                                   & ~b->getPieceBB<color>(QUEEN)
                                                   & ~b->getPieceBB<color>(ROOK));
        } else if constexpr (pieceType == QUEEN){
            attacks = lookUpRookAttack  (square, b->getOccupiedBB() & ~b->getPieceBB<color>(ROOK))
                    | lookUpBishopAttack(square, b->getOccupiedBB() & ~b->getPieceBB<color>(BISHOP));
        }
        
        // add to the attack table
        evalData.attacks   [color][pieceType] |= attacks;
        evalData.allAttacks[color]            |= attacks;
        
        // compute mobility values
        score        += mobilities[pieceType][bitCount(attacks & evalData.mobilitySquares[color])];
        
        // minors attacking rooks or queens
        if constexpr (pieceType == KNIGHT || pieceType == BISHOP){
            evalData.threats[color] += MINOR_ATTACK_ROOK  * bitCount(attacks & b->getPieceBB<!color>(ROOK));
            evalData.threats[color] += MINOR_ATTACK_QUEEN * bitCount(attacks & b->getPieceBB<!color>(QUEEN));
        }

        // knight specific code
        if constexpr (pieceType == KNIGHT) {
            score += KNIGHT_OUTPOST *
                     isOutpost(square, color, b->getPieceBB<!color>(PAWN), evalData.attacks[color][PAWN]);
            score += KNIGHT_DISTANCE_ENEMY_KING *
                     manhattanDistance(square, evalData.kingSquare[!color]);
            score += SAFE_KNIGHT_CHECK * bitCount(
                KNIGHT_ATTACKS[evalData.kingSquare[!color]] & ~b->getTeamOccupiedBB<color>()
                & attacks
                & ~evalData.attacks[!color][PAWN]);
        }
        
        // bishop specific code
        if constexpr (pieceType == BISHOP){
            
            // check if one white or black square
            U64 sameColoredSquares = squareBB & WHITE_SQUARES_BB ? WHITE_SQUARES_BB:BLACK_SQUARES_BB;
            
            
            // bonus for pawns of each color on the same colored squares
            score    += bishop_pawn_same_color_table_e[bitCount(b->getPieceBB<!color>(PAWN)
                & sameColoredSquares)];
            score    += bishop_pawn_same_color_table_o[bitCount(b->getPieceBB< color>(PAWN)
                & sameColoredSquares)];
            // bonus for hostile pieces on the same colored squares
            score    += BISHOP_PIECE_SAME_SQUARE_E
                           * bitCount(b->getTeamOccupiedBB<!color>() & sameColoredSquares);


            // as implemented in well-known engines i.e. Ethereal (it doesn't seem to gain if you
            // recognize full occupancy)
            if (!(CENTER_SQUARES_BB & squareBB)
                && bitCount(CENTER_SQUARES_BB & lookUpBishopAttack(
                          square, (b->getPieceBB<WHITE>(PAWN) | b->getPieceBB<BLACK>(PAWN)))) > 1)
                score += BISHOP_FIANCHETTO;
            
            // penality for bishops attacking defended pawns
            if (attacks & b->getPieceBB<!color>(PAWN) & evalData.attacks[!color][PAWN])
                score += BISHOP_STUNTED;
            
            // bonus for safe bishop checks
            score += SAFE_BISHOP_CHECK * bitCount(
                lookUpBishopAttack(
                             evalData.kingSquare[!color],
                             b->getOccupiedBB()) & ~b->getTeamOccupiedBB<color>()
                & attacks
                & ~evalData.attacks[!color][PAWN]);
        }
    
        // rook specific code
        if constexpr (pieceType == ROOK){
            // rooks attacking queens
            evalData.threats[color] += ROOK_ATTACK_QUEEN * bitCount(attacks & b->getPieceBB<!color>(QUEEN));
    
            // safe rook checks
            score += SAFE_ROOK_CHECK  * bitCount(
                lookUpRookAttack(
                    evalData.kingSquare[!color],
                    b->getOccupiedBB()) & ~b->getTeamOccupiedBB<color>()
                & attacks
                & ~evalData.attacks[!color][PAWN]);
        }
    
        // queen specific code
        if constexpr (pieceType == QUEEN){
            
            // distance to enemy king, acts sort of as some king safety
            score += QUEEN_DISTANCE_ENEMY_KING *
                        manhattanDistance(square, evalData.kingSquare[!color]);

            score += SAFE_QUEEN_CHECK
                     * bitCount((lookUpRookAttack(evalData.kingSquare[!color], b->getOccupiedBB())
                                     & ~b->getTeamOccupiedBB<color>()
                     | lookUpBishopAttack(evalData.kingSquare[!color], b->getOccupiedBB())
                                     & ~b->getTeamOccupiedBB<color>())
                                & attacks & ~evalData.attacks[!color][PAWN]);
        }
        
        // king safety
        evalData.ksAttackValue[!color] += kingSafetyAttackWeights[pieceType] *
                                          bitCount(evalData.kingZone[!color] &  attacks);
        
        k = lsbReset(k);
    }
    
    
    // all set-wise operations
    if constexpr (color == WHITE){
        if constexpr (pieceType == BISHOP){
            score += BISHOP_DOUBLED * (
                + (bitCount(b->getPieceBB()[WHITE_BISHOP]) == 2)
                - (bitCount(b->getPieceBB()[BLACK_BISHOP]) == 2));
        }
        
        if constexpr (pieceType == ROOK){
            score += ROOK_KING_LINE * (
                + bitCount(lookUpRookAttack(evalData.kingSquare[BLACK], b->getOccupiedBB())
                                   & b->getPieceBB(WHITE, ROOK))
                - bitCount(lookUpRookAttack(evalData.kingSquare[WHITE], b->getOccupiedBB())
                                   & b->getPieceBB(BLACK, ROOK)));
            score += ROOK_OPEN_FILE * (
                + bitCount(evalData.openFiles & b->getPieceBB(WHITE, ROOK))
                - bitCount(evalData.openFiles & b->getPieceBB(BLACK, ROOK)));
            score += ROOK_HALF_OPEN_FILE * (
                + bitCount(evalData.semiOpen[WHITE] & ~evalData.openFiles & b->getPieceBB(WHITE, ROOK))
                - bitCount(evalData.semiOpen[BLACK] & ~evalData.openFiles & b->getPieceBB(BLACK, ROOK)));
        }
    }
    
    return score;
}

template<Color color> EvalScore Evaluator::computeKings(Board* b) {
    EvalScore res = 0;
    
    evalData.attacks   [color][KING] = KING_ATTACKS[evalData.kingSquare[color]];
    evalData.allAttacks[color]      |= evalData.attacks                [color][KING];
    res += KING_PAWN_SHIELD
           * bitCount(KING_ATTACKS[evalData.kingSquare[color]] & b->getPieceBB<color>(PAWN));
    res += KING_CLOSE_OPPONENT
           * bitCount(KING_ATTACKS[evalData.kingSquare[color]] & b->getTeamOccupiedBB<!color>());
    
    
    return res;
}

/**
 * evaluates the board.
 * @param b
 * @return
 */
bb::Score Evaluator::evaluate(Board* b, Score alpha, Score beta) {
    UCI_ASSERT(b);

    Score res = 0;

    EvalScore featureScore  = M(0, 0);
    EvalScore mobScore      = M(0, 0);
    EvalScore materialScore = b->getBoardStatus()->material();

    phase = (24.0f + phaseValues[5]
             - phaseValues[0] * bitCount(b->getPieceBB()[WHITE_PAWN] | b->getPieceBB()[BLACK_PAWN])
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
    

    Square whiteKingSquare = bitscanForward(b->getPieceBB()[WHITE_KING]);
    Square blackKingSquare = bitscanForward(b->getPieceBB()[BLACK_KING]);

    evalData = {};
    evalData.kingZone[WHITE] = KING_ATTACKS[whiteKingSquare];
    evalData.kingZone[BLACK] = KING_ATTACKS[blackKingSquare];
    evalData.kingSquare[WHITE] = whiteKingSquare;
    evalData.kingSquare[BLACK] = blackKingSquare;


    featureScore += computePawns                (b);
    featureScore += computePieces<WHITE, KNIGHT>(b) - computePieces<BLACK, KNIGHT>(b);
    featureScore += computePieces<WHITE, BISHOP>(b) - computePieces<BLACK, BISHOP>(b);
    featureScore += computePieces<WHITE, ROOK  >(b) - computePieces<BLACK, ROOK  >(b);
    featureScore += computePieces<WHITE, QUEEN >(b) - computePieces<BLACK, QUEEN >(b);
    featureScore += computeKings <WHITE>        (b) - computeKings <BLACK>        (b);
    
    featureScore += CASTLING_RIGHTS
                    * (+ b->getCastlingRights(WHITE_QUEENSIDE_CASTLING)
                       + b->getCastlingRights(WHITE_KINGSIDE_CASTLING)
                       - b->getCastlingRights(BLACK_QUEENSIDE_CASTLING)
                       - b->getCastlingRights(BLACK_KINGSIDE_CASTLING));
    featureScore += SIDE_TO_MOVE             * (b->getActivePlayer() == WHITE ? 1 : -1);

    EvalScore hangingEvalScore = computeHangingPieces(b);
    EvalScore pinnedEvalScore  = computePinnedPieces<WHITE>(b) - computePinnedPieces<BLACK>(b);
    EvalScore passedScore      = computePassedPawns<WHITE>(b) - computePassedPawns<BLACK>(b);
    EvalScore threatScore      = evalData.threats[WHITE] - evalData.threats[BLACK];

    EvalScore kingSafetyScore  = + kingSafetyTable[evalData.ksAttackValue[BLACK]]
                                 - kingSafetyTable[evalData.ksAttackValue[WHITE]];


    EvalScore totalScore =
            + pinnedEvalScore
            + hangingEvalScore
            + featureScore
            + mobScore
            + passedScore
            + threatScore
            + kingSafetyScore;

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

