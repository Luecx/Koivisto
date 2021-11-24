
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
    
static const int piece_values[6] = {
    90, 463, 474, 577, 1359, 0,
};

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
    m_killer1       = m_sd->killer[c][m_ply][0];
    m_killer2       = m_sd->killer[c][m_ply][1];
    m_threatSquare  = threatSquare;
    m_checkerSq     = checkerSq;
}

Move moveGen::next() {
    switch (stage) {
        case GET_HASHMOVE:
            stage++;
            if (m_board->isPseudoLegal(m_hashMove))
                return m_hashMove;

        case GEN_NOISY:
            generateNoisy();
            stage++;

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

        case KILLER1:
            stage++;
            if (!sameMove(m_killer1, m_hashMove) && m_board->isPseudoLegal(m_killer1))
                return m_killer1;

        case KILLER2:
            stage++;
            if (!sameMove(m_killer2, m_hashMove) && m_board->isPseudoLegal(m_killer2))
                return m_killer2;

        case GEN_QUIET:
            if (shouldSkip()) {
                stage = GET_BAD_NOISY;
                return next();
            }
            generateQuiet();
            stage++;

        case GET_QUIET:
            if (quiet_index < quietSize)
                return nextQuiet();
            stage++;

        case GET_BAD_NOISY:
            if (noisy_index < noisySize) 
                return nextNoisy();
            stage++;

        case END:
            return 0;
        
        case QS_EVASIONS:
            if (quiet_index < quietSize)
                return nextQuiet();
            stage = END;
            return 0;
    }
}

void moveGen::addNoisy(Move m) {
    if (sameMove(m_hashMove, m))
        return;
    int score   = (isPromotion(m) && (getPromotionPieceType(m) != QUEEN)) ? 
              - 1 : m_board->staticExchangeEvaluation(m);
    int mvvLVA  = piece_values[(getCapturedPieceType(m))];
    if (score >= 0) {
        score = 100000 + mvvLVA + m_sd->getHistories(m, c, m_previous, m_followup, m_threatSquare);
        goodNoisyCount++;
    } else {
        score = 10000 + m_sd->getHistories(m, c, m_previous, m_followup, m_threatSquare);
    }
    noisy[noisySize] = m;
    noisyScores[noisySize++] = score;
}

void moveGen::addQuiet(Move m) {
    if (sameMove(m_hashMove, m) | sameMove(m_killer1, m) | sameMove(m_killer2, m))
        return;
    quiets[quietSize] = m;
    quietScores[quietSize++] = 20000 + m_sd->getHistories(m, c, m_previous, m_followup, m_threatSquare);
}

Move moveGen::nextNoisy() {
    if (m_skip)
        return noisy[noisy_index++];
    int bestNoisy = noisy_index;
    for (int i = noisy_index + 1; i < noisySize; i++) {
        if (noisyScores[i] > noisyScores[bestNoisy])
            bestNoisy = i;
    }
    Move m = noisy[bestNoisy];
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

void moveGen::generateNoisy() {
    
    const U64 relative_rank_8_bb = c == WHITE ? RANK_8_BB : RANK_1_BB;
    const U64 relative_rank_7_bb = c == WHITE ? RANK_7_BB : RANK_2_BB;
    const U64 relative_rank_4_bb = c == WHITE ? RANK_4_BB : RANK_5_BB;
    
    const Direction forward      = c == WHITE ? NORTH:SOUTH;
    const Direction right        = c == WHITE ? NORTH_EAST:SOUTH_EAST;
    const Direction left         = c == WHITE ? NORTH_WEST:SOUTH_WEST;
    
    const U64 opponents          = m_board->getTeamOccupiedBB(!c);
    const U64 friendly           = m_board->getTeamOccupiedBB(c);

    const U64 pawns              = m_board->getPieceBB(c, PAWN);
    const U64 occupied           = m_board->getOccupiedBB();
    
    const U64 pawnsLeft   =  c == WHITE ? shiftNorthWest(pawns) : shiftSouthWest(pawns);
    const U64 pawnsRight  =  c == WHITE ? shiftNorthEast(pawns) : shiftSouthEast(pawns);
    const U64 pawnsCenter = (c == WHITE ? shiftNorth (pawns) : shiftSouth(pawns)) & ~occupied;
    
    Piece movingPiece = c * 8 + PAWN;
    
    U64 nonPromoAttacks = opponents & ~relative_rank_8_bb;
    Square target;
    
    // Pawn
    U64 attacks = pawnsLeft & nonPromoAttacks;
    while (attacks) {
        target = bitscanForward(attacks);
        addNoisy(genMove(target - left, target, CAPTURE, movingPiece, m_board->getPiece(target)));
        attacks = lsbReset(attacks);
    }
    
    attacks = pawnsRight & nonPromoAttacks;
    while (attacks) {
        target = bitscanForward(attacks);
        addNoisy(genMove(target - right, target, CAPTURE, movingPiece, m_board->getPiece(target)));
        attacks = lsbReset(attacks);
    }
    
    if (pawnsLeft & m_board->getBoardStatus()->enPassantTarget) {
        target = m_board->getEnPassantSquare();
        addNoisy(genMove(target - left, target, EN_PASSANT, movingPiece));
    }
    
    if (pawnsRight & m_board->getBoardStatus()->enPassantTarget) {
        target = m_board->getEnPassantSquare();
        addNoisy(genMove(target - right, target, EN_PASSANT, movingPiece));
    }
 
    if (pawns & relative_rank_7_bb) {
        
        attacks = pawnsCenter & relative_rank_8_bb;
        while (attacks) {
            target = bitscanForward(attacks);
            addNoisy(genMove(target - forward, target, QUEEN_PROMOTION, movingPiece));
            attacks = lsbReset(attacks);
        }
        
        attacks = pawnsLeft & relative_rank_8_bb & opponents;
        while (attacks) {
            target = bitscanForward(attacks);
            addNoisy(genMove(target - left, target, QUEEN_PROMOTION_CAPTURE , movingPiece, m_board->getPiece(target)));
            attacks = lsbReset(attacks);
        }

        attacks = pawnsRight & relative_rank_8_bb & opponents;
        while (attacks) {
            target = bitscanForward(attacks);
            addNoisy(genMove(target - right, target, QUEEN_PROMOTION_CAPTURE , movingPiece, m_board->getPiece(target)));
            attacks = lsbReset(attacks);
        }
    }


    // Pieces
    for(Piece p = KNIGHT; p <= QUEEN; p++){
        U64 pieceOcc    = m_board->getPieceBB(c, p);
        U64 movingPiece = p + 8 * c;
        while(pieceOcc){
            Square square = bitscanForward(pieceOcc);
            U64 attacks = ZERO;
            switch (p) {
                case KNIGHT:
                    attacks = KNIGHT_ATTACKS[square];
                    break;
                case BISHOP:
                    attacks =
                        lookUpBishopAttack  (square, occupied);
                    break;
                case ROOK:
                    attacks =
                        lookUpRookAttack    (square,occupied);
                    break;
                case QUEEN:
                    attacks =
                        lookUpBishopAttack  (square, occupied) |
                        lookUpRookAttack    (square, occupied);
                    break;
                
            }
            attacks &= ~friendly & opponents;

            while(attacks){
                Square target = bitscanForward(attacks);
                addNoisy(genMove(square, target, CAPTURE, movingPiece, m_board->getPiece(target)));
                
                attacks = lsbReset(attacks);
            }
            pieceOcc = lsbReset(pieceOcc);
        }
    }
    

    // King
    movingPiece = KING + c * 8;
    
    U64 kings      = m_board->getPieceBB(c, KING);
    
    while (kings) {
        Square s    = bitscanForward(kings);
        U64 attacks = KING_ATTACKS[s] & ~friendly & opponents;
        while (attacks) {
            Square target = bitscanForward(attacks);
            addNoisy(genMove(s, target, CAPTURE, movingPiece, m_board->getPiece(target)));
            
            attacks = lsbReset(attacks);
        }
        kings = lsbReset(kings);
    }
}

void moveGen::generateQuiet() {
    
    const U64 relative_rank_8_bb = c == WHITE ? RANK_8_BB : RANK_1_BB;
    const U64 relative_rank_7_bb = c == WHITE ? RANK_7_BB : RANK_2_BB;
    const U64 relative_rank_4_bb = c == WHITE ? RANK_4_BB : RANK_5_BB;
    
    const Direction forward      = c == WHITE ? NORTH:SOUTH;
    const Direction right        = c == WHITE ? NORTH_EAST:SOUTH_EAST;
    const Direction left         = c == WHITE ? NORTH_WEST:SOUTH_WEST;
    
    const U64 opponents          = m_board->getTeamOccupiedBB(!c);
    const U64 friendly           = m_board->getTeamOccupiedBB(c);
    
    const U64 pawns              = m_board->getPieceBB(c, PAWN);
    const U64 occupied           = m_board->getOccupiedBB();
    
    const U64 pawnsCenter = (c == WHITE ? shiftNorth (pawns) : shiftSouth(pawns)) & ~occupied;
    
    Piece movingPiece = c * 8 + PAWN;

    Square target;

    // Pawn
    U64 pawnPushes = pawnsCenter & ~relative_rank_8_bb;
    U64 attacks = pawnPushes;
    while (attacks) {
        target = bitscanForward(attacks);
        addQuiet(genMove(target - forward, target, QUIET, movingPiece));
        attacks = lsbReset(attacks);
    }
        
    attacks = (c == WHITE ? shiftNorth(pawnPushes) : shiftSouth(pawnPushes)) & relative_rank_4_bb & ~occupied;
    while (attacks) {
        target = bitscanForward(attacks);
        addQuiet(genMove(target - forward * 2, target, DOUBLED_PAWN_PUSH, movingPiece));
        attacks = lsbReset(attacks);
    }


    // Piece
    for(Piece p = KNIGHT; p <= QUEEN; p++){
        U64 pieceOcc    = m_board->getPieceBB(c, p);
        U64 movingPiece = p + 8 * c;
        while(pieceOcc){
            Square square = bitscanForward(pieceOcc);
            U64 attacks   = ZERO;
            switch (p) {
                case KNIGHT:
                    attacks = KNIGHT_ATTACKS[square];
                    break;
                case BISHOP:
                    attacks =
                        lookUpBishopAttack  (square, occupied);
                    break;
                case ROOK:
                    attacks =
                        lookUpRookAttack    (square,occupied);
                    break;
                case QUEEN:
                    attacks =
                        lookUpBishopAttack  (square, occupied) |
                        lookUpRookAttack    (square, occupied);
                    break;
                
            }
            attacks &= ~friendly;
            attacks &= ~opponents;

            while(attacks){
                Square target = bitscanForward(attacks);
                addQuiet(genMove(square, target, QUIET, movingPiece));
                
                attacks = lsbReset(attacks);
            }
            pieceOcc = lsbReset(pieceOcc);
        }
    }
    
    
    // King
    movingPiece = KING + c * 8;
    
    U64 kings      = m_board->getPieceBB(c, KING);
    
    while (kings) {
        Square s    = bitscanForward(kings);
        U64 attacks = KING_ATTACKS[s] & ~friendly & ~opponents;
        while (attacks) {
            Square target = bitscanForward(attacks);
            addQuiet(genMove(s, target, QUIET, movingPiece));
            
            attacks = lsbReset(attacks);
        }
    
    
        if (c == WHITE) {
            if (m_board->getCastlingRights(WHITE_QUEENSIDE_CASTLING) && m_board->getPiece(A1) == WHITE_ROOK
                && (occupied & CASTLING_WHITE_QUEENSIDE_MASK) == 0) {
                addQuiet(genMove(E1, C1, QUEEN_CASTLE, WHITE_KING));
            }
            if (m_board->getCastlingRights(WHITE_KINGSIDE_CASTLING) && m_board->getPiece(H1) == WHITE_ROOK
                && (occupied & CASTLING_WHITE_KINGSIDE_MASK) == 0) {
                addQuiet(genMove(E1, G1, KING_CASTLE, WHITE_KING));
            }
            
        } else {
        
            if (m_board->getCastlingRights(BLACK_QUEENSIDE_CASTLING) && m_board->getPiece(A8) == BLACK_ROOK
                && (occupied & CASTLING_BLACK_QUEENSIDE_MASK) == 0) {
                addQuiet(genMove(E8, C8, QUEEN_CASTLE, BLACK_KING));
            }
            if (m_board->getCastlingRights(BLACK_KINGSIDE_CASTLING) && m_board->getPiece(H8) == BLACK_ROOK
                && (occupied & CASTLING_BLACK_KINGSIDE_MASK) == 0) {
                addQuiet(genMove(E8, G8, KING_CASTLE, BLACK_KING));
            }
        }
        kings = lsbReset(kings);
    }
}

void moveGen::generateEvasions() {
    
    const U64 occupied  = m_board->getOccupiedBB();
    Square target;
    Piece movingPiece   = KING + c * 8;
    U64 kings           = m_board->getPieceBB(c, KING);
    
    while (kings) {
        Square s    = bitscanForward(kings);
        U64 attacks = KING_ATTACKS[s] & ~occupied;
        while (attacks) {
            Square target = bitscanForward(attacks);
            addQuiet(genMove(s, target, QUIET, movingPiece));
            attacks = lsbReset(attacks);
        }
        kings = lsbReset(kings);
    }
}

void moveGen::updateHistory(int weight) {

    weight          = std::min(weight * weight + 5 * weight, 256);
    Move bestMove   = searched[searched_index - 1];    

    if (isCapture(bestMove)) {
        m_sd->captureHistory[c][getSqToSqFromCombination(bestMove)] +=
                    + weight
                    - weight * m_sd->captureHistory[c][getSqToSqFromCombination(bestMove)]
                    / MAX_HIST;

        for (int i = 0; i < searched_index - 1; i++) {
            Move m = searched[i];
            if (isCapture(m)) {
                    m_sd->captureHistory[c][getSqToSqFromCombination(m)] +=
                                - weight
                                - weight * m_sd->captureHistory[c][getSqToSqFromCombination(m)]
                                / MAX_HIST;
            }
        } 
    } else {
        m_sd->th[c][m_threatSquare][getSqToSqFromCombination(bestMove)] +=
                    + weight
                    - weight * m_sd->th[c][m_threatSquare][getSqToSqFromCombination(bestMove)]
                    / MAX_HIST;
        m_sd->cmh[getPieceTypeSqToCombination(m_previous)][c][getPieceTypeSqToCombination(bestMove)] +=
                    + weight
                    - weight * m_sd->cmh[getPieceTypeSqToCombination(m_previous)][c][getPieceTypeSqToCombination(bestMove)]
                    / MAX_HIST;
        m_sd->fmh[getPieceTypeSqToCombination(m_followup)][c][getPieceTypeSqToCombination(bestMove)] +=
                    + weight
                    - weight * m_sd->fmh[getPieceTypeSqToCombination(m_followup)][c][getPieceTypeSqToCombination(bestMove)]
                    / MAX_HIST;
        for (int i = 0; i < searched_index - 1; i++) {
            Move m = searched[i];
            if (isCapture(m)) {
                m_sd->captureHistory[c][getSqToSqFromCombination(m)] +=
                            - weight
                            - weight * m_sd->captureHistory[c][getSqToSqFromCombination(m)]
                            / MAX_HIST;
            } else {
                m_sd->th[c][m_threatSquare][getSqToSqFromCombination(m)] +=
                            - weight
                            - weight * m_sd->th[c][m_threatSquare][getSqToSqFromCombination(m)]
                            / MAX_HIST;
                m_sd->cmh[getPieceTypeSqToCombination(m_previous)][c][getPieceTypeSqToCombination(m)] +=
                            - weight
                            - weight * m_sd->cmh[getPieceTypeSqToCombination(m_previous)][c][getPieceTypeSqToCombination(m)]
                            / MAX_HIST;
                m_sd->fmh[getPieceTypeSqToCombination(m_followup)][c][getPieceTypeSqToCombination(m)] +=
                            - weight
                            - weight * m_sd->fmh[getPieceTypeSqToCombination(m_followup)][c][getPieceTypeSqToCombination(m)]
                            / MAX_HIST;
            }
        } 
    }
}

void moveGen::skip() {
    m_skip = true;
}

bool moveGen::shouldSkip() {
    return m_skip;
}