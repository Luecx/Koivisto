//
// Created by finne on 5/14/2020.
//

#ifndef CHESSCOMPUTER_BOARD_H
#define CHESSCOMPUTER_BOARD_H

#include "Bitboard.h"
#include "Move.h"
#include "Util.h"
#include "vector"

#include <ostream>
#include <sstream>    // std::stringstream
#include <stdio.h>
#include <string>

//#define SEE_CACHE_SIZE 2048

using namespace bb;
using namespace move;

constexpr U64 STATUS_INDEX_WHITE_QUEENSIDE_CASTLING = 0;
constexpr U64 STATUS_INDEX_WHITE_KINGSIDE_CASTLING  = 1;
constexpr U64 STATUS_INDEX_BLACK_QUEENSIDE_CASTLING = 2;
constexpr U64 STATUS_INDEX_BLACK_KINGSIDE_CASTLING  = 3;

constexpr U64 ZOBRIST_WHITE_BLACK_SWAP = 1;

static constexpr Score vals[] {100, 325, 325, 500, 1000, 10000};

/**
 * the board contains a list of board status.
 * It contains all the information not represented by the the m_pieces on the board.
 * This contains:
 *   - 50 move rule half counter
 *   - three-fold repetition counter
 *   - zobrist key
 *   - last move
 *   - information about castling rights
 *   - information about the en-passant square
 *
 */

struct seeCacheEntry {
    U64   key;
    Score score;
};

struct BoardStatus {
    public:
    BoardStatus(const BoardStatus& a)
        : zobrist(a.zobrist), enPassantTarget(a.enPassantTarget), metaInformation(a.metaInformation),
          fiftyMoveCounter(a.fiftyMoveCounter), repetitionCounter(a.repetitionCounter), moveCounter(a.moveCounter),
          move(a.move) {}

    BoardStatus(U64 p_zobrist, U64 p_enPassantTarget, U64 p_metaInformation, U64 p_fiftyMoveCounter,
                U64 p_repetitionCounter, U64 p_moveCounter, Move p_move)
        : zobrist(p_zobrist), enPassantTarget(p_enPassantTarget), metaInformation(p_metaInformation),
          fiftyMoveCounter(p_fiftyMoveCounter), repetitionCounter(p_repetitionCounter), moveCounter(p_moveCounter),
          move(p_move) {}

    U64  zobrist;
    U64  enPassantTarget;
    U64  metaInformation;
    U64  fiftyMoveCounter;
    U64  repetitionCounter;
    U64  moveCounter;
    Move move;

    bool operator==(const BoardStatus& rhs) const {
        return zobrist == rhs.zobrist && enPassantTarget == rhs.enPassantTarget
               && metaInformation == rhs.metaInformation && fiftyMoveCounter == rhs.fiftyMoveCounter
               && repetitionCounter == rhs.repetitionCounter && moveCounter == rhs.moveCounter && move == rhs.move;
    }

    bool operator!=(const BoardStatus& rhs) const { return !(rhs == *this); }

    friend std::ostream& operator<<(std::ostream& os, const BoardStatus& status) {
        os << "zobrist: " << status.zobrist << " metaInformation: " << status.metaInformation
           << " fiftyMoveCounter: " << status.fiftyMoveCounter << " repetitionCounter: " << status.repetitionCounter
           << " move: " << status.move;
        return os;
    }

    inline BoardStatus copy() {
        BoardStatus b {zobrist, enPassantTarget, metaInformation, fiftyMoveCounter, repetitionCounter, moveCounter,
                       move};
        return b;
    }
};

class Board {
    private:
    U64 m_pieces[12];
    U64 m_teamOccupied[2];
    U64 m_occupied;

#ifdef SEE_CACHE_SIZE
    struct seeCacheEntry seeCache[SEE_CACHE_SIZE] {};
#endif
    Piece m_pieceBoard[64];

    Color m_activePlayer;

    std::vector<BoardStatus> m_boardStatusHistory;

    void computeNewRepetition();

    public:
    Board(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    Board(Board* board);

    virtual ~Board();

    friend std::ostream& operator<<(std::ostream& os, Board& board);

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

    void getPseudoLegalMoves(MoveList* moves);

    void getNonQuietMoves(MoveList* moves);

    Move getPreviousMove();

    Score staticExchangeEvaluation(Move m);

    U64 attacksTo(U64 occupied, Square sq);

    U64 getAttackedSquares(Color attacker);

    U64 getLeastValuablePiece(U64 attadef, Score bySide, Piece& piece);

    U64 getPinnedPieces(Color color, U64& pinners);

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

    U64* getOccupied();

    const U64* getTeamOccupied() const;

    const U64* getPieces() const;

    U64 getPieces(Color color, Piece piece);
};

#endif    // CHESSCOMPUTER_BOARD_H
