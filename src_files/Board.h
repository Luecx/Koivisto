
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
#ifndef CHESSCOMPUTER_BOARD_H
#define CHESSCOMPUTER_BOARD_H

#include "Bitboard.h"
#include "Move.h"
#include "Util.h"
#include "eval.h"
#include "vector"

#include <ostream>
#include <sstream>
#include <stdio.h>
#include <string>

using namespace bb;
using namespace move;

// we cache see entries. this specifies the amount of hashed total entries.
// gives approx 3-5 elo
//#define SEE_CACHE_SIZE 2048

// this struct contains information about the cached see entries.
// this contains zobrist keys and the see-score.
struct seeCacheEntry {
    U64   key;
    Score score;
};

enum CastlingRights{
    WHITE_QUEENSIDE_CASTLING,
    WHITE_KINGSIDE_CASTLING,
    BLACK_QUEENSIDE_CASTLING,
    BLACK_KINGSIDE_CASTLING,
};

// zobrist key for white/black to move
constexpr U64 ZOBRIST_WHITE_BLACK_SWAP = 1;

// static exchange evaluation piece values. we do not use the same values as for the evaluation.
static constexpr Score see_piece_vals[] {100, 325, 325, 500, 1000, 10000};

// the board has an internal status object which keeps track of all information which are not represented by
// the pieces on the board. we compute this when doing a move. when undoing a move, we simply pop the last board status
// from a stack. this contains zobrist keys, en-passant information, castling rights etc.
struct BoardStatus {
    public:
    BoardStatus(const BoardStatus& a)
        : zobrist(a.zobrist), enPassantTarget(a.enPassantTarget), castlingRights(a.castlingRights),
          fiftyMoveCounter(a.fiftyMoveCounter), repetitionCounter(a.repetitionCounter), moveCounter(a.moveCounter),
          move(a.move){}
    
    BoardStatus(U64 p_zobrist, U64 p_enPassantTarget, U64 p_metaInformation, U64 p_fiftyMoveCounter,
                U64 p_repetitionCounter, U64 p_moveCounter, Move p_move)
        : zobrist(p_zobrist), enPassantTarget(p_enPassantTarget), castlingRights(p_metaInformation),
          fiftyMoveCounter(p_fiftyMoveCounter), repetitionCounter(p_repetitionCounter), moveCounter(p_moveCounter),
          move(p_move){}
    
    U64  zobrist{};
    U64  enPassantTarget{};
    U64  castlingRights{};
    U64  fiftyMoveCounter{};
    U64  repetitionCounter{};
    U64  moveCounter{};
    Move move{};
    
    bool operator==(const BoardStatus& rhs) const {
        return  zobrist == rhs.zobrist &&
                enPassantTarget == rhs.enPassantTarget &&
                castlingRights == rhs.castlingRights &&
                fiftyMoveCounter == rhs.fiftyMoveCounter &&
                repetitionCounter == rhs.repetitionCounter &&
                moveCounter == rhs.moveCounter &&
                move == rhs.move;
    }
    
    bool operator!=(const BoardStatus& rhs) const { return !(rhs == *this); }
    
    friend std::ostream& operator<<(std::ostream& os, const BoardStatus& status) {
        os << "zobrist: " << status.zobrist << " castlingRights: " << status.castlingRights
           << " fiftyMoveCounter: " << status.fiftyMoveCounter << " repetitionCounter: " << status.repetitionCounter
           << " move: " << status.move;
        return os;
    }
    
    inline BoardStatus copy() {
        BoardStatus b {zobrist, enPassantTarget, castlingRights, fiftyMoveCounter, repetitionCounter, moveCounter,
                       move};
        return b;
    }
};

class Board {
    private:
    // we store a bitboard for each piece which marks the occupied squares.
    // not that the white pieces come first and the black pawn starts at index=6
    U64 m_piecesBB[N_PIECES];
    // beside the piece occupancy, we also keep track of the occupied squares for each side
    U64 m_teamOccupiedBB[N_COLORS];
    // furthermore we keep track of all the squares occupied. mainly used for move generation.
    U64 m_occupiedBB;
    
    // for caching see entries, we allocate an array.
    // note that this might be very slow if a lot of board objects are requires (e.g. tuning).
    // thats when the cache should be disabled.
#ifdef SEE_CACHE_SIZE
    struct seeCacheEntry seeCache[SEE_CACHE_SIZE] {};
#endif
    
    // store an evaluator which can be efficiently updated
    nn::Evaluator evaluator{};
    
    // we also store the piece for each square.
    Piece m_pieceBoard[N_SQUARES];
    
    // keeping track of the player who has to move
    Color m_activePlayer;
    
    // we have a stack of board-status. We compute a new board status with all relevant meta information with every
    // move. instead of computing the inverse, we simply pop from the stack
    std::vector<BoardStatus> m_boardStatusHistory;
    
    // for each move, we need to compute the current repetition counter.
    // as this is only used internally, there is no need to make this public.
    void computeNewRepetition();
    
    // sets the piece on a given square. considers zobrist-key and all relevant fields.
    template<bool updateNN=true>
    void setPiece(Square sq, Piece piece);
    
    // unsets the piece on a given square. considers zobrist-key and all relevant fields.
    template<bool updateNN=true>
    void unsetPiece(Square sq);
    
    // replaces the piece on a given square. considers zobrist-key and all relevant fields.
    template<bool updateNN=true>
    void replacePiece(Square sq, Piece piece);
    
    public:
    // the default constructor uses a fen-representation of the board. if nothing is specified, the starting position
    // will be used
    Board(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    // instead of providing the fen, we can directly clone a board object.
    Board(Board* board);
    
    // for the sake of completeness, we define a destructor which doesnt do anything.
    virtual ~Board();
    
    // appending the board object to a stream. especially useful for debugging aswell as for uci->print
    friend std::ostream& operator<<(std::ostream& os, Board& board);
    
    // returns the fen of the current board
    std::string fen();
    
    // returns the zobrist key for the current board
    U64 zobrist();
    
    // returns true if the given player is in check.
    bool isInCheck(Color player);
    
    // returns true if the game is a draw by threefold or 50-move rule.
    // does not consider stalemates. this is handled during search as this requires information about each possible
    // move.
    bool isDraw();
    
    // returns the piece on a given square
    Piece getPiece(Square sq);
    
    // changes the active player. not that this does NOT take care of changing the zobrist which is only done
    // during the move(Move m) function.
    void changeActivePlayer();
    
    // returns the active player
    Color getActivePlayer();
    
    // given a move object, does the move on the board. computes repetitions etc.
    void move(Move m);
    
    // undoes the last move. does not require the move as the move is stored within the meta information.
    void undoMove();
    
    // does a null-move
    void move_null();
    
    // undoes a null-move
    void undoMove_null();
    
    // returns the previous move which lead to the current position.
    // this is stored within the meta information.
    Move getPreviousMove(Depth ply = 1);
    
    // computes the static exchange evaluation for a given move. used the cache if defined.
    Score staticExchangeEvaluation(Move m);
    
    // returns a bitboard of all squares which attack a specific square. mainly used for see.
    U64 attacksTo(U64 occupied, Square sq);
    
    // returns a bitboard of all attacked squares by a given color
    template<Color attacker>
    U64 getAttackedSquares();
    
    // returns the least value piece. mainly used for see as well.
    U64 getLeastValuablePiece(U64 attadef, Score bySide, Piece& piece);
    
    // returns a map of all absolute pinned pieces. stores the pieces which pin other pieces inside the given bitboard.
    template<Color side>
    U64 getPinnedPieces(U64& pinners);
    
    // returns true if the given square is attacked by the attacker
    template<Color attacker>
    bool isUnderAttack(Square sq);
    
    // returns true if the given square is attacked by the attacker
    bool isUnderAttack(Square sq, Color attacker);
    
    // returns true if the move gives check
    bool givesCheck(Move m);
    
    // returns true if the given move is legal. this is the only way to check if a move is legal or not.
    // no legal move generation is implemented so this is also used for perft.
    bool isLegal(Move m);
    

    // Checks if the move is likely pseudo-legal. Doesn't cover en-passant, etc.
    bool isPseudoLegal(Move m);
    
    // returns the castling rights for the given index. note that no square is required but an index.
    // for the indices, look at the start of Board.h
    bool getCastlingRights(int index);
    
    // sets the castling rights.  Note that no square is required but an index.
    // for the indices, look at the start of Board.h
    void setCastlingRights(int index, bool val);
    
    // returns how many times this position has occurred in the history of the board.
    int getCurrentRepetitionCount();
    
    // returns the counter for the current 50 move rule.
    int getCurrent50MoveRuleCount();
    
    // returns the square to which e.p. is possible.
    Square getEnPassantSquare();
    
    // one can also set the e.p. square yet this should be avoided.
    void setEnPassantSquare(Square square);
    
    // returns the entire meta information about the board.
    [[nodiscard]] inline BoardStatus* getBoardStatus(){ return &m_boardStatusHistory.back();}
    
    // returns all occupied squares.
    [[nodiscard]] inline U64 getOccupiedBB(){return m_occupiedBB;}
    
    // returns all occupied squares by both teams (arrays with 2 entries).
    [[nodiscard]] inline const U64* getTeamOccupiedBB() const {return m_teamOccupiedBB;}
    
    // returns all occupied squares by the team
    [[nodiscard]] inline U64 getTeamOccupiedBB(Color color) const {return m_teamOccupiedBB[color];}
    
    // returns all occupied squares by the team
    template<Color color>
    [[nodiscard]] inline U64 getTeamOccupiedBB() const {return m_teamOccupiedBB[color];}
    
    // returns the occupied squares by each piece (array with 14 entries).
    [[nodiscard]] inline const U64* getPieceBB() const {return m_piecesBB;};
    
    // does the same as getPieceBB() above yet this only returns a single bitboard.
    [[nodiscard]] inline U64 getPieceBB(Color color, Piece piece) const { return m_piecesBB[color * 8 + piece];}
    
    // does the same as getPieceBB() above yet this only returns a single bitboard.
    template<Color color>
    [[nodiscard]] inline U64 getPieceBB(Piece piece) const{return m_piecesBB[color * 8 + piece];}
    
    template<Color color, PieceType piece_type>
    [[nodiscard]] inline U64 getPieceBB() const{return m_piecesBB[color * 8 + piece_type];}
    
    Score evaluate();
};

#endif    // CHESSCOMPUTER_BOARD_H
