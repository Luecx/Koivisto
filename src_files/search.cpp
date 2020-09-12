//
// Created by finne on 5/30/2020.
//

#include "search.h"
#include <thread>
#include "History.h"
#include "TimeManager.h"
#include "syzygy/tbprobe.h"

TranspositionTable *     table;
TimeManager *            search_timeManager;
std::vector<std::thread> runningThreads;
int                      threadCount = 1;
bool                     useTB       = false;
bool                     printInfo   = true;

SearchOverview overview;

int lmrReductions[256][256];

// data about each thread. this contains nodes, depth etc as well as a pointer to the history tables
ThreadData **tds = new ThreadData *[MAX_THREADS];

int RAZOR_MARGIN     = 198;
int FUTILITY_MARGIN  = 92;
int SE_MARGIN_STATIC = 0;
int LMR_DIV          = 215;

void initLmr() {
    int d, m;

    for (d = 0; d < 256; d++)
        for (m = 0; m < 256; m++) lmrReductions[d][m] = (0.5 + log(d) * log(m)) * 100 / LMR_DIV;
}

int lmp[2][11] = {{0, 2, 3, 5, 9, 13, 18, 25, 34, 45, 55},
                  {0, 5, 6, 9, 14, 21, 30, 41, 55, 69, 84}};

/**
 * =================================================================================
 *                              S E A R C H
 *                             H E L P E R S
 * =================================================================================
 */

U64 totalNodes() {
    U64 tn = 0;
    for (int i = 0; i < threadCount; i++) {
        tn += tds[i]->nodes;
    }
    return tn;
}

int selDepth() {
    int maxSd = 0;
    for (int i = 0; i < threadCount; i++) {
        maxSd = tds[i]->seldepth > maxSd ? tds[i]->seldepth : maxSd;
    }
    return maxSd;
}

int tbHits() {
    int th = 0;
    for (int i = 0; i < threadCount; i++) {
        th += tds[i]->tbhits;
    }
    return th;
}

void search_enable_infoStrings() {
    printInfo = true;
}

void search_disable_infoStrings() {
    printInfo = false;
}

void search_clearHash() {
    table->clear();
}

/**
 * enables/disables tb probing during search
 */
void search_useTB(bool val) {
    useTB = val;
}

/**
 * stops the search
 */
void search_stop() {
    if (search_timeManager) search_timeManager->stopSearch();
}

/**
 * checks if given side has only pawns left
 * @return
 */
bool hasOnlyPawns(Board *board, Color color) {
    return board->getTeamOccupied()[color] ==
           ((board->getPieces()[PAWN + color * 6] | board->getPieces()[KING + color * 6]));
}

/**
 * checks if there is time left and the search should continue.
 * @return
 */
bool isTimeLeft() {
    return search_timeManager->isTimeLeft();
}

/**
 * used to change the hash size
 * @param hashSize
 */
void search_setHashSize(int hashSize) {
    delete table;
    table = new TranspositionTable(hashSize);
}

/**
 * called at the start of the program
 */
void search_init(int hashSize) {
    table = new TranspositionTable(hashSize);
    initLmr();

    for (int i = 0; i < MAX_THREADS; i++) {
        tds[i] = new ThreadData(i);
    }
}

/**
 * called at the exit of the program
 */
void search_cleanUp() {
    delete table;
    table = nullptr;

    for (int i = 0; i < MAX_THREADS; i++) {
        delete tds[i];
    }
}

/**
 * extracts the pv for the given board using the transposition table.
 * It stores the moves recursively in the given list.
 * It does not clear the list so this has to be done beforehand.
 * to avoid infinite sequences, this search is limited by the depth
 * @param b
 * @param mvList
 */
void extractPV(Board *b, MoveList *mvList, Depth depth) {
    if (depth <= 0) return;

    U64   zob = b->zobrist();
    Entry en  = table->get(zob);
    if (en.zobrist == zob) {
        // extract the move from the table
        Move mov = en.move;

        // get a movelist which can be used to store all pseudo legal moves
        MoveList mvStorage;
        // extract pseudo legal moves
        b->getPseudoLegalMoves(&mvStorage);

        bool moveContained = false;
        // check if the move is actually valid for the position
        for (int i = 0; i < mvStorage.getSize(); i++) {
            Move stor = mvStorage.getMove(i);

            if (sameMove(stor, mov)) {
                moveContained = true;
            }
        }

        // return if the move doesnt exist for this board
        if (!moveContained) return;

        // check if its also legal
        if (!b->isLegal(mov)) return;

        mvList->add(mov);
        b->move(en.move);

        extractPV(b, mvList, depth - 1);
        b->undoMove();
    }
}

/**
 * prints the info string displaying:
 *  - score
 *  - depth
 *  - nodes
 *  - hashfull
 *  - principal variation
 * @param b
 * @param d
 * @param score
 */
void printInfoString(Board *b, Depth d, Score score) {
    if (!printInfo) return;

    U64 nodes = totalNodes();

    U64 nps =
        static_cast<U64>(nodes * 1000) / static_cast<U64>(search_timeManager->elapsedTime() + 1);

    std::cout << "info"
              << " depth " << static_cast<int>(d) << " seldepth " << static_cast<int>(selDepth());

    if (abs(score) > MIN_MATE_SCORE) {
        std::cout << " score mate " << (MAX_MATE_SCORE - abs(score) + 1) / 2 * (score > 0 ? 1 : -1);
    } else {
        std::cout << " score cp " << score;
    }

    if (tbHits() != 0) {
        std::cout << " tbhits " << tbHits();
    }

    std::cout <<

        " nodes " << nodes << " nps " << nps << " time " << search_timeManager->elapsedTime()
              << " hashfull " << static_cast<int>(table->usage() * 1000);

    MoveList em;
    em.clear();
    extractPV(b, &em, selDepth());
    std::cout << " pv";
    for (int i = 0; i < em.getSize(); i++) {
        std::cout << " " << toString(em.getMove(i));
    }

    std::cout << std::endl;
}

Score getWDL(Board *board) {
    if (bitCount(*board->getOccupied()) > (signed)TB_LARGEST) return MAX_MATE_SCORE;

    unsigned res = tb_probe_wdl(board->getTeamOccupied()[WHITE],
                                board->getTeamOccupied()[BLACK],
                                board->getPieces()[WHITE_KING] | board->getPieces()[BLACK_KING],
                                board->getPieces()[WHITE_QUEEN] | board->getPieces()[BLACK_QUEEN],
                                board->getPieces()[WHITE_ROOK] | board->getPieces()[BLACK_ROOK],
                                board->getPieces()[WHITE_BISHOP] | board->getPieces()[BLACK_BISHOP],
                                board->getPieces()[WHITE_KNIGHT] | board->getPieces()[BLACK_KNIGHT],
                                board->getPieces()[WHITE_PAWN] | board->getPieces()[BLACK_PAWN],
                                board->getCurrent50MoveRuleCount(),
                                board->getCastlingChance(0) | board->getCastlingChance(1) |
                                    board->getCastlingChance(2) | board->getCastlingChance(3),
                                board->getEnPassantSquare() != 64 ? board->getEnPassantSquare() : 0,
                                board->getActivePlayer() == WHITE);

    if (res == TB_RESULT_FAILED) {
        return MAX_MATE_SCORE;
    }
    if (res == TB_LOSS) {
        return -TB_WIN_SCORE;
    }
    if (res == TB_WIN) {
        return TB_WIN_SCORE;
    }
    if (res == TB_BLESSED_LOSS) {
        return -TB_CURSED_SCORE;
    }
    if (res == TB_CURSED_WIN) {
        return TB_CURSED_SCORE;
    }
    if (res == TB_DRAW) {
        return 0;
    }
    return MAX_MATE_SCORE;
}

Move getDTZMove(Board *board) {
    if (bitCount(*board->getOccupied()) > (signed)TB_LARGEST) return 0;

    unsigned result =
        tb_probe_root(board->getTeamOccupied()[WHITE],
                      board->getTeamOccupied()[BLACK],
                      board->getPieces()[WHITE_KING] | board->getPieces()[BLACK_KING],
                      board->getPieces()[WHITE_QUEEN] | board->getPieces()[BLACK_QUEEN],
                      board->getPieces()[WHITE_ROOK] | board->getPieces()[BLACK_ROOK],
                      board->getPieces()[WHITE_BISHOP] | board->getPieces()[BLACK_BISHOP],
                      board->getPieces()[WHITE_KNIGHT] | board->getPieces()[BLACK_KNIGHT],
                      board->getPieces()[WHITE_PAWN] | board->getPieces()[BLACK_PAWN],
                      board->getCurrent50MoveRuleCount(),
                      board->getCastlingChance(0) | board->getCastlingChance(1) |
                          board->getCastlingChance(2) | board->getCastlingChance(3),
                      board->getEnPassantSquare() != 64 ? board->getEnPassantSquare() : 0,
                      board->getActivePlayer() == WHITE,
                      NULL);

    if (result == TB_RESULT_FAILED || result == TB_RESULT_CHECKMATE ||
        result == TB_RESULT_STALEMATE)
        return 0;

    int dtz = TB_GET_DTZ(result);
    int wdl = TB_GET_WDL(result);

    Score s = 0;

    if (wdl == TB_LOSS) {
        s = -TB_WIN_SCORE;
    }
    if (wdl == TB_WIN) {
        s = TB_WIN_SCORE;
    }
    if (wdl == TB_BLESSED_LOSS) {
        s = -TB_CURSED_SCORE;
    }
    if (wdl == TB_CURSED_WIN) {
        s = TB_CURSED_SCORE;
    }
    if (wdl == TB_DRAW) {
        s = 0;
    }

    Piece promo = 6 - TB_GET_PROMOTES(result);

    Square sqFrom = TB_GET_FROM(result);
    Square sqTo   = TB_GET_TO(result);

    MoveList *mv = new MoveList();
    board->getPseudoLegalMoves(mv);

    for (int i = 0; i < mv->getSize(); i++) {
        Move m = mv->getMove(i);

        if (getSquareFrom(m) == sqFrom && getSquareTo(m) == sqTo) {
            if ((promo == 6 && !isPromotion(m)) ||
                (isPromotion(m) && promo < 6 && promotionPiece(m) % 6 == promo)) {
                std::cout << "info"
                             " depth "
                          << static_cast<int>(dtz) << " seldepth " << static_cast<int>(selDepth());

                std::cout << " score cp " << s;

                if (tbHits() != 0) {
                    std::cout << " tbhits " << 1;
                }

                std::cout <<

                    " nodes " << 1 << " nps " << 1 << " time " << search_timeManager->elapsedTime()
                          << " hashfull " << static_cast<int>(table->usage() * 1000);
                std::cout << std::endl;

                return m;
            }
        }
    }

    return 0;
    //
}

SearchOverview search_overview() {
    return overview;
}

/**
 * =================================================================================
 *                                M A I N
 *                              S E A R C H
 * =================================================================================
 */

/**
 * returns the best move for the given board.
 * the search will stop if either the max depth is reached.
 * @param b
 * @return
 */
Move bestMove(Board *b, Depth maxDepth, TimeManager *timeManager, int threadId) {
    // if the main thread calls this function, we need to generate the search data for all the
    // threads first
    if (threadId == 0) {
        // if there is a dtz move available, do not start any threads or search at all. just do the
        // dtz move
        Move dtzMove = getDTZMove(b);
        if (dtzMove != 0) return dtzMove;

        // make sure that the given depth isnt too large
        if (maxDepth > MAX_PLY) maxDepth = MAX_PLY;

        // if no dtz move has been found, set the time manager so that the search can be stopped
        search_timeManager = timeManager;

        // we need to reset the hash between searches
        table->incrementAge();

        // for each thread, we will generate a new search data object
        for (int i = 0; i < threadCount; i++) {
            // reseting the thread data
            tds[i]->threadID = i;
            tds[i]->tbhits   = 0;
            tds[i]->nodes    = 0;
            tds[i]->seldepth = 0;
        }

        // we will call this function for the other threads which will skip this part and jump
        // straight to the part below
        for (int n = 1; n < threadCount; n++) {
            runningThreads.push_back(std::thread(bestMove, new Board(b), maxDepth, timeManager, n));
        }
    }

    // the thread id starts at 0 for the first thread
    ThreadData *td = tds[threadId];

    SearchData *sd = new SearchData();
    td->searchData = sd;

    // start the basic search on all threads
    Depth d = 1;
    Score s = 0;
    for (d = 1; d <= maxDepth; d++) {
        // call the pvs framework
        s = pvSearch(b, -MAX_MATE_SCORE, MAX_MATE_SCORE, d, 0, td, 0);

        // if the search finished due to timeout, we also need to stop here
        if (!isTimeLeft()) break;
    }

    delete sd;

    // if the main thread finishes, we will record the data of this thread
    if (threadId == 0) {
        // tell all other threads if they are running to stop the search
        timeManager->stopSearch();
        for (std::thread &th : runningThreads) {
            th.join();
        }
        runningThreads.clear();

        // retrieve the best move from the search
        Move best = table->get(b->zobrist()).move;

        // collect some information which can be used for benching
        overview.nodes = totalNodes();
        overview.depth = d;
        overview.score = s;
        overview.time  = timeManager->elapsedTime();
        overview.move  = best;

        // return the best move if its the main thread
        return best;
    }
    // return nothing (doesnt matter)
    return 0;
}

/**
 * main search for both full-windows and null-windows.
 * @param b
 * @param alpha
 * @param beta
 * @param depth
 * @param ply
 * @return
 */
Score pvSearch(Board *     b,
               Score       alpha,
               Score       beta,
               Depth       depth,
               Depth       ply,
               ThreadData *td,
               Move        skipMove) {
    td->nodes++;

    if (!isTimeLeft()) {
        return beta;
    }

    if (b->isDraw() && ply > 0) {
        return 0;
    }

    if (ply > td->seldepth) {
        td->seldepth = ply;
    }

    bool inCheck = b->isInCheck(b->getActivePlayer());

    // depth > MAX_PLY means that it overflowed because depth is unsigned.
    if (depth == 0 || depth > MAX_PLY) {
        // Don't drop into qsearch if in check
        if (inCheck) {
            depth++;
        } else {
            return qSearch(b, alpha, beta, ply, td);
        }
    }

    SearchData *sd         = td->searchData;
    U64         zobrist    = b->zobrist();
    bool        pv         = (beta - alpha) != 1;
    Score       staticEval = sd->evaluator.evaluate(b) * ((b->getActivePlayer() == WHITE) ? 1 : -1);
    Score       originalAlpha = alpha;
    Score       highestScore  = -MAX_MATE_SCORE;
    Score       score         = -MAX_MATE_SCORE;
    Move        bestMove      = 0;
    Move        hashMove      = 0;

    sd->setHistoricEval(staticEval, b->getActivePlayer(), ply);

    /**************************************************************************************
     *                  T R A N S P O S I T I O N - T A B L E   P R O B E                 *
     **************************************************************************************/
    Entry en = table->get(zobrist);

    if (en.zobrist == zobrist && !skipMove) {
        hashMove = en.move;

        // adjusting eval
        if ((en.type == PV_NODE) || (en.type == CUT_NODE && staticEval < en.score) ||
            (en.type == ALL_NODE && staticEval > en.score)) {
            staticEval = en.score;
        }

        if (!pv && en.depth >= depth) {
            if (en.type == PV_NODE) {
                return en.score;
            } else if (en.type == CUT_NODE) {
                if (en.score >= beta) {
                    return beta;
                }
            } else if (en.type == ALL_NODE) {
                if (en.score <= alpha) {
                    return alpha;
                }
            }
        }
    }

    /**************************************************************************************
     *                            T A B L E B A S E - P R O B E                           *
     **************************************************************************************/
    // search the wdl table if we are not at the root and the root did not use the wdl table to sort
    // the moves
    if (useTB && ply > 0) {
        Score res = getWDL(b);

        // MAX_MATE_SCORE is used for no result
        if (res != MAX_MATE_SCORE) {
            td->tbhits++;
            // indicates a winning or losing position
            if (abs(res) > 2) {
                // take the winning positions closest to the root
                if (res > 0) {
                    return res - ply;
                }
                // take the losing position furthest away
                else {
                    return res + ply;
                }
            }

            return res;
        }
    }

    if (!skipMove && !inCheck && !pv) {
        /**************************************************************************************
         *                              R A Z O R I N G                                       *
         **************************************************************************************/
        if (depth <= 3 && staticEval + RAZOR_MARGIN < beta) {
            score = qSearch(b, alpha, beta, ply, td);
            if (score < beta) return score;
        }
        /**************************************************************************************
         *                      F U T I L I T Y   P R U N I N G                               *
         **************************************************************************************/
        if (depth <= 6 && staticEval >= beta + depth * FUTILITY_MARGIN) return staticEval;

        /**************************************************************************************
         *                  N U L L - M O V E   P R U N I N G                                 *
         **************************************************************************************/
        if (depth >= 2 && staticEval >= beta && !hasOnlyPawns(b, b->getActivePlayer())) {
            b->move_null();

            score = -pvSearch(
                b, -beta, 1 - beta, depth - (depth / 4 + 3) * ONE_PLY, ply + ONE_PLY, td, 0);
            b->undoMove_null();
            if (score >= beta) {
                return beta;
            }
        }
    }

    /**************************************************************************************
     *        I N T E R N A L   I T E R A T I V E   D E E P E N I N G                     *
     **************************************************************************************/

    /*
     * internal iterative deepening
     */
    if (depth >= 6 && pv && !hashMove && !skipMove) {
        pvSearch(b, alpha, beta, depth - 2, ply, td, 0);
        en = table->get(zobrist);
        if (en.zobrist == zobrist) {
            hashMove = en.move;
        }
    }

    /**************************************************************************************
     *              M A T E - D I S T A N C E   P R U N I N G                             *
     **************************************************************************************/
    Score matingValue = MAX_MATE_SCORE - ply;
    if (matingValue < beta) {
        beta = matingValue;
        if (alpha >= matingValue) return matingValue;
    }

    matingValue = -MAX_MATE_SCORE + ply;
    if (matingValue > alpha) {
        alpha = matingValue;
        if (beta <= matingValue) return matingValue;
    }

    MoveList *mv = sd->moves[ply];
    b->getPseudoLegalMoves(mv);

    MoveOrderer moveOrderer{};
    moveOrderer.setMovesPVSearch(mv, hashMove, sd, b, ply);

    // count the legal moves
    int legalMoves = 0;
    int quiets     = 0;

    while (moveOrderer.hasNext()) {
        Move m = moveOrderer.next();

        if (!b->isLegal(m)) continue;

        if (sameMove(m, skipMove)) continue;

        bool givesCheck  = b->givesCheck(m);
        bool isPromotion = move::isPromotion(m);

        if (!pv && ply > 0 && legalMoves >= 1) {
            if (!isCapture(m) && !isPromotion && !givesCheck) {
                quiets++;
                // LMP
                if (depth <= 10 &&
                    quiets > lmp[sd->isImproving(staticEval, b->getActivePlayer(), ply)][depth]) {
                    moveOrderer.skip = true;
                    continue;
                }
            }

            // SEE Pruning
            if (depth <= 5 && (getCapturedPiece(m) % 6) < (getMovingPiece(m) % 6) &&
                b->staticExchangeEvaluation(m) <= -100 * depth)
                continue;
        }

        Score staticExchangeEval = 0;
        if (isCapture(m)) {
            staticExchangeEval = b->staticExchangeEvaluation(m);
        }

        int extension = 0;

        if (givesCheck && staticExchangeEval > 0) {
            extension = 1;
        }

        // singular extensions
        if (!extension && depth >= 8 && !skipMove && legalMoves == 0 && sameMove(m, hashMove) &&
            ply > 0 && b->getActivePlayer() != sd->sideToReduce && en.zobrist == zobrist &&
            abs(en.score) < MIN_MATE_SCORE && (en.type == CUT_NODE || en.type == PV_NODE) &&
            en.depth >= depth - 3) {
            Score betaCut = en.score - SE_MARGIN_STATIC - depth * 2;
            score         = pvSearch(b, betaCut - 1, betaCut, depth >> 1, ply, td, m);
            if (score < betaCut) extension++;
            b->getPseudoLegalMoves(mv);
            moveOrderer.setMovesPVSearch(mv, hashMove, sd, b, ply);

            m = moveOrderer.next();
        }

        if (ply == 0) {
            sd->sideToReduce = b->getActivePlayer();
            if (legalMoves == 0) sd->sideToReduce = !b->getActivePlayer();
        }

        b->move(m);

        Depth lmr = (legalMoves == 0 || depth <= 2 || isCapture(m) || isPromotion)
                        ? 0
                        : lmrReductions[depth][legalMoves];

        if (lmr) {
            int history = sd->getHistoryMoveScore(m, !b->getActivePlayer()) - 512;
            lmr         = lmr - history / 256;
            if (sd->sideToReduce == b->getActivePlayer()) {
                lmr = lmr + 1;
            }
            if (lmr > MAX_PLY) {
                lmr = 0;
            }
            if (lmr > depth - 2) {
                lmr = depth - 2;
            }
        }

        if (legalMoves == 0) {
            score = -pvSearch(b, -beta, -alpha, depth - ONE_PLY + extension, ply + ONE_PLY, td, 0);
        } else {
            score = -pvSearch(
                b, -alpha - 1, -alpha, depth - ONE_PLY - lmr + extension, ply + ONE_PLY, td, 0);
            if (lmr && score > alpha)
                score = -pvSearch(b,
                                  -alpha - 1,
                                  -alpha,
                                  depth - ONE_PLY + extension,
                                  ply + ONE_PLY,
                                  td,
                                  0);  // re-search
            if (score > alpha && score < beta)
                score = -pvSearch(b,
                                  -beta,
                                  -alpha,
                                  depth - ONE_PLY + extension,
                                  ply + ONE_PLY,
                                  td,
                                  0);  // re-search
        }

        b->undoMove();

        // if we got a new best score for this node, update the highest score and keep track of the
        // best move
        if (score > highestScore) {
            highestScore = score;
            bestMove     = m;
        }

        // beta -cutoff
        if (score >= beta) {
            if (!skipMove) {
                // put the beta cutoff into the tt
                table->put(zobrist, highestScore, m, CUT_NODE, depth);
                // also set this move as a killer move into the history
                sd->setKiller(m, ply, b->getActivePlayer());
                // if the move is not a capture, we also update counter move history tables and
                // history scores.
                if (!isCapture(m)) {
                    sd->addHistoryScore(m, depth, mv, b->getActivePlayer());
                    sd->addCounterMoveHistoryScore(b->getPreviousMove(), m, depth, mv);
                }
            }
            return beta;
        }

        if (score > alpha) {
            // we only record pv changes at the root if its the main thread
            if (!skipMove && ply == 0 && isTimeLeft() && td->threadID == 0) {
                // we need to put the transposition in here so that printInfoString displays the
                // correct pv
                table->put(zobrist, score, bestMove, PV_NODE, depth);
                // print an updated version of the info string including nodes, nps etc.
                printInfoString(b, depth, score);
                // the time manager needs to be updated to know if its safe to stop the search
                search_timeManager->updatePV(m, score, depth);
            }
            // increase alpha
            alpha = score;
            // store the best move for this node
            bestMove = m;
        }

        // if this loop finished, we can increment the legal move counter by one which is important
        // for detecting mates
        legalMoves++;
    }

    // if there are no legal moves, its either stalemate or checkmate.
    if (legalMoves == 0) {
        // if we are not in check, it must be stalemate (draw)
        if (!inCheck) {
            return 0;
        } else {
            return -MAX_MATE_SCORE + ply;
        }
    }

    if (!skipMove) {
        if (alpha > originalAlpha) {
            table->put(zobrist, alpha, bestMove, PV_NODE, depth);
        } else {
            table->put(zobrist, highestScore, bestMove, ALL_NODE, depth);
        }
    }

    return alpha;
}

/**
 * qSearch.
 *
 * @param b
 * @param alpha
 * @param beta
 * @param ply
 * @return
 */
Score qSearch(Board *b, Score alpha, Score beta, Depth ply, ThreadData *td) {
    // increase the nodes for this thread
    td->nodes++;

    // if its a draw (3-fold), return 0
    if (b->isDraw()) return 0;

    // extract information like search data (history tables), zobrist etc
    SearchData *sd         = td->searchData;
    U64         zobrist    = b->zobrist();
    Entry       en         = table->get(b->zobrist());
    NodeType    ttNodeType = ALL_NODE;

    // if there is an entry found, probe the tt for the score
    if (en.zobrist == zobrist) {
        if (en.type == PV_NODE) {
            return en.score;
        } else if (en.type == CUT_NODE) {
            if (en.score >= beta) {
                return beta;
            }
        } else if (en.type == ALL_NODE) {
            if (en.score <= alpha) {
                return alpha;
            }
        }
    }

    bool  inCheck   = b->isInCheck(b->getActivePlayer());
    Score stand_pat = -MAX_MATE_SCORE + ply;
    if (!inCheck) {
        stand_pat = sd->evaluator.evaluate(b) * ((b->getActivePlayer() == WHITE) ? 1 : -1);
    }

    if (stand_pat >= beta) return beta;
    if (alpha < stand_pat) alpha = stand_pat;

    /**
     * extract all:
     *  - captures (including e.p.)
     *  - promotions
     *
     *  moves that give check are not considered non-quiet in
     *  getNonQuietMoves() allthough they are not quiet.
     */
    MoveList *mv = sd->moves[ply];
    b->getNonQuietMoves(mv);

    MoveOrderer moveOrderer{};
    moveOrderer.setMovesQSearch(mv, b);

    Move  bestMove  = 0;
    Score bestScore = -MAX_MATE_SCORE;

    for (int i = 0; i < mv->getSize(); i++) {
        Move m = moveOrderer.next();

        if (!b->isLegal(m)) continue;

        if (!inCheck && (getCapturedPiece(m) % 6) < (getMovingPiece(m) % 6) &&
            b->staticExchangeEvaluation(m) < 0)
            continue;

        b->move(m);

        Score score = -qSearch(b, -beta, -alpha, ply + ONE_PLY, td);

        b->undoMove();

        if (score > bestScore) {
            bestScore = score;
            bestMove  = m;
            if (score >= beta) {
                ttNodeType = CUT_NODE;
                table->put(zobrist, bestScore, m, ttNodeType, 0);
                return beta;
            }
            if (score > alpha) {
                ttNodeType = PV_NODE;
                alpha      = score;
            }
        }
    }
    if (bestMove) table->put(zobrist, bestScore, bestMove, ttNodeType, 0);
    return alpha;

    //    return 0;
}
