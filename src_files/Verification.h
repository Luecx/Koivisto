
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

#ifndef KOIVISTO_VERIFICATION_H
#define KOIVISTO_VERIFICATION_H

#include "Board.h"
#include "Move.h"

/**
 * verifies the given static-exchange evaluation case. requires a fen and a move and compares the see output with the
 * expected output
 * @param fen
 * @param m
 * @param s
 */
void verifySEECase(std::string fen, Move m, Score s) {
    Board* b = new Board(fen);

    Score see = b->staticExchangeEvaluation(m);

    if (see != s) {
        std::cerr << "problem in: " << fen << " result=" << see << " expected: " << s << std::endl;
    }

    delete b;
}

/**
 * verifies a list of see testing positions. Used for debugging the SEE code.
 * Promoting positions have been removed for now
 */
void verifySEE() {

    // 100,325,325,500,1000,10000

    // clang-format off
    verifySEECase("4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - -", genMove(H5,G4,CAPTURE, BLACK_PAWN, WHITE_PAWN), 0);
    verifySEECase("4R3/2r3p1/5bk1/1p1r1p1p/p2PR1P1/P1BK1P2/1P6/8 b - -", genMove(H5,G4,CAPTURE, BLACK_PAWN, WHITE_PAWN), 0);
    verifySEECase("4r1k1/5pp1/nbp4p/1p2p2q/1P2P1b1/1BP2N1P/1B2QPPK/3R4 b - -", genMove(G4, F3, CAPTURE, BLACK_BISHOP, WHITE_KNIGHT), 0);
    verifySEECase("2r1r1k1/pp1bppbp/3p1np1/q3P3/2P2P2/1P2B3/P1N1B1PP/2RQ1RK1 b - -", genMove(D6, E5, CAPTURE, BLACK_PAWN, WHITE_PAWN), 100);
    verifySEECase("7r/5qpk/p1Qp1b1p/3r3n/BB3p2/5p2/P1P2P2/4RK1R w - -", genMove(E1, E8, QUIET, WHITE_ROOK), 0);
    verifySEECase("6rr/6pk/p1Qp1b1p/2n5/1B3p2/5p2/P1P2P2/4RK1R w - -", genMove(E1, E8, QUIET, WHITE_ROOK), -500);
    verifySEECase("7r/5qpk/2Qp1b1p/1N1r3n/BB3p2/5p2/P1P2P2/4RK1R w - -",  genMove(E1, E8, QUIET, WHITE_ROOK), -500);
//    verifySEECase("6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - -", genMove(F7, F8, QUEEN_PROMOTION, WHITE_PAWN), 225);
//    verifySEECase("6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - -", genMove(F7, F8, KNIGHT_PROMOTION, WHITE_PAWN), 225);
//    verifySEECase("7R/5P2/8/8/8/3K2r1/5p2/4k3 w - -", genMove(F7, F8, QUEEN_PROMOTION, WHITE_PAWN), 900);
//    verifySEECase("7R/5P2/8/8/8/3K2r1/5p2/4k3 w - -", genMove(F7, F8, BISHOP_PROMOTION, WHITE_PAWN), 225);
//    verifySEECase("7R/4bP2/8/8/1q6/3K4/5p2/4k3 w - -", genMove(F7, F8, ROOK_PROMOTION, WHITE_PAWN), -100);
    verifySEECase("8/4kp2/2npp3/1Nn5/1p2PQP1/7q/1PP1B3/4KR1r b - -", genMove(H1, F1, CAPTURE, BLACK_ROOK, WHITE_ROOK), 0);
    verifySEECase("8/4kp2/2npp3/1Nn5/1p2P1P1/7q/1PP1B3/4KR1r b - -", genMove(H1, F1, CAPTURE, BLACK_ROOK, WHITE_ROOK), 0);
    verifySEECase("2r2r1k/6bp/p7/2q2p1Q/3PpP2/1B6/P5PP/2RR3K b - -", genMove(C5, C1, CAPTURE, BLACK_QUEEN, WHITE_ROOK), 2*500-1000);
    verifySEECase("r2qk1nr/pp2ppbp/2b3p1/2p1p3/8/2N2N2/PPPP1PPP/R1BQR1K1 w kq -", genMove(F3,E5,CAPTURE,WHITE_KNIGHT,BLACK_PAWN), 100);
    verifySEECase("6r1/4kq2/b2p1p2/p1pPb3/p1P2B1Q/2P4P/2B1R1P1/6K1 w - -", genMove(F4, E5, CAPTURE, WHITE_BISHOP, BLACK_BISHOP), 0);
//    verifySEECase("3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R4B/PQ3P1P/3R2K1 w - h6", genMove(G5, H6, EN_PASSANT, WHITE_PAWN, BLACK_PAWN), 0);
//    verifySEECase("3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R1B2B/PQ3P1P/3R2K1 w - h6", genMove(G5, H6, EN_PASSANT, WHITE_PAWN, BLACK_PAWN), 100);
    verifySEECase("2r4r/1P4pk/p2p1b1p/7n/BB3p2/2R2p2/P1P2P2/4RK2 w - -",  genMove(C3, C8, CAPTURE, WHITE_ROOK, BLACK_ROOK), 500);
    verifySEECase("2r5/1P4pk/p2p1b1p/5b1n/BB3p2/2R2p2/P1P2P2/4RK2 w - -", genMove(C3, C8, CAPTURE, WHITE_ROOK, BLACK_ROOK), 500);
    verifySEECase("2r4k/2r4p/p7/2b2p1b/4pP2/1BR5/P1R3PP/2Q4K w - -", genMove(C3, C5, CAPTURE, WHITE_ROOK, BLACK_BISHOP), 325);
    verifySEECase("8/pp6/2pkp3/4bp2/2R3b1/2P5/PP4B1/1K6 w - -", genMove(E5, C3, CAPTURE, BLACK_BISHOP, WHITE_PAWN), 100-325);
    verifySEECase("4q3/1p1pr1k1/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - -", genMove(E6, E4, CAPTURE, BLACK_ROOK, WHITE_PAWN), 100-500);
    verifySEECase("4q3/1p1pr1kb/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - -", genMove(H7, E4, CAPTURE, BLACK_BISHOP, WHITE_PAWN), 100);

    // clang-format on
}

#endif    // KOIVISTO_VERIFICATION_H
