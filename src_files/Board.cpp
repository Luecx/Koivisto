//
// Created by finne on 5/14/2020.
//

#include "Board.h"


using namespace bb;


Board::Board(std::string fen) {
    
    pieces       = new U64[12]{0};
    teamOccupied = new U64[2]{0};
    occupied     = new U64();
    pieceBoard   = new Piece[64];
    
    
    activePlayer = 0;
    
    for (int i = 0; i < 64; i++) {
        pieceBoard[i] = -1;
    }
    
    
    BoardStatus boardStatus{0, 0, 0, 0, ONE, ONE, 0};
    this->boardStatusHistory.push_back(boardStatus);
    
    
    
    
    //#########################################################################################################
    //#-------------------------------- F E N - P A R S I N G ------------------------------------------------#
    //#########################################################################################################
    
    //<editor-fold desc="splitting/trimming string">
    std::vector<std::string> split{};
    std::string              str{fen};
    str = trim(str);
    findAndReplaceAll(str, "  ", " ");
    splitString(str, split, ' ');
    //</editor-fold>
    
    //<editor-fold desc="parsing pieces">
    File      x{0};
    Rank      y{7};
    for (char c : split[0]) {
        
        if (c == '/') {
            x = 0;
            y--;
            continue;
        }
        
        if (c < '9') {
            x += (c - '0');
            continue;
        } else {
            int offset = (c >= 'a') ? 6 : 0;
            
            Square sq = squareIndex(y, x);
            
            switch (toupper(c)) {
                case 'P':
                    setPiece(sq, 0 + offset);
                    break;
                case 'N':
                    setPiece(sq, 1 + offset);
                    break;
                case 'B':
                    setPiece(sq, 2 + offset);
                    break;
                case 'R':
                    setPiece(sq, 3 + offset);
                    break;
                case 'Q':
                    setPiece(sq, 4 + offset);
                    break;
                case 'K':
                    setPiece(sq, 5 + offset);
                    break;
            }
            
            x++;
        }
    }
    //</editor-fold>
    
    //<editor-fold desc="parsing active player">
    if (split.size() >= 2 && split[1].length() == 1) {
        if (split[1].at(0) != 'w') {
            changeActivePlayer();
        }
    }
    //</editor-fold>
    
    //<editor-fold desc="parsing castling">
    if (split.size() >= 3) {
        
        for (int i = 0; i < 4; i++) {
            setCastlingChance(i, false);
        }
        
        for (char c:split[2]) {
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
    //</editor-fold>
    
    //<editor-fold desc="parsing en passant">
    if (split.size() >= 4) {
        if (split[3].at(0) != '-') {
            Square square = squareIndex(split[3]);
            setEnPassantSquare(square);
        }
    }
    //</editor-fold>
    
    
    
    
}


Board::Board(Board *board) {
    pieces       = new U64[12]{0};
    teamOccupied = new U64[2]{0};
    occupied     = new U64();
    pieceBoard   = new Piece[64];
    
    
    teamOccupied[WHITE] = board->getTeamOccupied()[WHITE];
    teamOccupied[BLACK] = board->getTeamOccupied()[BLACK];
    
    for(int n = 0; n < 12; n++){
        pieces[n] = board->pieces[n];
    }
    
    *occupied = *board->getOccupied();
    activePlayer = board->getActivePlayer();
    
    for(int i = 0; i < 64; i++){
        pieceBoard[i] = board->pieceBoard[i];
    }
    
    for(int n = 0; n < (int)board->boardStatusHistory.size(); n++){
        boardStatusHistory.push_back(board->boardStatusHistory.at(n).copy());
    }
}


Board::~Board() {
    delete Board::pieces;
    delete Board::pieceBoard;
    delete Board::teamOccupied;
    delete Board::occupied;
}

std::string Board::fen() {
    
    std::stringstream ss;
    
    for (Rank n = 7; n >= 0; n--) {
        int       counting = 0;
        for (File i        = 0; i < 8; i++) {
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
        if (n != 0) ss << "/";
    }
    
    ss << " ";
    ss << ((getActivePlayer() == 0) ? "w" : "b");
    ss << " ";
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
    if (anyCastling == false) {
        ss << "-";
    }
    
    
    if (getEnPassantSquare() >= 0) {
        ss << " ";
        ss << SQUARE_IDENTIFIER[getEnPassantSquare()];
    } else {
        ss << " -";
    }
    
    ss << " " << getBoardStatus()->fiftyMoveCounter;
    ss << " " << getBoardStatus()->moveCounter;
    
    
    return ss.str();
}

U64 Board::zobrist() {
    return getBoardStatus()->zobrist;
}

bool Board::isInCheck(Color player) {
    if (player == WHITE) {
        return isUnderAttack(bitscanForward(pieces[WHITE_KING]), BLACK);
    } else {
        
        return isUnderAttack(bitscanForward(pieces[BLACK_KING]), WHITE);
    }
}

bool Board::isDraw() {
    return getCurrent50MoveRuleCount() >= 50 || getCurrentRepetitionCount() >= 2;
}

Piece Board::getPiece(Square sq) {
    return pieceBoard[sq];
}

void Board::setPiece(Square sq, Piece piece) {
    
    
    pieceBoard[sq] = piece;
    
    U64 sqBB = (ONE << sq);
    
    pieces[piece] |= sqBB;
    teamOccupied[piece / 6] |= sqBB;
    *occupied |= sqBB;
    
    BoardStatus *st = getBoardStatus();
    st->zobrist ^= getHash(piece, sq);
    
}

void Board::unsetPiece(Square sq) {
    
    
    Piece p = getPiece(sq);
    
    U64 sqBB = ~(ONE << sq);
    pieces[p] &= sqBB;
    teamOccupied[p / 6] &= sqBB;
    *occupied &= sqBB;
    
    BoardStatus *st = getBoardStatus();
    st->zobrist ^= getHash(p, sq);
    
    pieceBoard[sq] = -1;
}

void Board::replacePiece(Square sq, Piece piece) {
    Piece p = getPiece(sq);
    
    U64 sqBB = (ONE << sq);
    
    pieces[p] &= ~sqBB;                     //unset
    pieces[piece] |= sqBB;                  //set
    teamOccupied[p / 6] &= ~sqBB;           //unset
    teamOccupied[piece / 6] |= sqBB;        //set
    
    BoardStatus *st = getBoardStatus();
    st->zobrist ^= (getHash(p, sq) ^ getHash(piece, sq));
    
    pieceBoard[sq] = piece;
}

void Board::changeActivePlayer() {
    activePlayer = 1 - activePlayer;
}

void Board::move(Move m) {
    BoardStatus *previousStatus = getBoardStatus();
    BoardStatus newBoardStatus  = {
            previousStatus->zobrist,                                           //zobrist will be changed later
            0ULL,                                                 //reset en passant. might be set later
            previousStatus->metaInformation,                                   //copy meta. might be changed
            previousStatus->fiftyMoveCounter + 1,                  //increment fifty move counter. might be reset
            1ULL,                                                //set rep to 1 (no rep)
            previousStatus->moveCounter + getActivePlayer(),        //increment move counter
            m
    };
    
    
    Square sqFrom = getSquareFrom(m);
    Square sqTo   = getSquareTo(m);
    Piece  pFrom  = getMovingPiece(m);
    Type   mType  = getType(m);
    Color  color  = getActivePlayer();
    int    factor = getActivePlayer() == 0 ? 1 : -1;
    
    
    if (isCapture(m)) {
        //reset fifty move counter if a piece has been captured
        newBoardStatus.fiftyMoveCounter = 0;
    }
    
    newBoardStatus.zobrist ^= ZOBRIST_WHITE_BLACK_SWAP;
    if (pFrom % 6 == PAWN) {
        
        //reset fifty move counter if pawn has moved
        newBoardStatus.fiftyMoveCounter = 0;
        
        //if a pawn advances by 2 squares, enabled enPassant capture next move
        if (mType == DOUBLED_PAWN_PUSH) {
            newBoardStatus.enPassantTarget = (ONE << (sqFrom + 8 * factor));
        }
            
            //promotions are handled differently because the new piece at the target square is not the piece that initially moved.
        else if (isPromotion(m)) {
            
            //we handle this case seperately so we return after this finished.
            boardStatusHistory.emplace_back(std::move(newBoardStatus));
            this->changeActivePlayer();
            
            
            //setting pieces
            this->unsetPiece(sqFrom);
            
            //do the "basic" move
            if (isCapture(m)) {
                this->replacePiece(sqTo, promotionPiece(m));
            } else {
                this->setPiece(sqTo, promotionPiece(m));
            }
            
            return;
        } else if (mType == EN_PASSANT) {
            
            
            boardStatusHistory.emplace_back(std::move(newBoardStatus));
            this->changeActivePlayer();
            
            //make sure to capture the pawn
            unsetPiece(sqTo - 8 * factor);
            
            //move to pawn
            this->unsetPiece(sqFrom);
            this->setPiece(sqTo, pFrom);
            
            return;
        }
    } else if (pFrom % 6 == KING) {
        
        //revoke castling rights if king moves
        newBoardStatus.metaInformation &= ~(ONE << (color * 2));
        newBoardStatus.metaInformation &= ~(ONE << (color * 2 + 1));
        
        
        //we handle this case seperately so we return after this finished.
        boardStatusHistory.emplace_back(std::move(newBoardStatus));
        this->changeActivePlayer();
        
        if (isCastle(m)) {
            //move the rook as well. castling rights are handled below
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
        
        //we need to compute the repetition count
        this->computeNewRepetition();
        
        return;
        
    }
        
        //revoke castling rights if rook moves and it is on the initial square
    else if (pFrom % 6 == ROOK) {
        if (color == WHITE) {
            if (sqFrom == A1) {
                newBoardStatus.metaInformation &= ~(ONE << (color * 2));
            } else if (sqFrom == H1) {
                newBoardStatus.metaInformation &= ~(ONE << (color * 2 + 1));
            }
        } else {
            if (sqFrom == A8) {
                newBoardStatus.metaInformation &= ~(ONE << (color * 2));
            } else if (sqFrom == H8) {
                newBoardStatus.metaInformation &= ~(ONE << (color * 2 + 1));
            }
        }
//        if (f == 0) {
//            newBoardStatus.metaInformation &= ~(ONE << (color * 2));
//        } else {
//            newBoardStatus.metaInformation &= ~(ONE << (color * 2 + 1));
//        }
    }
    
    boardStatusHistory.emplace_back(std::move(newBoardStatus));
    
    
    //doing the initial move
    this->unsetPiece(sqFrom);
    
    if (mType != EN_PASSANT && isCapture(m)) {
        this->replacePiece(sqTo, pFrom);
    } else {
        this->setPiece(sqTo, pFrom);
    }
    
    
    this->changeActivePlayer();
    this->computeNewRepetition();
}

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
    
    boardStatusHistory.pop_back();
}

void Board::move_null() {
    BoardStatus *previousStatus = getBoardStatus();
    BoardStatus newBoardStatus  = {
            previousStatus->zobrist ^ ZOBRIST_WHITE_BLACK_SWAP,
            0ULL,
            previousStatus->metaInformation,
            previousStatus->fiftyMoveCounter + 1,
            1ULL,
            previousStatus->moveCounter + getActivePlayer(),
            0ULL,
    };
    
    boardStatusHistory.emplace_back(std::move(newBoardStatus));
    changeActivePlayer();
    
}

void Board::undoMove_null() {
    
    boardStatusHistory.pop_back();
    changeActivePlayer();
}

void Board::getPseudoLegalMoves(MoveList *moves) {
    
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
        
        attackableSquares = ~teamOccupied[WHITE];
        opponents         = teamOccupied[BLACK];
        
        knights = pieces[WHITE_KNIGHT];
        bishops = pieces[WHITE_BISHOP];
        rooks   = pieces[WHITE_ROOK];
        queens  = pieces[WHITE_QUEEN];
        kings   = pieces[WHITE_KING];
        
        U64 pawnsLeft   = shiftNorthWest(pieces[WHITE_PAWN] & ~FILE_A);
        U64 pawnsRight  = shiftNorthEast(pieces[WHITE_PAWN] & ~FILE_H);
        U64 pawnsCenter = shiftNorth(pieces[WHITE_PAWN]) & ~*occupied;
        
        
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
        
        U64 h = pawnsCenter & ~RANK_8;
        attacks = h;
        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(target - 8, target, QUIET, WHITE_PAWN));
            attacks = lsbReset(attacks);
            
        }
        
        attacks = shiftNorth(h) & RANK_4 & ~*occupied;
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
        
        if (pieces[WHITE_PAWN] & RANK_7) {
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
        
        attackableSquares = ~teamOccupied[BLACK];
        opponents         = teamOccupied[WHITE];
        
        knights = pieces[BLACK_KNIGHT];
        bishops = pieces[BLACK_BISHOP];
        rooks   = pieces[BLACK_ROOK];
        queens  = pieces[BLACK_QUEEN];
        kings   = pieces[BLACK_KING];
        
        U64 pawnsLeft   = shiftSouthWest(pieces[BLACK_PAWN] & ~FILE_A);
        U64 pawnsRight  = shiftSouthEast(pieces[BLACK_PAWN] & ~FILE_H);
        U64 pawnsCenter = shiftSouth(pieces[BLACK_PAWN]) & ~*occupied;
        
        
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
        
        U64 h = pawnsCenter & ~RANK_1;
        attacks = h;
        while (attacks) {
            target = bitscanForward(attacks);
            moves->add(genMove(target + 8, target, QUIET, BLACK_PAWN));
            attacks = lsbReset(attacks);
        }
        
        attacks = shiftSouth(h) & RANK_5 & ~*occupied;
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
        
        
        if (pieces[BLACK_PAWN] & RANK_2) {
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
        attacks = lookUpBishopAttack(s, *occupied) & attackableSquares;
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
        attacks = lookUpRookAttack(s, *occupied) & attackableSquares;
        
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
        
        attacks = (lookUpRookAttack(s, *occupied) | lookUpBishopAttack(s, *occupied))
                  & attackableSquares;
        
        while (attacks) {
            target = bitscanForward(attacks);
            
            if (getPiece(target) >= 0) {
                //genMove(s, target, CAPTURE, QUEEN + pieceOffset, getPiece(target));
                moves->add(genMove(s, target, CAPTURE, QUEEN + pieceOffset, getPiece(target)));
            } else {
                //genMove(s, target, QUIET, QUEEN + pieceOffset);
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
            
            if (getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING) &&
                getPiece(A1) == WHITE_ROOK &&
                (*occupied & CASTLING_WHITE_QUEENSIDE_MASK) == 0) {
                moves->add(genMove(E1, C1, QUEEN_CASTLE, WHITE_KING));
            }
            if (getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING) &&
                getPiece(H1) == WHITE_ROOK &&
                (*occupied & CASTLING_WHITE_KINGSIDE_MASK) == 0) {
                moves->add(genMove(E1, G1, KING_CASTLE, WHITE_KING));
            }
            
            
        } else {
            
            if (getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING) &&
                getPiece(A8) == BLACK_ROOK &&
                (*occupied & CASTLING_BLACK_QUEENSIDE_MASK) == 0) {
                
                moves->add(genMove(E8, C8, QUEEN_CASTLE, BLACK_KING));
            }
            if (getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING) &&
                getPiece(H8) == BLACK_ROOK &&
                (*occupied & CASTLING_BLACK_KINGSIDE_MASK) == 0) {
                moves->add(genMove(E8, G8, KING_CASTLE, BLACK_KING));
            }
        }
        kings = lsbReset(kings);
    }
}

void Board::getNonQuietMoves(MoveList *moves) {
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
        
        opponents = teamOccupied[BLACK];
        
        knights = pieces[WHITE_KNIGHT];
        bishops = pieces[WHITE_BISHOP];
        rooks   = pieces[WHITE_ROOK];
        queens  = pieces[WHITE_QUEEN];
        kings   = pieces[WHITE_KING];
        
        U64 pawnsLeft   = shiftNorthWest(pieces[WHITE_PAWN] & ~FILE_A);
        U64 pawnsRight  = shiftNorthEast(pieces[WHITE_PAWN] & ~FILE_H);
        U64 pawnsCenter = shiftNorth(pieces[WHITE_PAWN]) & ~*occupied;
        
        
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
        
        if (pieces[WHITE_PAWN] & RANK_7) {
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
        
        opponents = teamOccupied[WHITE];
        
        knights = pieces[BLACK_KNIGHT];
        bishops = pieces[BLACK_BISHOP];
        rooks   = pieces[BLACK_ROOK];
        queens  = pieces[BLACK_QUEEN];
        kings   = pieces[BLACK_KING];
        
        U64 pawnsLeft   = shiftSouthWest(pieces[BLACK_PAWN] & ~FILE_A);
        U64 pawnsRight  = shiftSouthEast(pieces[BLACK_PAWN] & ~FILE_H);
        U64 pawnsCenter = shiftSouth(pieces[BLACK_PAWN]) & ~*occupied;
        
        
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
        
        if (pieces[BLACK_PAWN] & RANK_2) {
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
        attacks = lookUpBishopAttack(s, *occupied) & opponents;
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
        attacks = lookUpRookAttack(s, *occupied) & opponents;
        
        while (attacks) {
            target = bitscanForward(attacks);
            
            moves->add(genMove(s, target, CAPTURE, ROOK + pieceOffset, getPiece(target)));
            
            attacks = lsbReset(attacks);
            
        }
        rooks = lsbReset(rooks);
    }
    while (queens) {
        s = bitscanForward(queens);
        
        attacks = (lookUpRookAttack(s, *occupied) | lookUpBishopAttack(s, *occupied))
                  & opponents;
        
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


Move Board::getPreviousMove() {
    if (boardStatusHistory.empty()) return 0;
    return boardStatusHistory.back().move;
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
        attacks |= shiftNorthEast(pieces[WHITE_PAWN]) | shiftNorthWest((pieces[WHITE_PAWN]));
    } else {
        attacks |= shiftSouthEast(pieces[BLACK_PAWN]) | shiftSouthWest((pieces[BLACK_PAWN]));
    }
    
    U64 knights(pieces[KNIGHT + 6 * attacker]);
    U64 bishops(pieces[BISHOP + 6 * attacker]);
    U64 rooks(pieces[ROOK + 6 * attacker]);
    U64 queens(pieces[QUEEN + 6 * attacker]);
    U64 kings(pieces[KING + 6 * attacker]);
    
    
    while (knights) {
        Square s = bitscanForward(knights);
        attacks |= KNIGHT_ATTACKS[s];
        knights  = lsbReset(knights);
    }
    while (bishops) {
        Square s = bitscanForward(bishops);
        attacks |= lookUpBishopAttack(s, *occupied);
        bishops  = lsbReset(bishops);
    }
    while (rooks) {
        Square s = bitscanForward(rooks);
        attacks |= lookUpRookAttack(s, *occupied);
        rooks    = lsbReset(rooks);
    }
    while (queens) {
        Square s = bitscanForward(queens);
        attacks |= lookUpRookAttack(s, *occupied);
        attacks |= lookUpBishopAttack(s, *occupied);
        queens   = lsbReset(queens);
    }
    while (kings) {
        Square s = bitscanForward(kings);
        attacks |= KING_ATTACKS[s];
        kings    = lsbReset(kings);
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
U64 Board::getLeastValuablePiece(U64 attadef, Score bySide, Piece &piece) {
    for (piece = PAWN + bySide * 6; piece <= KING + bySide * 6; piece += 1) {
        U64 subset = attadef & pieces[piece];
        if (subset)
            return subset & -subset; // single bit
    }
    return 0; // empty set
}

Score Board::staticExchangeEvaluation(Move m) {
    
    static constexpr Score vals[]{100, 325, 325, 500, 1000, 10000};
    
    Square sqFrom         = getSquareFrom(m);
    Square sqTo           = getSquareTo(m);
    Piece  capturedPiece  = isCapture(m) ? getCapturedPiece(m) : -1;
    Piece  capturingPiece = getMovingPiece(m);
    
    Color attacker    = capturingPiece < BLACK_PAWN ? WHITE : BLACK;
    
    Score gain[32], d = 0;
    U64   fromSet     = ONE << sqFrom;
    U64   occ         = *occupied;
    
    U64 sqBB = ONE << sqTo;
    U64 bishopsQueens, rooksQueens;
    rooksQueens =
    bishopsQueens = pieces[WHITE_QUEEN] | pieces[BLACK_QUEEN];
    rooksQueens |= pieces[WHITE_ROOK] | pieces[BLACK_ROOK];
    bishopsQueens |= pieces[WHITE_BISHOP] | pieces[BLACK_BISHOP];
    
    
    U64 fixed = ((shiftNorthWest(sqBB) | shiftNorthEast(sqBB)) & pieces[BLACK_PAWN])
                | ((shiftSouthWest(sqBB) | shiftSouthEast(sqBB)) & pieces[WHITE_PAWN])
                | (KNIGHT_ATTACKS[sqTo] & (pieces[WHITE_KNIGHT] | pieces[BLACK_KNIGHT]))
                | (KING_ATTACKS[sqTo] & (pieces[WHITE_KING] | pieces[BLACK_KING]));
    
    //fixed is the attackset of attackers that cannot pin other pieces like pawns, kings, knights
    
    
    U64 attadef = (fixed
                   | ((lookUpBishopAttack(sqTo, occ) & bishopsQueens)
                      | (lookUpRookAttack(sqTo, occ) & rooksQueens)));
    
    
    if (isCapture(m))
        gain[d] = vals[capturedPiece % 6];
    else {
        gain[d] = 0;
    }
    
    do {
        d++;
        attacker = 1 - attacker;
        
        gain[d] = vals[capturingPiece % 6] - gain[d - 1];
        
        
        if (-gain[d - 1] < 0 && gain[d] < 0) break; // pruning does not influence the result
        
        attadef ^= fromSet; // reset bit in set to traverse
        occ ^= fromSet;
        attadef |=
                occ & ((lookUpBishopAttack(sqTo, occ) & bishopsQueens) | (lookUpRookAttack(sqTo, occ) & rooksQueens));
        //attadef |= (attacksTo(occ, sqTo) & occ);
        fromSet = getLeastValuablePiece(attadef, attacker, capturingPiece);
        
    } while (fromSet);
    
    while (--d) {
        gain[d - 1] = -(-gain[d - 1] > gain[d] ? -gain[d - 1] : gain[d]);
    }
    
    
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
    knights = pieces[WHITE_KNIGHT] | pieces[BLACK_KNIGHT];
    kings   = pieces[WHITE_KING] | pieces[BLACK_KING];
    rooksQueens =
    bishopsQueens = pieces[WHITE_QUEEN] | pieces[BLACK_QUEEN];
    rooksQueens |= pieces[WHITE_ROOK] | pieces[BLACK_ROOK];
    bishopsQueens |= pieces[WHITE_BISHOP] | pieces[BLACK_BISHOP];
    
    
    return ((shiftNorthWest(sqBB) | shiftNorthEast(sqBB)) & pieces[BLACK_PAWN])
           | ((shiftSouthWest(sqBB) | shiftSouthEast(sqBB)) & pieces[WHITE_PAWN])
           | (KNIGHT_ATTACKS[sq] & knights)
           | (KING_ATTACKS[sq] & kings)
           | (lookUpBishopAttack(sq, p_occupied) & bishopsQueens)
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
        return
                (lookUpRookAttack(square, *occupied) & (pieces[WHITE_QUEEN] | pieces[WHITE_ROOK])) != 0 ||
                (lookUpBishopAttack(square, *occupied) & (pieces[WHITE_QUEEN] | pieces[WHITE_BISHOP])) != 0 ||
                (KNIGHT_ATTACKS[square] & pieces[WHITE_KNIGHT]) != 0 ||
                ((shiftSouthEast(sqBB) | shiftSouthWest(sqBB)) & pieces[WHITE_PAWN]) != 0 ||
                (KING_ATTACKS[square] & pieces[WHITE_KING]) != 0;
    } else {
        return
                (lookUpRookAttack(square, *occupied) & (pieces[BLACK_QUEEN] | pieces[BLACK_ROOK])) != 0 ||
                (lookUpBishopAttack(square, *occupied) & (pieces[BLACK_QUEEN] | pieces[BLACK_BISHOP])) != 0 ||
                (KNIGHT_ATTACKS[square] & pieces[BLACK_KNIGHT]) != 0 ||
                ((shiftNorthEast(sqBB) | shiftNorthWest(sqBB)) & pieces[BLACK_PAWN]) != 0 ||
                (KING_ATTACKS[square] & pieces[BLACK_KING]) != 0;
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
        opponentKing    = pieces[WHITE_KING];
        opponentKingPos = bitscanForward(opponentKing);
    } else {
        opponentKing    = pieces[BLACK_KING];
        opponentKingPos = bitscanForward(opponentKing);
        
    }
    
    //replace the moving piece with the piece to promote to if promotion to detect direct check
    if (isPromotion(m)) {
        pFrom = promotionPiece(m);
    }

//    std::cout << (int)pFrom << "---";
    
    //direct check
    switch (pFrom % 6) {
        case QUEEN: {
            
            U64 att = lookUpBishopAttack(sqTo, *occupied) | lookUpRookAttack(sqTo, *occupied);
            
            if (att & opponentKing) {
                return true;
            }
            break;
        }
        case BISHOP: {
            U64 att = lookUpBishopAttack(sqTo, *occupied);
            if (att & opponentKing) {
                return true;
            }
            break;
        }
        case ROOK: {
            U64 att = lookUpRookAttack(sqTo, *occupied);
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
    
    //discovered check
    U64 occ   = *occupied;
    
    unsetBit(*occupied, sqFrom);
    setBit(*occupied, sqTo);
    
    if (isUnderAttack(opponentKingPos, getActivePlayer())) {
        *occupied = occ;
        return true;
    }
    *occupied = occ;
    
    
    
    //castling check
    if (isCastle(m)) {
        unsetBit(*occupied, sqFrom);
        Square rookSquare = getActivePlayer() == WHITE ?
                            (sqTo - sqFrom) > 0 ? F1 : D1 :
                            (sqTo - sqFrom) > 0 ? F8 : D8;
        if ((lookUpRookAttack(rookSquare, *occupied) & opponentKing) != 0) {
            *occupied = occ;
            return true;
        }
        *occupied         = occ;
    }
    
    
    //en passant
    if (isEnPassant(m)) {
        if (getActivePlayer() == WHITE) {
            unsetBit(*occupied, sqTo - 8);
            if (isUnderAttack(opponentKingPos, WHITE)) {
                *occupied = occ;
                return true;
            }
            *occupied = occ;
        } else {
            unsetBit(*occupied, sqTo + 8);
            if (isUnderAttack(opponentKingPos, BLACK)) {
                *occupied = occ;
                return true;
            }
            *occupied = occ;
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
        thisKing               = bitscanForward(pieces[WHITE_KING]);
        opponentQueenBitboard  = pieces[BLACK_QUEEN];
        opponentRookBitboard   = pieces[BLACK_ROOK];
        opponentBishopBitboard = pieces[BLACK_BISHOP];
    } else {
        thisKing               = bitscanForward(pieces[BLACK_KING]);
        opponentQueenBitboard  = pieces[WHITE_QUEEN];
        opponentRookBitboard   = pieces[WHITE_ROOK];
        opponentBishopBitboard = pieces[WHITE_BISHOP];
    }
    
    
    if (isEnPassant(m)){
        
        
        this->move(m);
        bool isOk =
                     (bb::lookUpRookAttack(thisKing, *occupied) & (opponentQueenBitboard | opponentRookBitboard)) ==
                     0 &&
                     (bb::lookUpBishopAttack(thisKing, *occupied) & (opponentQueenBitboard | opponentBishopBitboard)) ==
                     0;
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
    
    
    U64 occCopy = *occupied;
    
    unsetBit(*occupied, sqFrom);
    setBit(*occupied, sqTo);
    
    
    bool isAttacked = false;
    
    
    if (getMovingPiece(m) % 6 == KING) {
        thisKing = sqTo;
    }
    
    if (isCap) {
        Piece captured = getCapturedPiece(m);
        
        
        unsetBit(this->pieces[captured], sqTo);
        isAttacked = isUnderAttack(thisKing, 1 - this->getActivePlayer());
        setBit(this->pieces[captured], sqTo);
        
    } else {
//        if(thisKing == 64){
//            std::cout << *this;
//
//            *occupied = occCopy;
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
    
    
    *occupied = occCopy;
    
    
    return !isAttacked;
}

/**
 * returns true if castling for the given index is possible.
 * For a reference to the indexing, check Board.h
 *
 * @param index
 * @return
 */
bool Board::getCastlingChance(Square index) {
    return getBit(getBoardStatus()->metaInformation, index);
}

/**
 * enables/disables castling.
 * For a reference to the indexing, check Board.h
 */
void Board::setCastlingChance(Square index, bool val) {
    if (val) {
        setBit(getBoardStatus()->metaInformation, index);
    } else {
        unsetBit(getBoardStatus()->metaInformation, index);
    }
}


void Board::setEnPassantSquare(Square square) {
    if (square < 0) getBoardStatus()->enPassantTarget = 0;
    else getBoardStatus()->enPassantTarget = (ONE << square);
}

void Board::computeNewRepetition() {
    int maxChecks = getBoardStatus()->fiftyMoveCounter;
    
    int lim = boardStatusHistory.size() - 1 - maxChecks;
    
    for (int i = boardStatusHistory.size() - 3; i >= lim; i -= 2) {
        if (boardStatusHistory.at(i).zobrist == getBoardStatus()->zobrist) {
            getBoardStatus()->repetitionCounter = boardStatusHistory.at(i).repetitionCounter + 1;
        }
    }
}

int Board::getCurrentRepetitionCount() {
    return getBoardStatus()->repetitionCounter;
}

int Board::getCurrent50MoveRuleCount() {
    return getBoardStatus()->fiftyMoveCounter / 2;
}

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

Board Board::copy() {
    return Board();
}

Color Board::getActivePlayer() {
    return activePlayer;
}

BoardStatus *Board::getBoardStatus() {
    return &boardStatusHistory.back();
}

std::ostream &operator<<(std::ostream &os, Board &board) {
    
    
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
            //os << squareIndex(r,f);
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


U64 *Board::getOccupied() {
    return occupied;
}

U64 *Board::getPieces() const {
    return pieces;
}

U64 *Board::getTeamOccupied() const {
    return teamOccupied;
}

U64 Board::getPieces(Color color, Piece piece) {
    return pieces[color * 6 + piece];
}

U64 Board::getPinnedPieces(Color color, U64 &pinners) {
    U64 pinned = 0;
    
    Square kingSq = bitscanForward(getPieces(color, KING));
    
    Color them = color ^1;
    
    U64 pinner = lookUpRookXRayAttack(kingSq, *occupied, teamOccupied[color]) &
                 (getPieces(them, ROOK) | getPieces(them, QUEEN));
    pinners |= pinner;
    while (pinner) {
        Square s = bitscanForward(pinner);
        pinned |= inBetweenSquares[kingSq][s] & teamOccupied[color];
        pinner   = lsbReset(pinner);
    }
    
    pinner = lookUpBishopXRayAttack(kingSq, *occupied, teamOccupied[color]) &
             (getPieces(them, BISHOP) | getPieces(them, QUEEN));
    pinners |= pinner;
    
    while (pinner) {
        Square s = bitscanForward(pinner);
        pinned |= inBetweenSquares[kingSq][s] & teamOccupied[color];
        pinner   = lsbReset(pinner);
    }
    return pinned;
}






