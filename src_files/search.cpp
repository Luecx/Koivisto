
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

#include "search.h"

#include "History.h"
#include "TimeManager.h"
#include "UCIAssert.h"
#include "movegen.h"
#include "polyglot.h"
#include "syzygy/tbprobe.h"

#include <thread>

TranspositionTable*      table = nullptr;
TimeManager*             search_timeManager;
std::vector<std::thread> runningThreads;
int                      threadCount = 1;
bool                     useTB       = false;
bool                     printInfo   = true;

SearchOverview           overview;

int                      lmrReductions[256][256];

// data about each thread. this contains nodes, depth etc as well as a pointer to the history tables
ThreadData               tds[MAX_THREADS] {};

int                      RAZOR_MARGIN     = 198;
int                      FUTILITY_MARGIN  = 92;
int                      SE_MARGIN_STATIC = 0;
int                      LMR_DIV          = 215;

void                     initLMR() {
    int d, m;

    for (d = 0; d < 256; d++)
        for (m = 0; m < 256; m++)
            lmrReductions[d][m] = 1.25 + log(d) * log(m) * 100 / LMR_DIV;
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
        tn += tds[i].nodes;
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
        maxSd = tds[i].seldepth > maxSd ? tds[i].seldepth : maxSd;
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
        th += tds[i].tbhits;
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
 * clears the history table of all the active threads
 */
void search_clearHistory() {
    for (int i = 0; i < threadCount; i++) {
        if (tds[i].searchData != nullptr) {
            delete tds[i].searchData;
        }
        tds[i].searchData = new SearchData();
    }
}

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
    UCI_ASSERT(board);

    return board->getTeamOccupiedBB()[color]
           == ((board->getPieceBB()[PAWN + color * 8] | board->getPieceBB()[KING + color * 8]));
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
void search_setHashSize(int hashSize) { table->setSize(hashSize); }

void search_setThreads(int threads) {
    int processor_count = (int) std::thread::hardware_concurrency();
    if (processor_count == 0)
        processor_count = MAX_THREADS;
    if (processor_count < threads)
        threads = processor_count;
    if (threads < 1)
        threads = 1;
    if (threads > MAX_THREADS)
        threads = MAX_THREADS;
    threadCount = threads;
    for (int i = 0; i < threadCount; i++) {
        if (tds[i].searchData != nullptr) {
            delete tds[i].searchData;
        }
        tds[i].searchData = new SearchData();
    }
}

/**
 * called at the start of the program
 */
void search_init(int hashSize) {
    if (table != nullptr)
        delete table;
    table = new TranspositionTable(hashSize);
    initLMR();

    for (int i = 0; i < MAX_THREADS; i++) {
        tds[i].threadID = i;
    }
    tds[0].searchData = new SearchData();
}

/**
 * called at the exit of the program to cleanup and deallocate arrays.
 */
void search_cleanUp() {
    delete table;
    table = nullptr;

    for (int i = 0; i < MAX_THREADS; i++) {
        if (tds[i].searchData != nullptr) {
            delete tds[i].searchData;
            tds[i].searchData = nullptr;
        }
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
    UCI_ASSERT(b);
    UCI_ASSERT(mvList);

    if (depth <= 0)
        return;

    U64   zob = b->zobrist();
    Entry en  = table->get(zob);
    if (en.zobrist == zob && en.type == PV_NODE) {

        // extract the move from the table
        Move     mov = en.move;

        // get a movelist which can be used to store all pseudo legal moves
        MoveList mvStorage;
        // extract pseudo legal moves
        generatePerftMoves(b, &mvStorage);

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
    UCI_ASSERT(b);

    if (!printInfo)
        return;

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

    std::cout << " nodes " << nodes << " nps " << nps << " time " << search_timeManager->elapsedTime()
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
/**
 * probes the wdl tables if tablebases can be used.
 */
Score getWDL(Board* board) {
    UCI_ASSERT(board);

    // we cannot prove the tables if there are too many pieces on the board
    if (bitCount(board->getOccupiedBB()) > (signed) TB_LARGEST)
        return MAX_MATE_SCORE;

    // use the given files to prove the tables using the information from the board.
    unsigned res = tb_probe_wdl(board->getTeamOccupiedBB()[WHITE], board->getTeamOccupiedBB()[BLACK],
                                board->getPieceBB()[WHITE_KING] | board->getPieceBB()[BLACK_KING],
                                board->getPieceBB()[WHITE_QUEEN] | board->getPieceBB()[BLACK_QUEEN],
                                board->getPieceBB()[WHITE_ROOK] | board->getPieceBB()[BLACK_ROOK],
                                board->getPieceBB()[WHITE_BISHOP] | board->getPieceBB()[BLACK_BISHOP],
                                board->getPieceBB()[WHITE_KNIGHT] | board->getPieceBB()[BLACK_KNIGHT],
                                board->getPieceBB()[WHITE_PAWN] | board->getPieceBB()[BLACK_PAWN],
                                board->getBoardStatus()->fiftyMoveCounter,
                                board->getCastlingRights(0) | board->getCastlingRights(1)
                                    | board->getCastlingRights(2) | board->getCastlingRights(3),
                                board->getEnPassantSquare() != -1 ? board->getEnPassantSquare() : 0,
                                board->getActivePlayer() == WHITE);

    // if the result failed, we return the max_mate_score internally. This is not used within the
    // search and will be catched later.
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
 * The displayed depth is usually the distance to zero which is the distance until the 50-move rule is
 * reset.
 */
Move getDTZMove(Board* board) {
    UCI_ASSERT(board);

    if (bitCount(board->getOccupiedBB()) > (signed) TB_LARGEST)
        return 0;

    unsigned result =
        tb_probe_root(board->getTeamOccupiedBB()[WHITE], board->getTeamOccupiedBB()[BLACK],
                      board->getPieceBB()[WHITE_KING] | board->getPieceBB()[BLACK_KING],
                      board->getPieceBB()[WHITE_QUEEN] | board->getPieceBB()[BLACK_QUEEN],
                      board->getPieceBB()[WHITE_ROOK] | board->getPieceBB()[BLACK_ROOK],
                      board->getPieceBB()[WHITE_BISHOP] | board->getPieceBB()[BLACK_BISHOP],
                      board->getPieceBB()[WHITE_KNIGHT] | board->getPieceBB()[BLACK_KNIGHT],
                      board->getPieceBB()[WHITE_PAWN] | board->getPieceBB()[BLACK_PAWN],
                      board->getBoardStatus()->fiftyMoveCounter,
                      board->getCastlingRights(0) | board->getCastlingRights(1)
                          | board->getCastlingRights(2) | board->getCastlingRights(3),
                      board->getEnPassantSquare() != -1 ? board->getEnPassantSquare() : 0,
                      board->getActivePlayer() == WHITE, NULL);

    // if the result failed for some reason or the game is over, dont do anything
    if (result == TB_RESULT_FAILED || result == TB_RESULT_CHECKMATE || result == TB_RESULT_STALEMATE)
        return 0;

    // we need the wdl and the dtz values to get the score.
    int   dtz = TB_GET_DTZ(result);
    int   wdl = TB_GET_WDL(result);

    Score s   = 0;

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

    // get the promotion piece if the target move is a promotion (this does not yet work the way it
    // should)
    Piece     promo  = 6 - TB_GET_PROMOTES(result);

    // gets the square from and square to for the move which should be played
    Square    sqFrom = TB_GET_FROM(result);
    Square    sqTo   = TB_GET_TO(result);

    // we generate all pseudo legal moves and check for equality between the moves to make sure the
    // bits are correct.
    MoveList* mv     = new MoveList();
    generatePerftMoves(board, mv);

    for (int i = 0; i < mv->getSize(); i++) {
        // get the current move from the movelist
        Move m = mv->getMove(i);

        // check if its the same.
        if (getSquareFrom(m) == sqFrom && getSquareTo(m) == sqTo) {
            if ((promo == 6 && !isPromotion(m))
                || (isPromotion(m) && promo < 6 && getPromotionPieceType(m) == promo)) {

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
Move           bestMove(Board* b, Depth maxDepth, TimeManager* timeManager, int threadId) {
    UCI_ASSERT(b);
    UCI_ASSERT(timeManager);

    // if the main thread calls this function, we need to generate the search data for all the threads
    // first
    if (threadId == 0) {

        // if there is a dtz move available, do not start any threads or search at all. just do the
        // dtz move
        Move dtzMove = getDTZMove(b);
        if (dtzMove != 0)
            return dtzMove;

        if (PolyGlot::book.enabled) {
            Move bookmove = PolyGlot::book.probe(*b);
            if (bookmove)
                return bookmove;
        }

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
            tds[i].threadID = i;
            tds[i].tbhits   = 0;
            tds[i].nodes    = 0;
            tds[i].seldepth = 0;
        }

        // we will call this function for the other threads which will skip this part and jump
        // straight to the part below
        for (int n = 1; n < threadCount; n++) {
            runningThreads.emplace_back(bestMove, b, maxDepth, timeManager, n);
        }
    }

    // the thread id starts at 0 for the first thread
    ThreadData* td = &tds[threadId];
    // start the basic search on all threads
    Depth       d  = 1;
    Score       s  = 0;

    // we will create a copy of the board object which will be used during search
    // This is relevant as multiple threads can clearly not use the same object.
    // Also its relevant because if we stop the search even if the search has not finished, the board
    // object will have a random position from the tree. Using this would lead to an illegal/not
    // existing pv
    Board       searchBoard {b};
    Board       printBoard {b};
    td->dropOut = false;
    for (d = 1; d <= maxDepth; d++) {

        if (d < 6) {
            s = pvSearch(&searchBoard, -MAX_MATE_SCORE, MAX_MATE_SCORE, d, 0, td, 0, 2);
        } else {
            Score window = 10;
            Score alpha  = s - window;
            Score beta   = s + window;

            while (isTimeLeft()) {
                s = pvSearch(&searchBoard, alpha, beta, d, 0, td, 0, 2);

                window += window;
                if (window > 500)
                    window = MIN_MATE_SCORE;
                if (s >= beta) {
                    beta += window;
                } else if (s <= alpha) {
                    beta = (alpha + beta) / 2;
                    alpha -= window;
                } else {
                    break;
                }
            }
        }

        if (threadId == 0) {
            printInfoString(&printBoard, d, s);
        }

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
        Move best      = td->searchData->bestMove;

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
Score pvSearch(Board* b, Score alpha, Score beta, Depth depth, Depth ply, ThreadData* td,
               Move skipMove, int behindNMP, Depth* lmrFactor) {
    UCI_ASSERT(b);
    UCI_ASSERT(td);
    UCI_ASSERT(beta > alpha);
    UCI_ASSERT(ply >= 0);

    // increment the node counter for the current thread
    td->nodes++;

    // force a stop when enough nodes have been searched
    if (search_timeManager->getNodeLimit() <= td->nodes) {
        search_timeManager->stopSearch();
    }

    // check if a stop is forced
    if (search_timeManager->isForceStopped()) {
        td->dropOut = true;
        return beta;
    }

    // if the time is over, we fail hard to stop the search. We don't want to call the system clock
    // too often for speed reasons so we only apply this when the depth is larger than 6.
    if ((depth > 6 && !isTimeLeft())) {
        td->dropOut = true;
        return beta;
    }

    // if its a draw by 3-fold or 50-move rule, we return a drawscore
    if (b->isDraw() && ply > 0) {
        // The idea of draw randomization originated in sf. According to conventional wisdom the key
        // point is to force the search to explore different variations. For example in Stockfish and
        // Ethereal the evaluation is increased / decreased by 1 score grain. The implementation in
        // Koivisto is based on a different idea, namely the Beal effect. (see
        // https://www.chessprogramming.org/Search_with_Random_Leaf_Values).

        //  Later note: This has not shown to be better in other engines, altough it gained over the
        //  standard implementation in Koi
        //   Weiss now also has a similar implementation to Koi, but its unclear if it is better than
        //   standard either.

        return 8 - (td->nodes & MASK<4>);
    }

    // beside keeping track of the nodes, we need to keep track of the selective depth for this
    // thread.
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
    Score       ownThreats   = 0;
    Score       enemyThreats = 0;
    // the idea for the static evaluation is that if the last move has been a null move, we can reuse
    // the eval and simply adjust the tempo-bonus. We also get the threat information if the position
    // has actually been evaluated.
    
    if (inCheck)
        staticEval = -MAX_MATE_SCORE + ply;
    else {
        staticEval = b->evaluate() * ((b->getActivePlayer() == WHITE) ? 1 : -1);
//        ownThreats   = sd->evaluator.evalData.threats[b->getActivePlayer()];
//        enemyThreats = sd->evaluator.evalData.threats[!b->getActivePlayer()];
    }

    // we check if the evaluation improves across plies.
    sd->setHistoricEval(staticEval, b->getActivePlayer(), ply);
    bool  isImproving = inCheck ? false : sd->isImproving(staticEval, b->getActivePlayer(), ply);

    // **************************************************************************************************************
    // transposition table probing:
    // we probe the transposition table and check if there is an entry with the same zobrist key as
    // the current position. First, we adjust the static evaluation and second, we might be able to
    // return the tablebase score if the depth of that entry is larger than our current depth.
    // **************************************************************************************************************
    Entry en          = table->get(zobrist);

    if (en.zobrist == zobrist && !skipMove) {
        hashMove = en.move;

        // adjusting eval
        if ((en.type == PV_NODE) || (en.type == CUT_NODE && staticEval < en.score)
            || (en.type == ALL_NODE && staticEval > en.score)) {

            staticEval = en.score;
        }

        // We treat child nodes of null moves differently. The reason a null move
        // search has to be searched to great depth is to make sure that we dont
        // cut in an unsafe way. Well if the nullmove search fails high, we dont cut anything,
        // we still do a normal search. Thus the standard of proof required is different.
        if (!pv && en.depth + (!b->getPreviousMove() && en.score >= beta) * 100 >= depth) {
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
    // search the wdl table if we are not at the root and the root did not use the wdl table to sort
    // the moves
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

    // reset killer of granchildren
    sd->killer[b->getActivePlayer()][ply + 2][0] = 0;
    sd->killer[b->getActivePlayer()][ply + 2][1] = 0;

    if (!skipMove && !inCheck && !pv) {
        // **********************************************************************************************************
        // razoring:
        // if a qsearch on the current position is far below beta at low depth, we can fail soft.
        // **********************************************************************************************************
        if (depth <= 3 && staticEval + RAZOR_MARGIN < beta) {
            score = qSearch(b, alpha, beta, ply, td);
            if (score < beta) {
                return score;
            } else if (depth == 1)
                return beta;
        }
        // **********************************************************************************************************
        // static null move pruning:
        // if the static evaluation is already above beta with a specific margin, assume that the we
        // will definetly be above beta and stop the search here and fail soft. Also reuse information
        // from eval to prevent pruning if the oponent has multiple threats.
        // **********************************************************************************************************
        if (depth <= 7
//            && MgScore(enemyThreats) < 43
            && staticEval >= beta + depth * FUTILITY_MARGIN
            && staticEval < MIN_MATE_SCORE)
            return staticEval;

        // **********************************************************************************************************
        // threat pruning:
        // if the static evaluation is already above beta at depth 1 and we have strong threats, asume
        // that we can atleast achieve beta
        // **********************************************************************************************************
        if (depth == 1 && staticEval > beta && ownThreats && !enemyThreats)
            return beta;

        // **********************************************************************************************************
        // null move pruning:
        // if the evaluation from a very shallow search after doing nothing is still above beta, we
        // assume that we could achieve beta, so we can return early. Don't do nmp when the oponent
        // has threats or the position or we don't have non-pawn material.
        // **********************************************************************************************************
        if (staticEval >= beta + (5 > depth ? 30 : 0) && !(depth < 5 && enemyThreats > 0)
            && !hasOnlyPawns(b, b->getActivePlayer())) {
            b->move_null();
            score =
                -pvSearch(b, -beta, 1 - beta,
                          depth - (depth / 4 + 3) * ONE_PLY
                              - (staticEval - beta < 300 ? (staticEval - beta) / FUTILITY_MARGIN : 3),
                          ply + ONE_PLY, td, 0, !b->getActivePlayer());
            b->undoMove_null();
            if (score >= beta) {
                return score;
            }
        }
    }

    // we reuse movelists for memory reasons.
    MoveList* mv      = sd->moves[ply];

    // **********************************************************************************************************
    // probcut was first implemented in StockFish by Gary Linscott. See
    // https://www.chessprogramming.org/ProbCut. apart from only doing probcut when we have threats,
    // this is based on other top engines.
    // **********************************************************************************************************

    Score     betaCut = beta + FUTILITY_MARGIN;
    if (!inCheck && !pv && depth > 4 && !skipMove && ownThreats
        && !(hashMove && en.depth >= depth - 3 && en.score < betaCut)) {
        generateNonQuietMoves(b, mv, hashMove, sd, ply);
        MoveOrderer moveOrderer {mv};
        while (moveOrderer.hasNext()) {
            // get the current move
            Move m = moveOrderer.next(0);

            if (!b->isLegal(m))
                continue;

            b->move(m);

            Score qScore = -qSearch(b, -betaCut, -betaCut + 1, ply + 1, td);

            if (qScore >= betaCut)
                qScore = -pvSearch(b, -betaCut, -betaCut + 1, depth - 4, ply + 1, td, 0, behindNMP);

            b->undoMove();

            if (qScore >= betaCut) {
                table->put(zobrist, qScore, m, CUT_NODE, depth - 3);
                return betaCut;
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

    // create a moveorderer and assign the movelist to score the moves.
    generateMoves(b, mv, hashMove, sd, ply);
    MoveOrderer moveOrderer {mv};

    // count the legal and quiet moves.
    int         legalMoves = 0;
    int         quiets     = 0;

    // speedup stuff for movepicking
    Square      kingSq     = bitscanForward(b->getPieceBB(!b->getActivePlayer(), KING));
    U64         kingBB     = *BISHOP_ATTACKS[kingSq] | *ROOK_ATTACKS[kingSq] | KNIGHT_ATTACKS[kingSq];

    // loop over all moves in the movelist
    while (moveOrderer.hasNext()) {

        // get the current move
        Move m = moveOrderer.next(kingBB);

        if (!m)
            break;

        // if the move is the move we want to skip, skip this move (used for extensions)
        if (sameMove(m, skipMove))
            continue;

        // check if the move gives check and/or its promoting
        bool givesCheck  = b->givesCheck(m);
        bool isPromotion = move::isPromotion(m);
        bool quiet       = !isCapture(m) && !isPromotion && !givesCheck;

        if (ply > 0 && legalMoves >= 1 && highestScore > -MIN_MATE_SCORE) {

            Depth moveDepth = std::max(1, depth - lmrReductions[depth][legalMoves]);

            if (quiet) {
                quiets++;
                // **************************************************************************************************
                // late move pruning:
                // if the depth is small enough and we searched enough quiet moves, dont consider this
                // move
                // **************************************************************************************************
                if (depth <= 7 && quiets > lmp[isImproving][depth]) {
                    moveOrderer.skip = true;
                    continue;
                }
                // **************************************************************************************************
                // history pruning:
                // if the history score for a move is really bad at low depth, dont consider this
                // move.
                // **************************************************************************************************
                if (sd->getHistories(m, b->getActivePlayer(), b->getPreviousMove())
                    < std::min(200 - 30 * (depth * depth), 0)) {
                    continue;
                }
            }

            // ******************************************************************************************************
            // static exchange evaluation pruning (see pruning):
            // if the depth we are going to search the move at is small enough and the static exchange
            // evaluation for the given move is very negative, dont consider this quiet move as well.
            // ******************************************************************************************************
            if (moveDepth <= 5 + quiet * 3 && (getCapturedPieceType(m)) < (getMovingPieceType(m))
                && b->staticExchangeEvaluation(m) <= (quiet ? -40 * moveDepth : -100 * moveDepth))
                continue;
        }

        // dont search illegal moves
        if (!b->isLegal(m))
            continue;

        // compute the static exchange evaluation if the move is a capture
        Score staticExchangeEval = 0;
        if (isCapture(m) && (getCapturedPieceType(m)) < (getMovingPieceType(m))) {
            staticExchangeEval = b->staticExchangeEvaluation(m);
        }

        // keep track of the depth we want to extend by
        int extension = 0;

        // *********************************************************************************************************
        // singular extensions
        // standard implementation apart from the fact that we cancel lmr of parent node in-case the
        // node turns out to be singular. Also standard multi-cut.
        // *********************************************************************************************************
        if (depth >= 8 && !skipMove && legalMoves == 0 && sameMove(m, hashMove) && ply > 0 && !inCheck
            && en.zobrist == zobrist && abs(en.score) < MIN_MATE_SCORE
            && (en.type == CUT_NODE || en.type == PV_NODE) && en.depth >= depth - 3) {

            betaCut = en.score - SE_MARGIN_STATIC - depth * 2;
            score   = pvSearch(b, betaCut - 1, betaCut, depth >> 1, ply, td, m, behindNMP);
            if (score < betaCut) {
                if (lmrFactor != nullptr) {
                    depth += *lmrFactor;
                    *lmrFactor = 0;
                }
                extension++;
            } else if (score >= beta) {
                return score;
            } else if (en.score >= beta) {
                score = pvSearch(b, beta - 1, beta, (depth >> 1) + 3, ply, td, m, behindNMP);
                if (score >= beta)
                    return score;
            }
            generateMoves(b, mv, hashMove, sd, ply);
            moveOrderer = {mv};

            m           = moveOrderer.next(0);
        }

        // *********************************************************************************************************
        // kk reductions:
        // we reduce more/less depending on which side we are currently looking at. The idea behind
        // this is probably quite similar to the cutnode stuff found in stockfish, altough the
        // implementation is quite different and it also is different functionally. Stockfish type
        // cutnode stuff has not gained in Koivisto, while this has.
        // *********************************************************************************************************
        if (pv) {
            sd->sideToReduce = !b->getActivePlayer();
            sd->reduce       = false;
            if (legalMoves == 0) {
                sd->reduce = true;
            }
        }

        // compute the lmr based on the depth, the amount of legal moves etc.
        // we dont want to reduce if its the first move we search, or a capture with a positive see
        // score or if the depth is too small. furthermore no queen promotions are reduced
        Depth lmr = (legalMoves < 2 || depth <= 2 || (isCapture(m) && staticExchangeEval >= 0)
                     || (isPromotion && (getPromotionPieceType(m) == QUEEN)))
                        ? 0
                        : lmrReductions[depth][legalMoves];

        // increase reduction if we are behind a null move, depending on which side we are looking at.
        // this is a sound reduction in theory.
        if (legalMoves > 0 && depth > 2 && b->getActivePlayer() == behindNMP)
            lmr++;

        // depending on if lmr is used, we adjust the lmr score using history scores and kk-reductions
        // etc. Most conditions are standard and should be considered self explanatory.
        if (lmr) {
            lmr = lmr - sd->getHistories(m, b->getActivePlayer(), b->getPreviousMove()) / 150;
            lmr += !isImproving;
            lmr -= pv;
            if (sd->isKiller(m, ply, b->getActivePlayer()))
                lmr--;
            if (sd->reduce && sd->sideToReduce != b->getActivePlayer())
                lmr++;
            if (lmr > MAX_PLY) {
                lmr = 0;
            }
            if (lmr > depth - 2) {
                lmr = depth - 2;
            }
        }

        // doing the move
        b->move(m);

        // adjust the extension policy for checks. we could use the givesCheck value but it has not
        // been validated to work 100%
        if (extension == 0 && b->isInCheck(b->getActivePlayer()))
            extension = 1;

        mv->scoreMove(moveOrderer.counter - 1, depth);

        // principal variation search recursion.
        if (legalMoves == 0) {
            score = -pvSearch(b, -beta, -alpha, depth - ONE_PLY + extension, ply + ONE_PLY, td, 0,
                              behindNMP);
        } else {
            // kk reduction logic.
            if (ply == 0 && lmr) {
                sd->reduce       = true;
                sd->sideToReduce = sd->sideToReduce = !b->getActivePlayer();
            }
            // reduced search.
            score = -pvSearch(b, -alpha - 1, -alpha, depth - ONE_PLY - lmr + extension, ply + ONE_PLY,
                              td, 0, behindNMP, &lmr);
            // more kk reduction logic.
            if (pv)
                sd->reduce = true;
            if (ply == 0) {
                sd->sideToReduce = sd->sideToReduce = b->getActivePlayer();
            }
            // at root we research the reduced move with slowly increasing depth untill it
            // fails/proves to be best.
            if (ply == 0) {
                if (lmr && score > alpha) {
                    for (int i = lmr - 1; i > 0; i--) {
                        score = -pvSearch(b, -alpha - 1, -alpha, depth - ONE_PLY - i + extension,
                                          ply + ONE_PLY, td, 0, behindNMP);    // re-search
                        if (score <= alpha)
                            break;
                    }
                }
                // if the move passes all null window searches, search with the full aspiration
                // window.
                if (score > alpha && score < beta)
                    score = -pvSearch(b, -beta, -alpha, depth - ONE_PLY + extension, ply + ONE_PLY,
                                      td, 0, behindNMP);    // re-search
            } else {
                // if not at root use standard logic
                if (lmr && score > alpha)
                    score = -pvSearch(b, -alpha - 1, -alpha, depth - ONE_PLY + extension,
                                      ply + ONE_PLY, td, 0, behindNMP);    // re-search
                if (score > alpha && score < beta)
                    score = -pvSearch(b, -beta, -alpha, depth - ONE_PLY + extension, ply + ONE_PLY,
                                      td, 0, behindNMP);    // re-search
            }
        }

        // undo the move
        b->undoMove();

        // if we got a new best score for this node, update the highest score and keep track of the
        // best move
        if (score > highestScore) {
            highestScore = score;
            bestMove     = m;
            if (ply == 0 && (isTimeLeft() || depth <= 2) && td->threadID == 0) {
                // Store bestMove for bestMove
                sd->bestMove = m;
                alpha        = highestScore;
            }
        }

        // beta -cutoff
        if (score >= beta) {
            if (!skipMove && !td->dropOut) {
                // put the beta cutoff into the perft_tt
                table->put(zobrist, score, m, CUT_NODE, depth);
            }
            // also set this move as a killer move into the history
            if (!isCapture(m))
                sd->setKiller(m, ply, b->getActivePlayer());

            // update history scores
            sd->updateHistories(m, depth, mv, b->getActivePlayer(), b->getPreviousMove());

            return highestScore;
        }

        // we consider this seperate to having a new best score for simplicity
        if (score > alpha) {
            // increase alpha
            alpha = score;
        }

        // if this loop finished, we can increment the legal move counter by one which is important
        // for detecting mates
        legalMoves++;
    }

    // if we are inside a tournament game and at the root and there is only one legal move, no need to
    // search at all.
    if (search_timeManager->getMode() == TOURNAMENT && ply == 0 && legalMoves == 1
        && td->threadID == 0) {
        sd->bestMove = bestMove;
        search_timeManager->stopSearch();
        return staticEval;
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

    // we need to write the current score/position into the transposition table if and only if we
    // havent skipped a move due to our extension policy.
    if (!skipMove && !td->dropOut) {
        if (alpha > originalAlpha) {
            table->put(zobrist, highestScore, bestMove, PV_NODE, depth);
        } else {
            table->put(zobrist, highestScore, bestMove, ALL_NODE, depth);
        }
    }

    return highestScore;
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
Score qSearch(Board* b, Score alpha, Score beta, Depth ply, ThreadData* td, bool inCheck) {
    UCI_ASSERT(b);
    UCI_ASSERT(td);
    UCI_ASSERT(beta > alpha);

    // increase the nodes for this thread
    td->nodes++;

    // extract information like search data (history tables), zobrist etc
    SearchData* sd         = td->searchData;
    U64         zobrist    = b->zobrist();
    Entry       en         = table->get(b->zobrist());
    NodeType    ttNodeType = ALL_NODE;

    // **************************************************************************************************************
    // transposition table probing:
    // we probe the transposition table and check if there is an entry with the same zobrist key as
    // the current position. As we have no information about the depth, we will allways use the
    // perft_tt entry.
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

    // the idea for the static evaluation is that if the last move has been a null move, we can reuse
    // the eval and simply adjust the tempo-bonus.
    Score stand_pat;
    Score bestScore = -MAX_MATE_SCORE;

    stand_pat       = bestScore =
        inCheck ? -MAX_MATE_SCORE + ply : b->evaluate() * ((b->getActivePlayer() == WHITE) ? 1 : -1);

    // we can also use the perft_tt entry to adjust the evaluation.
    if (en.zobrist == zobrist) {
        // adjusting eval
        if ((en.type == PV_NODE) || (en.type == CUT_NODE && stand_pat < en.score)
            || (en.type == ALL_NODE && stand_pat > en.score)) {

            bestScore = en.score;
        }
    }

    if (bestScore >= beta)
        return beta;
    if (alpha < bestScore)
        alpha = bestScore;

    // extract all:
    //- captures (including e.p.)
    //- promotions
    //
    // moves that give check are not considered non-quiet in
    // getNonQuietMoves() although they are not quiet.
    //
    MoveList* mv = sd->moves[ply];

    // create a moveorderer to sort the moves during the search
    generateNonQuietMoves(b, mv);
    MoveOrderer moveOrderer {mv};

    // keping track of the best move for the transpositions
    Move        bestMove = 0;

    for (int i = 0; i < mv->getSize(); i++) {

        Move m = moveOrderer.next(0);

        // do not consider illegal moves
        if (!b->isLegal(m))
            continue;

        // if the move seems to be really good just return beta.
        if (+see_piece_vals[(getPieceType(getCapturedPiece(m)))]
                - see_piece_vals[getPieceType(getMovingPiece(m))] - 300 + stand_pat
            > beta)
            return beta;

        // *******************************************************************************************
        // static exchange evaluation pruning (see pruning):
        // if the depth is small enough and the static exchange evaluation for the given move is very
        // negative, dont consider this quiet move as well.
        // *******************************************************************************************
        if (!inCheck && (getCapturedPieceType(m)) < (getMovingPieceType(m))
            && b->staticExchangeEvaluation(m) < 0)
            continue;

        b->move(m);

        bool  inCheckOpponent = b->isInCheck(b->getActivePlayer());

        Score score           = -qSearch(b, -beta, -alpha, ply + ONE_PLY, td, inCheckOpponent);

        b->undoMove();

        if (score > bestScore) {
            bestScore = score;
            bestMove  = m;
            if (score >= beta) {
                ttNodeType = CUT_NODE;
                // store the move with higher depth in tt incase the same capture would improve on
                // beta in ordinary pvSearch too.
                table->put(zobrist, bestScore, m, ttNodeType, !inCheckOpponent);
                return score;
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
    return bestScore;

    //    return 0;
}
