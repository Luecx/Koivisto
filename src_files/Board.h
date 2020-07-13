//
// Created by finne on 5/14/2020.
//

#ifndef CHESSCOMPUTER_BOARD_H
#define CHESSCOMPUTER_BOARD_H

#include <stdio.h>
#include <string>
#include <sstream>      // std::stringstream
#include <ostream>
#include "Bitboard.h"
#include "Move.h"
#include "vector"
#include "Util.h"

using namespace bb;
using namespace move;

    
    constexpr U64 STATUS_INDEX_WHITE_QUEENSIDE_CASTLING     = 0;
    constexpr U64 STATUS_INDEX_WHITE_KINGSIDE_CASTLING      = 1;
    constexpr U64 STATUS_INDEX_BLACK_QUEENSIDE_CASTLING     = 2;
    constexpr U64 STATUS_INDEX_BLACK_KINGSIDE_CASTLING      = 3;
    
    constexpr U64 ZOBRIST_WHITE_BLACK_SWAP                  = 1;


/**
 * the board contains a list of board status.
 * It contains all the information not represented by the the pieces on the board.
 * This contains:
 *   - 50 move rule half counter
 *   - three-fold repetition counter
 *   - zobrist key
 *   - last move
 *   - information about castling rights
 *   - information about the en-passant square
 *
 */
struct BoardStatus{
    public:
        
        BoardStatus(const BoardStatus& a) :
                zobrist(a.zobrist),
                enPassantTarget(a.enPassantTarget),
                metaInformation(a.metaInformation),
                fiftyMoveCounter(a.fiftyMoveCounter),
                repetitionCounter(a.repetitionCounter),
                moveCounter(a.moveCounter),
                move(a.move){}
        
        BoardStatus(
                U64 zobrist,
                U64 enPassantTarget,
                U64 metaInformation,
                U64 fiftyMoveCounter,
                U64 repetitionCounter,
                U64 moveCounter,
                Move move) :
                zobrist(zobrist),
                enPassantTarget(enPassantTarget),
                metaInformation(metaInformation),
                fiftyMoveCounter(fiftyMoveCounter),
                repetitionCounter(repetitionCounter),
                moveCounter(moveCounter),
                move(move) {}
        
        U64 zobrist;
        U64 enPassantTarget;
        U64 metaInformation;
        U64 fiftyMoveCounter;
        U64 repetitionCounter;
        U64 moveCounter;
        Move move;
        
        bool operator==(const BoardStatus &rhs) const {
            return zobrist == rhs.zobrist &&
                   enPassantTarget == rhs.enPassantTarget &&
                   metaInformation == rhs.metaInformation &&
                   fiftyMoveCounter == rhs.fiftyMoveCounter &&
                   repetitionCounter == rhs.repetitionCounter &&
                   moveCounter == rhs.moveCounter &&
                   move == rhs.move;
        }
        
        bool operator!=(const BoardStatus &rhs) const {
            return !(rhs == *this);
        }
        
        friend std::ostream &operator<<(std::ostream &os, const BoardStatus &status) {
            os << "zobrist: " << status.zobrist << " metaInformation: " << status.metaInformation
               << " fiftyMoveCounter: " << status.fiftyMoveCounter << " repetitionCounter: " << status.repetitionCounter
               << " move: " << status.move;
            return os;
        }
        
        inline BoardStatus copy() {
            BoardStatus b{zobrist, enPassantTarget, metaInformation, fiftyMoveCounter, repetitionCounter, moveCounter, move};
            return b;
        }
};

class Board {
    private:
        U64 *pieces;
        U64 *teamOccupied;
        U64 *occupied;
        
        Piece *pieceBoard;
        
        Color activePlayer;
        
        std::vector<BoardStatus> boardStatusHistory;
        
        void computeNewRepetition();
        
    public:
        Board(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        
        virtual ~Board();
        
        friend std::ostream &operator<<(std::ostream &os, Board &board);
        
        std::string fen();
        
        U64 zobrist();
        
        bool isInCheck(Color player);
        
        bool isDraw();
        
        Piece getPiece(Square sq);
        
        void setPiece(Square sq, Piece piece);
        
        void unsetPiece(Square sq);
        
        void replacePiece(Square sq, Piece piece);
        
        void changeActivePlayer();
        
        Color getActivePlayer();
        
        void move(Move m);
        
        void undoMove();
        
        void move_null();
        
        void undoMove_null();
        
        void getPseudoLegalMoves(MoveList *moves);
        
        void getNonQuietMoves(MoveList *moves);
        
        Move getPreviousMove();
        
        Score staticExchangeEvaluation(Move m);
        
        U64 attacksTo(U64 occupied, Square sq);
        
        U64 getAttackedSquares(Color attacker);
        
        U64 getLeastValuablePiece(U64 attadef, Score bySide, Piece &piece);
        
        bool isUnderAttack(Square sq, Color attacker);
        
        bool givesCheck(Move m);
        
        bool isLegal(Move m);
        
        Board copy();
        
        bool getCastlingChance(Square index);
        
        void setCastlingChance(Square index, bool val);
        
        int getCurrentRepetitionCount();
        
        int getCurrent50MoveRuleCount();
        
        Square getEnPassantSquare();
        
        void setEnPassantSquare(Square square);
        
        BoardStatus* getBoardStatus();
        
        U64 *getOccupied();
        
        U64 *getPieces() const;
        
        U64 *getTeamOccupied() const;
        
        
};


#endif //CHESSCOMPUTER_BOARD_H
