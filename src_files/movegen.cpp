
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
#include "movegen.h"
#include "UCIAssert.h"


// check if all moves should be generated or only quiet moves
enum MoveGenConfig{
    GENERATE_ALL,
    GENERATE_NON_QUIET,
};

template<Color c, MoveTypes t, MoveGenConfig m>
inline void scoreMove(Board* board, MoveList* mv, Move hashMove, SearchData* sd, Depth ply){
    
    Move move = mv->getMove(mv->getSize()-1);
    int  idx  = mv->getSize()-1;
    
    static const int piece_values[6] = {
        90,463, 474,577,1359,0,
    };
    
    constexpr bool  isCapture   = t & CAPTURE_MASK;
    constexpr bool  isPromotion = t & PROMOTION_MASK;
    
    // score hash move
    if(sameMove(move, hashMove)){
        mv->scoreMove(idx, 1e6);
        return;
    }
    
    
    if constexpr (m == GENERATE_ALL){
        // scoring for moves when all moves are generated
        
        if constexpr (isCapture){
            Score     SEE    = board->staticExchangeEvaluation(move);
            MoveScore mvvLVA = piece_values[(getCapturedPieceType(move))];
            if (SEE >= 0) {
                mv->scoreMove(idx, 100000 + mvvLVA + sd->getHistories(move, board->getActivePlayer(), board->getPreviousMove()));
            } else {
                mv->scoreMove(idx, 10000 + sd->getHistories(move, board->getActivePlayer(), board->getPreviousMove()));
            }
        } else if constexpr (isPromotion){
            MoveScore mvvLVA = (getCapturedPieceType(move)) - (getMovingPieceType(move));
            mv->scoreMove(idx, 40000 + mvvLVA + getPromotionPiece(move));
        } else if (sd->isKiller(move, ply, c)){
            mv->scoreMove(idx, 30000 + sd->isKiller(move, ply, c));
        } else{
            mv->scoreMove(idx, 20000 + sd->getHistories(move, board->getActivePlayer(), board->getPreviousMove()));
        }
        
    }else if constexpr (m == GENERATE_NON_QUIET){
        // scoring when only non quiet moves are generated
        MoveScore mvvLVA = 100 * (getCapturedPieceType(move)) - 10 * (getMovingPieceType(move))
                           + (getSquareTo(board->getPreviousMove()) == getSquareTo(move));
        mv->scoreMove(idx, 240 + mvvLVA);
        
    }
}


template<Color c, MoveGenConfig m, bool score>
void generatePawnMoves(
    Board* b,
    MoveList* mv,
    Move hashMove=0,
    SearchData* sd= nullptr,
    Depth ply=0){
    UCI_ASSERT(b);
    UCI_ASSERT(mv);
    
    constexpr Color us   =  c;
    constexpr Color them = !c;
    
    constexpr U64 relative_rank_8_bb = c == WHITE ? RANK_8_BB : RANK_1_BB;
    constexpr U64 relative_rank_7_bb = c == WHITE ? RANK_7_BB : RANK_2_BB;
    constexpr U64 relative_rank_4_bb = c == WHITE ? RANK_4_BB : RANK_5_BB;
    
    constexpr Direction forward      = c == WHITE ? NORTH:SOUTH;
    constexpr Direction right        = c == WHITE ? NORTH_EAST:SOUTH_EAST;
    constexpr Direction left         = c == WHITE ? NORTH_WEST:SOUTH_WEST;
    
    const U64 opponents         =  b->getTeamOccupiedBB<them>();
    
    const U64 pawns             = b->getPieceBB<us>(PAWN);
    const U64 occupied          = b->getOccupiedBB();
    
    const U64 pawnsLeft   =  c == WHITE ? shiftNorthWest(pawns) : shiftSouthWest(pawns);
    const U64 pawnsRight  =  c == WHITE ? shiftNorthEast(pawns) : shiftSouthEast(pawns);
    const U64 pawnsCenter = (c == WHITE ? shiftNorth (pawns) : shiftSouth(pawns)) & ~occupied;
    
    const Piece movingPiece = us * 8 + PAWN;
    
    U64 nonPromoAttacks = opponents & ~relative_rank_8_bb;
    Square target;
    
    U64 attacks = pawnsLeft & nonPromoAttacks;
    while (attacks) {
        target = bitscanForward(attacks);
        mv->add(genMove(target - left, target, CAPTURE, movingPiece, b->getPiece(target)));
        if constexpr (score) scoreMove<c, CAPTURE, m>(b, mv, hashMove, sd, ply);
        attacks = lsbReset(attacks);
    }
    
    attacks = pawnsRight & nonPromoAttacks;
    while (attacks) {
        target = bitscanForward(attacks);
        mv->add(genMove(target - right, target, CAPTURE, movingPiece, b->getPiece(target)));
        if constexpr (score) scoreMove<c, CAPTURE, m>(b, mv, hashMove, sd, ply);
        attacks = lsbReset(attacks);
    }
    
    if constexpr (m != GENERATE_NON_QUIET){
        U64 pawnPushes = pawnsCenter & ~relative_rank_8_bb;
        attacks = pawnPushes;
        while (attacks) {
            target = bitscanForward(attacks);
            mv->add(genMove(target - forward, target, QUIET, movingPiece));
            if constexpr (score) scoreMove<c, QUIET, m>(b, mv, hashMove, sd, ply);
            attacks = lsbReset(attacks);
        }
        
        attacks = (c == WHITE ? shiftNorth(pawnPushes) : shiftSouth(pawnPushes)) & relative_rank_4_bb & ~occupied;
        while (attacks) {
            target = bitscanForward(attacks);
            mv->add(genMove(target - forward * 2, target, DOUBLED_PAWN_PUSH, movingPiece));
            if constexpr (score) scoreMove<c, DOUBLED_PAWN_PUSH, m>(b, mv, hashMove, sd, ply);
            attacks = lsbReset(attacks);
        }
    }
    
    if (pawnsLeft & b->getBoardStatus()->enPassantTarget) {
        target = b->getEnPassantSquare();
        mv->add(genMove(target - left, target, EN_PASSANT, movingPiece));
        if constexpr (score) scoreMove<c, EN_PASSANT, m>(b, mv, hashMove, sd, ply);
    }
    
    if (pawnsRight & b->getBoardStatus()->enPassantTarget) {
        target = b->getEnPassantSquare();
        mv->add(genMove(target - right, target, EN_PASSANT, movingPiece));
        if constexpr (score) scoreMove<c, EN_PASSANT, m>(b, mv, hashMove, sd, ply);
    }
 
    if (pawns & relative_rank_7_bb) {
        
        attacks = pawnsCenter & relative_rank_8_bb;
        while (attacks) {
            target = bitscanForward(attacks);
            mv->add(genMove(target - forward, target, QUEEN_PROMOTION, movingPiece));
            if constexpr (score) scoreMove<c, QUEEN_PROMOTION,  m>(b, mv, hashMove, sd, ply);
            if constexpr (m != GENERATE_NON_QUIET) {
                mv->add(genMove(target - forward, target, ROOK_PROMOTION, movingPiece));
                if constexpr (score) scoreMove<c, ROOK_PROMOTION,   m>(b, mv, hashMove, sd, ply);
                mv->add(genMove(target - forward, target, BISHOP_PROMOTION, movingPiece));
                if constexpr (score) scoreMove<c, BISHOP_PROMOTION, m>(b, mv, hashMove, sd, ply);
                mv->add(genMove(target - forward, target, KNIGHT_PROMOTION, movingPiece));
                if constexpr (score) scoreMove<c, KNIGHT_PROMOTION, m>(b, mv, hashMove, sd, ply);
            }
            attacks = lsbReset(attacks);
        }
        
        attacks = pawnsLeft & relative_rank_8_bb & opponents;
        while (attacks) {
            target = bitscanForward(attacks);
            mv->add(genMove(target - left, target, QUEEN_PROMOTION_CAPTURE , movingPiece, b->getPiece(target)));
            if constexpr (score) scoreMove<c, QUEEN_PROMOTION_CAPTURE,  m>(b, mv, hashMove, sd, ply);
            if constexpr (m != GENERATE_NON_QUIET) {
                mv->add(genMove(target - left, target, ROOK_PROMOTION_CAPTURE  , movingPiece, b->getPiece(target)));
                if constexpr (score) scoreMove<c, ROOK_PROMOTION_CAPTURE,   m>(b, mv, hashMove, sd, ply);
                mv->add(genMove(target - left, target, BISHOP_PROMOTION_CAPTURE, movingPiece, b->getPiece(target)));
                if constexpr (score) scoreMove<c, BISHOP_PROMOTION_CAPTURE, m>(b, mv, hashMove, sd, ply);
                mv->add(genMove(target - left, target, KNIGHT_PROMOTION_CAPTURE, movingPiece, b->getPiece(target)));
                if constexpr (score) scoreMove<c, KNIGHT_PROMOTION_CAPTURE, m>(b, mv, hashMove, sd, ply);
            }
            attacks = lsbReset(attacks);
        }

        attacks = pawnsRight & relative_rank_8_bb & opponents;
        while (attacks) {
            target = bitscanForward(attacks);
            mv->add(genMove(target - right, target, QUEEN_PROMOTION_CAPTURE , movingPiece, b->getPiece(target)));
            if constexpr (score) scoreMove<c, QUEEN_PROMOTION_CAPTURE,  m>(b, mv, hashMove, sd, ply);
            if constexpr (m != GENERATE_NON_QUIET) {
                mv->add(genMove(target - right, target, ROOK_PROMOTION_CAPTURE  , movingPiece, b->getPiece(target)));
                if constexpr (score) scoreMove<c, ROOK_PROMOTION_CAPTURE,   m>(b, mv, hashMove, sd, ply);
                mv->add(genMove(target - right, target, BISHOP_PROMOTION_CAPTURE, movingPiece, b->getPiece(target)));
                if constexpr (score) scoreMove<c, BISHOP_PROMOTION_CAPTURE, m>(b, mv, hashMove, sd, ply);
                mv->add(genMove(target - right, target, KNIGHT_PROMOTION_CAPTURE, movingPiece, b->getPiece(target)));
                if constexpr (score) scoreMove<c, KNIGHT_PROMOTION_CAPTURE, m>(b, mv, hashMove, sd, ply);
            }
            attacks = lsbReset(attacks);
        }
    }


    
    
}

template<Color c, MoveGenConfig m, bool score>
void generatePieceMoves(
    Board* b,
    MoveList* mv,
    Move hashMove=0,
    SearchData* sd= nullptr,
    Depth ply=0){
    UCI_ASSERT(b);
    UCI_ASSERT(mv);
    
    constexpr Color us   =  c;
    
    const U64 occupied   = b->getOccupiedBB();
    const U64 friendly   = b->template getTeamOccupiedBB<us  >();
    
    for(Piece p = KNIGHT; p <= QUEEN; p++){
        U64 pieceOcc    = b->getPieceBB<c>(p);
        U64 movingPiece = p + 8 * us;
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
            attacks &= ~friendly;

//            if constexpr (m != GENERATE_NON_QUIET){
//                U64 quiets   = attacks & ~opponents;
//                while(quiets){
//                    Square target = bitscanForward(quiets);
//                    mv->add(genMove(square, target, QUIET, movingPiece));
//                    if constexpr (score) scoreMove<c, QUIET, m>(b, mv, hashMove, sd, ply);
//                    quiets = lsbReset(quiets);
//                }
//            }
           
//            attacks &= opponents;
            while(attacks){
                Square target = bitscanForward(attacks);
                if(b->getPiece(target) != -1){
                    mv->add(genMove(square, target, CAPTURE, movingPiece, b->getPiece(target)));
                    if constexpr (score) scoreMove<c, CAPTURE, m>(b, mv, hashMove, sd, ply);
                }else if constexpr (m != GENERATE_NON_QUIET){
                    mv->add(genMove(square, target, QUIET, movingPiece));
                    if constexpr (score) scoreMove<c, QUIET, m>(b, mv, hashMove, sd, ply);
                }
                attacks = lsbReset(attacks);
               
            }
            
            pieceOcc = lsbReset(pieceOcc);
        }
        
        
    }
}

template<Color c, MoveGenConfig m, bool score>
void generateKingMoves(
    Board* b,
    MoveList* mv,
    Move hashMove=0,
    SearchData* sd= nullptr,
    Depth ply=0){
    UCI_ASSERT(b);
    UCI_ASSERT(mv);
    
    constexpr Color us   =  c;
    
    constexpr Piece movingPiece = KING + us * 8;
    
    const U64 occupied   = b->getOccupiedBB();
    const U64 friendly   = b->getTeamOccupiedBB<us  >();
    
    U64 kings      = b->getPieceBB<us>(KING);
    
    while (kings) {
        Square s       = bitscanForward(kings);
        U64 attacks = KING_ATTACKS[s] & ~friendly;
        while (attacks) {
            Square target = bitscanForward(attacks);
            
            if (b->getPiece(target) >= 0) {
                mv->add(genMove(s, target, CAPTURE, movingPiece, b->getPiece(target)));
                if constexpr (score) scoreMove<c, CAPTURE, m>(b, mv, hashMove, sd, ply);
            } else {
                if constexpr (m != GENERATE_NON_QUIET) {
                    mv->add(genMove(s, target, QUIET, movingPiece));
                    if constexpr (score) scoreMove<c, QUIET, m>(b, mv, hashMove, sd, ply);
                }
            }
            
            attacks = lsbReset(attacks);
        }
    
    
        if constexpr (m != GENERATE_NON_QUIET) {
            if constexpr (c == WHITE) {
                if (b->getCastlingRights(WHITE_QUEENSIDE_CASTLING) && b->getPiece(A1) == WHITE_ROOK
                    && (occupied & CASTLING_WHITE_QUEENSIDE_MASK) == 0) {
                    mv->add(genMove(E1, C1, QUEEN_CASTLE, WHITE_KING));
                    if constexpr (score) scoreMove<c, QUEEN_CASTLE, m>(b, mv, hashMove, sd, ply);
                }
                if (b->getCastlingRights(WHITE_KINGSIDE_CASTLING) && b->getPiece(H1) == WHITE_ROOK
                    && (occupied & CASTLING_WHITE_KINGSIDE_MASK) == 0) {
                    mv->add(genMove(E1, G1, KING_CASTLE, WHITE_KING));
                    if constexpr (score) scoreMove<c, KING_CASTLE, m>(b, mv, hashMove, sd, ply);
                }
                
            } else {
                
                if (b->getCastlingRights(BLACK_QUEENSIDE_CASTLING) && b->getPiece(A8) == BLACK_ROOK
                    && (occupied & CASTLING_BLACK_QUEENSIDE_MASK) == 0) {
                    mv->add(genMove(E8, C8, QUEEN_CASTLE, BLACK_KING));
                    if constexpr (score) scoreMove<c, QUEEN_CASTLE, m>(b, mv, hashMove, sd, ply);
                }
                if (b->getCastlingRights(BLACK_KINGSIDE_CASTLING) && b->getPiece(H8) == BLACK_ROOK
                    && (occupied & CASTLING_BLACK_KINGSIDE_MASK) == 0) {
                    mv->add(genMove(E8, G8, KING_CASTLE, BLACK_KING));
                    if constexpr (score) scoreMove<c, KING_CASTLE, m>(b, mv, hashMove, sd, ply);
                }
            }
        }
        kings = lsbReset(kings);
    }
}

template<MoveGenConfig config, bool score> void generate(
    Board* b,
    MoveList* mv,
    Move hashMove = 0,
    SearchData* sd = nullptr,
    Depth ply = 0) {
    UCI_ASSERT(b);
    UCI_ASSERT(mv);
    
    mv->clear();
    
    if(b->getActivePlayer() == WHITE){
        generatePawnMoves <WHITE, config, score>(b, mv, hashMove, sd, ply);
        generatePieceMoves<WHITE, config, score>(b, mv, hashMove, sd, ply);
        generateKingMoves <WHITE, config, score>(b, mv, hashMove, sd, ply);
    }else{
        generatePawnMoves <BLACK, config, score>(b, mv, hashMove, sd, ply);
        generatePieceMoves<BLACK, config, score>(b, mv, hashMove, sd, ply);
        generateKingMoves <BLACK, config, score>(b, mv, hashMove, sd, ply);
    }
    
}

void generateMoves(Board* b, MoveList* mv, Move hashMove, SearchData* sd, Depth ply) {
    UCI_ASSERT(b);
    UCI_ASSERT(mv);
    UCI_ASSERT(sd);
    generate<GENERATE_ALL, true>(b, mv, hashMove, sd, ply);
}
void generateNonQuietMoves(Board* b, MoveList* mv, Move hashMove, SearchData* sd, Depth ply) {
    UCI_ASSERT(b);
    UCI_ASSERT(mv);
    generate<GENERATE_NON_QUIET, true>(b, mv, hashMove, sd, ply);
}
void generatePerftMoves(Board* b, MoveList* mv) {
    UCI_ASSERT(b);
    UCI_ASSERT(mv);
    generate<GENERATE_ALL, false>(b, mv);
}
