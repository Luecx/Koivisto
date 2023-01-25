
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


#include "attacks.h"
#include "transpositiontable.h"
#include "board.h"
#include "bitboard.h"
#include "uciassert.h"

using namespace bb;
using namespace move;

constexpr float phaseValues[bb::N_PIECE_TYPES] {
    0.552938, 1.55294, 1.50862, 2.64379, 4.0
};

/**
 * The default constructor uses a fen-representation of the board. if nothing is specified, the starting position
 * will be used. This might crash if the given fen is illegal in its structure. e.g. not all rows/columns specified.
 * @param fen
 */
Board::Board(const std::string& fen) {
    // first we set all piece occupancies to zero.
    for (int i = 0; i < N_PIECES; i++) {
        m_piecesBB[i] = 0;
    }
    
    // also unset the occupancy for both teams
    m_teamOccupiedBB[WHITE] = 0;
    m_teamOccupiedBB[BLACK] = 0;
    
    // set the total occupancy to zero
    m_occupiedBB = 0;
    
    // assume white is the active player
    m_activePlayer = WHITE;
    
    // we use default values of -1 for the piece table for each square if no piece is on the square
    for (int i = 0; i < N_SQUARES; i++) {
        m_pieceBoard[i] = -1;
    }
    
    // we need to push a default board status.
    BoardStatus boardStatus {0, 0, 0, 0, ONE, ONE, 0};
    this->m_boardStatusHistory.reserve(512);
    this->m_boardStatusHistory.push_back(boardStatus);
    
    // using some string utilties defined in Util.h, we split the fen into parts.
    std::vector<std::string> split = splitString(fen);
    
    // first we parse the pieces on the board.
    File x {0};
    Rank y {7};
    for (char c : split[0]) {
        // we continue to the next rank and reset the file
        if (c == '/') {
            x = 0;
            y--;
            continue;
        }
        
        // if we need to skip a few squares, we do this here
        if (c < '9') {
            x += (c - '0');
            continue;
        } else {
            int offset = (c >= 'a') ? 8 : 0;
            
            Square sq = squareIndex(y, x);
            
            switch (toupper(c)) {
                case 'P': setPiece<false>(sq, 0 + offset); break;
                case 'N': setPiece<false>(sq, 1 + offset); break;
                case 'B': setPiece<false>(sq, 2 + offset); break;
                case 'R': setPiece<false>(sq, 3 + offset); break;
                case 'Q': setPiece<false>(sq, 4 + offset); break;
                case 'K': setPiece<false>(sq, 5 + offset); break;
            }
            
            x++;
        }
    }
    
    // if the fen is large enough, we parse the color next.
    if (split.size() >= 2 && split[1].length() == 1) {
        if (split[1].at(0) != 'w') {
            changeActivePlayer();
            getBoardStatus()->zobrist ^= ZOBRIST_WHITE_BLACK_SWAP;
        }
    }
    
    // if the fen is large enough, we parse the castling rights next.
    if (split.size() >= 3) {
        for (int i = 0; i < 4; i++) {
            setCastlingRights(i, false);
        }
        
        for (char c : split[2]) {
            switch (c) {
                case 'K':
                    if (getPiece(E1) == WHITE_KING)
                        setCastlingRights(WHITE_KINGSIDE_CASTLING, true);
                    break;
                case 'Q':
                    if (getPiece(E1) == WHITE_KING)
                        setCastlingRights(WHITE_QUEENSIDE_CASTLING, true);
                    break;
                case 'k':
                    if (getPiece(E8) == BLACK_KING)
                        setCastlingRights(BLACK_KINGSIDE_CASTLING, true);
                    break;
                case 'q':
                    if (getPiece(E8) == BLACK_KING)
                        setCastlingRights(BLACK_QUEENSIDE_CASTLING, true);
                    break;
            }
        }
    }
    
    // e.p. square.
    if (split.size() >= 4) {
        if (split[3].at(0) != '-') {
            Square square = squareIndex(split[3]);
            setEnPassantSquare(square);
        }
    }
    
    if (split.size() >= 5)
        getBoardStatus()->fiftyMoveCounter = std::stoi(split[4]);

    if (split.size() >= 6) 
        getBoardStatus()->moveCounter = std::stoi(split[5]);
    
    this->evaluator.reset(this);
    // note that we do not read information about move counts. This is usually not required for playing games.
}

Board::Board(const Board& board) {
    *this = board;
}

/**
 * Beside using the FEN for a position, one can also copy directly for another board object.
 * Copies the entire history as well as all relevant fields.
 * @param board
 */
Board& Board::operator=(const Board& board) {
    // we need to copy occupancy bitboards for the teams
    m_teamOccupiedBB[WHITE] = board.getTeamOccupiedBB()[WHITE];
    m_teamOccupiedBB[BLACK] = board.getTeamOccupiedBB()[BLACK];
    
    // we need to copy occupancy bitboards for each piece
    for (int n = 0; n < N_PIECES; n++) {
        m_piecesBB[n] = board.m_piecesBB[n];
    }
    
    // we need to copy occupancy bitboards for all pieces
    m_occupiedBB = board.getOccupiedBB();
    
    // we also need to copy the active player
    m_activePlayer = board.getActivePlayer();
    
    // copying the piece board for each square
    for (int i = 0; i < N_SQUARES; i++) {
        m_pieceBoard[i] = board.m_pieceBoard[i];
    }
    
    // next we copy the entire history of the board.
    m_boardStatusHistory.clear();
    for (int n = 0; n < static_cast<int>(board.m_boardStatusHistory.size()); n++) {
        m_boardStatusHistory.push_back(board.m_boardStatusHistory.at(n).copy());
    }
    
    this->evaluator.reset(this);

    return *this;
}

/**
 * Returns a FEN-representation of the board object which can be used for other engines, and debugging.
 */
std::string Board::fen() const {
    std::stringstream ss;
    
    // we do it in the same way we read a fen.
    // first, we write the pieces
    for (Rank n = 7; n >= 0; n--) {
        int counting = 0;
        for (File i = 0; i < 8; i++) {
            const Square s = squareIndex(n, i);
            
            int piece = getPiece(s);
            if (piece == -1) {
                counting++;
            } else {
                if (counting != 0) {
                    ss << counting;
                }
                counting = 0;
                ss << PIECE_IDENTIFIER[piece];
            }
        }
        if (counting != 0) {
            ss << counting;
        }
        if (n != 0)
            ss << "/";
    }
    
    // adding the active player (w for white, b for black) padded by spaces.
    ss << " ";
    ss << ((getActivePlayer() == WHITE) ? "w" : "b");
    ss << " ";
    
    // its relevant to add a '-' if no castling rights exist
    bool anyCastling = false;
    if (getCastlingRights(WHITE_QUEENSIDE_CASTLING)) {
        anyCastling = true;
        ss << "Q";
    }
    if (getCastlingRights(WHITE_KINGSIDE_CASTLING)) {
        anyCastling = true;
        ss << "K";
    }
    if (getCastlingRights(BLACK_QUEENSIDE_CASTLING)) {
        anyCastling = true;
        ss << "q";
    }
    if (getCastlingRights(BLACK_KINGSIDE_CASTLING)) {
        anyCastling = true;
        ss << "k";
    }
    // if not castling rights exist, add a '-' in order to be able to read the e.p. square.
    if (anyCastling == false) {
        ss << "-";
    }
    // similar to castling rights, we need to add a '-' if there is no e.p. square.
    if (getEnPassantSquare() >= 0) {
        ss << " ";
        ss << SQUARE_IDENTIFIER[getEnPassantSquare()];
    } else {
        ss << " -";
    }
    
    // we also add the fifty move counter and the move counter to the fen (note that we dont parse those)
    ss << " " << getBoardStatus()->fiftyMoveCounter;
    ss << " " << getBoardStatus()->moveCounter;
    
    return ss.str();
}

/**
 * Returns the zobrist key for the current board.
 * The zobrist-key is stored within the meta information of the board.
 */
U64 Board::zobrist() const { return getBoardStatus()->zobrist; }

/**
 * Returns true if the given player is in check by the opponent.
 */
bool Board::isInCheck(Color player) const {
    // we do this by casting rays from the king position and checking if an opponent piece is on those rays
    // which could attack the given ray.
    if (player == WHITE) {
        return isUnderAttack<BLACK>(bitscanForward(m_piecesBB[WHITE_KING]));
    } else {
        return isUnderAttack<WHITE>(bitscanForward(m_piecesBB[BLACK_KING]));
    }
}

/**
 * Returns true if the board is a draw by either the 50-move rule or a threefold repetition.
 * Stalemates are not considering here as they require dynamic knowledge about the position
 * which is only given during the search.
 * @return
 */
bool Board::isDraw() const { return getCurrent50MoveRuleCount() >= 50 || getCurrentRepetitionCount() >= 2; }

/**
 * returns the piece which occupies the board at the given index
 * @param sq
 * @return
 */
Piece Board::getPiece(Square sq) const { return m_pieceBoard[sq]; }

/**
 * Sets the piece on the given square.
 * Deals with zobrist-keys.
 * @param sq
 * @param piece
 */
template<bool updateNN, bool updateZobrist>
void Board::setPiece(Square sq, Piece piece) {
    // first we set the piece on the piece board
    m_pieceBoard[sq] = piece;
    
    // we need the square as a bitboard for the occupancy bitboards
    const U64 sqBB = (ONE << sq);
    
    // settings the occupancy for the team, the piece and the total occupancy.
    m_piecesBB[piece] |= sqBB;
    m_teamOccupiedBB[piece / 8] |= sqBB;
    m_occupiedBB |= sqBB;
    
    // update the evaluator
    if constexpr (updateNN) {
        evaluator.setPieceOnSquare<true>(getPieceType(piece),
                                         getPieceColor(piece),
                                         sq,
                                         bitscanForward(m_piecesBB[WHITE_KING]),
                                         bitscanForward(m_piecesBB[BLACK_KING]));
    }

    // also adjust the zobrist key
    if constexpr (updateZobrist) {
        BoardStatus* st = getBoardStatus();
        st->zobrist ^= getHash(piece, sq);
    }
}

/**
 * Unsets the piece on the given square.
 * Deals with zobrist-keys.
 * @param sq
 */
template<bool updateNN, bool updateZobrist>
void Board::unsetPiece(Square sq) {
    UCI_ASSERT(0 <= sq && sq <= 63);
    
    // we need to know first which piece is contained on the given square.
    const Piece p = getPiece(sq);
    
    // similar to setPiece() we need the square as a bitboard for upccancy bitboards.
    // as we need to remove bits from the occupancy bitboards, we use the inverse.
    const U64 sqBB = ~(ONE << sq);
    
    // update the evaluator
    if constexpr (updateNN){
        evaluator.setPieceOnSquare<false>(getPieceType(p), getPieceColor(p), sq,
                                          bitscanForward(m_piecesBB[WHITE_KING]),
                                          bitscanForward(m_piecesBB[BLACK_KING]));
    }
    
    // actually removing bits from the occupancy bitboards.
    m_piecesBB[p] &= sqBB;
    m_teamOccupiedBB[p / 8] &= sqBB;
    m_occupiedBB &= sqBB;
    
    // also adjust the zobrist key
    if constexpr (updateZobrist) {
        BoardStatus* st = getBoardStatus();
        st->zobrist ^= getHash(p, sq);  
    }
    
    // removing the piece from the square-wise piece table.
    m_pieceBoard[sq] = -1;
}

/**
 * Replaces the piece on the given square with the given new piece.
 * Deals with zobrist-keys.
 * @param sq
 * @param piece
 */
template<bool updateNN, bool updateZobrist>
void Board::replacePiece(Square sq, Piece piece) {
    UCI_ASSERT(0 <= sq && sq <= 63);
    
    // we need to know first which piece is contained on the given square.
    const Piece p = getPiece(sq);
    
    // similar to setPiece() we need the square as a bitboard for upccancy bitboards.
    const U64 sqBB = (ONE << sq);
    
    m_piecesBB[p] &= ~sqBB;                 // unset
    m_piecesBB[piece] |= sqBB;              // set
    m_teamOccupiedBB[p / 8] &= ~sqBB;       // unset
    m_teamOccupiedBB[piece / 8] |= sqBB;    // set
    
    // also adjust the zobrist key
    if constexpr (updateZobrist) {
        BoardStatus* st = getBoardStatus();
        st->zobrist ^= (getHash(p, sq) ^ getHash(piece, sq));      
    }
    
    // update the evaluator
    if constexpr (updateNN){
        evaluator.setPieceOnSquare<false>(getPieceType(p    ), getPieceColor(p    ), sq,
                                          bitscanForward(m_piecesBB[WHITE_KING]),
                                          bitscanForward(m_piecesBB[BLACK_KING]));
        evaluator.setPieceOnSquare<true >(getPieceType(piece), getPieceColor(piece), sq,
                                         bitscanForward(m_piecesBB[WHITE_KING]),
                                         bitscanForward(m_piecesBB[BLACK_KING]));
    }

    // removing the piece from the square-wise piece table.
    m_pieceBoard[sq] = piece;
}

/**
 * Sets the hash for piece on the given square.
 * Deals with zobrist-keys.
 * @param sq
 * @param piece
 */
void Board::setPieceHash(Square sq, Piece piece) {
    BoardStatus* st = getBoardStatus();
    st->zobrist ^= getHash(piece, sq);
}

/**
 * Unsets the hash for piece on the given square.
 * Deals with zobrist-keys.
 * @param sq
 */
void Board::unsetPieceHash(Square sq) {
    UCI_ASSERT(0 <= sq && sq <= 63);
    
    // we need to know first which piece is contained on the given square.
    Piece p = getPiece(sq);
    BoardStatus* st = getBoardStatus();
    st->zobrist ^= getHash(p, sq);  
}

/**
 * Replaces the hash for piece on the given square with the given new piece.
 * Deals with zobrist-keys.
 * @param sq
 * @param piece
 */
void Board::replacePieceHash(Square sq, Piece piece) {
    UCI_ASSERT(0 <= sq && sq <= 63);
    
    // we need to know first which piece is contained on the given square.
    Piece p = getPiece(sq);
    BoardStatus* st = getBoardStatus();
    st->zobrist ^= (getHash(p, sq) ^ getHash(piece, sq));      
}

/**
 * changes the active player. This does not deal with the zobrist key so this function
 * should usually not be used.
 */
void Board::changeActivePlayer() { m_activePlayer = 1 - m_activePlayer; }

/**
 * Does a move. This function will crash if the move is not okay. (illegal moves are ok).
 * Computes repetition counters as well as updating the zobrist key.
 * @param m
 */
template<bool prefetch> void Board::move(Move m, TranspositionTable* table) {
    BoardStatus* previousStatus = getBoardStatus();
    BoardStatus  newBoardStatus = {previousStatus->zobrist,           // zobrist will be changed later
                                  0ULL,                              // reset en passant. might be set later
                                  previousStatus->castlingRights,    // copy meta. might be changed
                                  previousStatus->fiftyMoveCounter
                                      + 1,    // increment fifty move counter. might be reset
                                  1ULL,       // set rep to 1 (no rep)
                                  previousStatus->moveCounter + getActivePlayer(),    // increment move counter
                                  m};
    
        
    this->evaluator.addNewAccumulation();
    
    const Square   sqFrom = getSquareFrom(m);
    const Square   sqTo   = getSquareTo(m);
    const Piece    pFrom  = getMovingPiece(m);
    const MoveType mType  = getType(m);
    const Color    color  = getActivePlayer();
    const int      factor = getActivePlayer() == WHITE ? 1 : -1;
    
    const Square wKingSq = bitscanForward(getPieceBB<WHITE, KING>());
    const Square bKingSq = bitscanForward(getPieceBB<BLACK, KING>());

    const PieceType movingPieceType    = getMovingPieceType(m);
    const PieceType promotionPieceType = getPromotionPieceType(m);
    const PieceType capturedPieceType  = getCapturedPieceType(m);

    if (isCapture(m)) {
        // reset fifty move counter if a piece has been captured
        newBoardStatus.fiftyMoveCounter = 0;
        
        if (getPieceType(getPiece(sqTo)) == ROOK) {
            if (color == BLACK) {
                if (sqTo == A1) {
                    newBoardStatus.castlingRights &= ~(ONE << (WHITE_QUEENSIDE_CASTLING));
                } else if (sqTo == H1) {
                    newBoardStatus.castlingRights &= ~(ONE << (WHITE_KINGSIDE_CASTLING));
                }
            } else {
                if (sqTo == A8) {
                    newBoardStatus.castlingRights &= ~(ONE << (BLACK_QUEENSIDE_CASTLING));
                } else if (sqTo == H8) {
                    newBoardStatus.castlingRights &= ~(ONE << (BLACK_KINGSIDE_CASTLING));
                }
            }
        }
    }
    
    newBoardStatus.zobrist ^= ZOBRIST_WHITE_BLACK_SWAP;
    if (getPieceType(pFrom) == PAWN) {
        // reset fifty move counter if pawn has moved
        newBoardStatus.fiftyMoveCounter = 0;
        
        // if a pawn advances by 2 squares, enabled enPassant capture next move
        if (mType == DOUBLED_PAWN_PUSH) {
            newBoardStatus.enPassantTarget = (ONE << (sqFrom + 8 * factor));
        }
            
        // promotions are handled differently because the new piece at the target square is not the piece that initially
        // moved.
        else if (isPromotion(m)) {
            // we handle this case seperately so we return after this finished.
            m_boardStatusHistory.emplace_back(std::move(newBoardStatus));
            this->changeActivePlayer();
            
            // setting m_piecesBB
            this->unsetPiece<false>(sqFrom);
            // do the "basic" move
            if (isCapture(m)) {
                this->replacePiece<false>(sqTo, getPromotionPiece(m));
            } else {
                this->setPiece<false>(sqTo, getPromotionPiece(m));
            }
            
            if(isCapture(m)){
                evaluator.setUnsetUnsetPiece(
                    nn::Index{promotionPieceType, color, sqTo  , wKingSq, bKingSq},
                    nn::Index{movingPieceType   , color, sqFrom, wKingSq, bKingSq},
                    nn::Index{capturedPieceType ,!color, sqTo  , wKingSq, bKingSq});
            }else{
                evaluator.setUnsetPiece(
                    nn::Index{promotionPieceType, color, sqTo  , wKingSq, bKingSq},
                    nn::Index{movingPieceType   , color, sqFrom, wKingSq, bKingSq});
            }
            
            return;
        } else if (mType == EN_PASSANT) {
            m_boardStatusHistory.emplace_back(std::move(newBoardStatus));
            this->changeActivePlayer();
            
            Square epSquare = sqTo - 8 * factor;
            this->unsetPiece<false>(epSquare);
            this->unsetPiece<false>(sqFrom);
            this->setPiece  <false>(sqTo, pFrom);
            
            evaluator.setUnsetUnsetPiece(
                nn::Index{PAWN , color, sqTo    , wKingSq, bKingSq},
                nn::Index{PAWN , color, sqFrom  , wKingSq, bKingSq},
                nn::Index{PAWN ,!color, epSquare, wKingSq, bKingSq});
            
            return;
        }
    } else if (getPieceType(pFrom) == KING) {
        // revoke castling rights if king moves
        newBoardStatus.castlingRights &= ~(ONE << (color * 2));
        newBoardStatus.castlingRights &= ~(ONE << (color * 2 + 1));
        
        // we handle this case seperately so we return after this finished.
        m_boardStatusHistory.emplace_back(std::move(newBoardStatus));
        this->changeActivePlayer();
        
        if (isCastle(m)) {
            // move the rook as well. castling rights are handled below
            Square rookSquare = sqFrom + (mType == QUEEN_CASTLE ? -4 : 3);
            Square rookTarget = sqTo + (mType == QUEEN_CASTLE ? 1 : -1);
            this->unsetPiece<false>(rookSquare);
            this->setPiece  <false>(rookTarget, ROOK + 8 * color);
            this->unsetPiece<false>(sqFrom);
            this->setPiece  <false>(sqTo, pFrom);
            this->evaluator.setSetUnsetUnsetPiece(
                {KING, color, sqTo      , wKingSq, bKingSq},
                {ROOK, color, rookTarget, wKingSq, bKingSq},
                {KING, color, sqFrom    , wKingSq, bKingSq},
                {ROOK, color, rookSquare, wKingSq, bKingSq});
        }else if(isCapture(m)){
            this->unsetPiece  <false>(sqFrom);
            this->replacePiece<false>(sqTo, pFrom);
            this->evaluator.setUnsetUnsetPiece(
                {KING             , color, sqTo  , wKingSq, bKingSq},
                {KING             , color, sqFrom, wKingSq, bKingSq},
                {capturedPieceType,!color, sqTo  , wKingSq, bKingSq});
        }else{
            this->unsetPiece<false>(sqFrom);
            this->setPiece  <false>(sqTo, pFrom);
            
            this->evaluator.setUnsetPiece(
                {movingPieceType, color, sqTo  , wKingSq, bKingSq},
                {movingPieceType, color, sqFrom, wKingSq, bKingSq});
        }
        
        // check if it crossed squares
        if(     nn::kingSquareIndex(sqTo, color) !=
                nn::kingSquareIndex(sqFrom, color)
                ||  fileIndex(sqFrom) + fileIndex(sqTo) == 7){
            this->evaluator.resetAccumulator(this, color);
        }
        
        // we need to compute the repetition count
        this->computeNewRepetition();
        
        
        return;
    }
        
    // revoke castling rights if rook moves and it is on the initial square
    else if (movingPieceType == ROOK) {
        if (color == WHITE) {
            if (sqFrom == A1) {
                newBoardStatus.castlingRights &= ~(ONE << (color * 2));
            } else if (sqFrom == H1) {
                newBoardStatus.castlingRights &= ~(ONE << (color * 2 + 1));
            }
        } else {
            if (sqFrom == A8) {
                newBoardStatus.castlingRights &= ~(ONE << (color * 2));
            } else if (sqFrom == H8) {
                newBoardStatus.castlingRights &= ~(ONE << (color * 2 + 1));
            }
        }
    }
    
    m_boardStatusHistory.emplace_back(std::move(newBoardStatus));
    
    // doing the initial move
    this->unsetPieceHash(sqFrom);
    if (mType != EN_PASSANT && isCapture(m)) {
        this->replacePieceHash(sqTo, pFrom);
    } else {
        this->setPieceHash(sqTo, pFrom);
    }
    if constexpr (prefetch)
        table->prefetch(getBoardStatus()->zobrist);

    
    
    if (mType != EN_PASSANT && isCapture(m)) {
        this->unsetPiece<false, false>(sqFrom);
        this->replacePiece<false, false>(sqTo, pFrom);
        this->evaluator.setUnsetUnsetPiece({movingPieceType  , color, sqTo  , wKingSq, bKingSq},
                                           {movingPieceType  , color, sqFrom, wKingSq, bKingSq},
                                           {capturedPieceType,!color, sqTo  , wKingSq, bKingSq});
    } else {
        this->evaluator.setUnsetPiece({movingPieceType, color, sqTo  , wKingSq, bKingSq},
                                      {movingPieceType, color, sqFrom, wKingSq, bKingSq});
        this->unsetPiece<false, false>(sqFrom);
        this->setPiece<false , false>(sqTo, pFrom);
    }
    
    this->changeActivePlayer();
    this->computeNewRepetition();
}
template void Board::move<false>(Move m, TranspositionTable* table);
template void Board::move<true >(Move m, TranspositionTable* table);

/**
 * undoes the last move. Assumes the last move has not been a null move.
 */
void Board::undoMove() {
    const Move m = getBoardStatus()->move;
    
    changeActivePlayer();
    
    const Square   sqFrom   = getSquareFrom(m);
    const Square   sqTo     = getSquareTo(m);
    const Piece    pFrom    = getMovingPiece(m);
    const MoveType mType    = getType(m);
    const Piece    captured = getCapturedPiece(m);
    const bool     isCap    = isCapture(m);
    const Color    color    = getActivePlayer();
    const int      factor   = getActivePlayer() == 0 ? 1 : -1;
    
    if (mType == EN_PASSANT) {
        setPiece<false>(sqTo - 8 * factor, (1 - color) * 8);
    }
    
    if (getPieceType(pFrom) == KING && isCastle(m)) {
        const Square rookSquare = sqFrom + (mType == QUEEN_CASTLE ? -4 : 3);
        const Square rookTarget = sqTo + (mType == QUEEN_CASTLE ? 1 : -1);
        setPiece  <false>(rookSquare, ROOK + 8 * color);
        unsetPiece<false>(rookTarget);
    }
    
    if (mType != EN_PASSANT && isCap) {
        replacePiece<false>(sqTo, captured);
    } else {
        unsetPiece  <false>(sqTo);
    }
    
    setPiece<false>(sqFrom, pFrom);
    
    m_boardStatusHistory.pop_back();
    this->evaluator.popAccumulation();
}

/**
 * does a null move.
 */
void Board::move_null() {
    const BoardStatus* previousStatus = getBoardStatus();
    const BoardStatus  newBoardStatus = {previousStatus->zobrist ^ ZOBRIST_WHITE_BLACK_SWAP,
                                         0ULL,
                                         previousStatus->castlingRights,
                                         previousStatus->fiftyMoveCounter + 1,
                                         1ULL,
                                         previousStatus->moveCounter + getActivePlayer(),
                                         0ULL};
    
    m_boardStatusHistory.emplace_back(std::move(newBoardStatus));
    changeActivePlayer();
}

/**
 * undoes a null move. Assumes that the previous move has been a null move.
 */
void Board::undoMove_null() {
    m_boardStatusHistory.pop_back();
    changeActivePlayer();
}

/**
 * returns the Move which lead to the current position.
 * @return
 */
Move Board::getPreviousMove(Depth ply) const {
    if (m_boardStatusHistory.size() <= ply)
        return 0;
    return m_boardStatusHistory[m_boardStatusHistory.size() - ply].move;
}

/**
 * returns a bitboard of all the attacked squares by the given color.
 * this does not check for en passant captures.
 * @param attacker
 * @return
 */
template<Color attacker> U64 Board::getAttackedSquares() const {
    U64 att = ZERO;
    
    if (attacker == WHITE) {
        att |= shiftNorthEast(m_piecesBB[WHITE_PAWN]) | shiftNorthWest((m_piecesBB[WHITE_PAWN]));
    } else {
        att |= shiftSouthEast(m_piecesBB[BLACK_PAWN]) | shiftSouthWest((m_piecesBB[BLACK_PAWN]));
    }
    
    U64 knights(m_piecesBB[KNIGHT + 8 * attacker]);
    U64 bishops(m_piecesBB[BISHOP + 8 * attacker]);
    U64 rooks(m_piecesBB[ROOK + 8 * attacker]);
    U64 queens(m_piecesBB[QUEEN + 8 * attacker]);
    U64 kings(m_piecesBB[KING + 8 * attacker]);
    
    while (knights) {
        const Square s = bitscanForward(knights);
        att |= attacks::KNIGHT_ATTACKS[s];
        knights = lsbReset(knights);
    }
    while (bishops) {
        const Square s = bitscanForward(bishops);
        att |= attacks::lookUpBishopAttacks(s, m_occupiedBB);
        bishops = lsbReset(bishops);
    }
    while (rooks) {
        const Square s = bitscanForward(rooks);
        att |= attacks::lookUpRookAttacks(s, m_occupiedBB);
        rooks = lsbReset(rooks);
    }
    while (queens) {
        const Square s = bitscanForward(queens);
        att |= attacks::lookUpRookAttacks(s, m_occupiedBB);
        att |= attacks::lookUpBishopAttacks(s, m_occupiedBB);
        queens = lsbReset(queens);
    }
    while (kings) {
        const Square s = bitscanForward(kings);
        att |= attacks::KING_ATTACKS[s];
        kings = lsbReset(kings);
    }
    
    return att;
}

/**
 * returns a bitboard of the least valuable piece which is in the attadef bitboard.
 * The piece itself will be stored in the given piece variable.
 * @param attadef
 * @param bySide
 * @param piece
 * @return
 */
U64 Board::getLeastValuablePiece(U64 attadef, Score bySide, Piece& piece) const {
    for (piece = PAWN + bySide * 8; piece <= KING + bySide * 8; piece += 1) {
        const U64 subset = attadef & m_piecesBB[piece];
        if (subset)
            return subset & -subset;    // single bit
    }
    return 0;    // empty set
}

/**
 * returns the static exchange evaluation for the given move.
 * this does not consider promotions during captures.
 * @param m
 * @return
 */
Score Board::staticExchangeEvaluation(Move m) const {
    
    // track gains across captures
    int gain[32];
    Depth d = 0;
    
    // get basic information for the first move
    Square sqFrom         = getSquareFrom(m);
    Square sqTo           = getSquareTo(m);
    Piece  capturedPiece  = isCapture(m) ? getCapturedPiece(m) : -1;
    Piece  capturingPiece = getMovingPiece(m);
    
    // start with the opponent to attack
    Color attacker = !getMovingPieceColor(m);
    
    // occupancy bitboard
    // remove move
    U64   occBB   = m_occupiedBB;
    U64 sqToBB    = ONE << sqTo;
    U64 sqFromBB  = ONE << sqFrom;
    unsetBit(occBB,sqFrom);
    setBit  (occBB,sqTo);
    
    // the thing with attackers is that some attacker may be discovered after certain attackers
    // have attacked. Its relevant during the swap algorithm to update the attackers bitboard
    // after every move to check for discovered attacks. Discovered attacks can only happen
    // for sliding pieces (rooks, queens, bishops). They are extracted at the first point
    // for easier access later on
    U64 rooksBB = getPieceTypeBB<ROOK>();
    U64 queensBB = getPieceTypeBB<QUEEN>();
    U64 bishopsBB = getPieceTypeBB<BISHOP>();
    
    // they can be divided into diagonal sliding pieces and horizontal ones
    U64 horSliderBB = (rooksBB   | queensBB) & ~sqFromBB;
    U64 diaSliderBB = (bishopsBB | queensBB) & ~sqFromBB;
    
    // get initial attacks (exclude any potentially discovered attacks since they will update during
    // SEE)
    U64 attackersBB = ( ( shiftNorthWest(sqToBB) | shiftNorthEast(sqToBB)) & m_piecesBB[BLACK_PAWN])
                      | ((shiftSouthWest(sqToBB) | shiftSouthEast(sqToBB)) & m_piecesBB[WHITE_PAWN])
                      | (attacks::KNIGHT_ATTACKS[sqTo] & (getPieceTypeBB<KNIGHT>()))
                      | (attacks::KING_ATTACKS  [sqTo] & (getPieceTypeBB<KING>  ()))
                      | (attacks::lookUpRookAttacks  (sqTo, occBB) &  horSliderBB)
                      | (attacks::lookUpBishopAttacks(sqTo, occBB) &  diaSliderBB);
    // remove the initially moved piece
    attackersBB &= occBB;
    
    if (isCapture(m))
        gain[d] = see_piece_vals[getPieceType(capturedPiece)];
    else {
        gain[d] = 0;
    }
    
    while(true){
        
        // make sure we dont have any artifacts here
        attackersBB &= occBB;
        
        // increase depth
        d ++;
        
        // get a subset of the attackers which is currently moving
        U64 activeAttackersBB = attackersBB & getTeamOccupiedBB(attacker);
        
        // if there are no attackers, exit the algorithm
        if(!activeAttackersBB)
            break;
        
        // get the least valuable piece which would capture next
        PieceType pt = PAWN;
        for(pt = PAWN; pt <= KING; pt ++){
            if(getPieceBB(attacker, pt) & activeAttackersBB){
                break;
            }
        }
        
        // safety stuff
        if(pt > KING)
            break;
        
        // perform the move on the occ bitboard
        U64 pieceBB = getPieceBB(attacker, pt) & attackersBB;
        Square pieceSqFrom = bitscanForward(pieceBB);
        
        // adjusting gain
        gain[d] = see_piece_vals[getPieceType(capturingPiece)] - gain[d - 1];
        if (std::max(-gain[d-1], gain[d]) < 0) break; // pruning does not influence the result
        capturingPiece = pt;
        
        // remove attacker from both bitboard
        unsetBit(occBB, pieceSqFrom);
        
        // update attacker bitboard
        if(pt == PAWN || pt == BISHOP || pt == QUEEN){
            attackersBB |= attacks::lookUpBishopAttacks(sqTo, occBB) & diaSliderBB;
        }
        if(pt == ROOK || pt == QUEEN){
            attackersBB |= attacks::lookUpRookAttacks(sqTo, occBB) & horSliderBB;
        }
        
        // flip attacking side
        attacker = !attacker;
    }
    
    while (--d){
        gain[d-1]= -std::max (-gain[d-1], gain[d]);
    }

    return gain[0];
}

/**
 * returns a bitboard with all squares highlighted which either attack or defend the given square
 * @param occupied
 * @param sq
 * @return
 */
U64 Board::attacksTo(U64 p_occupied, Square sq) const {
    const U64 sqBB = ONE << sq;
    U64 knights, kings, bishopsQueens, rooksQueens;
    knights     = m_piecesBB[WHITE_KNIGHT] | m_piecesBB[BLACK_KNIGHT];
    kings       = m_piecesBB[WHITE_KING] | m_piecesBB[BLACK_KING];
    rooksQueens = bishopsQueens = m_piecesBB[WHITE_QUEEN] | m_piecesBB[BLACK_QUEEN];
    rooksQueens |= m_piecesBB[WHITE_ROOK] | m_piecesBB[BLACK_ROOK];
    bishopsQueens |= m_piecesBB[WHITE_BISHOP] | m_piecesBB[BLACK_BISHOP];
    
    return ((shiftNorthWest(sqBB) | shiftNorthEast(sqBB)) & m_piecesBB[BLACK_PAWN])
           | ((shiftSouthWest(sqBB) | shiftSouthEast(sqBB)) & m_piecesBB[WHITE_PAWN]) | (attacks::KNIGHT_ATTACKS[sq] & knights)
           | (attacks::KING_ATTACKS[sq] & kings) | (attacks::lookUpBishopAttacks(sq, p_occupied) & bishopsQueens)
           | (attacks::lookUpRookAttacks(sq, p_occupied) & rooksQueens);
}

/**
 * this does not check for en passent attacks!
 * @param square
 * @param attacker
 * @return
 */
template<Color attacker> bool Board::isUnderAttack(Square square) const {
    const U64 sqBB = ONE << square;

    UCI_ASSERT(square < 64)
    UCI_ASSERT(sqBB)
    
    if constexpr (attacker == WHITE) {
        return (   attacks::lookUpRookAttacks  (square, m_occupiedBB) & (m_piecesBB[WHITE_QUEEN] |
                                                                    m_piecesBB[WHITE_ROOK]))  != 0
               || (attacks::lookUpBishopAttacks(square, m_occupiedBB) & (m_piecesBB[WHITE_QUEEN] |
                                                                         m_piecesBB[WHITE_BISHOP]))!= 0
               || (attacks::KNIGHT_ATTACKS[square]                    &  m_piecesBB[WHITE_KNIGHT]) != 0
               || (attacks::KING_ATTACKS  [square]                    &  m_piecesBB[WHITE_KING])   != 0
               || ((shiftSouthEast(sqBB) | shiftSouthWest(sqBB))      &  m_piecesBB[WHITE_PAWN])   != 0;
    } else {
        return (  attacks::lookUpRookAttacks   (square, m_occupiedBB) & (m_piecesBB[BLACK_QUEEN] |
                                                                    m_piecesBB[BLACK_ROOK]))  != 0
               || (attacks::lookUpBishopAttacks(square, m_occupiedBB) & (m_piecesBB[BLACK_QUEEN] |
                                                                         m_piecesBB[BLACK_BISHOP]))!= 0
               || (attacks::KNIGHT_ATTACKS[square]                    &  m_piecesBB[BLACK_KNIGHT]) != 0
               || (attacks::KING_ATTACKS  [square]                    &  m_piecesBB[BLACK_KING])   != 0
               || ((shiftNorthEast(sqBB) | shiftNorthWest(sqBB))      & m_piecesBB[BLACK_PAWN])    != 0;
    }
}

/**
 * this does not check for en passent attacks!
 * @param square
 * @param attacker
 * @return
 */
bool Board::isUnderAttack(Square square, Color attacker) const {
    if (attacker == WHITE) {
        return isUnderAttack<WHITE>(square);
    } else {
        return isUnderAttack<BLACK>(square);
    }
}

/**
 * checks if the given move gives check
 * @param m
 * @return
 */
bool Board::givesCheck(Move m) {
    int opponentKingPos;
    U64 opponentKing;
    
    Piece        pFrom  = getMovingPiece(m);
    const Square sqTo   = getSquareTo(m);
    const Square sqFrom = getSquareFrom(m);
    
    if (getActivePlayer() == BLACK) {
        opponentKing    = m_piecesBB[WHITE_KING];
        opponentKingPos = bitscanForward(opponentKing);
    } else {
        opponentKing    = m_piecesBB[BLACK_KING];
        opponentKingPos = bitscanForward(opponentKing);
    }
    
    const U64 occ = m_occupiedBB;
    
    // replace the moving piece with the piece to promote to if promotion to detect direct check
    if (isPromotion(m)) {
        unsetBit(m_occupiedBB, sqFrom);
        pFrom = getPromotionPiece(m);
    }
    
    // direct check
    switch (getPieceType(pFrom)) {
        case QUEEN: {
            const U64 att = attacks::lookUpBishopAttacks(sqTo, m_occupiedBB) | attacks::lookUpRookAttacks(sqTo, m_occupiedBB);
            //            printBitmap(m_occupiedBB);
            //            printBitmap(att);
            if (att & opponentKing) {
                m_occupiedBB = occ;
                return true;
            }
            break;
        }
        case BISHOP: {
            const U64 att = attacks::lookUpBishopAttacks(sqTo, m_occupiedBB);
            if (att & opponentKing) {
                m_occupiedBB = occ;
                return true;
            }
            break;
        }
        case ROOK: {
            const U64 att = attacks::lookUpRookAttacks(sqTo, m_occupiedBB);
            if (att & opponentKing) {
                m_occupiedBB = occ;
                return true;
            }
            break;
        }
        case KNIGHT: {
            const U64 att = attacks::KNIGHT_ATTACKS[sqTo];
            if (att & opponentKing) {
                m_occupiedBB = occ;
                return true;
            }
            break;
        }
        case PAWN: {
            const U64 toBB = ONE << sqTo;
            
            if (getActivePlayer() == WHITE) {
                if (((shiftNorthEast(toBB) | shiftNorthWest(toBB)) & opponentKing) != 0) {
                    m_occupiedBB = occ;
                    return true;
                }
            } else {
                if (((shiftSouthEast(toBB) | shiftSouthWest(toBB)) & opponentKing) != 0) {
                    m_occupiedBB = occ;
                    return true;
                }
            }
            break;
        }
    }
    
    // discovered check
    
    unsetBit(m_occupiedBB, sqFrom);
    setBit(m_occupiedBB, sqTo);
    
    if (getActivePlayer() == WHITE) {
        if (isUnderAttack<WHITE>(opponentKingPos)) {
            m_occupiedBB = occ;
            return true;
        }
    } else {
        if (isUnderAttack<BLACK>(opponentKingPos)) {
            m_occupiedBB = occ;
            return true;
        }
    }
    m_occupiedBB = occ;
    
    // castling check
    if (isCastle(m)) {
        unsetBit(m_occupiedBB, sqFrom);
        Square rookSquare = getActivePlayer() == WHITE ? (sqTo - sqFrom) > 0 ? F1 : D1 : (sqTo - sqFrom) > 0 ? F8 : D8;
        if ((attacks::lookUpRookAttacks(rookSquare, m_occupiedBB) & opponentKing) != 0) {
            m_occupiedBB = occ;
            return true;
        }
    }
    
    // en passant
    else if (isEnPassant(m)) {
        if (getActivePlayer() == WHITE) {
            unsetBit(m_occupiedBB, sqTo - 8);
            unsetBit(m_occupiedBB, sqFrom);
            setBit(m_occupiedBB, sqTo);
            if (isUnderAttack<WHITE>(opponentKingPos)) {
                m_occupiedBB = occ;
                return true;
            }
        } else {
            unsetBit(m_occupiedBB, sqTo + 8);
            unsetBit(m_occupiedBB, sqFrom);
            setBit(m_occupiedBB, sqTo);
            if (isUnderAttack<BLACK>(opponentKingPos)) {
                m_occupiedBB = occ;
                return true;
            }
        }
    }
    m_occupiedBB = occ;
    return false;
}

/**
 * checks if the move is legal
 * @param m
 * @return
 */
bool Board::isLegal(Move m) {
    Square thisKing;
    U64    opponentQueenBitboard;
    U64    opponentRookBitboard;
    U64    opponentBishopBitboard;
    
    if (this->getActivePlayer() == WHITE) {
        thisKing               = bitscanForward(m_piecesBB[WHITE_KING]);
        opponentQueenBitboard  = m_piecesBB[BLACK_QUEEN];
        opponentRookBitboard   = m_piecesBB[BLACK_ROOK];
        opponentBishopBitboard = m_piecesBB[BLACK_BISHOP];
    } else {
        thisKing               = bitscanForward(m_piecesBB[BLACK_KING]);
        opponentQueenBitboard  = m_piecesBB[WHITE_QUEEN];
        opponentRookBitboard   = m_piecesBB[WHITE_ROOK];
        opponentBishopBitboard = m_piecesBB[WHITE_BISHOP];
    }

    if (isEnPassant(m)) {
        this->move(m);
        bool isOk =
            (attacks::lookUpRookAttacks(thisKing, m_occupiedBB) & (opponentQueenBitboard | opponentRookBitboard)) == 0
            && (attacks::lookUpBishopAttacks(thisKing, m_occupiedBB) & (opponentQueenBitboard | opponentBishopBitboard)) == 0;
        this->undoMove();
        
        return isOk;
    } else if (isCastle(m)) {
        U64 secure = ZERO;
        
        MoveType t = getType(m);
        if (this->getActivePlayer() == WHITE) {
            secure = (t == QUEEN_CASTLE) ? bb::CASTLING_WHITE_QUEENSIDE_SAFE : bb::CASTLING_WHITE_KINGSIDE_SAFE;
            return (getAttackedSquares<BLACK>() & secure) == 0;
        } else {
            secure = (t == QUEEN_CASTLE) ? bb::CASTLING_BLACK_QUEENSIDE_SAFE : bb::CASTLING_BLACK_KINGSIDE_SAFE;
            return (getAttackedSquares<WHITE>() & secure) == 0;
        }
    }
    
    const Square sqFrom = getSquareFrom(m);
    const Square sqTo   = getSquareTo(m);
    const bool   isCap  = isCapture(m);
    
    const U64 occCopy = m_occupiedBB;
    
    unsetBit(m_occupiedBB, sqFrom);
    setBit(m_occupiedBB, sqTo);
    
    bool isAttacked = false;
    
    if (getPieceType(getMovingPiece(m)) == KING) {
        thisKing = sqTo;
    }
    
    if (isCap) {
        Piece captured = getCapturedPiece(m);
        
        unsetBit(this->m_piecesBB[captured], sqTo);
        isAttacked = isUnderAttack(thisKing, !this->getActivePlayer());
        setBit(this->m_piecesBB[captured], sqTo);
    } else {
        isAttacked = isUnderAttack(thisKing, !this->getActivePlayer());
    }
    
    m_occupiedBB = occCopy;
    
    return !isAttacked;
}

/**
 * checks if a given move is pseudo legal which includes:
 * - valid from square
 * - valid capture target
 * - valid type
 * Note that this does not check if its legal or even possible.
 * It assumes that this is mainly called for moves coming from the hash table which have been generated by the move generator.
 * This means that moves like pawn from a2 to c3 will be alright if there is a pawn on a2.
 * @param m
 * @return
 */
bool Board::isPseudoLegal(Move m) const {
    if (!m)
        return false;
    
    // first we extract some information which are definetly required
    const Square sqFrom        = move::getSquareFrom(m);
    const Square sqTo          = move::getSquareTo(m);
    const Piece  pieceFrom     = move::getMovingPiece(m);
    const Piece  pieceTo       = move::getCapturedPiece(m);
    const Color  activePlayer  = move::getMovingPieceColor(m);
    const bool   isCapture     = move::isCapture(m);
    
    // check if the piece at the starting square is also the moving piece
    if(getPiece(sqFrom) != pieceFrom) return false;
    
    // make sure to only move pieces of the correct color
    if(getPieceColor(pieceFrom) != getActivePlayer()) return false;
    
    // make sure that the piece we capture is also the piece on the square we move to
    if((isCapture && !isEnPassant(m)) && pieceTo != getPiece(sqTo)) return false;
    
    // don't capture your own pieces
    if((isCapture && !isEnPassant(m)) && getPieceColor(pieceTo) == getActivePlayer()) return false;
    
    // no capture should also not move to any square where there is a piece
    if(!(isCapture && !isEnPassant(m)) && (pieceTo != 0 || getPiece(sqTo) != -1)) return false;
    
    // square from and square to cannot be the same
    if(sqFrom == sqTo) return false;
    
    // check that promotions, or e.p. is only done for pawns
    if((    isDoubledPawnPush(m)
         || isPromotion      (m)
         || isEnPassant      (m)) && getPieceType(pieceFrom) != PAWN) return false;
    
    // check that castling moves can only be played by kings
    if(isCastle(m) && getPieceType(pieceFrom) != KING) return false;
    
    // 6 or 7 are illegal
    if(getType(m) == 6 || getType(m) == 7) return false;
    
    // forward direction for pawns
    Direction forward = (getActivePlayer() == WHITE) ? NORTH : SOUTH;
    
    // check piece specific stuff
    switch(getPieceType(pieceFrom)){
        case PAWN:
            // normal moves or double pawn pushes must make sure the square in front is empty
            if (getType(m) == QUIET){
                if(getPiece(sqFrom + forward) != -1) return false;
                if(sqFrom + forward != sqTo)         return false;
                if((getActivePlayer() == WHITE && rankIndex(sqFrom) >= 6) ||
                   (getActivePlayer() == BLACK && rankIndex(sqFrom) <= 1)) return false;
            }
            // furthermore double pawn pushes must check the square which is two squares ahead
            // is empty
            if (isDoubledPawnPush(m)){
                // validate that the squares are empty
                if(getPiece(sqFrom + forward)     != -1) return false;
                if(getPiece(sqFrom + forward * 2) != -1) return false;
                if(sqFrom + forward * 2 != sqTo)         return false;
                
                // validate that the pawn is on the 2nd rank
                if((getActivePlayer() == WHITE && rankIndex(sqFrom) != 1) ||
                   (getActivePlayer() == BLACK && rankIndex(sqFrom) != 6)) return false;
            }
            if(isEnPassant(m)){
                if (sqTo != getEnPassantSquare()) return false;
                if (getPiece(sqTo - forward) != bb::getPiece(!getActivePlayer(), PAWN)) return false;
            }
            if(isCapture){
                // only allow diagonal captures for pawns in the forward direction
                if(fileIndex(sqFrom) == 7 && (sqTo == sqFrom + forward + 1)) return false;
                if(fileIndex(sqFrom) == 0 && (sqTo == sqFrom + forward - 1)) return false;
                
                if(    ((sqTo != sqFrom + forward + 1))
                    && ((sqTo != sqFrom + forward - 1))) return false;
            }
            
            if(isPromotion(m)){
                // validate that the pawn is on the 2nd rank
                if((getActivePlayer() == WHITE && rankIndex(sqFrom) != 6) ||
                   (getActivePlayer() == BLACK && rankIndex(sqFrom) != 1)) return false;
                if((getActivePlayer() == WHITE && rankIndex(sqTo  ) != 7) ||
                   (getActivePlayer() == BLACK && rankIndex(sqTo  ) != 0)) return false;
                if(!isCapture && sqTo != sqFrom + forward) return false;
            }else{
                if((getActivePlayer() == WHITE && rankIndex(sqFrom) >= 6) ||
                   (getActivePlayer() == BLACK && rankIndex(sqFrom) <= 1)) return false;
            }
            break;
        case KNIGHT:
            if(~attacks::KNIGHT_ATTACKS[sqFrom] & (ONE << sqTo)) return false;
            break;
        case BISHOP:
            // check its on the same diagonal
            if(    diagonalIndex    (sqFrom) != diagonalIndex    (sqTo)
                && antiDiagonalIndex(sqFrom) != antiDiagonalIndex(sqTo)){
                return false;
            }
            // check there is no piece in between
            if((IN_BETWEEN_SQUARES[sqFrom][sqTo] & m_occupiedBB) != 0) return false;
            break;
        case ROOK:
            // check its on the same file or rank
            if(    fileIndex(sqFrom) != fileIndex(sqTo)
                && rankIndex(sqFrom) != rankIndex(sqTo)){
                return false;
            }
            // check there is no piece in between
            if((IN_BETWEEN_SQUARES[sqFrom][sqTo] & m_occupiedBB) != 0) return false;
            break;
        case QUEEN:
            // check its on the same file or rank
            if(    fileIndex        (sqFrom) != fileIndex        (sqTo)
                && rankIndex        (sqFrom) != rankIndex        (sqTo)
                && diagonalIndex    (sqFrom) != diagonalIndex    (sqTo)
                && antiDiagonalIndex(sqFrom) != antiDiagonalIndex(sqTo)){
                return false;
            }
            // if there is a piece in the way, this move is invalid
            if((IN_BETWEEN_SQUARES[sqFrom][sqTo] & m_occupiedBB) != 0) return false;
            break;
        case KING:
            // we only need to check if the castling move is valid.
            // for this we can look into the meta information
            if(isCastle(m)){
                bool kingSideCastle = sqTo > sqFrom;
                
                if( kingSideCastle && getType(m) == QUEEN_CASTLE) return false;
                if(!kingSideCastle && getType(m) == KING_CASTLE ) return false;
                
                if(sqTo - sqFrom != 2 &&  kingSideCastle) return false;
                if(sqFrom - sqTo != 2 && !kingSideCastle) return false;
                
                U64  canCastleMask  = m_occupiedBB & (activePlayer == WHITE ?
                     (kingSideCastle ? CASTLING_WHITE_KINGSIDE_MASK : CASTLING_WHITE_QUEENSIDE_MASK)
                   : (kingSideCastle ? CASTLING_BLACK_KINGSIDE_MASK : CASTLING_BLACK_QUEENSIDE_MASK));
                if(canCastleMask == 0 && getCastlingRights(activePlayer * 2 + kingSideCastle)){
                    return true;
                }
                // we can exit early as this cannot be a capture
                return false;
            }
            // moves which pass more than 1 square are obviously not possible
            if(~attacks::KING_ATTACKS[sqFrom] & (ONE << sqTo)){
                return false;
            }
            break;
    }
    return true;
}

/**
 * returns true if castling for the given index is possible.
 * For a reference to the indexing, check Board.h
 *
 * @param index
 * @return
 */
bool Board::getCastlingRights(int index) const { return getBit(getBoardStatus()->castlingRights, index); }

/**
 * enables/disables castling.
 * For a reference to the indexing, check Board.h
 */
void Board::setCastlingRights(int index, bool val) {
    if (val) {
        setBit(getBoardStatus()->castlingRights, index);
    } else {
        unsetBit(getBoardStatus()->castlingRights, index);
    }
}

/**
 * this sets the e.p. square to the given square. Negative squares are not allowed.
 * @param square
 */
void Board::setEnPassantSquare(Square square) {
    if (square < 0)
        getBoardStatus()->enPassantTarget = 0;
    else
        getBoardStatus()->enPassantTarget = (ONE << square);
}

/**
 * for each move, we need to compute the current repetition counter.
 * as this is only used internally, there is no need to make this public.
 */
void Board::computeNewRepetition() {
    const int maxChecks = getBoardStatus()->fiftyMoveCounter;
    
    const int lim = m_boardStatusHistory.size() - 1 - maxChecks;
    const int end = std::max(0,lim);

    for (int i = m_boardStatusHistory.size() - 3; i >= end; i -= 2) {
        if (m_boardStatusHistory.at(i).zobrist == getBoardStatus()->zobrist) {
            getBoardStatus()->repetitionCounter = m_boardStatusHistory.at(i).repetitionCounter + 1;
        }
    }
}

/**
 * Returns the amount this position has occurred before.
 * @return
 */
int Board::getCurrentRepetitionCount() const { return getBoardStatus()->repetitionCounter; }

/**
 * returns the 50-move counter which is used for draw detection.
 * @return
 */
int Board::getCurrent50MoveRuleCount() const { return getBoardStatus()->fiftyMoveCounter / 2; }

/*
 * returns the square to which e.p. is possible.
 * if e.p. is not possible, -1 is returned.
 * @return
 */
Square Board::getEnPassantSquare() const {
    const U64 ePT = getBoardStatus()->enPassantTarget;
    if (ePT == 0) {
        return -1;
    }
    const int pos = bitscanForward(ePT);
    if (pos == 64 || pos < 0) {
        return -1;
    }
    return pos;
}

/**
 * returns the active player which has to do the next move.
 * @return
 */
Color Board::getActivePlayer() const { return m_activePlayer; }

/**
 * writes the board object to the given stream
 */
std::ostream& operator<<(std::ostream& os, Board& board) {
    os << "zobrist key              " << board.zobrist() << "\n";
    os << "repetition               " << board.getCurrentRepetitionCount() << "\n";
    os << "50 move rule             " << board.getCurrent50MoveRuleCount() << "\n";
    os << "white kingside castle    " << (board.getCastlingRights(1) ? "true" : "false") << "\n";
    os << "white queenside castle   " << (board.getCastlingRights(0) ? "true" : "false") << "\n";
    os << "black kingside castle    " << (board.getCastlingRights(3) ? "true" : "false") << "\n";
    os << "black queenside castle   " << (board.getCastlingRights(2) ? "true" : "false") << "\n";
    os << "en passent square        "
       << (board.getEnPassantSquare() >= 0 ? SQUARE_IDENTIFIER[board.getEnPassantSquare()] : "-") << "\n";
    
    os << " +---+---+---+---+---+---+---+---+\n";
    
    for (Rank r = 7; r >= 0; r--) {
        for (File f = 0; f <= 7; ++f) {
            // os << squareIndex(r,f);
            const Square sq = bb::squareIndex(r, f);
            if (board.getPiece(sq) >= 0) {
                os << " | " << PIECE_IDENTIFIER[board.getPiece(bb::squareIndex(r, f))];
            } else {
                os << " |  ";
            }
        }
        
        os << " |\n +---+---+---+---+---+---+---+---+\n";
    }
    os << "fen: " << board.fen() << std::endl;
    return os;
}

/**
 * returns a bitboard of all pinned pieces of the given color.
 * It furthermore writes the pinning pieces into the given pinners bitboard.
 * @param color
 * @param pinners
 * @return
 */
template<Color side> U64 Board::getPinnedPieces(U64& pinners) const {
    U64 pinned = 0;
    
    const Square kingSq = bitscanForward(getPieceBB(side, KING));
    
    constexpr Color them = side ^ 1;
    
    U64 pinner = attacks::lookUpRookXRayAttack(kingSq, m_occupiedBB, m_teamOccupiedBB[side])
                 & (getPieceBB(them, ROOK) | getPieceBB(them, QUEEN));
    pinners |= pinner;
    while (pinner) {
        Square s = bitscanForward(pinner);
        pinned |= IN_BETWEEN_SQUARES[kingSq][s] & m_teamOccupiedBB[side];
        pinner = lsbReset(pinner);
    }
    
    pinner = attacks::lookUpBishopXRayAttack(kingSq, m_occupiedBB, m_teamOccupiedBB[side])
             & (getPieceBB(them, BISHOP) | getPieceBB(them, QUEEN));
    pinners |= pinner;
    
    while (pinner) {
        Square s = bitscanForward(pinner);
        pinned |= IN_BETWEEN_SQUARES[kingSq][s] & m_teamOccupiedBB[side];
        pinner = lsbReset(pinner);
    }
    return pinned;
}

Score Board::evaluate(){
    constexpr float evaluation_mg_scalar = 1.5;
    constexpr float evaluation_eg_scalar = 1.15;

    constexpr float phase_sum = 39.6684;
    float phase = (phase_sum
        - phaseValues[PAWN  ] * bitCount(getPieceBB()[WHITE_PAWN  ] | getPieceBB()[BLACK_PAWN  ])
        - phaseValues[KNIGHT] * bitCount(getPieceBB()[WHITE_KNIGHT] | getPieceBB()[BLACK_KNIGHT])
        - phaseValues[BISHOP] * bitCount(getPieceBB()[WHITE_BISHOP] | getPieceBB()[BLACK_BISHOP])
        - phaseValues[ROOK  ] * bitCount(getPieceBB()[WHITE_ROOK  ] | getPieceBB()[BLACK_ROOK  ])
        - phaseValues[QUEEN ] * bitCount(getPieceBB()[WHITE_QUEEN ] | getPieceBB()[BLACK_QUEEN ]))
                / phase_sum;
    return (+     evaluation_mg_scalar
                - phase * (evaluation_mg_scalar - evaluation_eg_scalar))
            * (this->evaluator.evaluate(this->getActivePlayer()));
}

template void Board::setPiece<true, true>(Square sq, Piece piece);
template void Board::setPiece<true, false>(Square sq, Piece piece);
template void Board::setPiece<false, true>(Square sq, Piece piece);
template void Board::setPiece<false, false>(Square sq, Piece piece);

template void Board::unsetPiece<true, true>(Square sq);
template void Board::unsetPiece<true, false>(Square sq);
template void Board::unsetPiece<false, true>(Square sq);
template void Board::unsetPiece<false, false>(Square sq);

template void Board::replacePiece<true, true>(Square sq, Piece piece);
template void Board::replacePiece<true, false>(Square sq, Piece piece);
template void Board::replacePiece<false, true>(Square sq, Piece piece);
template void Board::replacePiece<false, false>(Square sq, Piece piece);
