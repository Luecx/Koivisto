
#include "eval.h"

#ifdef TUNE

namespace tuning {

// phase - psqt_index - square
float psqt_gradients[2][11][64] {};

void collectGradients_psqt(Board* board, float evalGrad, float phase) {
    // target: incrementing psqt_gradients for the given board using the evaluation gradient
    // which is the same as the derivative of the evaluation output with respect to the loss

    bool wKSide = (fileIndex(bitscanForward(board->getPieces()[WHITE_KING])) > 3 ? 0 : 1);
    bool bKSide = (fileIndex(bitscanForward(board->getPieces()[BLACK_KING])) > 3 ? 0 : 1);

    bool opposingSide = wKSide != bKSide;

    // do pawns - queens first (kings are handled later)
    for (Piece p = PAWN; p <= QUEEN; p++) {

        // this has to match the field: psqt
        int psqt_index = p * 2 + opposingSide;

        // do it for both colors
        for (Color c = WHITE; c <= BLACK; c++) {

            U64 k = board->getPieces(c, p);
            while (k) {
                Square s = bitscanForward(k);

                // computing relative ranks/files
                Rank relativeRank = c == WHITE ? rankIndex(s) : 7 - rankIndex(s);
                File relativeFile = c == WHITE ? (wKSide ? fileIndex(s) : 7 - fileIndex(s))
                                               : (bKSide ? fileIndex(s) : 7 - fileIndex(s));
    
                psqt_gradients[0][psqt_index][squareIndex(relativeRank, relativeFile)] +=
                    (c == WHITE ? (1 - phase) : -(1 - phase)) * evalGrad;
                psqt_gradients[1][psqt_index][squareIndex(relativeRank, relativeFile)] +=
                    (c == WHITE ? (phase) : -(phase)) * evalGrad;

                k = lsbReset(k);
            }
        }
    }

    // doing the king psqt (note that they are independent of king relations)
    // do it for both colors
    for (Color c = WHITE; c <= BLACK; c++) {

        // the the piece occupancy of the king(s)
        U64 k = board->getPieces(c, KING);
        while (k) {
            Square s = bitscanForward(k);

            // we use index 10 as the first 10 are used by pawns to queens.
            psqt_gradients[0][10][pst_index_white_s(s)] +=
                (c == WHITE ? (1 - phase) : -(1 - phase)) * evalGrad;
            psqt_gradients[1][10][pst_index_black_s(s)] +=
                (c == WHITE ? (phase) : -(phase)) * evalGrad;

            k = lsbReset(k);
        }
    }
}

void updateGradients() {
    for (int i = 0; i < 10; i++) {
        for (Square s = 0; s < 64; s++) {

            float midGrad = psqt_gradients[0][i][s];
            float endGrad = psqt_gradients[1][i][s];

            EvalScore change = M(
                midGrad > 0 ? -1 : midGrad < 0 ? 1 : 0,
                endGrad > 0 ? -1 : endGrad < 0 ? 1 : 0);
            psqt[i][s] += change;
        }
    }

    eval_init();
}

void clearGradients() {
    for (int i = 0; i < 11; i++) {
        for (int s = 0; s < 64; s++) {
            psqt_gradients[0][i][s] = 0;
            psqt_gradients[1][i][s] = 0;
        }
    }
}

void collectGradients(std::vector<TrainingEntry>& entries, double K) {

    Evaluator evaluator {};

    for (TrainingEntry& en : entries) {
        
        bool matingMaterial = hasMatingMaterial(&en.board, en.board.getActivePlayer());
        
        // we need the phase to compute gradients for early and late phase
        float phase =
            (24.0f + phaseValues[5]
             - phaseValues[0] * bitCount(en.board.getPieces()[WHITE_PAWN] | en.board.getPieces()[BLACK_PAWN])
             - phaseValues[1] * bitCount(en.board.getPieces()[WHITE_KNIGHT] | en.board.getPieces()[BLACK_KNIGHT])
             - phaseValues[2] * bitCount(en.board.getPieces()[WHITE_BISHOP] | en.board.getPieces()[BLACK_BISHOP])
             - phaseValues[3] * bitCount(en.board.getPieces()[WHITE_ROOK] | en.board.getPieces()[BLACK_ROOK])
             - phaseValues[4] * bitCount(en.board.getPieces()[WHITE_QUEEN] | en.board.getPieces()[BLACK_QUEEN]))
            / 24.0f;
        if (phase > 1)
            phase = 1;
        if (phase < 0)
            phase = 0;

        // compute the gradient of the loss function with respect to the output
        Score  q_i      = evaluator.evaluate(&en.board);
        double expected = en.target;

        double sig       = sigmoid(q_i, K);
        double sigPrime  = sigmoidPrime(q_i, K);
        double lossPrime = -2 * (expected - sig);
        
        if(matingMaterial){
            lossPrime /= 10;
        }
        
        collectGradients_psqt(&en.board, lossPrime * sigPrime, phase);
    }

}

void optimiseGradients(double K) {
    clearGradients();
    collectGradients(training_entries, K);
    updateGradients();
}

}    // namespace tuning

#endif
