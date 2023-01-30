
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
#include "newmovegen.h"
#include "attacks.h"
#include "movegen.h"

using namespace attacks;
using namespace bb;
using namespace move;

void moveGen::init(SearchData* sd, Board* b, Depth ply, Move hashMove, Move previous, Move followup, int mode, Square threatSquare, U64 checkerSq) {
    m_sd            = sd;
    m_board         = b;
    m_ply           = ply;
    m_hashMove      = hashMove;
    m_previous      = previous;
    m_followup      = followup;
    m_mode          = mode;
    stage           = GET_HASHMOVE;
    quietSize       = 0;
    noisySize       = 0;
    goodNoisyCount  = 0;
    noisy_index     = 0;
    quiet_index     = 0;
    searched_index  = 0;
    c               = b->getActivePlayer();
    m_skip          = false;
    m_killer1       = KILLER1(m_sd, c, m_ply);
    m_killer2       = KILLER2(m_sd, c, m_ply);
    m_threatSquare  = threatSquare;
    m_checkerSq     = checkerSq;
    m_cmh           = &CMH(sd, previous, c, 0);
    m_fmh           = &sd->fmh[followup ? getPieceTypeSqToCombination(followup) : 384][c][0];
    m_th            = &THREAT_HISTORY(sd, c, m_threatSquare, 0);}

Move moveGen::next() {
    switch (stage) {
        case GET_HASHMOVE:
            stage++;
            if (m_board->isPseudoLegal(m_hashMove))
                return m_hashMove;
            // fallthrough
        case GEN_NOISY:
            generateNoisy();
            stage++;
            // fallthrough
        case GET_GOOD_NOISY:
            if (noisy_index < (m_mode & Q_SEARCHCHECK ? noisySize : goodNoisyCount)) 
                return nextNoisy();
            if (m_mode == Q_SEARCH)
                return 0;
            if (m_mode == Q_SEARCHCHECK) {
                stage = QS_EVASIONS;
                m_killer1 = 0;
                m_killer2 = 0;
                generateEvasions();
                if (quiet_index < quietSize)
                    return nextQuiet();
                return 0;
            }
            stage++;
            // fallthrough
        case KILLER1:
            stage++;
            if (!sameMove(m_killer1, m_hashMove) && m_board->isPseudoLegal(m_killer1))
                return m_killer1;
            // fallthrough
        case KILLER2:
            stage++;
            if (!sameMove(m_killer2, m_hashMove) && m_board->isPseudoLegal(m_killer2))
                return m_killer2;
            // fallthrough
        case GEN_QUIET:
            if (shouldSkip()) {
                stage = GET_BAD_NOISY;
                return next();
            }
            generateQuiet();
            stage++;
            // fallthrough
        case GET_QUIET:
            if (quiet_index < quietSize)
                return nextQuiet();
            stage++;
            // fallthrough
        case GET_BAD_NOISY:
            if (noisy_index < noisySize) 
                return nextNoisy();
            stage++;
            // fallthrough
        case END:
            return 0;
        
        case QS_EVASIONS:
            if (quiet_index < quietSize)
                return nextQuiet();
            stage = END;
            return 0;
    }

    return 0;
}

void moveGen::addNoisy(Move m) {
    if (sameMove(m_hashMove, m))
        return;
    int score   = m_board->staticExchangeEvaluation(m);
    noisySee[noisySize] = score;
    //int mvvLVA  = piece_values[(getCapturedPieceType(m))];
    if (score >= 0) {
        score = 100000 + m_sd->getHistories(m, c, m_previous, m_followup, m_threatSquare) + score  + 150 * (getSquareTo(m) == getSquareTo(m_previous));
        goodNoisyCount++;
    } else {
        score = 10000 + m_sd->getHistories(m, c, m_previous, m_followup, m_threatSquare);
    }
    noisy[noisySize] = m;
    noisyScores[noisySize++] = score;
}

void moveGen::addQuiet(Move m) {
    if (sameMove(m_hashMove, m) || sameMove(m_killer1, m) || sameMove(m_killer2, m))
        return;
    quiets[quietSize] = m;
    quietScores[quietSize++] = m_th[getSqToSqFromCombination(m)] 
                             + m_cmh[getPieceTypeSqToCombination(m)] 
                             + m_fmh[getPieceTypeSqToCombination(m)];
}

Move moveGen::nextNoisy() {
    if (m_skip) {
        lastSee = noisySee[noisy_index];
        return noisy[noisy_index++];
    }
    int bestNoisy = noisy_index;
    for (int i = noisy_index + 1; i < noisySize; i++) {
        if (noisyScores[i] > noisyScores[bestNoisy])
            bestNoisy = i;
    }
    Move m  = noisy[bestNoisy];
    lastSee = noisySee[bestNoisy];
    noisySee[bestNoisy]     = noisySee[noisy_index];
    noisyScores[bestNoisy]  = noisyScores[noisy_index];
    noisy[bestNoisy]        = noisy[noisy_index++];
    return m;
}

Move moveGen::nextQuiet() {
    if (m_skip) {
        for (int i = quiet_index; i < quietSize; i++) {
            if ((m_checkerSq & (ONE << getSquareTo(quiets[i])))) {
                quiet_index = i;
                return quiets[quiet_index++];
            }
        }
        stage++;
        return next();
    }
    int bestQuiet = quiet_index;
    for (int i = quiet_index + 1; i < quietSize; i++) {
        if (quietScores[i] > quietScores[bestQuiet])
            bestQuiet = i;
    }
    Move m = quiets[bestQuiet];
    quietScores[bestQuiet]  = quietScores[quiet_index];
    quiets[bestQuiet]       = quiets[quiet_index++];
    return m;
}

void moveGen::addSearched(Move m) {
    searched[searched_index++] = m;
}

template<Color us, Stage stage>
inline void generatePawnMoves(Board* board, U64 occupied_bb, U64 mask_bb, moveGen* move_gen){
    constexpr Color them = !us;
    constexpr Piece piece = bb::getPiece(us, PAWN);
    
    // masks for promotions and non-promotions
    constexpr U64 promo_mask = (us == WHITE ? RANK_8_BB : RANK_1_BB);
    constexpr U64 non_promo_mask = (us == WHITE ? ~RANK_8_BB : ~RANK_1_BB);
    
    // get piece bb
    U64 piece_bb = board->getPieceBB<us, PAWN>();
    
    // enemy bb
    const U64 enemy_bb = board->getTeamOccupiedBB<them>();
    
    // adjust promotion possibility for quiet moves
    if constexpr (stage == GEN_QUIET){
        mask_bb |= (us == WHITE ? ~RANK_8_BB : ~RANK_1_BB);
    }
    
    if(stage == GEN_NOISY){
        
        // inverse direction for left and right attacks
        // used for pawn left and right attacks to compute origin square
        constexpr Direction inv_la = us == WHITE ? SOUTH_EAST : NORTH_EAST;
        constexpr Direction inv_ra = us == WHITE ? SOUTH_WEST : NORTH_WEST;
        constexpr Direction inv_fw = us == WHITE ? SOUTH      : NORTH     ;
        
        // attacks to the left and right
        U64 left    = us == WHITE ? shiftNorthWest(piece_bb) : shiftSouthWest(piece_bb);
        U64 right   = us == WHITE ? shiftNorthEast(piece_bb) : shiftSouthEast(piece_bb);
        U64 forward = us == WHITE ? shiftNorth    (piece_bb) : shiftSouth    (piece_bb);
        
        // e.p. left and right
        U64 ep_left  = left  & board->getBoardStatus()->enPassantTarget;
        U64 ep_right = right & board->getBoardStatus()->enPassantTarget;
        
        // remove attacks which dont actual attack enemies
        left &= enemy_bb & mask_bb;
        right &= enemy_bb & mask_bb;
        // remove forwards if there is a piece in front
        forward &= ~occupied_bb & mask_bb;
        
        // promotions left and right
        U64 promo_left = left & promo_mask;
        U64 promo_right = right & promo_mask;
        // actual attacks left and right
        U64 attacks_left = left & non_promo_mask;
        U64 attacks_right = right & non_promo_mask;
        // forward promos
        U64 promos = forward & promo_mask & ~occupied_bb;
        
        
        while (attacks_left) {
            Square target = bitscanForward(attacks_left);
            move_gen->addNoisy(genMove(target + inv_la, target,  CAPTURE, piece, board->getPiece(target)));
            attacks_left = lsbReset(attacks_left);
        }
        while (attacks_right) {
            Square target = bitscanForward(attacks_right);
            move_gen->addNoisy(genMove(target + inv_ra, target,  CAPTURE, piece, board->getPiece(target)));
            attacks_right = lsbReset(attacks_right);
        }
        if(ep_left){
            Square sq_to = bitscanForward(ep_left);
            move_gen->addNoisy(genMove(sq_to + inv_la, sq_to,  EN_PASSANT, piece));
        }
        if(ep_right){
            Square sq_to = bitscanForward(ep_right);
            move_gen->addNoisy(genMove(sq_to + inv_ra, sq_to,  EN_PASSANT, piece));
        }
        while (promos) {
            Square target = bitscanForward(promos);
            move_gen->addNoisy(genMove(target + inv_fw, target,  QUEEN_PROMOTION, piece));
            promos = lsbReset(promos);
        }
        while (promo_left) {
            Square target = bitscanForward(promo_left);
            move_gen->addNoisy(genMove(target + inv_la, target,  QUEEN_PROMOTION_CAPTURE, piece, board->getPiece(target)));
            promo_left = lsbReset(promo_left);
        }
        while (promo_right) {
            Square target = bitscanForward(promo_right);
            move_gen->addNoisy(genMove(target + inv_ra, target,  QUEEN_PROMOTION_CAPTURE, piece, board->getPiece(target)));
            promo_right = lsbReset(promo_right);
        }
        
    }else{
        const U64           quiet_squares  = ~occupied_bb & mask_bb;
        constexpr U64       double_pp_mask = (us == WHITE ? RANK_4_BB : RANK_5_BB);

        constexpr Direction inv_la         = us == WHITE ? SOUTH_EAST : NORTH_EAST;
        constexpr Direction inv_ra         = us == WHITE ? SOUTH_WEST : NORTH_WEST;
        constexpr Direction inv_fw         = us == WHITE ? SOUTH : NORTH;
        constexpr Direction inv_fw2        = inv_fw * 2;
        
        // attacks to the left and right
        U64 left     = (us == WHITE ? shiftNorthWest(piece_bb) : shiftSouthWest(piece_bb));
        U64 right    = (us == WHITE ? shiftNorthEast(piece_bb) : shiftSouthEast(piece_bb));
        U64 forward  = (us == WHITE ? shiftNorth    (piece_bb) : shiftSouth    (piece_bb)) & quiet_squares;
        U64 forward2 = (us == WHITE ? shiftNorth    (forward ) : shiftSouth    (forward )) & double_pp_mask & quiet_squares;
        
        // remove attacks which dont actual attack enemies
        left  &= enemy_bb & mask_bb;
        right &= enemy_bb & mask_bb;

        // all promotions
        U64 promos      = forward & promo_mask & mask_bb;
        U64 promo_left  = left    & promo_mask & mask_bb;
        U64 promo_right = right   & promo_mask & mask_bb;

        // pawn pushes
        forward  &= non_promo_mask;
        forward2 &= non_promo_mask;

        while (forward) {
            Square target = bitscanForward(forward);
            move_gen->addQuiet(genMove(target + inv_fw, target, QUIET, piece));
            forward = lsbReset(forward);
        }
        while (forward2) {
            Square target = bitscanForward(forward2);
            move_gen->addQuiet(genMove(target + inv_fw2, target, DOUBLED_PAWN_PUSH, piece));
            forward2 = lsbReset(forward2);
        }
        while (promos) {
            Square target = bitscanForward(promos);
//            move_gen->addQuiet(genMove(target + inv_fw, target,   ROOK_PROMOTION, piece));
            move_gen->addQuiet(genMove(target + inv_fw, target, KNIGHT_PROMOTION, piece));
//            move_gen->addQuiet(genMove(target + inv_fw, target, BISHOP_PROMOTION, piece));
            promos = lsbReset(promos);
        }
        while (promo_left) {
            Square target = bitscanForward(promo_left);
//            move_gen->addQuiet(genMove(target + inv_la, target,   ROOK_PROMOTION_CAPTURE, piece, board->getPiece(target)));
            move_gen->addQuiet(genMove(target + inv_la, target, KNIGHT_PROMOTION_CAPTURE, piece, board->getPiece(target)));
//            move_gen->addQuiet(genMove(target + inv_la, target, BISHOP_PROMOTION_CAPTURE, piece, board->getPiece(target)));
            promo_left = lsbReset(promo_left);
        }
        while (promo_right) {
            Square target = bitscanForward(promo_right);
//            move_gen->addQuiet(genMove(target + inv_ra, target,   ROOK_PROMOTION_CAPTURE, piece, board->getPiece(target)));
            move_gen->addQuiet(genMove(target + inv_ra, target, KNIGHT_PROMOTION_CAPTURE, piece, board->getPiece(target)));
//            move_gen->addQuiet(genMove(target + inv_ra, target, BISHOP_PROMOTION_CAPTURE, piece, board->getPiece(target)));
            promo_right = lsbReset(promo_right);
        }
        
    }
    
}

template<PieceType pt, Color us, Stage stage>
inline void generatePieceMoves(Board* board, U64 occupied_bb, U64 friendly_bb,
                               U64 mask_bb, moveGen* move_gen) {
    // get piece bb
    U64 piece_bb = board->getPieceBB<us, pt>();

    // go through each piece
    while(piece_bb){
        Square sq_from = bitscanForward(piece_bb);

        // get attacks
        U64 attacks = ZERO;
        if constexpr (pt == BISHOP || pt == QUEEN){
            attacks |= lookUpBishopAttacks(sq_from, occupied_bb);
        }
        if constexpr (pt == ROOK || pt == QUEEN){
            attacks |= lookUpRookAttacks(sq_from, occupied_bb);
        }
        if constexpr (pt == KNIGHT){
            attacks |= KNIGHT_ATTACKS[sq_from];
        }
        // mask off all squares we dont look at anyway
        attacks &= mask_bb;

        if constexpr (stage == GEN_QUIET){
            attacks &= ~occupied_bb;
        }else{
            attacks &= ~friendly_bb & occupied_bb;
        }

        while(attacks){
            Square sq_to = bitscanForward(attacks);

            if constexpr (stage == GEN_QUIET){
                move_gen->addQuiet(genMove(sq_from, sq_to, QUIET, getPiece(us, pt)));
            }else{
                Piece cap_piece = board->getPiece(sq_to);
                move_gen->addNoisy(genMove(sq_from, sq_to, CAPTURE, getPiece(us, pt), cap_piece));
            }

            attacks = lsbReset(attacks);
        }

        piece_bb = lsbReset(piece_bb);
    }
}

template<Color us, Stage stage>
inline void generateKingMoves(Board* board, U64 occupied_bb, U64 friendly_bb, moveGen* move_gen){
    constexpr Piece moving_piece = getPiece(us, KING);
    
    // get king square
    const Square king_sq = bitscanForward(board->getPieceBB(us, KING));
    
    // normal moves
    U64 attacks = KING_ATTACKS[king_sq];
    
    // mask off captures for quiets and evasions
    if constexpr (stage == GEN_QUIET || stage == QS_EVASIONS){
        attacks &= ~occupied_bb;
    }
    if constexpr (stage == GEN_NOISY){
        attacks &= ~friendly_bb & occupied_bb;
    }
    
    // get all the king moves
    while (attacks) {
        Square sq_to = bitscanForward(attacks);
        if constexpr (stage == GEN_QUIET || stage == QS_EVASIONS){
            move_gen->addQuiet(genMove(king_sq, sq_to, QUIET, moving_piece));
        }
        if constexpr (stage == GEN_NOISY){
            Piece cap_piece = board->getPiece(sq_to);
            move_gen->addNoisy(genMove(king_sq, sq_to, CAPTURE, moving_piece, cap_piece));
        }
        attacks = lsbReset(attacks);
    }
    
    if constexpr(stage == GEN_QUIET){
        if constexpr (us == WHITE) {
            if (board->getCastlingRights(WHITE_QUEENSIDE_CASTLING)
                && board->getPiece(A1) == WHITE_ROOK
                && (occupied_bb & CASTLING_WHITE_QUEENSIDE_MASK) == 0) {
                move_gen->addQuiet(genMove(E1, C1, QUEEN_CASTLE, WHITE_KING));
            }
            if (board->getCastlingRights(WHITE_KINGSIDE_CASTLING)
                && board->getPiece(H1) == WHITE_ROOK
                && (occupied_bb & CASTLING_WHITE_KINGSIDE_MASK) == 0) {
                move_gen->addQuiet(genMove(E1, G1, KING_CASTLE, WHITE_KING));
            }
        } else {
            if (board->getCastlingRights(BLACK_QUEENSIDE_CASTLING)
                && board->getPiece(A8) == BLACK_ROOK
                && (occupied_bb & CASTLING_BLACK_QUEENSIDE_MASK) == 0) {
                move_gen->addQuiet(genMove(E8, C8, QUEEN_CASTLE, BLACK_KING));
            }
            if (board->getCastlingRights(BLACK_KINGSIDE_CASTLING)
                && board->getPiece(H8) == BLACK_ROOK
                && (occupied_bb & CASTLING_BLACK_KINGSIDE_MASK) == 0) {
                move_gen->addQuiet(genMove(E8, G8, KING_CASTLE, BLACK_KING));
            }
        }
    }
}

void moveGen::generateNoisy() {
    const U64 friendly = m_board->getTeamOccupiedBB(c);
    const U64 occupied = m_board->getOccupiedBB();

    U64 mask = ~ZERO;
    U64 checkers = m_board->getBoardStatus()->m_checkersBB;
    
    // if more than 1 checker, we can only capture with the king that one piece
    // while escaping the other one
    if(bitCount(checkers) >= 2){
        if(c == WHITE)
            generateKingMoves <WHITE, GEN_NOISY>(m_board, occupied, friendly, this);
        else
            generateKingMoves <BLACK, GEN_NOISY>(m_board, occupied, friendly, this);
        return;
    }else if(bitCount(checkers) == 1){
        mask = checkers;
    }
    
    if(c == WHITE){
        generatePawnMoves<WHITE, GEN_NOISY>(m_board, occupied, mask, this);
        
        generatePieceMoves<KNIGHT, WHITE, GEN_NOISY>(m_board, occupied, friendly, mask, this);
        generatePieceMoves<BISHOP, WHITE, GEN_NOISY>(m_board, occupied, friendly, mask, this);
        generatePieceMoves<ROOK  , WHITE, GEN_NOISY>(m_board, occupied, friendly, mask, this);
        generatePieceMoves<QUEEN , WHITE, GEN_NOISY>(m_board, occupied, friendly, mask, this);
        
        generateKingMoves <WHITE, GEN_NOISY>(m_board, occupied, friendly, this);
    }else{
        generatePawnMoves<BLACK, GEN_NOISY>(m_board, occupied, mask, this);
        
        generatePieceMoves<KNIGHT, BLACK, GEN_NOISY>(m_board, occupied, friendly, mask, this);
        generatePieceMoves<BISHOP, BLACK, GEN_NOISY>(m_board, occupied, friendly, mask, this);
        generatePieceMoves<ROOK  , BLACK, GEN_NOISY>(m_board, occupied, friendly, mask, this);
        generatePieceMoves<QUEEN , BLACK, GEN_NOISY>(m_board, occupied, friendly, mask, this);
        generateKingMoves <BLACK, GEN_NOISY>(m_board, occupied, friendly, this);
    }
}

void moveGen::generateQuiet() {
    const U64 friendly = m_board->getTeamOccupiedBB(c);
    const U64 occupied = m_board->getOccupiedBB();

    if(c == WHITE){
        generatePawnMoves<WHITE, GEN_QUIET>(m_board, occupied, ~ZERO, this);
        
        generatePieceMoves<KNIGHT, WHITE, GEN_QUIET>(m_board, occupied, friendly, ~ZERO, this);
        generatePieceMoves<BISHOP, WHITE, GEN_QUIET>(m_board, occupied, friendly, ~ZERO, this);
        generatePieceMoves<ROOK  , WHITE, GEN_QUIET>(m_board, occupied, friendly, ~ZERO, this);
        generatePieceMoves<QUEEN , WHITE, GEN_QUIET>(m_board, occupied, friendly, ~ZERO, this);
        
        generateKingMoves <WHITE, GEN_QUIET>(m_board, occupied, friendly, this);
    }else{
        generatePawnMoves<BLACK, GEN_QUIET>(m_board, occupied, ~ZERO, this);
        
        generatePieceMoves<KNIGHT, BLACK, GEN_QUIET>(m_board, occupied, friendly, ~ZERO, this);
        generatePieceMoves<BISHOP, BLACK, GEN_QUIET>(m_board, occupied, friendly, ~ZERO, this);
        generatePieceMoves<ROOK  , BLACK, GEN_QUIET>(m_board, occupied, friendly, ~ZERO, this);
        generatePieceMoves<QUEEN , BLACK, GEN_QUIET>(m_board, occupied, friendly, ~ZERO, this);
        
        generateKingMoves <BLACK, GEN_QUIET>(m_board, occupied, friendly, this);
    }
}

void moveGen::generateEvasions() {
    const U64 friendly = m_board->getTeamOccupiedBB(c);
    const U64 occupied = m_board->getOccupiedBB();
    if(c == WHITE){
        generateKingMoves <WHITE, QS_EVASIONS>(m_board, occupied, friendly, this);
    }else{
        generateKingMoves <BLACK, QS_EVASIONS>(m_board, occupied, friendly, this);
    }
}

void moveGen::updateHistory(int weight) {
    weight          = std::min(weight * weight + 5 * weight, 384);
    Move bestMove   = searched[searched_index - 1];

#define UPDATE_HISTORY_UP(value)   ((value) += + weight - weight * (value) / MAX_HIST)
#define UPDATE_HISTORY_DOWN(value) ((value) += - weight - weight * (value) / MAX_HIST)
    
    if (isCapture(bestMove)) {
        UPDATE_HISTORY_UP(CAPTURE_HISTORY(m_sd,c,bestMove));
        weight = std::min(weight, 128);
        for (int i = 0; i < searched_index - 1; i++) {
            Move m = searched[i];
            if (isCapture(m)) {
                UPDATE_HISTORY_DOWN(CAPTURE_HISTORY(m_sd, c, m));
            }
        }
    } else {
        UPDATE_HISTORY_UP(THREAT_HISTORY(m_sd,c,m_threatSquare,bestMove));
        UPDATE_HISTORY_UP(CMH(m_sd,m_previous,c,bestMove));
        UPDATE_HISTORY_UP(FMH(m_sd,m_followup,c,bestMove));
        
        weight = std::min(weight, 128);
        for (int i = 0; i < searched_index - 1; i++) {
            Move m = searched[i];
            if (isCapture(m)) {
                UPDATE_HISTORY_DOWN(CAPTURE_HISTORY(m_sd, c, m));
            } else {
                UPDATE_HISTORY_DOWN(THREAT_HISTORY(m_sd, c, m_threatSquare, m));
                UPDATE_HISTORY_DOWN(CMH(m_sd, m_previous, c, m));
                UPDATE_HISTORY_DOWN(FMH(m_sd, m_followup, c, m));
            }
        }
    }
#undef UPDATE_HISTORY_UP
#undef UPDATE_HISTORY_DOWN
}

void moveGen::skip() {
    m_skip = true;
}

bool moveGen::shouldSkip() const {
    return m_skip;
}