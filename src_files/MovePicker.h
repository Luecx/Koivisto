//
// Created by Luecx on 28.05.2021.
//

#ifndef KOIVISTO_MOVEPICKER_H
#define KOIVISTO_MOVEPICKER_H

#include "Board.h"
#include "History.h"
#include "Move.h"
#include "MoveList.h"
#include "UCIAssert.h"

enum PickerType {
    PV_SEARCH,
    Q_SEARCH,
    PERFT,
};

enum Stage {
    TT_MOVE,
    GENERATE_CAPS,
    GOOD_CAPS,
    GENERATE_PROMO,
    PROMOS,
    GENERATE_QUIETS,
    QUIETS,
    BAD_CAPS,
    NO_MOVES_LEFT
};

template<PickerType type> class MovePicker {
    private:
    Board*         board;
    SearchData*    searchData;
    Color          color;
    Depth          ply;

    Stage          stage;

    move::Move     hashMove = 0;
    move::MoveList goodCaptures {};
    move::MoveList badCaptures {};
    move::MoveList promotions {};
    move::MoveList quiets {};

    public:
    move::MoveList searchedMoves {};
    bool           ignoreQuietScore = false;

    MovePicker(Board* board, SearchData* searchData, Depth ply, Move hashMove) {
        this->board      = board;
        this->searchData = searchData;
        this->ply        = ply;
        this->color      = board->getActivePlayer();
        this->hashMove   = hashMove;
        UCI_ASSERT(type == PV_SEARCH);
        stage = !hashMove ? GENERATE_CAPS : TT_MOVE;
    }

    MovePicker(Board* board) {
        this->board = board;
        this->color = board->getActivePlayer();
        UCI_ASSERT(type != PV_SEARCH);
        generate<GOOD_CAPS>();
        stage = GOOD_CAPS;
        if (goodCaptures.movesLeft() == 0) {
            stage = NO_MOVES_LEFT;
        }
    }

    bool       hasNext() { return stage != NO_MOVES_LEFT; }

    move::Move next() {

        if constexpr (type == PERFT) {
            if (goodCaptures.movesLeft() > 1) {
                return goodCaptures.pollNext();
            } else {
                stage = NO_MOVES_LEFT;
                return goodCaptures.pollNext();
            }
        } else if constexpr (type == Q_SEARCH) {
            if (goodCaptures.movesLeft() > 1) {
                return goodCaptures.pollBest();
            } else {
                stage = NO_MOVES_LEFT;
                return goodCaptures.pollNext();
            }
        } else if constexpr (type == PV_SEARCH) {
            switch (stage) {
                case TT_MOVE:
                    stage = GENERATE_CAPS;
                    if (board->isPseudoLegal(hashMove)) {
                        searchedMoves.add(hashMove);
                        return hashMove;
                    }
                case GENERATE_CAPS: generate<GENERATE_CAPS>(); stage = GOOD_CAPS;
                case GOOD_CAPS:
                    if (goodCaptures.movesLeft()) {
                        Move m = goodCaptures.pollBest();
                        searchedMoves.add(m);
                        return m;
                    } else {
                        stage = GENERATE_PROMO;
                    }
                case GENERATE_PROMO: generate<GENERATE_PROMO>(); stage = PROMOS;
                case PROMOS:
                    if (promotions.movesLeft()) {
                        Move m = promotions.pollBest();
                        searchedMoves.add(m);
                        return m;
                    } else {
                        stage = GENERATE_PROMO;
                    }
                case GENERATE_QUIETS: generate<GENERATE_QUIETS>(); stage = QUIETS;
                case QUIETS:
                    if (quiets.movesLeft()) {
                        Move m;
                        if(ignoreQuietScore)
                            m = quiets.pollNext();
                        else
                            m = quiets.pollBest();
                        searchedMoves.add(m);
                        return m;
                    } else {
                        stage = BAD_CAPS;
                    }
                case BAD_CAPS:
                    if (badCaptures.movesLeft() > 1) {
                        Move m ;
                        if(ignoreQuietScore)
                            m = badCaptures.pollNext();
                        else
                            m = badCaptures.pollBest();
                        searchedMoves.add(m);
                        return m;
                    } else {
                        stage = NO_MOVES_LEFT;
                        return badCaptures.pollNext();
                    }
            }
        }
        return 0;
    }

    template<Stage stage> void addMove(move::Move move) {

        if constexpr (type == PERFT) {
            goodCaptures.add(move);
        } else if constexpr (type == Q_SEARCH) {
            MoveScore mvvLVA = 100 * (getCapturedPiece(move) % 8) - 10 * (getMovingPiece(move) % 8)
                               + (getSquareTo(board->getPreviousMove()) == getSquareTo(move));
            goodCaptures.add(move, 240 + mvvLVA);
        } else if constexpr (type == PV_SEARCH) {
            if constexpr (stage == GENERATE_CAPS) {
                Score     SEE    = board->staticExchangeEvaluation(move);
                MoveScore mvvLVA = MgScore(piece_values[(getCapturedPiece(move) % 8)]);
                if (SEE >= 0) {
                    goodCaptures.add(move,
                                     100000 + mvvLVA
                                         + searchData->getHistories(move, board->getActivePlayer(),
                                                                    board->getPreviousMove()));
                } else {
                    badCaptures.add(move,
                                    10000
                                        + searchData->getHistories(move, board->getActivePlayer(),
                                                                   board->getPreviousMove()));
                }
            } else if constexpr (stage == GENERATE_QUIETS) {
                if (searchData->isKiller(move, ply, color)) {
                    quiets.add(move, 30000 + searchData->isKiller(move, ply, color));
                } else {
                    quiets.add(move, 20000
                                         + searchData->getHistories(move, board->getActivePlayer(),
                                                                    board->getPreviousMove()));
                }
            } else if constexpr (stage == GENERATE_PROMO) {
                MoveScore mvvLVA = (getCapturedPiece(move) % 8) - (getMovingPiece(move) % 8);
                promotions.add(move, 40000 + mvvLVA + getPromotionPiece(move));
            }
        }
    }

    template<Stage stage> void generate() {

        generatePawnMoves<stage>();
        generatePieceMoves<stage>();
        generateKingMoves<stage>();
        
    }

    template<Stage stage> void generatePawnMoves() {

        const Color     us                 = color;
        const Color     them               = !color;

        const U64       relative_rank_8_bb = color == WHITE ? RANK_8_BB : RANK_1_BB;
        const U64       relative_rank_7_bb = color == WHITE ? RANK_7_BB : RANK_2_BB;
        const U64       relative_rank_4_bb = color == WHITE ? RANK_4_BB : RANK_5_BB;

        const Direction forward            = color == WHITE ? NORTH : SOUTH;
        const Direction right              = color == WHITE ? NORTH_EAST : SOUTH_EAST;
        const Direction left               = color == WHITE ? NORTH_WEST : SOUTH_WEST;

        const U64       opponents          = board->getTeamOccupiedBB(them);

        const U64       pawns              = board->getPieceBB(us, PAWN);
        const U64       occupied           = *board->getOccupiedBB();

        const U64       pawnsLeft  = color == WHITE ? shiftNorthWest(pawns) : shiftSouthWest(pawns);
        const U64       pawnsRight = color == WHITE ? shiftNorthEast(pawns) : shiftSouthEast(pawns);
        const U64 pawnsCenter = (color == WHITE ? shiftNorth(pawns) : shiftSouth(pawns)) & ~occupied;

        const Piece movingPiece = us * 8 + PAWN;

        // deal with promotions first
        if (pawns & relative_rank_7_bb) {

            // look at normal promotions if in qsearch, or generating them or for perft
            if constexpr (type == Q_SEARCH || type == PERFT || stage == GENERATE_PROMO) {
                U64 attacks = pawnsCenter & relative_rank_8_bb;
                while (attacks) {
                    Square target = bitscanForward(attacks);
                    addMove<stage>(genMove(target - forward, target, QUEEN_PROMOTION, movingPiece));
                    addMove<stage>(genMove(target - forward, target, ROOK_PROMOTION, movingPiece));
                    addMove<stage>(genMove(target - forward, target, BISHOP_PROMOTION, movingPiece));
                    addMove<stage>(genMove(target - forward, target, KNIGHT_PROMOTION, movingPiece));

                    attacks = lsbReset(attacks);
                }
            }

            // remaining promotions will only be generated if in qsearch, perft or captures
            if constexpr (type == Q_SEARCH || type == PERFT || stage == GENERATE_CAPS) {
                U64 attacks = pawnsLeft & relative_rank_8_bb & opponents;
                while (attacks) {
                    Square target = bitscanForward(attacks);

                    addMove<stage>(genMove(target - left, target, QUEEN_PROMOTION_CAPTURE,
                                           movingPiece, board->getPiece(target)));
                    addMove<stage>(genMove(target - left, target, ROOK_PROMOTION_CAPTURE, movingPiece,
                                           board->getPiece(target)));
                    addMove<stage>(genMove(target - left, target, BISHOP_PROMOTION_CAPTURE,
                                           movingPiece, board->getPiece(target)));
                    addMove<stage>(genMove(target - left, target, KNIGHT_PROMOTION_CAPTURE,
                                           movingPiece, board->getPiece(target)));

                    attacks = lsbReset(attacks);
                }

                attacks = pawnsRight & relative_rank_8_bb & opponents;
                while (attacks) {
                    Square target = bitscanForward(attacks);

                    addMove<stage>(genMove(target - right, target, QUEEN_PROMOTION_CAPTURE,
                                           movingPiece, board->getPiece(target)));
                    addMove<stage>(genMove(target - right, target, ROOK_PROMOTION_CAPTURE,
                                           movingPiece, board->getPiece(target)));
                    addMove<stage>(genMove(target - right, target, BISHOP_PROMOTION_CAPTURE,
                                           movingPiece, board->getPiece(target)));
                    addMove<stage>(genMove(target - right, target, KNIGHT_PROMOTION_CAPTURE,
                                           movingPiece, board->getPiece(target)));
                    attacks = lsbReset(attacks);
                }
            }
        }

        // look at normal captures
        if constexpr (stage == GENERATE_CAPS || type == Q_SEARCH || type == PERFT) {
            U64    nonPromoAttacks = opponents & ~relative_rank_8_bb;
            Square target;

            U64    attacks = pawnsLeft & nonPromoAttacks;
            while (attacks) {
                target = bitscanForward(attacks);
                addMove<stage>(
                    genMove(target - left, target, CAPTURE, movingPiece, board->getPiece(target)));

                attacks = lsbReset(attacks);
            }

            attacks = pawnsRight & nonPromoAttacks;
            while (attacks) {
                target = bitscanForward(attacks);
                addMove<stage>(
                    genMove(target - right, target, CAPTURE, movingPiece, board->getPiece(target)));
                attacks = lsbReset(attacks);
            }
        }

        // look at normal pawn pushes
        if constexpr (stage == GENERATE_QUIETS || type == PERFT) {
            U64 pawnPushes = pawnsCenter & ~relative_rank_8_bb;
            U64 attacks    = pawnPushes;
            while (attacks) {
                Square target = bitscanForward(attacks);
                addMove<stage>(genMove(target - forward, target, QUIET, movingPiece));
                attacks = lsbReset(attacks);
            }

            attacks = (color == WHITE ? shiftNorth(pawnPushes) : shiftSouth(pawnPushes))
                      & relative_rank_4_bb & ~occupied;
            while (attacks) {
                Square target = bitscanForward(attacks);
                addMove<stage>(genMove(target - forward * 2, target, DOUBLED_PAWN_PUSH, movingPiece));
                attacks = lsbReset(attacks);
            }
        }

        // look at e.p. capture
        if constexpr (stage == GENERATE_CAPS || type == Q_SEARCH || type == PERFT) {
            if (pawnsLeft & board->getBoardStatus()->enPassantTarget) {
                Square target = board->getEnPassantSquare();
                addMove<stage>(genMove(target - left, target, EN_PASSANT, movingPiece));
            }

            if (pawnsRight & board->getBoardStatus()->enPassantTarget) {
                Square target = board->getEnPassantSquare();
                addMove<stage>(genMove(target - right, target, EN_PASSANT, movingPiece));
            }
        }
    }

    template<Stage stage> void generatePieceMoves() {
        const Color us       = color;

        const U64   occupied = *board->getOccupiedBB();
        const U64   friendly = board->getTeamOccupiedBB(us);
        const U64   opponent = board->getTeamOccupiedBB(!us);

        for (Piece p = KNIGHT; p <= QUEEN; p++) {
            U64 pieceOcc    = board->getPieceBB(color, p);
            U64 movingPiece = p + 8 * us;
            while (pieceOcc) {
                Square square  = bitscanForward(pieceOcc);
                U64    attacks = ZERO;
                switch (p) {
                    case KNIGHT:
                        attacks = KNIGHT_ATTACKS[square]; break;
                    case BISHOP:
                        attacks = lookUpBishopAttack(square, occupied); break;
                    case ROOK:
                        attacks = lookUpRookAttack(square, occupied); break;
                    case QUEEN:
                        attacks =
                            lookUpBishopAttack(square, occupied) | lookUpRookAttack(square, occupied);
                        break;
                }
                attacks &= ~friendly;
                
                if constexpr (type != PERFT) {
                    if constexpr (stage == GENERATE_CAPS || type == Q_SEARCH) {
                        attacks &= opponent;
                    } else if constexpr (stage == GENERATE_QUIETS) {
                        attacks &= ~opponent;
                    }
                }

                while (attacks) {
                    Square target = bitscanForward(attacks);

                    if constexpr (type == PERFT) {

                        if (board->getPiece(target) != -1) {
                            addMove<stage>(genMove(square, target, CAPTURE, movingPiece,
                                                   board->getPiece(target)));
                        } else {
                            addMove<stage>(genMove(square, target, QUIET, movingPiece));
                        }

                    } else {

                        if constexpr (stage == GENERATE_CAPS || type == Q_SEARCH) {
                            addMove<stage>(genMove(square, target, CAPTURE, movingPiece,
                                                   board->getPiece(target)));
                        } else if constexpr (stage == GENERATE_QUIETS) {
                            addMove<stage>(genMove(square, target, QUIET, movingPiece));
                        }
                    }

                    attacks = lsbReset(attacks);
                }

                pieceOcc = lsbReset(pieceOcc);
            }
        }
    }

    template<Stage stage> void generateKingMoves() {

        const Color us          = color;

        const Piece movingPiece = KING + us * 8;

        const U64   occupied    = *board->getOccupiedBB();
        const U64   friendly    = board->getTeamOccupiedBB(us);

        Square      kingSq      = bitscanForward(board->getPieceBB(us, KING));
        U64         attacks     = KING_ATTACKS[kingSq] & ~friendly;

        if constexpr (type != PERFT) {
            U64 mask = stage == GENERATE_CAPS ?  board->getTeamOccupiedBB(!us)
                                              : ~board->getTeamOccupiedBB(!us);
            attacks &= mask;
        }

        while (attacks) {
            Square target = bitscanForward(attacks);

            if constexpr (type == PERFT) {
                if (board->getPiece(target) != -1) {
                    addMove<stage>(
                        genMove(kingSq, target, CAPTURE, movingPiece, board->getPiece(target)));
                } else {
                    addMove<stage>(genMove(kingSq, target, QUIET, movingPiece));
                }
            } else {
                if constexpr (stage == GENERATE_CAPS) {
                    addMove<stage>(
                        genMove(kingSq, target, CAPTURE, movingPiece, board->getPiece(target)));
                } else if constexpr (stage == GENERATE_QUIETS) {
                    addMove<stage>(genMove(kingSq, target, QUIET, movingPiece));
                }
            }
            attacks = lsbReset(attacks);
        }

        if constexpr (type == PERFT || stage == GENERATE_QUIETS) {
            if (color == WHITE) {
                if (board->getCastlingRights(WHITE_QUEENSIDE_CASTLING)
                    && board->getPiece(A1) == WHITE_ROOK
                    && (occupied & CASTLING_WHITE_QUEENSIDE_MASK) == 0) {

                    addMove<stage>(genMove(E1, C1, QUEEN_CASTLE, WHITE_KING));
                }
                if (board->getCastlingRights(WHITE_KINGSIDE_CASTLING)
                    && board->getPiece(H1) == WHITE_ROOK
                    && (occupied & CASTLING_WHITE_KINGSIDE_MASK) == 0) {

                    addMove<stage>(genMove(E1, G1, KING_CASTLE, WHITE_KING));
                }

            } else {

                if (board->getCastlingRights(BLACK_QUEENSIDE_CASTLING)
                    && board->getPiece(A8) == BLACK_ROOK
                    && (occupied & CASTLING_BLACK_QUEENSIDE_MASK) == 0) {

                    addMove<stage>(genMove(E8, C8, QUEEN_CASTLE, BLACK_KING));
                }
                if (board->getCastlingRights(BLACK_KINGSIDE_CASTLING)
                    && board->getPiece(H8) == BLACK_ROOK
                    && (occupied & CASTLING_BLACK_KINGSIDE_MASK) == 0) {

                    addMove<stage>(genMove(E8, G8, KING_CASTLE, BLACK_KING));
                }
            }
        }
    }
};

#endif    // KOIVISTO_MOVEPICKER_H
