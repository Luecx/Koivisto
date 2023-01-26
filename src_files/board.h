
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

#include "bitboard.h"
#include "move.h"
#include "util.h"
#include "transpositiontable.h"
#include "eval.h"
#include "vector"

#include <ostream>
#include <sstream>
#include <stdio.h>
#include <string>

enum CastlingRights{
    WHITE_QUEENSIDE_CASTLING,
    WHITE_KINGSIDE_CASTLING,
    BLACK_QUEENSIDE_CASTLING,
    BLACK_KINGSIDE_CASTLING,
};

// zobrist key for white/black to move
constexpr bb::U64 ZOBRIST_WHITE_BLACK_SWAP = 1;

// static exchange evaluation piece values. we do not use the same values as for the evaluation.
static constexpr bb::Score see_piece_vals[] {100, 325, 325, 500, 1000, 10000};

// the board has an internal status object which keeps track of all information which are not represented by
// the pieces on the board. we compute this when doing a move. when undoing a move, we simply pop the last board status
// from a stack. this contains zobrist keys, en-passant information, castling rights etc.
struct BoardStatus {
    public:
    bb::U64    zobrist {};
    bb::U64    enPassantTarget {};
    uint32_t   castlingRights {};
    uint32_t   fiftyMoveCounter {};
    uint32_t   repetitionCounter {};
    uint32_t   moveCounter {};
    move::Move move {};
};

struct BoardStatusHistory{
    BoardStatus history[bb::MAX_INTERNAL_PLY * 2] {};
    int size;
    
    void push(const BoardStatus& new_entry){
        history[size++] = new_entry;
    }
    
    void pop(){
        size --;
    }
    
    void clear(){
        size = 0;
    }
    
    BoardStatus& operator[](uint32_t idx){
        return history[idx];
    }
    BoardStatus operator[](uint32_t idx) const{
        return history[idx];
    }
    
    BoardStatus& current(){
        return history[size-1];
    }
    BoardStatus current() const{
        return history[size-1];
    }
};

class Board {
    private:
    // we store a bitboard for each piece which marks the occupied squares.
    // not that the white pieces come first and the black pawn starts at index=6
    bb::U64 m_piecesBB[bb::N_PIECES];
    // beside the piece occupancy, we also keep track of the occupied squares for each side
    bb::U64 m_teamOccupiedBB[bb::N_COLORS];
    // furthermore we keep track of all the squares occupied. mainly used for move generation.
    bb::U64 m_occupiedBB;
    
    // store an evaluator which can be efficiently updated
    nn::Evaluator evaluator{};
    
    // we also store the piece for each square.
    bb::Piece m_pieceBoard[bb::N_SQUARES];
    
    // keeping track of the player who has to move
    bb::Color m_activePlayer;
    
    // we have a stack of board-status. We compute a new board status with all relevant meta information with every
    // move. instead of computing the inverse, we simply pop from the stack
//    std::vector<BoardStatus> m_boardStatusHistory;
    BoardStatusHistory m_boardStatusHistory{};
    
    
    // for each move, we need to compute the current repetition counter.
    // as this is only used internally, there is no need to make this public.
    void computeNewRepetition();
    
    public:
    // sets the piece on a given square. considers zobrist-key and all relevant fields.
    template<bool updateNN=true, bool updateZobrist=true>
    void setPiece(bb::Square sq, bb::Piece piece);
    
    // unsets the piece on a given square. considers zobrist-key and all relevant fields.
    template<bool updateNN=true, bool updateZobrist=true>
    void unsetPiece(bb::Square sq);
    
    // replaces the piece on a given square. considers zobrist-key and all relevant fields.
    template<bool updateNN=true, bool updateZobrist=true>
    void replacePiece(bb::Square sq, bb::Piece piece);

    private:
    // sets the piece on a given square. considers zobrist-key and all relevant fields.
    void setPieceHash(bb::Square sq, bb::Piece piece);
    
    // unsets the piece on a given square. considers zobrist-key and all relevant fields.
    void unsetPieceHash(bb::Square sq);
    
    // replaces the piece on a given square. considers zobrist-key and all relevant fields.
    void replacePieceHash(bb::Square sq, bb::Piece piece);
    
    
    public:
    // the default constructor uses a fen-representation of the board. if nothing is specified, the starting position
    // will be used
    Board(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    // instead of providing the fen, we can directly clone a board object.
    Board(const Board& board);
    Board& operator=(const Board& board);
    
    // for the sake of completeness, we define a destructor which doesnt do anything.
    ~Board() = default;
    
    // appending the board object to a stream. especially useful for debugging aswell as for uci->print
    friend std::ostream& operator<<(std::ostream& os, Board& board);
    
    // returns the fen of the current board
    [[nodiscard]] std::string fen() const;
    
    // returns the zobrist key for the current board
    [[nodiscard]] bb::U64 zobrist() const;
    
    // returns true if the given player is in check.
    bool isInCheck(bb::Color player) const;
    
    // returns true if the game is a draw by threefold or 50-move rule.
    // does not consider stalemates. this is handled during search as this requires information about each possible
    // move.
    bool isDraw() const;
    
   
    
    // returns the piece on a given square
    [[nodiscard]] bb::Piece getPiece(bb::Square sq) const;
    
    // changes the active player. not that this does NOT take care of changing the zobrist which is only done
    // during the move(move::Move m) function.
    void changeActivePlayer();
    
    // returns the active player
    [[nodiscard]] bb::Color getActivePlayer() const;
    
    // given a move object, does the move on the board. computes repetitions etc.
    template<bool prefetch=false>
    void move(move::Move m, TranspositionTable* table = nullptr);
    
    // undoes the last move. does not require the move as the move is stored within the meta information.
    void undoMove();
    
    // does a null-move
    void move_null();
    
    // undoes a null-move
    void undoMove_null();
    
    // returns the previous move which lead to the current position.
    // this is stored within the meta information.
    [[nodiscard]] move::Move getPreviousMove(bb::Depth ply = 1) const;
    
    // computes the static exchange evaluation for a given move. used the cache if defined.
    [[nodiscard]] bb::Score staticExchangeEvaluation(move::Move m) const;
    
    // returns a bitboard of all squares which attack a specific square. mainly used for see.
    [[nodiscard]] bb::U64 attacksTo(bb::U64 occupied, bb::Square sq) const;
    
    // returns a bitboard of all attacked squares by a given color
    template<bb::Color attacker>
    [[nodiscard]] bb::U64 getAttackedSquares() const;
    
    // returns the least value piece. mainly used for see as well.
    [[nodiscard]] bb::U64 getLeastValuablePiece(bb::U64 attadef, bb::Score bySide, bb::Piece& piece) const;
    
    // returns a map of all absolute pinned pieces. stores the pieces which pin other pieces inside the given bitboard.
    template<bb::Color side>
    [[nodiscard]] bb::U64 getPinnedPieces(bb::U64& pinners) const;
    
    // returns true if the given square is attacked by the attacker
    template<bb::Color attacker>
    [[nodiscard]] bool isUnderAttack(bb::Square sq) const;
    
    // returns true if the given square is attacked by the attacker
    [[nodiscard]] bool isUnderAttack(bb::Square sq, bb::Color attacker) const;
    
    // returns true if the move gives check
    [[nodiscard]] bool givesCheck(move::Move m);
    
    // returns true if the given move is legal. this is the only way to check if a move is legal or not.
    // no legal move generation is implemented so this is also used for perft.
    [[nodiscard]] bool isLegal(move::Move m);

    // Checks if the move is likely pseudo-legal. Doesn't cover en-passant, etc.
    [[nodiscard]] bool isPseudoLegal(move::Move m) const;
    
    // returns the castling rights for the given index. note that no square is required but an index.
    // for the indices, look at the start of Board.h
    [[nodiscard]] bool getCastlingRights(int index) const;
    
    // sets the castling rights.  Note that no square is required but an index.
    // for the indices, look at the start of Board.h
    void setCastlingRights(int index, bool val);
    
    // returns how many times this position has occurred in the history of the board.
    [[nodiscard]] int getCurrentRepetitionCount() const;
    
    // returns the counter for the current 50 move rule.
    [[nodiscard]] int getCurrent50MoveRuleCount() const;
    
    // returns the square to which e.p. is possible.
    [[nodiscard]] bb::Square getEnPassantSquare() const;
    
    // one can also set the e.p. square yet this should be avoided.
    void setEnPassantSquare(bb::Square square);
    
    // clears the history objects associated with this struct
    void clearHistories();
    
//    // returns the entire meta information about the board.
//    [[nodiscard]] inline BoardStatus* getBoardStatus() { return &m_boardStatusHistory.current();}
//
//    // returns the entire meta information about the board.
//    [[nodiscard]] inline BoardStatus getBoardStatus() const { return m_boardStatusHistory.current();}
    
    // returns all occupied squares.
    [[nodiscard]] inline bb::U64 getOccupiedBB() const {return m_occupiedBB;}
    
    // returns all occupied squares by both teams (arrays with 2 entries).
    [[nodiscard]] inline const bb::U64* getTeamOccupiedBB() const {return m_teamOccupiedBB;}
    
    // returns all occupied squares by the team
    [[nodiscard]] inline bb::U64 getTeamOccupiedBB(bb::Color color) const {return m_teamOccupiedBB[color];}
    
    // returns all occupied squares by the team
    template<bb::Color color>
    [[nodiscard]] inline bb::U64 getTeamOccupiedBB() const {return m_teamOccupiedBB[color];}
    
    // returns the occupied squares by each piece (array with 14 entries).
    [[nodiscard]] inline const bb::U64* getPieceBB() const {return m_piecesBB;};
    
    // does the same as getPieceBB() above yet this only returns a single bitboard.
    [[nodiscard]] inline bb::U64 getPieceBB(bb::Color color, bb::Piece piece) const { return m_piecesBB[color * 8 + piece];}
    
    // does the same as getPieceBB() above yet this only returns a single bitboard.
    template<bb::Color color>
    [[nodiscard]] inline bb::U64 getPieceBB(bb::Piece piece) const {return m_piecesBB[color * 8 + piece];}
    
    template<bb::Color color, bb::PieceType piece_type>
    [[nodiscard]] inline bb::U64 getPieceBB() const {return m_piecesBB[color * 8 + piece_type];}
    
    template<bb::PieceType piece_type>
    [[nodiscard]] inline bb::U64 getPieceTypeBB() const {return getPieceBB<bb::WHITE, piece_type>() | getPieceBB<bb::BLACK, piece_type>();}
    
    // returns the board status
    [[nodiscard]] inline BoardStatus& getBoardStatus(){
        return m_boardStatusHistory.current();
    }
    
    
    [[nodiscard]] bb::Score evaluate();
};

#endif    // CHESSCOMPUTER_BOARD_H
