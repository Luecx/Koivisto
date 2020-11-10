
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

#include "search.h"

#include "History.h"
#include "TimeManager.h"
#include "syzygy/tbprobe.h"

#include <thread>

TranspositionTable*      table;
TimeManager*             search_timeManager;
std::vector<std::thread> runningThreads;
int                      threadCount = 1;
bool                     useTB       = false;
bool                     printInfo   = true;

SearchOverview overview;

int lmrReductions[256][256];

// data about each thread. this contains nodes, depth etc as well as a pointer to the history tables
ThreadData** tds = new ThreadData*[MAX_THREADS];

int RAZOR_MARGIN     = 198;
int FUTILITY_MARGIN  = 92;
int SE_MARGIN_STATIC = 0;
int LMR_DIV          = 215;

void initLmr() {
    int d, m;

    for (d = 0; d < 256; d++)
        for (m = 0; m < 256; m++)
            lmrReductions[d][m] = (0.5 + log(d) * log(m)) * 100 / LMR_DIV;
}

int lmp[2][8] = {{0, 2, 3, 4, 6, 8, 13, 18}, {0, 3, 4, 6, 8, 12, 20, 30}};

/**
 * =================================================================================
 *                              S E A R C H
 *                             H E L P E R S
 * =================================================================================
 */

/**
 * returns the total amount of searched nodes across all threads
 * @return
 */
U64 totalNodes() {
    U64 tn = 0;
    for (int i = 0; i < threadCount; i++) {
        tn += tds[i]->nodes;
    }
    return tn;
}

/**
 * returns the maximum selective depth across all threads
 * @return
 */
int selDepth() {
    int maxSd = 0;
    for (int i = 0; i < threadCount; i++) {
        maxSd = tds[i]->seldepth > maxSd ? tds[i]->seldepth : maxSd;
    }
    return maxSd;
}

/**
 * returns the amount of tablebase hits across all threads
 * @return
 */
int tbHits() {
    int th = 0;
    for (int i = 0; i < threadCount; i++) {
        th += tds[i]->tbhits;
    }
    return th;
}

/**
 * enables uci info-string printing. This is usually enabled but might be disabled for fen-generation.
 */
void search_enable_infoStrings() { printInfo = true; }

/**
 * enables uci info-string printing. This is usually enabled but might be disabled for fen-generation.
 */
void search_disable_infoStrings() { printInfo = false; }

/**
 * clears the hash of the transposition table which is used for searches.
 */
void search_clearHash() { table->clear(); }

/**
 * enables/disables tb probing during search
 */
void search_useTB(bool val) { useTB = val; }

/**
 * stops the search
 */
void search_stop() {
    if (search_timeManager)
        search_timeManager->stopSearch();
}

/**
 * checks if given side has only pawns left
 * @return
 */
bool hasOnlyPawns(Board* board, Color color) {
    return board->getTeamOccupied()[color]
           == ((board->getPieces()[PAWN + color * 6] | board->getPieces()[KING + color * 6]));
}

/**
 * checks if there is time left and the search should continue.
 * @return
 */
bool isTimeLeft() { return search_timeManager->isTimeLeft(); }

/**
 * checks if there is root time left and the iterative deepening should continue.
 * @return
 */
bool rootTimeLeft() { return search_timeManager->rootTimeLeft(); }

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
 * called at the exit of the program to cleanup and deallocate arrays.
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
void extractPV(Board* b, MoveList* mvList, Depth depth) {

    if (depth <= 0)
        return;

    U64   zob = b->zobrist();
    Entry en  = table->get(zob);
    if (en.zobrist == zob && en.type == PV_NODE) {

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
        if (!moveContained)
            return;

        // check if its also legal
        if (!b->isLegal(mov))
            return;

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
void printInfoString(Board* b, Depth d, Score score) {

    if (!printInfo)
        return;

    U64 nodes = totalNodes();

    U64 nps = static_cast<U64>(nodes * 1000) / static_cast<U64>(search_timeManager->elapsedTime() + 1);

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

        " nodes " << nodes << " nps " << nps << " time " << search_timeManager->elapsedTime() << " hashfull "
              << static_cast<int>(table->usage() * 1000);

    MoveList em;
    em.clear();
    extractPV(b, &em, selDepth());
    std::cout << " pv";
    for (int i = 0; i < em.getSize(); i++) {
        std::cout << " " << toString(em.getMove(i));
    }

    std::cout << std::endl;
}

/**
 * probes the wdl tables if tablebases can be used.
 */
Score getWDL(Board* board) {

    // we cannot prove the tables if there are too many pieces on the board
    if (bitCount(*board->getOccupied()) > (signed) TB_LARGEST)
        return MAX_MATE_SCORE;

    // use the given files to prove the tables using the information from the board.
    unsigned res = tb_probe_wdl(
        board->getTeamOccupied()[WHITE], board->getTeamOccupied()[BLACK],
        board->getPieces()[WHITE_KING] | board->getPieces()[BLACK_KING],
        board->getPieces()[WHITE_QUEEN] | board->getPieces()[BLACK_QUEEN],
        board->getPieces()[WHITE_ROOK] | board->getPieces()[BLACK_ROOK],
        board->getPieces()[WHITE_BISHOP] | board->getPieces()[BLACK_BISHOP],
        board->getPieces()[WHITE_KNIGHT] | board->getPieces()[BLACK_KNIGHT],
        board->getPieces()[WHITE_PAWN] | board->getPieces()[BLACK_PAWN], board->getCurrent50MoveRuleCount(),
        board->getCastlingChance(0) | board->getCastlingChance(1) | board->getCastlingChance(2)
            | board->getCastlingChance(3),
        board->getEnPassantSquare() != 64 ? board->getEnPassantSquare() : 0, board->getActivePlayer() == WHITE);

    // if the result failed, we return the max_mate_score internally. This is not used within the search and will be
    // catched later.
    if (res == TB_RESULT_FAILED) {
        return MAX_MATE_SCORE;
    }
    // we defined our own tablebase scores.
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
    // if none of them above happened, act as if the result failed.
    return MAX_MATE_SCORE;
}

/**
 * probes the dtz table. If an entry is being found, it also displays the info string.
 * The displayed depth is usually the distance to zero which is the distance until the 50-move rule is reset.
 */
Move getDTZMove(Board* board) {

    if (bitCount(*board->getOccupied()) > (signed) TB_LARGEST)
        return 0;

    unsigned result = tb_probe_root(
        board->getTeamOccupied()[WHITE], board->getTeamOccupied()[BLACK],
        board->getPieces()[WHITE_KING] | board->getPieces()[BLACK_KING],
        board->getPieces()[WHITE_QUEEN] | board->getPieces()[BLACK_QUEEN],
        board->getPieces()[WHITE_ROOK] | board->getPieces()[BLACK_ROOK],
        board->getPieces()[WHITE_BISHOP] | board->getPieces()[BLACK_BISHOP],
        board->getPieces()[WHITE_KNIGHT] | board->getPieces()[BLACK_KNIGHT],
        board->getPieces()[WHITE_PAWN] | board->getPieces()[BLACK_PAWN], board->getCurrent50MoveRuleCount(),
        board->getCastlingChance(0) | board->getCastlingChance(1) | board->getCastlingChance(2)
            | board->getCastlingChance(3),
        board->getEnPassantSquare() != 64 ? board->getEnPassantSquare() : 0, board->getActivePlayer() == WHITE, NULL);

    // if the result failed for some reason or the game is over, dont do anything
    if (result == TB_RESULT_FAILED || result == TB_RESULT_CHECKMATE || result == TB_RESULT_STALEMATE)
        return 0;

    // we need the wdl and the dtz values to get the score.
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

    // get the promotion piece if the target move is a promotion (this does not yet work the way it should)
    Piece promo = 6 - TB_GET_PROMOTES(result);

    // gets the square from and square to for the move which should be played
    Square sqFrom = TB_GET_FROM(result);
    Square sqTo   = TB_GET_TO(result);

    // we generate all pseudo legal moves and check for equality between the moves to make sure the bits are correct.
    MoveList* mv = new MoveList();
    board->getPseudoLegalMoves(mv);

    for (int i = 0; i < mv->getSize(); i++) {
        // get the current move from the movelist
        Move m = mv->getMove(i);

        // check if its the same.
        if (getSquareFrom(m) == sqFrom && getSquareTo(m) == sqTo) {
            if ((promo == 6 && !isPromotion(m)) || (isPromotion(m) && promo < 6 && promotionPiece(m) % 6 == promo)) {

                std::cout << "info"
                             " depth "
                          << static_cast<int>(dtz) << " seldepth " << static_cast<int>(selDepth());

                std::cout << " score cp " << s;

                if (tbHits() != 0) {
                    std::cout << " tbhits " << 1;
                }

                std::cout <<

                    " nodes " << 1 << " nps " << 1 << " time " << search_timeManager->elapsedTime() << " hashfull "
                          << static_cast<int>(table->usage() * 1000);
                std::cout << std::endl;

                return m;
            }
        }
    }

    return 0;
}

/**
 * returns an overview of the search which is internally used for various reasons.
 */
SearchOverview search_overview() { return overview; }

/**
 * =================================================================================
 *                                M A I N
 *                              S E A R C H
 * =================================================================================
 */

/**
 * returns the best move for the given board.
 * the search will stop if either the max depth is reached.
 * If tablebases are used, the dtz move will be used if possible. If not, the WDL tables will be used
 * during the search.
 * @param b
 * @return
 */
Move bestMove(Board* b, Depth maxDepth, TimeManager* timeManager, int threadId) {

    // if the main thread calls this function, we need to generate the search data for all the threads first
    if (threadId == 0) {

        // if there is a dtz move available, do not start any threads or search at all. just do the dtz move
        Move dtzMove = getDTZMove(b);
        if (dtzMove != 0)
            return dtzMove;

        // make sure that the given depth isnt too large
        if (maxDepth > MAX_PLY)
            maxDepth = MAX_PLY;

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

        // we will call this function for the other threads which will skip this part and jump straight to the part
        // below
        for (int n = 1; n < threadCount; n++) {
            runningThreads.push_back(std::thread(bestMove, new Board(b), maxDepth, timeManager, n));
        }
    }

    // the thread id starts at 0 for the first thread
    ThreadData* td = tds[threadId];

    SearchData* sd = new SearchData();
    td->searchData = sd;

    // start the basic search on all threads
    Depth d = 1;
    Score s = 0;
    for (d = 1; d <= maxDepth; d++) {

        if (d < 6) {
            s = pvSearch(b, -MAX_MATE_SCORE, MAX_MATE_SCORE, d, 0, td, 0);
        } else {
            Score window = 10;
            Score alpha  = s - window;
            Score beta   = s + window;

            while (rootTimeLeft()) {
                s = pvSearch(b, alpha, beta, d, 0, td, 0);

                window += window;
                if (window > 500)
                    window = MIN_MATE_SCORE;
                if (s >= beta) {
                    beta += window;
                } else if (s <= alpha) {
                    alpha -= window;
                } else {
                    break;
                }
            }
        }

        if (threadId == 0)
            printInfoString(b, d, s);

        // if the search finished due to timeout, we also need to stop here
        if (!rootTimeLeft())
            break;
    }

    // if the main thread finishes, we will record the data of this thread
    if (threadId == 0) {

        // tell all other threads if they are running to stop the search
        timeManager->stopSearch();
        for (std::thread& th : runningThreads) {
            th.join();
        }
        runningThreads.clear();

        // retrieve the best move from the search
        Move best = sd->bestMove;

        delete sd;

        // collect some information which can be used for benching
        overview.nodes = totalNodes();
        overview.depth = d;
        overview.score = s;
        overview.time  = timeManager->elapsedTime();
        overview.move  = best;

        // return the best move if its the main thread
        return best;
    }

    delete sd;

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
Score pvSearch(Board* b, Score alpha, Score beta, Depth depth, Depth ply, ThreadData* td, Move skipMove) {

    // increment the node counter for the current thread
    td->nodes++;

    // if the time is over, we fail hard to stop the search. We don't want to call the system clock too often for speed
    // reasons so we only apply this when the depth is larger than 10.
    if (depth > 6 && !isTimeLeft()) {
        return beta;
    }

    // if its a draw by 3-fold or 50-move rule, we return 0
    if (b->isDraw() && ply > 0) {
        return 0;
    }

    // beside keeping track of the nodes, we need to keep track of the selective depth for this thread.
    if (ply > td->seldepth) {
        td->seldepth = ply;
    }

    // check if the active player is in check. used for various pruning decisions.
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

    // we extract a lot of information about various things.
    SearchData* sd            = td->searchData;
    U64         zobrist       = b->zobrist();
    bool        pv            = (beta - alpha) != 1;
    Score       originalAlpha = alpha;
    Score       highestScore  = -MAX_MATE_SCORE;
    Score       score         = -MAX_MATE_SCORE;
    Move        bestMove      = 0;
    Move        hashMove      = 0;
    Score       staticEval;
    // the idea for the static evaluation is that if the last move has been a null move, we can reuse the eval and
    // simply adjust the tempo-bonus.
    if (b->getPreviousMove() == 0 && ply != 0) {
        // reuse static evaluation from previous ply in case of nullmove
        staticEval = -sd->eval[1 - b->getActivePlayer()][ply - 1] + sd->evaluator.evaluateTempo(b) * 2;
    } else {
        staticEval =
            inCheck ? -MAX_MATE_SCORE + ply : sd->evaluator.evaluate(b) * ((b->getActivePlayer() == WHITE) ? 1 : -1);
    }
    // we check if the evaluation improves across plies.
    sd->setHistoricEval(staticEval, b->getActivePlayer(), ply);
    bool isImproving = inCheck ? false : sd->isImproving(staticEval, b->getActivePlayer(), ply);

    // **************************************************************************************************************
    // transposition table probing:
    // we probe the transposition table and check if there is an entry with the same zobrist key as the current
    // position. First, we adjust the static evaluation and second, we might be able to return the tablebase score if
    // the depth of that entry is larger than our current depth.
    // **************************************************************************************************************
    Entry en = table->get(zobrist);

    if (en.zobrist == zobrist && !skipMove) {
        hashMove = en.move;

        // adjusting eval
        if ((en.type == PV_NODE) || (en.type == CUT_NODE && staticEval < en.score)
            || (en.type == ALL_NODE && staticEval > en.score)) {

            staticEval = en.score;
        }

        if (!pv && en.depth >= depth) {
            if (en.type == PV_NODE) {
                return en.score;
            } else if (en.type == CUT_NODE) {
                if (en.score >= beta) {
                    return en.score;
                }
            } else if (en.type == ALL_NODE) {
                if (en.score <= alpha) {
                    return en.score;
                }
            }
        }
    }

    // **************************************************************************************************************
    // tablebase probing:
    // search the wdl table if we are not at the root and the root did not use the wdl table to sort the moves
    // **************************************************************************************************************
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
        // **********************************************************************************************************
        // razoring:
        // if a qsearch on the current position is below beta, we can fail soft. Note that this is only used during
        // within pv nodes which means that alpha = beta - 1.
        // **********************************************************************************************************
        if (depth <= 3 && staticEval + RAZOR_MARGIN < beta) {
            score = qSearch(b, alpha, beta, ply, td);
            if (score < beta)
                return score;
        }
        // **********************************************************************************************************
        // futlity pruning:
        // if the static evaluation is already above beta with a specific margin, assume that the we will definetly be
        // above beta and stop the search here and fail soft
        // **********************************************************************************************************
        if (depth <= 6 && staticEval >= beta + depth * FUTILITY_MARGIN && staticEval < MIN_MATE_SCORE)
            return staticEval;

        // **********************************************************************************************************
        // futlity pruning:
        // if the evaluation from a very shallow search after doing nothing is still above beta, we assume that we are
        // currently above beta as well and stop the search early.
        // **********************************************************************************************************
        if (staticEval >= beta && !hasOnlyPawns(b, b->getActivePlayer())) {
            b->move_null();

            score = -pvSearch(b, -beta, 1 - beta, depth - (depth / 4 + 3) * ONE_PLY, ply + ONE_PLY, td, 0);
            b->undoMove_null();
            if (score >= beta) {
                return beta;
            }
        }
    }

    // **********************************************************************************************************
    // internal iterative deepening by Ed Schröder::
    // http://talkchess.com/forum3/viewtopic.php?f=7&t=74769&sid=64085e3396554f0fba414404445b3120
    // **********************************************************************************************************
    if (depth >= 4 && !hashMove)
        depth--;

    // **********************************************************************************************************
    // mate distance pruning:
    // **********************************************************************************************************
    Score matingValue = MAX_MATE_SCORE - ply;
    if (matingValue < beta) {
        beta = matingValue;
        if (alpha >= matingValue)
            return matingValue;
    }

    matingValue = -MAX_MATE_SCORE + ply;
    if (matingValue > alpha) {
        alpha = matingValue;
        if (beta <= matingValue)
            return matingValue;
    }

    // we reuse movelists for memory reasons.
    MoveList* mv = sd->moves[ply];

    // store all the moves inside the movelist
    b->getPseudoLegalMoves(mv);

    // create a moveorderer and assign the movelist to score the moves.
    MoveOrderer moveOrderer {};
    moveOrderer.setMovesPVSearch(mv, hashMove, sd, b, ply);

    // count the legal and quiet moves.
    int legalMoves = 0;
    int quiets     = 0;

    // loop over all moves in the movelist
    while (moveOrderer.hasNext()) {

        // get the current move
        Move m = moveOrderer.next();

        // dont search illegal moves
        if (!b->isLegal(m))
            continue;

        // if the move is the move we want to skip, skip this move (used for extensions)
        if (sameMove(m, skipMove))
            continue;

        // check if the move gives check and/or its promoting
        bool givesCheck  = b->givesCheck(m);
        bool isPromotion = move::isPromotion(m);

        if (ply > 0 && legalMoves >= 1 && highestScore > -MIN_MATE_SCORE) {
            if (!isCapture(m) && !isPromotion && !givesCheck) {
                quiets++;
                // **************************************************************************************************
                // late move pruning:
                // if the depth is small enough and we searched enough quiet moves, dont consider this move
                // **************************************************************************************************
                if (depth <= 7 && quiets > lmp[isImproving][depth]) {
                    moveOrderer.skip = true;
                    continue;
                }
            }

            // ******************************************************************************************************
            // static exchange evaluation pruning (see pruning):
            // if the depth is small enough and the static exchange evaluation for the given move is very negative, dont
            // consider this quiet move as well.
            // ******************************************************************************************************
            if (depth <= 5 && (getCapturedPiece(m) % 6) < (getMovingPiece(m) % 6)
                && b->staticExchangeEvaluation(m) <= -100 * depth)
                continue;
        }

        // compute the static exchange evaluation if the move is a capture
        Score staticExchangeEval = 0;
        if (isCapture(m)) {
            staticExchangeEval = b->staticExchangeEvaluation(m);
        }

        // keep track of the depth we want to extend by
        int extension = 0;

        // *********************************************************************************************************
        // singular extensions:
        // *********************************************************************************************************
        if (!extension && depth >= 8 && !skipMove && legalMoves == 0 && sameMove(m, hashMove) && ply > 0
            && en.zobrist == zobrist && abs(en.score) < MIN_MATE_SCORE
            && (en.type == CUT_NODE || en.type == PV_NODE) && en.depth >= depth - 3) {

            Score betaCut = en.score - SE_MARGIN_STATIC - depth * 2;
            score         = pvSearch(b, betaCut - 1, betaCut, depth >> 1, ply, td, m);
            if (score < betaCut)
                extension++;
            b->getPseudoLegalMoves(mv);
            moveOrderer.setMovesPVSearch(mv, hashMove, sd, b, ply);

            m = moveOrderer.next();
        }

        // *********************************************************************************************************
        // kk reductions:
        // we reduce more/less depending on which side we are currently looking at.
        // *********************************************************************************************************
        if (ply == 0) {
            sd->sideToReduce = b->getActivePlayer();
            if (legalMoves == 0)
                sd->sideToReduce = !b->getActivePlayer();
        }

        // compute the lmr based on the depth, the amount of legal moves etc.
        // we dont want to reduce if its the first move we search, or a capture with a positive see score or if the
        // depth is too small.
        // furthermore no queen promotions are reduced
        Depth lmr = (legalMoves == 0 || depth <= 2 || (isCapture(m) && staticExchangeEval >= 0)
                     || (isPromotion && (promotionPiece(m) % 6 == QUEEN)))
                        ? 0
                        : lmrReductions[depth][legalMoves];

        // depending on if lmr is used, we adjust the lmr score using history scores and kk-reductions.
        if (lmr) {
            int history = sd->getHistoryMoveScore(m, b->getActivePlayer()) - 512;
            if (ply > 0)
                history += sd->getCounterMoveHistoryScore(b->getPreviousMove(), m) - 512;
            lmr = lmr - history / 256;
            lmr += !isImproving;
            if (sd->sideToReduce != b->getActivePlayer()) {
                lmr = lmr + 1;
            }
            if (lmr > MAX_PLY) {
                lmr = 0;
            }
            if (lmr > depth - 2) {
                lmr = depth - 2;
            }
        }

        // doing the move
        b->move(m);

        // adjust the extension policy for checks. we could use the givesCheck value but it has not been validated to
        // work 100%
        if (extension == 0 && b->isInCheck(b->getActivePlayer()))
            extension = 1;

        // principal variation search recursion.
        if (legalMoves == 0) {
            score = -pvSearch(b, -beta, -alpha, depth - ONE_PLY + extension, ply + ONE_PLY, td, 0);
        } else {
            score = -pvSearch(b, -alpha - 1, -alpha, depth - ONE_PLY - lmr + extension, ply + ONE_PLY, td, 0);
            if (lmr && score > alpha)
                score = -pvSearch(b, -alpha - 1, -alpha, depth - ONE_PLY + extension, ply + ONE_PLY, td,
                                  0);    // re-search
            if (score > alpha && score < beta)
                score = -pvSearch(b, -beta, -alpha, depth - ONE_PLY + extension, ply + ONE_PLY, td,
                                  0);    // re-search
        }

        // undo the move
        b->undoMove();

        // if we got a new best score for this node, update the highest score and keep track of the best move
        if (score > highestScore) {
            highestScore = score;
            bestMove     = m;
            if (ply == 0 && isTimeLeft() && td->threadID == 0) {
                // Store bestMove for bestMove
                sd->bestMove = m;
                // the time manager needs to be updated to know if its safe to stop the search
                search_timeManager->updatePV(m, score, depth);
            }
        }

        // beta -cutoff
        if (score >= beta) {
            if (!skipMove) {
                // put the beta cutoff into the perft_tt
                table->put(zobrist, score, m, CUT_NODE, depth);
                // also set this move as a killer move into the history
                sd->setKiller(m, ply, b->getActivePlayer());
                // if the move is not a capture, we also update counter move history tables and history scores.
                if (!isCapture(m)) {
                    sd->addHistoryScore(m, depth, mv, b->getActivePlayer());
                    sd->addCounterMoveHistoryScore(b->getPreviousMove(), m, depth, mv);
                }
            }
            return beta;
        }

        // we consider this seperate to having a new best score for simplicity
        if (score > alpha) {
            // increase alpha
            alpha = score;
        }

        // if this loop finished, we can increment the legal move counter by one which is important for detecting mates
        legalMoves++;
    }

    // if we are inside a tournament game and at the root and there is only one legal move, no need to search at all.
    if (search_timeManager->getMode() == TOURNAMENT && ply == 0 && legalMoves == 1) {
        search_timeManager->stopSearch();
        return alpha;
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

    // we need to write the current score/position into the transposition table if and only if we havent skipped a move
    // due to our extension policy.
    if (!skipMove) {
        if (alpha > originalAlpha) {
            table->put(zobrist, highestScore, bestMove, PV_NODE, depth);
        } else {
            table->put(zobrist, highestScore, bestMove, ALL_NODE, depth);
        }
    }

    return alpha;
}

/**
 * a more selective search than pv-search in which we only consider captures and promitions.
 *
 * @param b
 * @param alpha
 * @param beta
 * @param ply
 * @return
 */
Score qSearch(Board* b, Score alpha, Score beta, Depth ply, ThreadData* td) {

    // increase the nodes for this thread
    td->nodes++;

    // extract information like search data (history tables), zobrist etc
    SearchData* sd         = td->searchData;
    U64         zobrist    = b->zobrist();
    Entry       en         = table->get(b->zobrist());
    NodeType    ttNodeType = ALL_NODE;

    // **************************************************************************************************************
    // transposition table probing:
    // we probe the transposition table and check if there is an entry with the same zobrist key as the current
    // position. As we have no information about the depth, we will allways use the perft_tt entry.
    // **************************************************************************************************************
    if (en.zobrist == zobrist) {
        if (en.type == PV_NODE) {
            return en.score;
        } else if (en.type == CUT_NODE) {
            if (en.score >= beta) {
                return en.score;
            }
        } else if (en.type == ALL_NODE) {
            if (en.score <= alpha) {
                return en.score;
            }
        }
    }

    // check if we are currently in check
    bool inCheck = b->isInCheck(b->getActivePlayer());

    // the idea for the static evaluation is that if the last move has been a null move, we can reuse the eval and
    // simply adjust the tempo-bonus.
    Score stand_pat;
    if (b->getPreviousMove() == 0 && ply != 0) {
        // reuse static evaluation from previous ply incase of nullmove
        stand_pat = -sd->eval[1 - b->getActivePlayer()][ply - 1] + sd->evaluator.evaluateTempo(b) * 2;
    } else {
        stand_pat =
            inCheck ? -MAX_MATE_SCORE + ply : sd->evaluator.evaluate(b) * ((b->getActivePlayer() == WHITE) ? 1 : -1);
    }
    
    //we can also use the perft_tt entry to adjust the evaluation.
    if (en.zobrist == zobrist) {
        // adjusting eval
        if ((en.type == PV_NODE) || (en.type == CUT_NODE && stand_pat < en.score)
            || (en.type == ALL_NODE && stand_pat > en.score)) {

            stand_pat = en.score;
        }
    }
    
    if (stand_pat >= beta)
        return beta;
    if (alpha < stand_pat)
        alpha = stand_pat;

    // extract all:
    //- captures (including e.p.)
    //- promotions
    //
    // moves that give check are not considered non-quiet in
    // getNonQuietMoves() although they are not quiet.
    //
    MoveList* mv = sd->moves[ply];
    b->getNonQuietMoves(mv);

    // create a moveorderer to sort the moves during the search
    MoveOrderer moveOrderer {};
    moveOrderer.setMovesQSearch(mv, b);

    // keping track of the best move for the trasnpositions
    Move  bestMove  = 0;
    Score bestScore = -MAX_MATE_SCORE;

    for (int i = 0; i < mv->getSize(); i++) {

        Move m = moveOrderer.next();

        // do not consider illegal moves
        if (!b->isLegal(m))
            continue;

        // **********************************************************************************************************
        // static exchange evaluation pruning (see pruning):
        // if the depth is small enough and the static exchange evaluation for the given move is very negative, dont
        // consider this quiet move as well.
        // **********************************************************************************************************
        if (!inCheck && (getCapturedPiece(m) % 6) < (getMovingPiece(m) % 6) && b->staticExchangeEvaluation(m) < 0)
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

    // store the current position inside the transposition table
    if (bestMove)
        table->put(zobrist, bestScore, bestMove, ttNodeType, 0);
    return alpha;

    //    return 0;
}
