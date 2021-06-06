
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
EvalScore PAWN_STRUCTURE                = M(    7,    3);
EvalScore PAWN_PASSED_AND_DOUBLED       = M(  -11,  -39);
EvalScore PAWN_PASSED_AND_BLOCKED       = M(    2,  -37);
EvalScore PAWN_PASSED_COVERED_PROMO     = M(   -2,    8);
EvalScore PAWN_PASSED_HELPER            = M(    3,    3);
EvalScore PAWN_PASSED_AND_DEFENDED      = M(   10,    1);
EvalScore PAWN_PASSED_SQUARE_RULE       = M(    3,   16);
EvalScore PAWN_PASSED_EDGE_DISTANCE     = M(   -4,   -4);
EvalScore PAWN_PASSED_KING_TROPISM      = M(   -1,   13);
EvalScore PAWN_ISOLATED                 = M(   -0,   -7);
EvalScore PAWN_DOUBLED                  = M(   -5,   -7);
EvalScore PAWN_DOUBLED_AND_ISOLATED     = M(   -5,  -22);
EvalScore PAWN_BACKWARD                 = M(   -9,   -2);
EvalScore PAWN_OPEN                     = M(   -8,  -10);
EvalScore PAWN_BLOCKED                  = M(   -4,   -8);
EvalScore PAWN_CONNECTED                = M(    7,    8);
EvalScore KNIGHT_OUTPOST                = M(   21,   18);
EvalScore KNIGHT_DISTANCE_ENEMY_KING    = M(   -7,   -1);
EvalScore ROOK_OPEN_FILE                = M(   21,    1);
EvalScore ROOK_HALF_OPEN_FILE           = M(    1,   -8);
EvalScore ROOK_KING_LINE                = M(   18,    3);
EvalScore BISHOP_DOUBLED                = M(   11,   75);
EvalScore BISHOP_FIANCHETTO             = M(   22,   29);
EvalScore BISHOP_STUNTED                = M(   -6,  -10);
EvalScore BISHOP_PIECE_SAME_SQUARE_E    = M(    2,    4);
EvalScore QUEEN_DISTANCE_ENEMY_KING     = M(  -20,   -6);
EvalScore KING_CLOSE_OPPONENT           = M(  -11,   13);
EvalScore KING_PAWN_SHIELD              = M(   24,    7);
EvalScore CASTLING_RIGHTS               = M(   15,    1);
EvalScore MINOR_BEHIND_PAWN             = M(    5,   19);
EvalScore SAFE_QUEEN_CHECK              = M(    4,   24);
EvalScore SAFE_ROOK_CHECK               = M(    8,    5);
EvalScore SAFE_BISHOP_CHECK             = M(    8,    4);
EvalScore SAFE_KNIGHT_CHECK             = M(   10,    5);
EvalScore PAWN_ATTACK_MINOR             = M(   36,   65);
EvalScore PAWN_ATTACK_ROOK              = M(   38,   27);
EvalScore PAWN_ATTACK_QUEEN             = M(   28,   30);
EvalScore MINOR_ATTACK_ROOK             = M(   32,   26);
EvalScore MINOR_ATTACK_QUEEN            = M(   22,   38);
EvalScore ROOK_ATTACK_QUEEN             = M(   30,   16);

EvalScore mobilityKnight[9] = {
    M(  -71,   29), M(  -61,   73), M(  -56,   97), M(  -53,  112), M(  -49,  121),
    M(  -45,  130), M(  -39,  132), M(  -31,  128), M(  -21,  114), };

EvalScore mobilityBishop[14] = {
    M(  -21,    0), M(  -12,   53), M(   -6,   79), M(   -2,   95), M(    2,  108),
    M(    5,  118), M(    6,  126), M(    5,  129), M(    6,  132), M(    9,  132),
    M(   16,  127), M(   28,  120), M(   38,  128), M(   53,  100), };

EvalScore mobilityRook[15] = {
    M(  -78,  100), M(  -72,  135), M(  -69,  167), M(  -69,  190), M(  -68,  203),
    M(  -62,  210), M(  -57,  216), M(  -51,  219), M(  -46,  225), M(  -41,  231),
    M(  -37,  236), M(  -33,  239), M(  -25,  237), M(   -1,  219), M(   39,  195), };

EvalScore mobilityQueen[28] = {
    M( -196,  164), M( -181,  168), M( -170,  262), M( -166,  324), M( -164,  357),
    M( -164,  380), M( -163,  399), M( -161,  411), M( -159,  421), M( -157,  427),
    M( -155,  432), M( -153,  435), M( -151,  437), M( -151,  440), M( -151,  442),
    M( -152,  443), M( -152,  441), M( -152,  440), M( -151,  437), M( -144,  428),
    M( -136,  415), M( -134,  407), M( -135,  397), M( -117,  380), M( -158,  391),
    M(  -77,  327), M( -120,  388), M( -126,  409), };

EvalScore hangingEval[5] = {
    M(   -3,   -2), M(   -3,   -1), M(   -5,   -6), M(   -4,   -4), M(   -4,   -6), };

EvalScore pinnedEval[15] = {
    M(    2,   -3), M(   -9,    8), M(   -9,   46), M(  -17,  -55), M(  -21,  -13),
    M(  -18,   39), M(   -1,   -9), M(  -26,  -13), M(  -15,   29), M(   -9,  -10),
    M(    0,   -7), M(  -14,   32), M(   10,  -14), M(  -15,  -21), M(  -16,   46), };

EvalScore passer_rank_n[N_RANKS] = {
    M(    0,    0), M(  -16,    8), M(  -24,   21), M(  -17,   58),
    M(    8,   96), M(   27,  159), M(    3,   66), M(    0,    0), };

EvalScore candidate_passer[N_RANKS] = {
    M(    0,    0), M(  -20,    5), M(   -6,   14), M(   -4,   25),
    M(    1,   74), M(    4,   67), M(    0,    0), M(    0,    0), };

EvalScore bishop_pawn_same_color_table_o[9] = {
    M(  -50,   50), M(  -54,   49), M(  -53,   38),
    M(  -55,   28), M(  -57,   19), M(  -60,    7),
    M(  -61,   -9), M(  -60,  -23), M(  -66,  -61), };

EvalScore bishop_pawn_same_color_table_e[9] = {
    M(  -43,   36), M(  -57,   44), M(  -61,   38),
    M(  -64,   31), M(  -66,   23), M(  -68,   12),
    M(  -68,   -3), M(  -66,  -14), M(  -72,  -24), };

EvalScore kingSafetyTable[100] = {
    M(   -4,   -4), M(    0,    0), M(   -9,   -4), M(   -4,   -6), M(   -6,   -8),
    M(   17,   -7), M(    7,  -10), M(   26,   -7), M(   15,  -10), M(   44,  -21),
    M(   54,  -10), M(   71,  -21), M(   44,  -23), M(   84,  -13), M(   99,  -19),
    M(  110,  -20), M(   99,  -29), M(  160,  -22), M(  185,  -41), M(  201,  -55),
    M(  202,  -61), M(  170,  -29), M(  284,  -60), M(  217,   -8), M(  257,  -24),
    M(  251,   12), M(  337,    8), M(  364,  -71), M(  288,   85), M(  379,    8),
    M(  410, -135), M(  452,  -71), M(  529, -197), M(  639, -242), M(  527, -210),
    M( 1828,-3136), M(  126, 1259), M( 1646,-2024), M(  246,  706), M( 1168,  882),
    M(  439, -173), M(  953, -667), M( 1653, 1347), M(  500,  500), M(  501,  500),
    M( 1786, 1506), M(  500,  500), M( 1000,  711), M(  500,  500), M(-1519,-2480),
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

int egPawnReductionA = 1280;
int egPawnReductionB = 15;


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

void Evaluator::computePartialScoreReduction(Board* b, Score& mgScore, Score& egScore) {
    // reduce by pawn count for winning side -> less pawns = worse
    int egPawnReductionPawnFactor = 8 - bitCount(b->getPieceBB(egScore > 0 ? WHITE : BLACK, PAWN));
    egScore                       = egScore
              * (egPawnReductionA
               - egPawnReductionB
                       * egPawnReductionPawnFactor
                       * egPawnReductionPawnFactor) / 1024;
}

void Evaluator::computeFinalScoreReduction(Board* b, Score& finalScore) {
    if (!hasMatingMaterial(b, finalScore > 0 ? WHITE : BLACK))
        finalScore = finalScore / 10;
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
    
    // https://www.chessprogramming.org/Connected_Pawns
    featureScore += PAWN_CONNECTED * (
              bitCount(whiteConnectedPawns)
            - bitCount(blackConnectedPawns));

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

        // as implemented in well-known engines i.e. Ethereal (it doesn't seem to gain if you recognize full occupancy)
        if (!(CENTER_SQUARES_BB & sqBB) && bitCount(CENTER_SQUARES_BB & lookUpBishopAttack(square, (whitePawns | blackPawns))) > 1)
            featureScore += BISHOP_FIANCHETTO;

        if (attacks & blackPawns & evalData.attacks[BLACK][PAWN])
            featureScore += BISHOP_STUNTED;
                        
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
                           
        // as implemented in well-known engines i.e. Ethereal (it doesn't seem to gain if you recognize full occupancy)
        if (!(CENTER_SQUARES_BB & sqBB) && bitCount(CENTER_SQUARES_BB & lookUpBishopAttack(square, (whitePawns | blackPawns))) > 1)
            featureScore -= BISHOP_FIANCHETTO;

        if (attacks & whitePawns & evalData.attacks[WHITE][PAWN])
            featureScore -= BISHOP_STUNTED;
                        
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
    EvalScore threatScore      = evalData.threats[WHITE] - evalData.threats[BLACK];
    EvalScore kingSafetyScore  = kingSafetyTable[bkingSafety_valueOfAttacks]
                               - kingSafetyTable[wkingSafety_valueOfAttacks];
    

    featureScore += CASTLING_RIGHTS
                    * (+b->getCastlingRights(WHITE_QUEENSIDE_CASTLING)
                       + b->getCastlingRights(WHITE_KINGSIDE_CASTLING)
                       - b->getCastlingRights(BLACK_QUEENSIDE_CASTLING)
                       - b->getCastlingRights(BLACK_KINGSIDE_CASTLING));
    featureScore += SIDE_TO_MOVE             * (b->getActivePlayer() == WHITE ? 1 : -1);
    
    EvalScore totalScore = evalScore
                           + pinnedEvalScore
                           + hangingEvalScore
                           + featureScore
                           + mobScore
                           + passedScore
                           + threatScore
                           + kingSafetyScore
                           + materialScore;
    
    Score mgScore = MgScore(totalScore);
    Score egScore = EgScore(totalScore);
    
    computePartialScoreReduction(b, mgScore, egScore);
   
    res  = (int) ((float) mgScore * (1 - phase));
    res += (int) ((float) egScore * (phase));
    
    computeFinalScoreReduction(b, res);
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

