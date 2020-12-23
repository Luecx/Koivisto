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

#include "MovePicker.h"
#include "Move.h"
#include "Board.h"
#include "History.h"


// Kind of init movePicker. Actually just sets hashMove if there is one and resets index variabels. 
void MovePicker::init(move::Move hashMove) {
    stage               = hashMove ? HASH_MOVE : GENERATE_CAPTURE;
    eque[0]             = hashMove;
    hash_move           = hashMove;
    tried_index         = 0;
    quiet_eque_index    = 100;
    capture_eque_index  = 0;
}

// Get next move in pv/qsearch
move::Move MovePicker::nextMove(Board* b, SearchData* sd, Depth ply) {
    switch (stage) {

        case HASH_MOVE:
            addMoveToTried(eque[0]);
            stage = GENERATE_CAPTURE;
            return eque[0];

        case GENERATE_CAPTURE:
            getCaptureMoves(b);
            scoreCaptures(b, sd);
            stage = GOOD_CAPTURE;
        case GOOD_CAPTURE:
            return pickCapture(b, sd);

        case GENERATE_SILENT:
            // In case of qsearch do not generate silent moves. Note that all bad captures will be pruned in qsearch anyway
            if (type == QSEARCH)
                return 0;
            getSilentMoves(b);
            scoreQuiets(b, sd, ply);
            stage = SILENT;
        case SILENT:
            return pickQuiet(b, sd);

        case BAD_CAPTURE:
            return pickCapture(b, sd);

        default:
            return 0;
    }
}

// Add move to tried (used for updating history stats)
void MovePicker::addMoveToTried(move::Move m) {
    tried[tried_index]              = m;
    tried_index++;
}

// Add quiet move to eque 
void MovePicker::addQuietMoveToEque(move::Move m) {
    if (m != hash_move) {
        eque[quiet_eque_index]      = m;
        quiet_eque_index++;
    }
}

// Add move capture move to eque
void MovePicker::addCaptureMoveToEque(move::Move m) {
    if (m != hash_move) {
        eque[capture_eque_index]    = m;
        capture_eque_index++;
    }
}

// Score equed moves as captures
void MovePicker::scoreCaptures(Board* b, SearchData* sd) {
    Move m;
    for (int i = 0; i < capture_eque_index; i++) {
        m               = eque[i];
        Score SEE       = b->staticExchangeEvaluation(m);
        scores[i]       = (SEE > 0 ? 1e5 : 1e4) + 100 * (getCapturedPiece(m) % 6) - 10 * (getMovingPiece(m) % 6)
                        + (getSquareTo(b->getPreviousMove()) == getSquareTo(m));
    }
}

// Score equed moves as silent moves
void MovePicker::scoreQuiets(Board* b, SearchData* sd, Depth ply) {
    for (int i = 100; i < quiet_eque_index; i++)
        scores[i]       = (sd->isKiller(eque[i], ply, b->getActivePlayer()) ? 30000 : 20000 + sd->getHistories(eque[i], b->getActivePlayer(), b->getPreviousMove()));
}

// Pick best capture from moveList
move::Move MovePicker::pickCapture(Board* b, SearchData* sd) {
    int best = 0;
    for (int i = 1; i < capture_eque_index; i++) {
        if (scores[i] > scores[best])
            best = i;
    }
    // If best wasnt a good capture get a quiet move instead
    if (stage == GOOD_CAPTURE && scores[best] < 1e5) {
        stage = GENERATE_SILENT;
        return nextMove(b, sd);
    }
    Move m              = eque[best];
    tried[tried_index]  = m;
    tried_index++;
    scores[best]        = 0;
    eque[best]          = 0;
    return m;
}

move::Move MovePicker::pickQuiet(Board* b, SearchData* sd) {
    int best = 100;
    for (int i = 101; i < quiet_eque_index; i++) {
        if (scores[i]>scores[best])
            best = i;
    }
    // If ther was no move left get a quiet move instead
    if (eque[best] == 0) {
        stage = BAD_CAPTURE;
        return nextMove(b, sd);
    }
    Move m              = eque[best];
    scores[best]        = 0;
    tried[tried_index]  = eque[best];
    tried_index++;
    eque[best]          = 0;
    return m;
}


void MovePicker::getSilentMoves(Board* b) {

    U64 attackableSquares;
    U64 opponents;

    int pieceOffset = 6 * b->getActivePlayer();

    U64 knights;
    U64 bishops;
    U64 rooks;
    U64 queens;
    U64 kings;

    Square s;
    Square target;
    U64    attacks;

    if (b->getActivePlayer() == WHITE) {


        attackableSquares = ~(*b->getOccupied());
        opponents         = 0;

        knights = b->getPieces()[WHITE_KNIGHT];
        bishops = b->getPieces()[WHITE_BISHOP];
        rooks   = b->getPieces()[WHITE_ROOK];
        queens  = b->getPieces()[WHITE_QUEEN];
        kings   = b->getPieces()[WHITE_KING];

        U64 pawnsLeft   = shiftNorthWest(b->getPieces()[WHITE_PAWN] & ~FILE_A);
        U64 pawnsRight  = shiftNorthEast(b->getPieces()[WHITE_PAWN] & ~FILE_H);
        U64 pawnsCenter = shiftNorth(b->getPieces()[WHITE_PAWN]) & ~ *b->getOccupied();

        attacks = pawnsLeft & opponents & ~RANK_8;

        U64 h   = pawnsCenter & ~RANK_8;
        attacks = h;
        while (attacks) {
            target = bitscanForward(attacks);
            addQuietMoveToEque(genMove(target - 8, target, QUIET, WHITE_PAWN));
            attacks = lsbReset(attacks);
        }

        attacks = shiftNorth(h) & RANK_4 & ~ *b->getOccupied();
        while (attacks) {
            target = bitscanForward(attacks);
            addQuietMoveToEque(genMove(target - 16, target, DOUBLED_PAWN_PUSH, WHITE_PAWN));
            attacks = lsbReset(attacks);
        }

        if (b->getPieces()[WHITE_PAWN] & RANK_7) {
            attacks = pawnsCenter & RANK_8;
            while (attacks) {
                target = bitscanForward(attacks);
                addQuietMoveToEque(genMove(target - 8, target, ROOK_PROMOTION, WHITE_PAWN));
                addQuietMoveToEque(genMove(target - 8, target, BISHOP_PROMOTION, WHITE_PAWN));
                addQuietMoveToEque(genMove(target - 8, target, KNIGHT_PROMOTION, WHITE_PAWN));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsLeft & RANK_8 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                addQuietMoveToEque(genMove(target - 7, target, ROOK_PROMOTION_CAPTURE, WHITE_PAWN, b->getPiece(target)));
                addQuietMoveToEque(genMove(target - 7, target, BISHOP_PROMOTION_CAPTURE, WHITE_PAWN, b->getPiece(target)));
                addQuietMoveToEque(genMove(target - 7, target, KNIGHT_PROMOTION_CAPTURE, WHITE_PAWN, b->getPiece(target)));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsRight & RANK_8 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                addQuietMoveToEque(genMove(target - 9, target, ROOK_PROMOTION_CAPTURE, WHITE_PAWN, b->getPiece(target)));
                addQuietMoveToEque(genMove(target - 9, target, BISHOP_PROMOTION_CAPTURE, WHITE_PAWN, b->getPiece(target)));
                addQuietMoveToEque(genMove(target - 9, target, KNIGHT_PROMOTION_CAPTURE, WHITE_PAWN, b->getPiece(target)));
                attacks = lsbReset(attacks);
            }
        }
    } else {

        attackableSquares = ~(*b->getOccupied());
        opponents         = 0;

        knights = b->getPieces()[BLACK_KNIGHT];
        bishops = b->getPieces()[BLACK_BISHOP];
        rooks   = b->getPieces()[BLACK_ROOK];
        queens  = b->getPieces()[BLACK_QUEEN];
        kings   = b->getPieces()[BLACK_KING];

        U64 pawnsLeft   = shiftSouthWest(b->getPieces()[BLACK_PAWN] & ~FILE_A);
        U64 pawnsRight  = shiftSouthEast(b->getPieces()[BLACK_PAWN] & ~FILE_H);
        U64 pawnsCenter = shiftSouth(b->getPieces()[BLACK_PAWN]) & ~ *b->getOccupied();

        U64 h   = pawnsCenter & ~RANK_1;
        attacks = h;
        while (attacks) {
            target = bitscanForward(attacks);
            addQuietMoveToEque(genMove(target + 8, target, QUIET, BLACK_PAWN));
            attacks = lsbReset(attacks);
        }

        attacks = shiftSouth(h) & RANK_5 & ~ *b->getOccupied();
        while (attacks) {
            target = bitscanForward(attacks);
            addQuietMoveToEque(genMove(target + 16, target, DOUBLED_PAWN_PUSH, BLACK_PAWN));
            attacks = lsbReset(attacks);
        }
            
        if (b->getPieces()[BLACK_PAWN] & RANK_2) {
            attacks = pawnsCenter & RANK_1;
            while (attacks) {
                target = bitscanForward(attacks);
                addQuietMoveToEque(genMove(target + 8, target, ROOK_PROMOTION, BLACK_PAWN));
                addQuietMoveToEque(genMove(target + 8, target, BISHOP_PROMOTION, BLACK_PAWN));
                addQuietMoveToEque(genMove(target + 8, target, KNIGHT_PROMOTION, BLACK_PAWN));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsLeft & RANK_1 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                addQuietMoveToEque(genMove(target + 9, target, ROOK_PROMOTION_CAPTURE, BLACK_PAWN, b->getPiece(target)));
                addQuietMoveToEque(genMove(target + 9, target, BISHOP_PROMOTION_CAPTURE, BLACK_PAWN, b->getPiece(target)));
                addQuietMoveToEque(genMove(target + 9, target, KNIGHT_PROMOTION_CAPTURE, BLACK_PAWN, b->getPiece(target)));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsRight & RANK_1 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                addQuietMoveToEque(genMove(target + 7, target, ROOK_PROMOTION_CAPTURE, BLACK_PAWN, b->getPiece(target)));
                addQuietMoveToEque(genMove(target + 7, target, BISHOP_PROMOTION_CAPTURE, BLACK_PAWN, b->getPiece(target)));
                addQuietMoveToEque(genMove(target + 7, target, KNIGHT_PROMOTION_CAPTURE, BLACK_PAWN, b->getPiece(target)));
                attacks = lsbReset(attacks);
            }
        }
    }

    while (knights) {
        s       = bitscanForward(knights);
        attacks = KNIGHT_ATTACKS[s] & attackableSquares;
        while (attacks) {
            target = bitscanForward(attacks);

            addQuietMoveToEque(genMove(s, target, QUIET, KNIGHT + pieceOffset));

            attacks = lsbReset(attacks);
        }
        knights = lsbReset(knights);
    }
    while (bishops) {
        s       = bitscanForward(bishops);
        attacks = lookUpBishopAttack(s, *b->getOccupied()) & attackableSquares;
        while (attacks) {
            target = bitscanForward(attacks);

            addQuietMoveToEque(genMove(s, target, QUIET, BISHOP + pieceOffset));

            attacks = lsbReset(attacks);
        }
        bishops = lsbReset(bishops);
    }

    while (rooks) {
        s       = bitscanForward(rooks);
        attacks = lookUpRookAttack(s, *b->getOccupied()) & attackableSquares;

        while (attacks) {
            target = bitscanForward(attacks);

            addQuietMoveToEque(genMove(s, target, QUIET, ROOK + pieceOffset));

            attacks = lsbReset(attacks);
        }
        rooks = lsbReset(rooks);
    }

    while (queens) {
        s = bitscanForward(queens);

        attacks = (lookUpRookAttack(s, *b->getOccupied()) | lookUpBishopAttack(s, *b->getOccupied())) & attackableSquares;

        while (attacks) {
            target = bitscanForward(attacks);

            addQuietMoveToEque(genMove(s, target, QUIET, QUEEN + pieceOffset));
           
            attacks &= (attacks - 1);
        }

        queens &= (queens - 1);
    }

    while (kings) {
        s       = bitscanForward(kings);
        attacks = KING_ATTACKS[s] & attackableSquares;
        while (attacks) {
            target = bitscanForward(attacks);

            addQuietMoveToEque(genMove(s, target, QUIET, KING + pieceOffset));

            attacks = lsbReset(attacks);
        }

        if (b->getActivePlayer() == WHITE) {

            if (b->getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING) && b->getPiece(A1) == WHITE_ROOK
                && (*b->getOccupied() & CASTLING_WHITE_QUEENSIDE_MASK) == 0) {
                addQuietMoveToEque(genMove(E1, C1, QUEEN_CASTLE, WHITE_KING));
            }
            if (b->getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING) && b->getPiece(H1) == WHITE_ROOK
                && (*b->getOccupied() & CASTLING_WHITE_KINGSIDE_MASK) == 0) {
                addQuietMoveToEque(genMove(E1, G1, KING_CASTLE, WHITE_KING));
            }

        } else {

            if (b->getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING) && b->getPiece(A8) == BLACK_ROOK
                && (*b->getOccupied() & CASTLING_BLACK_QUEENSIDE_MASK) == 0) {

                addQuietMoveToEque(genMove(E8, C8, QUEEN_CASTLE, BLACK_KING));
            }
            if (b->getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING) && b->getPiece(H8) == BLACK_ROOK
                && (*b->getOccupied() & CASTLING_BLACK_KINGSIDE_MASK) == 0) {
                addQuietMoveToEque(genMove(E8, G8, KING_CASTLE, BLACK_KING));
            }
        }
        kings = lsbReset(kings);
    }
}

void MovePicker::getCaptureMoves(Board* b) {
    U64 opponents;

    int pieceOffset = 6 * b->getActivePlayer();

    U64 knights;
    U64 bishops;
    U64 rooks;
    U64 queens;
    U64 kings;

    Square s;
    Square target;
    U64    attacks;

    if (b->getActivePlayer() == WHITE) {

        opponents = b->getTeamOccupied()[BLACK];

        knights = b->getPieces()[WHITE_KNIGHT];
        bishops = b->getPieces()[WHITE_BISHOP];
        rooks   = b->getPieces()[WHITE_ROOK];
        queens  = b->getPieces()[WHITE_QUEEN];
        kings   = b->getPieces()[WHITE_KING];

        U64 pawnsLeft   = shiftNorthWest(b->getPieces()[WHITE_PAWN] & ~FILE_A);
        U64 pawnsRight  = shiftNorthEast(b->getPieces()[WHITE_PAWN] & ~FILE_H);
        U64 pawnsCenter = shiftNorth(b->getPieces()[WHITE_PAWN]) & ~*b->getOccupied();

        attacks = pawnsLeft & opponents & ~RANK_8;
        while (attacks) {
            target = bitscanForward(attacks);
            addCaptureMoveToEque(genMove(target - 7, target, CAPTURE, WHITE_PAWN, b->getPiece(target)));
            attacks = lsbReset(attacks);
        }

        attacks = pawnsRight & opponents & ~RANK_8;
        while (attacks) {
            target = bitscanForward(attacks);
           addCaptureMoveToEque(genMove(target - 9, target, CAPTURE, WHITE_PAWN, b->getPiece(target)));
            attacks = lsbReset(attacks);
        }

        if (pawnsLeft & b->getBoardStatus()->enPassantTarget) {
            target = b->getEnPassantSquare();
            addCaptureMoveToEque(genMove(target - 7, target, EN_PASSANT, WHITE_PAWN));
            attacks = lsbReset(attacks);
        }

        if (pawnsRight & b->getBoardStatus()->enPassantTarget) {
            target = b->getEnPassantSquare();
            addCaptureMoveToEque(genMove(target - 9, target, EN_PASSANT, WHITE_PAWN));
            attacks = lsbReset(attacks);
        }

        if (b->getPieces()[WHITE_PAWN] & RANK_7) {
            attacks = pawnsCenter & RANK_8;
            while (attacks) {
                target = bitscanForward(attacks);
                addCaptureMoveToEque(genMove(target - 8, target, QUEEN_PROMOTION, WHITE_PAWN));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsLeft & RANK_8 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                addCaptureMoveToEque(genMove(target - 7, target, QUEEN_PROMOTION_CAPTURE, WHITE_PAWN, b->getPiece(target)));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsRight & RANK_8 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                addCaptureMoveToEque(genMove(target - 9, target, QUEEN_PROMOTION_CAPTURE, WHITE_PAWN, b->getPiece(target)));
                attacks = lsbReset(attacks);
            }
        }

    } else {

        opponents = b->getTeamOccupied()[WHITE];

        knights = b->getPieces()[BLACK_KNIGHT];
        bishops = b->getPieces()[BLACK_BISHOP];
        rooks   = b->getPieces()[BLACK_ROOK];
        queens  = b->getPieces()[BLACK_QUEEN];
        kings   = b->getPieces()[BLACK_KING];

        U64 pawnsLeft   = shiftSouthWest(b->getPieces()[BLACK_PAWN] & ~FILE_A);
        U64 pawnsRight  = shiftSouthEast(b->getPieces()[BLACK_PAWN] & ~FILE_H);
        U64 pawnsCenter = shiftSouth(b->getPieces()[BLACK_PAWN]) & ~*b->getOccupied();

        attacks = pawnsLeft & opponents & ~RANK_1;
        while (attacks) {
            target = bitscanForward(attacks);
            addCaptureMoveToEque(genMove(target + 9, target, CAPTURE, BLACK_PAWN, b->getPiece(target)));
            attacks = lsbReset(attacks);
        }

        attacks = pawnsRight & opponents & ~RANK_1;
        while (attacks) {
            target = bitscanForward(attacks);
            addCaptureMoveToEque(genMove(target + 7, target, CAPTURE, BLACK_PAWN, b->getPiece(target)));
            attacks = lsbReset(attacks);
        }

        if (pawnsLeft & b->getBoardStatus()->enPassantTarget) {
            target = b->getEnPassantSquare();
            addCaptureMoveToEque(genMove(target + 9, target, EN_PASSANT, BLACK_PAWN));
        }

        if (pawnsRight & b->getBoardStatus()->enPassantTarget) {
            target = b->getEnPassantSquare();
            addCaptureMoveToEque(genMove(target + 7, target, EN_PASSANT, BLACK_PAWN));
        }

        if (b->getPieces()[BLACK_PAWN] & RANK_2) {
            attacks = pawnsCenter & RANK_1;
            while (attacks) {
                target = bitscanForward(attacks);
                addCaptureMoveToEque(genMove(target + 8, target, QUEEN_PROMOTION, BLACK_PAWN));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsLeft & RANK_1 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                addCaptureMoveToEque(genMove(target + 9, target, QUEEN_PROMOTION_CAPTURE, BLACK_PAWN, b->getPiece(target)));
                attacks = lsbReset(attacks);
            }

            attacks = pawnsRight & RANK_1 & opponents;
            while (attacks) {
                target = bitscanForward(attacks);
                addCaptureMoveToEque(genMove(target + 7, target, QUEEN_PROMOTION_CAPTURE, BLACK_PAWN, b->getPiece(target)));
                attacks = lsbReset(attacks);
            }
        }
    }

    while (knights) {
        s       = bitscanForward(knights);
        attacks = KNIGHT_ATTACKS[s] & opponents;
        while (attacks) {
            target = bitscanForward(attacks);
            addCaptureMoveToEque(genMove(s, target, CAPTURE, KNIGHT + pieceOffset, b->getPiece(target)));
            attacks = lsbReset(attacks);
        }
        knights = lsbReset(knights);
    }
    while (bishops) {
        s       = bitscanForward(bishops);
        attacks = lookUpBishopAttack(s, *b->getOccupied()) & opponents;
        while (attacks) {
            target = bitscanForward(attacks);
            if (b->getPiece(target) >= 0) {
                addCaptureMoveToEque(genMove(s, target, CAPTURE, BISHOP + pieceOffset, b->getPiece(target)));
            } else {
                addCaptureMoveToEque(genMove(s, target, QUIET, BISHOP + pieceOffset));
            }
            attacks = lsbReset(attacks);
        }
        bishops = lsbReset(bishops);
    }
    while (rooks) {
        s       = bitscanForward(rooks);
        attacks = lookUpRookAttack(s, *b->getOccupied()) & opponents;

        while (attacks) {
            target = bitscanForward(attacks);

            addCaptureMoveToEque(genMove(s, target, CAPTURE, ROOK + pieceOffset, b->getPiece(target)));

            attacks = lsbReset(attacks);
        }
        rooks = lsbReset(rooks);
    }
    while (queens) {
        s = bitscanForward(queens);

        attacks = (lookUpRookAttack(s, *b->getOccupied()) | lookUpBishopAttack(s, *b->getOccupied())) & opponents;

        while (attacks) {
            target = bitscanForward(attacks);

            addCaptureMoveToEque(genMove(s, target, CAPTURE, QUEEN + pieceOffset, b->getPiece(target)));

            attacks &= (attacks - 1);
        }

        queens &= (queens - 1);
    }
    while (kings) {
        s       = bitscanForward(kings);
        attacks = KING_ATTACKS[s] & opponents;
        while (attacks) {
            target = bitscanForward(attacks);

            addCaptureMoveToEque(genMove(s, target, CAPTURE, KING + pieceOffset, b->getPiece(target)));

            attacks = lsbReset(attacks);
        }

        kings = lsbReset(kings);
    }
}
