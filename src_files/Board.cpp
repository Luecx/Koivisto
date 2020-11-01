
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                           by. Kim Kahre, Finn Eggers and Eugenio Bruno                           *
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

#include "Board.h"

using namespace bb;

/**
 * The default constructor uses a fen-representation of the board. if nothing is specified, the starting position
 * will be used. This might crash if the given fen is illegal in its structure. e.g. not all rows/columns specified.
 * @param fen
 */
Board::Board(std::string fen) {

    // first we set all piece occupancies to zero.
    for (int i = 0; i < 12; i++) {
        m_pieces[i] = 0;
    }

    // also unset the occupancy for both teams
    m_teamOccupied[WHITE] = 0;
    m_teamOccupied[BLACK] = 0;

    // set the total occupancy to zero
    m_occupied = 0;

    // assume white is the active player
    m_activePlayer = WHITE;

    // we use default values of -1 for the piece table for each square if no piece is on the square
    for (int i = 0; i < 64; i++) {
        m_pieceBoard[i] = -1;
    }

    // we need to push a default board status.
    BoardStatus boardStatus {0, 0, 0, 0, ONE, ONE, 0};
    this->m_boardStatusHistory.push_back(boardStatus);

    // using some string utilties defined in Util.h, we split the fen into parts.
    std::vector<std::string> split {};
    std::string              str {fen};
    str = trim(str);
    findAndReplaceAll(str, "  ", " ");
    splitString(str, split, ' ');

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

            int offset = (c >= 'a') ? 6 : 0;

            Square sq = squareIndex(y, x);

            switch (toupper(c)) {
                case 'P': setPiece(sq, 0 + offset); break;
                case 'N': setPiece(sq, 1 + offset); break;
                case 'B': setPiece(sq, 2 + offset); break;
                case 'R': setPiece(sq, 3 + offset); break;
                case 'Q': setPiece(sq, 4 + offset); break;
                case 'K': setPiece(sq, 5 + offset); break;
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
            setCastlingChance(i, false);
        }

        for (char c : split[2]) {
            switch (c) {
                case 'K':
                    if (getPiece(E1) == WHITE_KING)
                        setCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING, true);
                    break;
                case 'Q':
                    if (getPiece(E1) == WHITE_KING)
                        setCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING, true);
                    break;
                case 'k':
                    if (getPiece(E8) == BLACK_KING)
                        setCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING, true);
                    break;
                case 'q':
                    if (getPiece(E8) == BLACK_KING)
                        setCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING, true);
                    break;
            }
        }
    }

    // the last thing we consider is e.p. square.
    if (split.size() >= 4) {
        if (split[3].at(0) != '-') {
            Square square = squareIndex(split[3]);
            setEnPassantSquare(square);
        }
    }

    // note that we do not read information about move counts. This is usually not required for playing games.
}

/**
 * Beside using the FEN for a position, one can also copy directly for another board object.
 * Copies the entire history as well as all relevant fields.
 * @param board
 */
Board::Board(Board* board) {

    // we need to copy occupancy bitboards for the teams
    m_teamOccupied[WHITE] = board->getTeamOccupied()[WHITE];
    m_teamOccupied[BLACK] = board->getTeamOccupied()[BLACK];

    // we need to copy occupancy bitboards for each piece
    for (int n = 0; n < 12; n++) {
        m_pieces[n] = board->m_pieces[n];
    }

    // we need to copy occupancy bitboards for all pieces
    m_occupied = *board->getOccupied();

    // we also need to copy the active player
    m_activePlayer = board->getActivePlayer();

    // copying the piece board for each square
    for (int i = 0; i < 64; i++) {
        m_pieceBoard[i] = board->m_pieceBoard[i];
    }

    // next we copy the entire history of the board.
    for (int n = 0; n < static_cast<int>(board->m_boardStatusHistory.size()); n++) {
        m_boardStatusHistory.push_back(board->m_boardStatusHistory.at(n).copy());
    }
}

/**
 * For the sake of completeness, we provide a destructor which might be filled in the future.
 */
Board::~Board() {}

/**
 * Returns a FEN-representation of the board object which can be used for other engines, and debugging.
 */
std::string Board::fen() {

    std::stringstream ss;

    // we do it in the same way we read a fen.
    // first, we write the pieces
    for (Rank n = 7; n >= 0; n--) {
        int counting = 0;
        for (File i = 0; i < 8; i++) {
            Square s = squareIndex(n, i);

            int piece = getPiece(s);
            if (piece == -1) {
                counting++;
            } else {
                if (counting != 0) {
                    ss << counting;
                }
                counting = 0;
                ss << PIECE_IDENTIFER[piece];
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
    if (getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)) {
        anyCastling = true;
        ss << "Q";
    }
    if (getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)) {
        anyCastling = true;
        ss << "K";
    }
    if (getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)) {
        anyCastling = true;
        ss << "q";
    }
    if (getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING)) {
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
U64 Board::zobrist() { return getBoardStatus()->zobrist; }

/**
 * Returns true if the given player is in check by the opponent.
 */
bool Board::isInCheck(Color player) {
    // we do this by casting rays from the king position and checking if an opponent piece is on those rays
    // which could attack the given ray.
    if (player == WHITE) {
        return isUnderAttack(bitscanForward(m_pieces[WHITE_KING]), BLACK);
    } else {
        return isUnderAttack(bitscanForward(m_pieces[BLACK_KING]), WHITE);
    }
}

/**
 * Returns true if the board is a draw by either the 50-move rule or a threefold repetition.
 * Stalemates are not considering here as they require dynamic knowledge about the position
 * which is only given during the search.
 * @return
 */
bool Board::isDraw() { return getCurrent50MoveRuleCount() >= 50 || getCurrentRepetitionCount() >= 2; }

/**
 * returns the piece which occupies the board at the given index
 * @param sq
 * @return
 */
Piece Board::getPiece(Square sq) { return m_pieceBoard[sq]; }

/**
 * Sets the piece on the given square.
 * Deals with zobrist-keys.
 * @param sq
 * @param piece
 */
void Board::setPiece(Square sq, Piece piece) {

    // first we set the piece on the piece board
    m_pieceBoard[sq] = piece;

    // we need the square as a bitboard for the occupancy bitboards
    U64 sqBB = (ONE << sq);

    // settings the occupancy for the team, the piece and the total occupancy.
    m_pieces[piece] |= sqBB;
    m_teamOccupied[piece / 6] |= sqBB;
    m_occupied |= sqBB;

    // also adjust the zobrist key
    BoardStatus* st = getBoardStatus();
    st->zobrist ^= getHash(piece, sq);
}

/**
 * Unsets the piece on the given square.
 * Deals with zobrist-keys.
 * @param sq
 */
void Board::unsetPiece(Square sq) {

    // we need to know first which piece is contained on the given square.
    Piece p = getPiece(sq);

    // similar to setPiece() we need the square as a bitboard for upccancy bitboards.
    // as we need to remove bits from the occupancy bitboards, we use the inverse.
    U64 sqBB = ~(ONE << sq);

    // actually removing bits from the occupancy bitboards.
    m_pieces[p] &= sqBB;
    m_teamOccupied[p / 6] &= sqBB;
    m_occupied &= sqBB;

    // also adjust the zobrist key
    BoardStatus* st = getBoardStatus();
    st->zobrist ^= getHash(p, sq);

    // removing the piece from the square-wise piece table.
    m_pieceBoard[sq] = -1;
}

/**
 * Replaces the piece on the given square with the given new piece.
 * Deals with zobrist-keys.
 * @param sq
 * @param piece
 */
void Board::replacePiece(Square sq, Piece piece) {
    // we need to know first which piece will be replaced on the given square.
    Piece p = getPiece(sq);

    // similar to setPiece() we need the square as a bitboard for upccancy bitboards.
    U64 sqBB = (ONE << sq);

    m_pieces[p] &= ~sqBB;                 // unset
    m_pieces[piece] |= sqBB;              // set
    m_teamOccupied[p / 6] &= ~sqBB;       // unset
    m_teamOccupied[piece / 6] |= sqBB;    // set

    // also adjust the zobrist key
    BoardStatus* st = getBoardStatus();
    st->zobrist ^= (getHash(p, sq) ^ getHash(piece, sq));

    // removing the piece from the square-wise piece table.
    m_pieceBoard[sq] = piece;
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
void Board::move(Move m) {
    BoardStatus* previousStatus = getBoardStatus();
    BoardStatus  newBoardStatus = {previousStatus->zobrist,           // zobrist will be changed later
                                  0ULL,                              // reset en passant. might be set later
                                  previousStatus->castlingRights,    // copy meta. might be changed
                                  previousStatus->fiftyMoveCounter
                                      + 1,    // increment fifty move counter. might be reset
                                  1ULL,       // set rep to 1 (no rep)
                                  previousStatus->moveCounter + getActivePlayer(),    // increment move counter
                                  m};

    Square sqFrom = getSquareFrom(m);
    Square sqTo   = getSquareTo(m);
    Piece  pFrom  = getMovingPiece(m);
    Type   mType  = getType(m);
    Color  color  = getActivePlayer();
    int    factor = getActivePlayer() == 0 ? 1 : -1;

    if (isCapture(m)) {
        // reset fifty move counter if a piece has been captured
        newBoardStatus.fiftyMoveCounter = 0;

        if (getPiece(sqTo) % 6 == ROOK) {
            if (color == BLACK) {
                if (sqTo == A1) {
                    newBoardStatus.castlingRights &= ~(ONE << (STATUS_INDEX_WHITE_QUEENSIDE_CASTLING));
                } else if (sqTo == H1) {
                    newBoardStatus.castlingRights &= ~(ONE << (STATUS_INDEX_WHITE_KINGSIDE_CASTLING));
                }
            } else {
                if (sqTo == A8) {
                    newBoardStatus.castlingRights &= ~(ONE << (STATUS_INDEX_BLACK_QUEENSIDE_CASTLING));
                } else if (sqTo == H8) {
                    newBoardStatus.castlingRights &= ~(ONE << (STATUS_INDEX_BLACK_KINGSIDE_CASTLING));
                }
            }
        }
    }

    newBoardStatus.zobrist ^= ZOBRIST_WHITE_BLACK_SWAP;
    if (pFrom % 6 == PAWN) {

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

            // setting m_pieces
            this->unsetPiece(sqFrom);

            // do the "basic" move
            if (isCapture(m)) {
                this->replacePiece(sqTo, promotionPiece(m));
            } else {
                this->setPiece(sqTo, promotionPiece(m));
            }

            return;
        } else if (mType == EN_PASSANT) {

            m_boardStatusHistory.emplace_back(std::move(newBoardStatus));
            this->changeActivePlayer();

            // make sure to capture the pawn
            unsetPiece(sqTo - 8 * factor);

            // move to pawn
            this->unsetPiece(sqFrom);
            this->setPiece(sqTo, pFrom);

            return;
        }
    } else if (pFrom % 6 == KING) {

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
            unsetPiece(rookSquare);
            setPiece(rookTarget, ROOK + 6 * color);
        }

        this->unsetPiece(sqFrom);
        if (isCapture(m)) {
            this->replacePiece(sqTo, pFrom);
        } else {
            this->setPiece(sqTo, pFrom);
        }

        // we need to compute the repetition count
        this->computeNewRepetition();

        return;

    }

    // revoke castling rights if rook moves and it is on the initial square
    else if (pFrom % 6 == ROOK) {
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
    this->unsetPiece(sqFrom);

    if (mType != EN_PASSANT && isCapture(m)) {
        this->replacePiece(sqTo, pFrom);
    } else {
        this->setPiece(sqTo, pFrom);
    }

    this->changeActivePlayer();
    this->computeNewRepetition();
}

/**
 * undoes the last move. Assumes the last move has not been a null move.
 */
void Board::undoMove() {

    Move m = getBoardStatus()->move;

    changeActivePlayer();

    Square sqFrom   = getSquareFrom(m);
    Square sqTo     = getSquareTo(m);
    Piece  pFrom    = getMovingPiece(m);
    Type   mType    = getType(m);
    Piece  captured = getCapturedPiece(m);
    bool   isCap    = isCapture(m);
    Color  color    = getActivePlayer();
    int    factor   = getActivePlayer() == 0 ? 1 : -1;

    if (mType == EN_PASSANT) {
        setPiece(sqTo - 8 * factor, (1 - color) * 6);
    }

    if (pFrom % 6 == KING && isCastle(m)) {
        Square rookSquare = sqFrom + (mType == QUEEN_CASTLE ? -4 : 3);
        Square rookTarget = sqTo + (mType == QUEEN_CASTLE ? 1 : -1);
        setPiece(rookSquare, ROOK + 6 * color);
        unsetPiece(rookTarget);
    }

    if (mType != EN_PASSANT && isCap) {
        replacePiece(sqTo, captured);
    } else {
        unsetPiece(sqTo);
    }

    setPiece(sqFrom, pFrom);

    m_boardStatusHistory.pop_back();
}

/**
 * does a null move.
 */
void Board::move_null() {
    BoardStatus* previousStatus = getBoardStatus();
    BoardStatus  newBoardStatus = {
        previousStatus->zobrist ^ ZOBRIST_WHITE_BLACK_SWAP,
        0ULL,
        previousStatus->castlingRights,
        previousStatus->fiftyMoveCounter + 1,
        1ULL,
        previousStatus->moveCounter + getActivePlayer(),
        0ULL,
    };

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
 * returns all moves. This is pseudo legal so they need to be checked for legality.
 * @param moves
 */
void Board::getPseudoLegalMoves(MoveList* moves) {

    U64 attackableSquares;
    U64 opponents;

    int pieceOffset = 6 * getActivePlayer();

    moves->clear();

    U64 knights;
    U64 bishops;
    U64 rooks;
    U64 queens;
    U64 kings;

    Square s;
    Square target;
    U64    attacks;

    if (getActivePlayer() == WHITE) {

        attackableSquares = ~m_teamOccupied[WHITE];
        opponents         = m_teamOccupied[BLACK];

        knights = m_pieces[WHITE_KNIGHT];
        bishops = m_pieces[WHITE_BISHOP];
        rooks   = m_pieces[WHITE_ROOK];
        queens  = m_pieces[WHITE_QUEEN];
        kings   = m_pieces[WHITE_KING];

        U64 pawnsLeft   = shiftNorthWest(m_pieces[WHITE_PAWN] & ~FILE_A);
        U64 pawnsRight  = shiftNorthEast(m_pieces[WHITE_PAWN] & ~FILE_H);
        U64 pawnsCenter = shiftNorth(m_pieces[WHITE_PAWN]) & ~m_occupied;

        attacks = pawnsLeft & opponents & ~RANK_8;

        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(target - 7, target, CAPTURE, WHITE_PAWN, getPiece(target)));
            attacks = lsbReset(attacks);
        }

        attacks = pawnsRight & opponents & ~RANK_8;
        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(target - 9, target, CAPTURE, WHITE_PAWN, getPiece(target)));
            attacks = lsbReset(attacks);
        }

        U64 h   = pawnsCenter & ~RANK_8;
        attacks = h;
        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(target - 8, target, QUIET, WHITE_PAWN));
            attacks = lsbReset(attacks);
        }

        attacks = shiftNorth(h) & RANK_4 & ~m_occupied;
        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(target - 16, target, DOUBLED_PAWN_PUSH, WHITE_PAWN));
            attacks = lsbReset(attacks);
        }

        if (pawnsLeft & getBoardStatus()->enPassantTarget) {
            target = getEnPassantSquare();
            moves->add(genMove(target - 7, target, EN_PASSANT, WHITE_PAWN));
            attacks = lsbReset(attacks);
        }

        if (pawnsRight & getBoardStatus()->enPassantTarget) {
            target = getEnPassantSquare();
            moves->add(genMove(target - 9, target, EN_PASSANT, WHITE_PAWN));
            attacks = lsbReset(attacks);
        }

        if (m_pieces[WHITE_PAWN] & RANK_7) {
            attacks = pawnsCenter & RANK_8;
            while (attacks) {
                target = bitscanForward(attacks);
                moves->add(genMove(target - 8, target, QUEEN_PROMOTION, WHITE_PAWN));
                moves->add(genMove(target - 8, target, ROOK_PROMOTION, WHITE_PAWN));
                moves->add(genMove(target - 8, target, BISHOP_PROMOTION, WHITE_PAWN));
                moves->add(genMove(target - 8, target, KNIGHT_PROMOTION, WHITE_PAWN));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsLeft & RANK_8 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                moves->add(genMove(target - 7, target, QUEEN_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                moves->add(genMove(target - 7, target, ROOK_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                moves->add(genMove(target - 7, target, BISHOP_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                moves->add(genMove(target - 7, target, KNIGHT_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsRight & RANK_8 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                moves->add(genMove(target - 9, target, QUEEN_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                moves->add(genMove(target - 9, target, ROOK_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                moves->add(genMove(target - 9, target, BISHOP_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                moves->add(genMove(target - 9, target, KNIGHT_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                attacks = lsbReset(attacks);
            }
        }

    } else {

        attackableSquares = ~m_teamOccupied[BLACK];
        opponents         = m_teamOccupied[WHITE];

        knights = m_pieces[BLACK_KNIGHT];
        bishops = m_pieces[BLACK_BISHOP];
        rooks   = m_pieces[BLACK_ROOK];
        queens  = m_pieces[BLACK_QUEEN];
        kings   = m_pieces[BLACK_KING];

        U64 pawnsLeft   = shiftSouthWest(m_pieces[BLACK_PAWN] & ~FILE_A);
        U64 pawnsRight  = shiftSouthEast(m_pieces[BLACK_PAWN] & ~FILE_H);
        U64 pawnsCenter = shiftSouth(m_pieces[BLACK_PAWN]) & ~m_occupied;

        attacks = pawnsLeft & opponents & ~RANK_1;
        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(target + 9, target, CAPTURE, BLACK_PAWN, getPiece(target)));
            attacks = lsbReset(attacks);
        }

        attacks = pawnsRight & opponents & ~RANK_1;
        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(target + 7, target, CAPTURE, BLACK_PAWN, getPiece(target)));
            attacks = lsbReset(attacks);
        }

        U64 h   = pawnsCenter & ~RANK_1;
        attacks = h;
        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(target + 8, target, QUIET, BLACK_PAWN));
            attacks = lsbReset(attacks);
        }

        attacks = shiftSouth(h) & RANK_5 & ~m_occupied;
        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(target + 16, target, DOUBLED_PAWN_PUSH, BLACK_PAWN));
            attacks = lsbReset(attacks);
        }

        if (pawnsLeft & getBoardStatus()->enPassantTarget) {
            target = getEnPassantSquare();
            moves->add(genMove(target + 9, target, EN_PASSANT, BLACK_PAWN));
        }

        if (pawnsRight & getBoardStatus()->enPassantTarget) {
            target = getEnPassantSquare();
            moves->add(genMove(target + 7, target, EN_PASSANT, BLACK_PAWN));
        }

        if (m_pieces[BLACK_PAWN] & RANK_2) {
            attacks = pawnsCenter & RANK_1;
            while (attacks) {
                target = bitscanForward(attacks);
                moves->add(genMove(target + 8, target, QUEEN_PROMOTION, BLACK_PAWN));
                moves->add(genMove(target + 8, target, ROOK_PROMOTION, BLACK_PAWN));
                moves->add(genMove(target + 8, target, BISHOP_PROMOTION, BLACK_PAWN));
                moves->add(genMove(target + 8, target, KNIGHT_PROMOTION, BLACK_PAWN));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsLeft & RANK_1 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                moves->add(genMove(target + 9, target, QUEEN_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                moves->add(genMove(target + 9, target, ROOK_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                moves->add(genMove(target + 9, target, BISHOP_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                moves->add(genMove(target + 9, target, KNIGHT_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsRight & RANK_1 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                moves->add(genMove(target + 7, target, QUEEN_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                moves->add(genMove(target + 7, target, ROOK_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                moves->add(genMove(target + 7, target, BISHOP_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                moves->add(genMove(target + 7, target, KNIGHT_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                attacks = lsbReset(attacks);
            }
        }
    }

    while (knights) {
        s       = bitscanForward(knights);
        attacks = KNIGHT_ATTACKS[s] & attackableSquares;
        while (attacks) {
            target = bitscanForward(attacks);

            if (getPiece(target) >= 0) {
                moves->add(genMove(s, target, CAPTURE, KNIGHT + pieceOffset, getPiece(target)));
            } else {
                moves->add(genMove(s, target, QUIET, KNIGHT + pieceOffset));
            }

            attacks = lsbReset(attacks);
        }
        knights = lsbReset(knights);
    }
    while (bishops) {
        s       = bitscanForward(bishops);
        attacks = lookUpBishopAttack(s, m_occupied) & attackableSquares;
        while (attacks) {
            target = bitscanForward(attacks);
            if (getPiece(target) >= 0) {
                moves->add(genMove(s, target, CAPTURE, BISHOP + pieceOffset, getPiece(target)));
            } else {
                moves->add(genMove(s, target, QUIET, BISHOP + pieceOffset));
            }
            attacks = lsbReset(attacks);
        }
        bishops = lsbReset(bishops);
    }

    while (rooks) {
        s       = bitscanForward(rooks);
        attacks = lookUpRookAttack(s, m_occupied) & attackableSquares;

        while (attacks) {
            target = bitscanForward(attacks);

            if (getPiece(target) >= 0) {
                moves->add(genMove(s, target, CAPTURE, ROOK + pieceOffset, getPiece(target)));
            } else {
                moves->add(genMove(s, target, QUIET, ROOK + pieceOffset));
            }
            attacks = lsbReset(attacks);
        }
        rooks = lsbReset(rooks);
    }

    while (queens) {
        s = bitscanForward(queens);

        attacks = (lookUpRookAttack(s, m_occupied) | lookUpBishopAttack(s, m_occupied)) & attackableSquares;

        while (attacks) {
            target = bitscanForward(attacks);

            if (getPiece(target) >= 0) {
                // genMove(s, target, CAPTURE, QUEEN + pieceOffset, getPiece(target));
                moves->add(genMove(s, target, CAPTURE, QUEEN + pieceOffset, getPiece(target)));
            } else {
                // genMove(s, target, QUIET, QUEEN + pieceOffset);
                moves->add(genMove(s, target, QUIET, QUEEN + pieceOffset));
            }
            attacks &= (attacks - 1);
        }

        queens &= (queens - 1);
    }

    while (kings) {
        s       = bitscanForward(kings);
        attacks = KING_ATTACKS[s] & attackableSquares;
        while (attacks) {
            target = bitscanForward(attacks);

            if (getPiece(target) >= 0) {
                moves->add(genMove(s, target, CAPTURE, KING + pieceOffset, getPiece(target)));
            } else {
                moves->add(genMove(s, target, QUIET, KING + pieceOffset));
            }

            attacks = lsbReset(attacks);
        }

        if (getActivePlayer() == WHITE) {

            if (getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING) && getPiece(A1) == WHITE_ROOK
                && (m_occupied & CASTLING_WHITE_QUEENSIDE_MASK) == 0) {
                moves->add(genMove(E1, C1, QUEEN_CASTLE, WHITE_KING));
            }
            if (getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING) && getPiece(H1) == WHITE_ROOK
                && (m_occupied & CASTLING_WHITE_KINGSIDE_MASK) == 0) {
                moves->add(genMove(E1, G1, KING_CASTLE, WHITE_KING));
            }

        } else {

            if (getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING) && getPiece(A8) == BLACK_ROOK
                && (m_occupied & CASTLING_BLACK_QUEENSIDE_MASK) == 0) {

                moves->add(genMove(E8, C8, QUEEN_CASTLE, BLACK_KING));
            }
            if (getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING) && getPiece(H8) == BLACK_ROOK
                && (m_occupied & CASTLING_BLACK_KINGSIDE_MASK) == 0) {
                moves->add(genMove(E8, G8, KING_CASTLE, BLACK_KING));
            }
        }
        kings = lsbReset(kings);
    }
}

/**
 * returns all non quiet-moves. This is pseudo legal so they need to be checked for legality.
 * This contains captures and promotions. We do not consider moves which give checks here.
 */
void Board::getNonQuietMoves(MoveList* moves) {
    U64 opponents;

    int pieceOffset = 6 * getActivePlayer();

    moves->clear();

    U64 knights;
    U64 bishops;
    U64 rooks;
    U64 queens;
    U64 kings;

    Square s;
    Square target;
    U64    attacks;

    if (getActivePlayer() == WHITE) {

        opponents = m_teamOccupied[BLACK];

        knights = m_pieces[WHITE_KNIGHT];
        bishops = m_pieces[WHITE_BISHOP];
        rooks   = m_pieces[WHITE_ROOK];
        queens  = m_pieces[WHITE_QUEEN];
        kings   = m_pieces[WHITE_KING];

        U64 pawnsLeft   = shiftNorthWest(m_pieces[WHITE_PAWN] & ~FILE_A);
        U64 pawnsRight  = shiftNorthEast(m_pieces[WHITE_PAWN] & ~FILE_H);
        U64 pawnsCenter = shiftNorth(m_pieces[WHITE_PAWN]) & ~m_occupied;

        attacks = pawnsLeft & opponents & ~RANK_8;
        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(target - 7, target, CAPTURE, WHITE_PAWN, getPiece(target)));
            attacks = lsbReset(attacks);
        }

        attacks = pawnsRight & opponents & ~RANK_8;
        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(target - 9, target, CAPTURE, WHITE_PAWN, getPiece(target)));
            attacks = lsbReset(attacks);
        }

        if (pawnsLeft & getBoardStatus()->enPassantTarget) {
            target = getEnPassantSquare();
            moves->add(genMove(target - 7, target, EN_PASSANT, WHITE_PAWN));
            attacks = lsbReset(attacks);
        }

        if (pawnsRight & getBoardStatus()->enPassantTarget) {
            target = getEnPassantSquare();
            moves->add(genMove(target - 9, target, EN_PASSANT, WHITE_PAWN));
            attacks = lsbReset(attacks);
        }

        if (m_pieces[WHITE_PAWN] & RANK_7) {
            attacks = pawnsCenter & RANK_8;
            while (attacks) {
                target = bitscanForward(attacks);
                moves->add(genMove(target - 8, target, QUEEN_PROMOTION, WHITE_PAWN));
                moves->add(genMove(target - 8, target, ROOK_PROMOTION, WHITE_PAWN));
                moves->add(genMove(target - 8, target, BISHOP_PROMOTION, WHITE_PAWN));
                moves->add(genMove(target - 8, target, KNIGHT_PROMOTION, WHITE_PAWN));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsLeft & RANK_8 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                moves->add(genMove(target - 7, target, QUEEN_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                moves->add(genMove(target - 7, target, ROOK_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                moves->add(genMove(target - 7, target, BISHOP_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                moves->add(genMove(target - 7, target, KNIGHT_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsRight & RANK_8 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                moves->add(genMove(target - 9, target, QUEEN_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                moves->add(genMove(target - 9, target, ROOK_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                moves->add(genMove(target - 9, target, BISHOP_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                moves->add(genMove(target - 9, target, KNIGHT_PROMOTION_CAPTURE, WHITE_PAWN, getPiece(target)));
                attacks = lsbReset(attacks);
            }
        }

    } else {

        opponents = m_teamOccupied[WHITE];

        knights = m_pieces[BLACK_KNIGHT];
        bishops = m_pieces[BLACK_BISHOP];
        rooks   = m_pieces[BLACK_ROOK];
        queens  = m_pieces[BLACK_QUEEN];
        kings   = m_pieces[BLACK_KING];

        U64 pawnsLeft   = shiftSouthWest(m_pieces[BLACK_PAWN] & ~FILE_A);
        U64 pawnsRight  = shiftSouthEast(m_pieces[BLACK_PAWN] & ~FILE_H);
        U64 pawnsCenter = shiftSouth(m_pieces[BLACK_PAWN]) & ~m_occupied;

        attacks = pawnsLeft & opponents & ~RANK_1;
        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(target + 9, target, CAPTURE, BLACK_PAWN, getPiece(target)));
            attacks = lsbReset(attacks);
        }

        attacks = pawnsRight & opponents & ~RANK_1;
        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(target + 7, target, CAPTURE, BLACK_PAWN, getPiece(target)));
            attacks = lsbReset(attacks);
        }

        if (pawnsLeft & getBoardStatus()->enPassantTarget) {
            target = getEnPassantSquare();
            moves->add(genMove(target + 9, target, EN_PASSANT, BLACK_PAWN));
        }

        if (pawnsRight & getBoardStatus()->enPassantTarget) {
            target = getEnPassantSquare();
            moves->add(genMove(target + 7, target, EN_PASSANT, BLACK_PAWN));
        }

        if (m_pieces[BLACK_PAWN] & RANK_2) {
            attacks = pawnsCenter & RANK_1;
            while (attacks) {
                target = bitscanForward(attacks);
                moves->add(genMove(target + 8, target, QUEEN_PROMOTION, BLACK_PAWN));
                moves->add(genMove(target + 8, target, ROOK_PROMOTION, BLACK_PAWN));
                moves->add(genMove(target + 8, target, BISHOP_PROMOTION, BLACK_PAWN));
                moves->add(genMove(target + 8, target, KNIGHT_PROMOTION, BLACK_PAWN));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsLeft & RANK_1 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                moves->add(genMove(target + 9, target, QUEEN_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                moves->add(genMove(target + 9, target, ROOK_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                moves->add(genMove(target + 9, target, BISHOP_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                moves->add(genMove(target + 9, target, KNIGHT_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsRight & RANK_1 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                moves->add(genMove(target + 7, target, QUEEN_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                moves->add(genMove(target + 7, target, ROOK_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                moves->add(genMove(target + 7, target, BISHOP_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                moves->add(genMove(target + 7, target, KNIGHT_PROMOTION_CAPTURE, BLACK_PAWN, getPiece(target)));
                attacks = lsbReset(attacks);
            }
        }
    }

    while (knights) {
        s       = bitscanForward(knights);
        attacks = KNIGHT_ATTACKS[s] & opponents;
        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(s, target, CAPTURE, KNIGHT + pieceOffset, getPiece(target)));
            attacks = lsbReset(attacks);
        }
        knights = lsbReset(knights);
    }
    while (bishops) {
        s       = bitscanForward(bishops);
        attacks = lookUpBishopAttack(s, m_occupied) & opponents;
        while (attacks) {
            target = bitscanForward(attacks);
            if (getPiece(target) >= 0) {
                moves->add(genMove(s, target, CAPTURE, BISHOP + pieceOffset, getPiece(target)));
            } else {
                moves->add(genMove(s, target, QUIET, BISHOP + pieceOffset));
            }
            attacks = lsbReset(attacks);
        }
        bishops = lsbReset(bishops);
    }
    while (rooks) {
        s       = bitscanForward(rooks);
        attacks = lookUpRookAttack(s, m_occupied) & opponents;

        while (attacks) {
            target = bitscanForward(attacks);

            moves->add(genMove(s, target, CAPTURE, ROOK + pieceOffset, getPiece(target)));

            attacks = lsbReset(attacks);
        }
        rooks = lsbReset(rooks);
    }
    while (queens) {
        s = bitscanForward(queens);

        attacks = (lookUpRookAttack(s, m_occupied) | lookUpBishopAttack(s, m_occupied)) & opponents;

        while (attacks) {
            target = bitscanForward(attacks);

            moves->add(genMove(s, target, CAPTURE, QUEEN + pieceOffset, getPiece(target)));

            attacks &= (attacks - 1);
        }

        queens &= (queens - 1);
    }
    while (kings) {
        s       = bitscanForward(kings);
        attacks = KING_ATTACKS[s] & opponents;
        while (attacks) {
            target = bitscanForward(attacks);

            moves->add(genMove(s, target, CAPTURE, KING + pieceOffset, getPiece(target)));

            attacks = lsbReset(attacks);
        }

        kings = lsbReset(kings);
    }
}

/**
 * returns the Move which lead to the current position.
 * @return
 */
Move Board::getPreviousMove() {
    if (m_boardStatusHistory.empty())
        return 0;
    return m_boardStatusHistory.back().move;
}

/**
 * returns a bitboard of all the attacked squares by the given color.
 * this does not check for en passant captures.
 * @param attacker
 * @return
 */
U64 Board::getAttackedSquares(Color attacker) {

    U64 attacks = ZERO;

    if (attacker == WHITE) {
        attacks |= shiftNorthEast(m_pieces[WHITE_PAWN]) | shiftNorthWest((m_pieces[WHITE_PAWN]));
    } else {
        attacks |= shiftSouthEast(m_pieces[BLACK_PAWN]) | shiftSouthWest((m_pieces[BLACK_PAWN]));
    }

    U64 knights(m_pieces[KNIGHT + 6 * attacker]);
    U64 bishops(m_pieces[BISHOP + 6 * attacker]);
    U64 rooks(m_pieces[ROOK + 6 * attacker]);
    U64 queens(m_pieces[QUEEN + 6 * attacker]);
    U64 kings(m_pieces[KING + 6 * attacker]);

    while (knights) {
        Square s = bitscanForward(knights);
        attacks |= KNIGHT_ATTACKS[s];
        knights = lsbReset(knights);
    }
    while (bishops) {
        Square s = bitscanForward(bishops);
        attacks |= lookUpBishopAttack(s, m_occupied);
        bishops = lsbReset(bishops);
    }
    while (rooks) {
        Square s = bitscanForward(rooks);
        attacks |= lookUpRookAttack(s, m_occupied);
        rooks = lsbReset(rooks);
    }
    while (queens) {
        Square s = bitscanForward(queens);
        attacks |= lookUpRookAttack(s, m_occupied);
        attacks |= lookUpBishopAttack(s, m_occupied);
        queens = lsbReset(queens);
    }
    while (kings) {
        Square s = bitscanForward(kings);
        attacks |= KING_ATTACKS[s];
        kings = lsbReset(kings);
    }

    return attacks;
}

/**
 * returns a bitboard of the least valuable piece which is in the attadef bitboard.
 * The piece itself will be stored in the given piece variable.
 * @param attadef
 * @param bySide
 * @param piece
 * @return
 */
U64 Board::getLeastValuablePiece(U64 attadef, Score bySide, Piece& piece) {
    for (piece = PAWN + bySide * 6; piece <= KING + bySide * 6; piece += 1) {
        U64 subset = attadef & m_pieces[piece];
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
Score Board::staticExchangeEvaluation(Move m) {

#ifdef SEE_CACHE_SIZE
    U64 zob = zobrist();
    if (seeCache[(zob ^ m) & (SEE_CACHE_SIZE - 1)].key == (zob ^ m)) {
        return seeCache[(zob ^ m) & (SEE_CACHE_SIZE - 1)].score;
    }
#endif

    Square sqFrom         = getSquareFrom(m);
    Square sqTo           = getSquareTo(m);
    Piece  capturedPiece  = isCapture(m) ? getCapturedPiece(m) : -1;
    Piece  capturingPiece = getMovingPiece(m);

    Color attacker = capturingPiece < BLACK_PAWN ? WHITE : BLACK;

    Score gain[16], d = 0;
    U64   fromSet = ONE << sqFrom;
    U64   occ     = m_occupied;

    U64 sqBB = ONE << sqTo;
    U64 bishopsQueens, rooksQueens;
    rooksQueens = bishopsQueens = m_pieces[WHITE_QUEEN] | m_pieces[BLACK_QUEEN];
    rooksQueens |= m_pieces[WHITE_ROOK] | m_pieces[BLACK_ROOK];
    bishopsQueens |= m_pieces[WHITE_BISHOP] | m_pieces[BLACK_BISHOP];

    U64 fixed = ((shiftNorthWest(sqBB) | shiftNorthEast(sqBB)) & m_pieces[BLACK_PAWN])
                | ((shiftSouthWest(sqBB) | shiftSouthEast(sqBB)) & m_pieces[WHITE_PAWN])
                | (KNIGHT_ATTACKS[sqTo] & (m_pieces[WHITE_KNIGHT] | m_pieces[BLACK_KNIGHT]))
                | (KING_ATTACKS[sqTo] & (m_pieces[WHITE_KING] | m_pieces[BLACK_KING]));

    // fixed is the attackset of attackers that cannot pin other m_pieces like
    // pawns, kings, knights

    U64 attadef =
        (fixed | ((lookUpBishopAttack(sqTo, occ) & bishopsQueens) | (lookUpRookAttack(sqTo, occ) & rooksQueens)));

    if (isCapture(m))
        gain[d] = see_piece_vals[capturedPiece % 6];
    else {
        gain[d] = 0;
    }

    do {
        d++;
        attacker = 1 - attacker;

        gain[d] = see_piece_vals[capturingPiece % 6] - gain[d - 1];

        if (-gain[d - 1] < 0 && gain[d] < 0)
            break;    // pruning does not influence the result

        attadef ^= fromSet;    // reset bit in set to traverse
        occ ^= fromSet;
        attadef |=
            occ & ((lookUpBishopAttack(sqTo, occ) & bishopsQueens) | (lookUpRookAttack(sqTo, occ) & rooksQueens));
        // attadef |= (attacksTo(occ, sqTo) & occ);
        fromSet = getLeastValuablePiece(attadef, attacker, capturingPiece);

    } while (fromSet);

    while (--d) {
        gain[d - 1] = -(-gain[d - 1] > gain[d] ? -gain[d - 1] : gain[d]);
    }

#ifdef SEE_CACHE_SIZE
    seeCache[(zob ^ m) & (SEE_CACHE_SIZE - 1)].score = gain[0];
    seeCache[(zob ^ m) & (SEE_CACHE_SIZE - 1)].key   = zob ^ m;
#endif
    return gain[0];
}

/**
 * returns a bitboard with all squares highlighted which either attack or defend the given square
 * @param occupied
 * @param sq
 * @return
 */
U64 Board::attacksTo(U64 p_occupied, Square sq) {
    U64 sqBB = ONE << sq;
    U64 knights, kings, bishopsQueens, rooksQueens;
    knights     = m_pieces[WHITE_KNIGHT] | m_pieces[BLACK_KNIGHT];
    kings       = m_pieces[WHITE_KING] | m_pieces[BLACK_KING];
    rooksQueens = bishopsQueens = m_pieces[WHITE_QUEEN] | m_pieces[BLACK_QUEEN];
    rooksQueens |= m_pieces[WHITE_ROOK] | m_pieces[BLACK_ROOK];
    bishopsQueens |= m_pieces[WHITE_BISHOP] | m_pieces[BLACK_BISHOP];

    return ((shiftNorthWest(sqBB) | shiftNorthEast(sqBB)) & m_pieces[BLACK_PAWN])
           | ((shiftSouthWest(sqBB) | shiftSouthEast(sqBB)) & m_pieces[WHITE_PAWN]) | (KNIGHT_ATTACKS[sq] & knights)
           | (KING_ATTACKS[sq] & kings) | (lookUpBishopAttack(sq, p_occupied) & bishopsQueens)
           | (lookUpRookAttack(sq, p_occupied) & rooksQueens);
}

/**
 * this does not check for en passent attacks!
 * @param square
 * @param attacker
 * @return
 */
bool Board::isUnderAttack(Square square, Color attacker) {
    U64 sqBB = ONE << square;

    if (attacker == WHITE) {
        return (lookUpRookAttack(square, m_occupied) & (m_pieces[WHITE_QUEEN] | m_pieces[WHITE_ROOK])) != 0
               || (lookUpBishopAttack(square, m_occupied) & (m_pieces[WHITE_QUEEN] | m_pieces[WHITE_BISHOP])) != 0
               || (KNIGHT_ATTACKS[square] & m_pieces[WHITE_KNIGHT]) != 0
               || ((shiftSouthEast(sqBB) | shiftSouthWest(sqBB)) & m_pieces[WHITE_PAWN]) != 0
               || (KING_ATTACKS[square] & m_pieces[WHITE_KING]) != 0;
    } else {
        return (lookUpRookAttack(square, m_occupied) & (m_pieces[BLACK_QUEEN] | m_pieces[BLACK_ROOK])) != 0
               || (lookUpBishopAttack(square, m_occupied) & (m_pieces[BLACK_QUEEN] | m_pieces[BLACK_BISHOP])) != 0
               || (KNIGHT_ATTACKS[square] & m_pieces[BLACK_KNIGHT]) != 0
               || ((shiftNorthEast(sqBB) | shiftNorthWest(sqBB)) & m_pieces[BLACK_PAWN]) != 0
               || (KING_ATTACKS[square] & m_pieces[BLACK_KING]) != 0;
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

    Piece  pFrom  = getMovingPiece(m);
    Square sqTo   = getSquareTo(m);
    Square sqFrom = getSquareFrom(m);

    if (getActivePlayer() == BLACK) {
        opponentKing    = m_pieces[WHITE_KING];
        opponentKingPos = bitscanForward(opponentKing);
    } else {
        opponentKing    = m_pieces[BLACK_KING];
        opponentKingPos = bitscanForward(opponentKing);
    }

    // replace the moving piece with the piece to promote to if promotion to detect direct check
    if (isPromotion(m)) {
        pFrom = promotionPiece(m);
    }

    // direct check
    switch (pFrom % 6) {
        case QUEEN: {

            U64 att = lookUpBishopAttack(sqTo, m_occupied) | lookUpRookAttack(sqTo, m_occupied);

            if (att & opponentKing) {
                return true;
            }
            break;
        }
        case BISHOP: {
            U64 att = lookUpBishopAttack(sqTo, m_occupied);
            if (att & opponentKing) {
                return true;
            }
            break;
        }
        case ROOK: {
            U64 att = lookUpRookAttack(sqTo, m_occupied);
            if (att & opponentKing) {
                return true;
            }
            break;
        }
        case KNIGHT: {
            U64 att = KNIGHT_ATTACKS[sqTo];
            if (att & opponentKing) {
                return true;
            }
            break;
        }
        case PAWN: {
            U64 toBB = ONE << sqTo;

            if (getActivePlayer() == WHITE) {
                if (((shiftNorthEast(toBB) | shiftNorthWest(toBB)) & opponentKing) != 0) {
                    return true;
                }
            } else {
                if (((shiftSouthEast(toBB) | shiftSouthWest(toBB)) & opponentKing) != 0) {
                    return true;
                }
            }
            break;
        }
    }

    // discovered check
    U64 occ = m_occupied;

    unsetBit(m_occupied, sqFrom);
    setBit(m_occupied, sqTo);

    if (isUnderAttack(opponentKingPos, getActivePlayer())) {
        m_occupied = occ;
        return true;
    }
    m_occupied = occ;

    // castling check
    if (isCastle(m)) {
        unsetBit(m_occupied, sqFrom);
        Square rookSquare = getActivePlayer() == WHITE ? (sqTo - sqFrom) > 0 ? F1 : D1 : (sqTo - sqFrom) > 0 ? F8 : D8;
        if ((lookUpRookAttack(rookSquare, m_occupied) & opponentKing) != 0) {
            m_occupied = occ;
            return true;
        }
        m_occupied = occ;
    }

    // en passant
    if (isEnPassant(m)) {
        if (getActivePlayer() == WHITE) {
            unsetBit(m_occupied, sqTo - 8);
            if (isUnderAttack(opponentKingPos, WHITE)) {
                m_occupied = occ;
                return true;
            }
            m_occupied = occ;
        } else {
            unsetBit(m_occupied, sqTo + 8);
            if (isUnderAttack(opponentKingPos, BLACK)) {
                m_occupied = occ;
                return true;
            }
            m_occupied = occ;
        }
    }
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
        thisKing               = bitscanForward(m_pieces[WHITE_KING]);
        opponentQueenBitboard  = m_pieces[BLACK_QUEEN];
        opponentRookBitboard   = m_pieces[BLACK_ROOK];
        opponentBishopBitboard = m_pieces[BLACK_BISHOP];
    } else {
        thisKing               = bitscanForward(m_pieces[BLACK_KING]);
        opponentQueenBitboard  = m_pieces[WHITE_QUEEN];
        opponentRookBitboard   = m_pieces[WHITE_ROOK];
        opponentBishopBitboard = m_pieces[WHITE_BISHOP];
    }

    if (isEnPassant(m)) {

        this->move(m);
        bool isOk =
            (bb::lookUpRookAttack(thisKing, m_occupied) & (opponentQueenBitboard | opponentRookBitboard)) == 0
            && (bb::lookUpBishopAttack(thisKing, m_occupied) & (opponentQueenBitboard | opponentBishopBitboard)) == 0;
        this->undoMove();

        return isOk;
    } else if (isCastle(m)) {

        U64 secure = ZERO;

        Type t = getType(m);
        if (this->getActivePlayer() == WHITE) {
            secure = (t == QUEEN_CASTLE) ? bb::CASTLING_WHITE_QUEENSIDE_SAFE : bb::CASTLING_WHITE_KINGSIDE_SAFE;
            return (getAttackedSquares(BLACK) & secure) == 0;
        } else {
            secure = (t == QUEEN_CASTLE) ? bb::CASTLING_BLACK_QUEENSIDE_SAFE : bb::CASTLING_BLACK_KINGSIDE_SAFE;
            return (getAttackedSquares(WHITE) & secure) == 0;
        }
    }

    Square sqFrom = getSquareFrom(m);
    Square sqTo   = getSquareTo(m);
    bool   isCap  = isCapture(m);

    U64 occCopy = m_occupied;

    unsetBit(m_occupied, sqFrom);
    setBit(m_occupied, sqTo);

    bool isAttacked = false;

    if (getMovingPiece(m) % 6 == KING) {
        thisKing = sqTo;
    }

    if (isCap) {
        Piece captured = getCapturedPiece(m);

        unsetBit(this->m_pieces[captured], sqTo);
        isAttacked = isUnderAttack(thisKing, 1 - this->getActivePlayer());
        setBit(this->m_pieces[captured], sqTo);

    } else {
        //        if(thisKing == 64){
        //            std::cout << *this;
        //
        //            *m_occupied = occCopy;
        //            undoMove();
        //            std::cout << *this;
        //            undoMove();
        //            std::cout << *this;
        //            undoMove();
        //            std::cout << *this;
        //            undoMove();
        //            std::cout << *this;
        //
        //            exit(-1);
        //        }
        isAttacked = isUnderAttack(thisKing, 1 - this->getActivePlayer());
    }

    m_occupied = occCopy;

    return !isAttacked;
}

/**
 * returns true if castling for the given index is possible.
 * For a reference to the indexing, check Board.h
 *
 * @param index
 * @return
 */
bool Board::getCastlingChance(Square index) { return getBit(getBoardStatus()->castlingRights, index); }

/**
 * enables/disables castling.
 * For a reference to the indexing, check Board.h
 */
void Board::setCastlingChance(Square index, bool val) {
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
    int maxChecks = getBoardStatus()->fiftyMoveCounter;

    int lim = m_boardStatusHistory.size() - 1 - maxChecks;

    for (int i = m_boardStatusHistory.size() - 3; i >= lim; i -= 2) {
        if (m_boardStatusHistory.at(i).zobrist == getBoardStatus()->zobrist) {
            getBoardStatus()->repetitionCounter = m_boardStatusHistory.at(i).repetitionCounter + 1;
        }
    }
}

/**
 * Returns the amount this position has occurred before.
 * @return
 */
int Board::getCurrentRepetitionCount() { return getBoardStatus()->repetitionCounter; }

/**
 * returns the 50-move counter which is used for draw detection.
 * @return
 */
int Board::getCurrent50MoveRuleCount() { return getBoardStatus()->fiftyMoveCounter / 2; }

/*
 * returns the square to which e.p. is possible.
 * if e.p. is not possible, -1 is returned.
 * @return
 */
Square Board::getEnPassantSquare() {
    U64 ePT = getBoardStatus()->enPassantTarget;
    if (ePT == 0) {
        return -1;
    }
    int pos = bitscanForward(ePT);
    if (pos == 64 || pos < 0) {
        return -1;
    }
    return pos;
}

/**
 * returns the active player which has to do the next move.
 * @return
 */
Color Board::getActivePlayer() { return m_activePlayer; }

/**
 * returns a pointer to the board status which is used internally.
 * @return
 */
BoardStatus* Board::getBoardStatus() { return &m_boardStatusHistory.back(); }

/**
 * writes the board object to the given stream
 */
std::ostream& operator<<(std::ostream& os, Board& board) {

    os << "zobrist key              " << board.zobrist() << "\n";
    os << "repetition               " << board.getCurrentRepetitionCount() << "\n";
    os << "50 move rule             " << board.getCurrent50MoveRuleCount() << "\n";
    os << "white kingside castle    " << (board.getCastlingChance(1) ? "true" : "false") << "\n";
    os << "white queenside castle   " << (board.getCastlingChance(0) ? "true" : "false") << "\n";
    os << "black kingside castle    " << (board.getCastlingChance(3) ? "true" : "false") << "\n";
    os << "black queenside castle   " << (board.getCastlingChance(2) ? "true" : "false") << "\n";
    os << "en passent square        "
       << (board.getEnPassantSquare() >= 0 ? SQUARE_IDENTIFIER[board.getEnPassantSquare()] : "-") << "\n";

    os << " +---+---+---+---+---+---+---+---+\n";

    for (Rank r = 7; r >= 0; r--) {

        for (File f = 0; f <= 7; ++f) {
            // os << squareIndex(r,f);
            Square sq = bb::squareIndex(r, f);
            if (board.getPiece(sq) >= 0) {
                os << " | " << PIECE_IDENTIFER[board.getPiece(bb::squareIndex(r, f))];
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
 * returns a pointer to a bitboard which highlights all occupied squares by all pieces on the board
 * @return
 */
U64* Board::getOccupied() { return &m_occupied; }

/**
 * returns an array (pointer to the first element) which contains a bitboard for each piece on the board (12)
 * @return
 */
const U64* Board::getPieces() const { return m_pieces; }

/**
 * returns an array (pointer to the first element) which contains a bitboard for both teams.
 * @return
 */
const U64* Board::getTeamOccupied() const { return m_teamOccupied; }

/**
 * does the same as getPieces() although this only returns a single bitboard for the given piece type.
 * @param color
 * @param piece
 * @return
 */
U64 Board::getPieces(Color color, Piece piece) { return m_pieces[color * 6 + piece]; }

/**
 * returns a bitboard of all pinned pieces of the given color.
 * It furthermore writes the pinning pieces into the given pinners bitboard.
 * @param color
 * @param pinners
 * @return
 */
U64 Board::getPinnedPieces(Color color, U64& pinners) {
    U64 pinned = 0;

    Square kingSq = bitscanForward(getPieces(color, KING));

    Color them = color ^ 1;

    U64 pinner = lookUpRookXRayAttack(kingSq, m_occupied, m_teamOccupied[color])
                 & (getPieces(them, ROOK) | getPieces(them, QUEEN));
    pinners |= pinner;
    while (pinner) {
        Square s = bitscanForward(pinner);
        pinned |= inBetweenSquares[kingSq][s] & m_teamOccupied[color];
        pinner = lsbReset(pinner);
    }

    pinner = lookUpBishopXRayAttack(kingSq, m_occupied, m_teamOccupied[color])
             & (getPieces(them, BISHOP) | getPieces(them, QUEEN));
    pinners |= pinner;

    while (pinner) {
        Square s = bitscanForward(pinner);
        pinned |= inBetweenSquares[kingSq][s] & m_teamOccupied[color];
        pinner = lsbReset(pinner);
    }
    return pinned;
}
