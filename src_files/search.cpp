
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
#include "attacks.h"
#include "bitboard.h"
#include "history.h"
#include "timemanager.h"
#include "uciassert.h"
#include "movegen.h"
#include "newmovegen.h"
#include "polyglot.h"
#include "syzygy/tbprobe.h"

#include <thread>

using namespace attacks;
using namespace bb;
using namespace move;

int  lmrReductions[256][256];

int  RAZOR_MARGIN     = 190;
int  FUTILITY_MARGIN  = 68;
int  SE_MARGIN_STATIC = 0;
int  LMR_DIV          = 267;

int  lmp[2][8]        = {{0, 2, 3, 5, 8, 12, 17, 23}, {0, 3, 6, 9, 12, 18, 28, 40}};

/**
 * =================================================================================
 *                              S E A R C H
 *                             H E L P E R S
 * =================================================================================
 */

/**
 * checks if given side has only pawns left
 * @return
 */
bool hasOnlyPawns(Board* board, Color color) {
    UCI_ASSERT(board);

    // compare total team occupation with pawn and king bitboard
    return board->getTeamOccupiedBB(color)
           == ((board->getPieceBB(color, PAWN)
              | board->getPieceBB(color, KING)));
}

template<Color color>
U64 getThreatsOfSide(Board* b, SearchData* sd, Depth ply){
    const U64 occupied         = b->getOccupiedBB();
    
    const U64 opp_major  = b->getPieceBB<!color, QUEEN >()
                         | b->getPieceBB<!color, ROOK  >();
    const U64 opp_minor  = b->getPieceBB<!color, KNIGHT>()
                         | b->getPieceBB<!color, BISHOP>();
    const U64 opp_queen  = b->getPieceBB<!color, QUEEN >();
    const U64 pawns      = b->getPieceBB< color, PAWN  >();
    
    // pawn attacks
    U64 pawn_attacks     = color == WHITE ?
                                     shiftNorthEast(pawns) | shiftNorthWest(pawns) :
                                     shiftSouthEast(pawns) | shiftSouthWest(pawns);
    
    // minor attacks
    U64 minor_attacks = 0;
    U64 k = b->getPieceBB<color, KNIGHT>();
    while (k) {
        minor_attacks |= KNIGHT_ATTACKS[bitscanForward(k)];
        k = lsbReset(k);
    }
    k = b->getPieceBB<color, BISHOP>();
    while (k) {
        minor_attacks |= lookUpBishopAttacks(bitscanForward(k), occupied);
        k = lsbReset(k);
    }
    
    // rook attacks
    U64 rook_attacks = 0;
    k = b->getPieceBB(color, ROOK);
    while (k) {
        rook_attacks |= lookUpRookAttacks(bitscanForward(k), occupied);
        k = lsbReset(k);
    }

    // mask pawn attacks only to minor and major pieces
    pawn_attacks  &= opp_major | opp_minor;
    minor_attacks &= opp_major;
    rook_attacks  &= opp_queen;

    sd->threatCount[ply][color]  = bitCount(pawn_attacks );
    sd->threatCount[ply][color] += bitCount(minor_attacks);
    sd->threatCount[ply][color] += bitCount(rook_attacks );

    return pawn_attacks | rook_attacks | minor_attacks;
}

void getThreats(Board* b, SearchData* sd, Depth ply) {
    // compute threats for both sides
    U64 whiteThreats = getThreatsOfSide<WHITE>(b, sd, ply);
    U64 blackThreats = getThreatsOfSide<BLACK>(b, sd, ply);
    
    // get the threats to the active player
    U64 threats = b->getActivePlayer() == WHITE ? blackThreats : whiteThreats;
    
    // store
    if(threats){
        sd->mainThreat[ply] = bitscanForward(threats);
    }else{
        sd->mainThreat[ply] = 64;
    }
}

U64 getNewThreats(Board* b, move::Move m) {
    const Piece  p        = getMovingPieceType(m);
    const U64    occupied = b->getOccupiedBB();
    const Square sqTo     = getSquareTo(m);
    const Square sqFrom   = getSquareFrom(m);
    const Color  color    = b->getActivePlayer();

    U64    attacks        = 0;
    U64 sqBB              = ONE << sqTo; 

    switch (p) {
        case QUEEN:
            return 0;
        case ROOK:
            attacks = lookUpRookAttacks(sqTo, occupied) & ~lookUpRookAttacks(sqFrom, occupied);
            return attacks & (b->getPieceBB(!color, QUEEN));
        case BISHOP:
            attacks = lookUpBishopAttacks(sqTo, occupied) & ~lookUpBishopAttacks(sqFrom, occupied);
            return attacks & (b->getPieceBB(!color, QUEEN) | b->getPieceBB(!color, ROOK));
        case KNIGHT:
            attacks = KNIGHT_ATTACKS[sqTo];
            return attacks & (b->getPieceBB(!color, QUEEN) | b->getPieceBB(!color, ROOK));
        case PAWN:
            attacks = color == WHITE ?
                                     shiftNorthEast(sqBB) | shiftNorthWest(sqBB) :
                                     shiftSouthEast(sqBB) | shiftSouthWest(sqBB);
            return attacks & (b->getPieceBB(!color, QUEEN) | b->getPieceBB(!color, ROOK) | b->getPieceBB(!color, BISHOP) | b->getPieceBB(!color, KNIGHT));
        case KING:
            return 0;
        default:
            __builtin_unreachable();
            return 0;
    }
}

void initLMR() {
    for (int d = 0; d < 256; d++){
        for (int m = 0; m < 256; m++){
            lmrReductions[d][m] = 1.25 + log(d) * log(m) * 100 / LMR_DIV;
        }
    }
}

/**
 * returns the best move for the given board.
 * the search will stop if either the max depth is reached.
 * If tablebases are used, the dtz move will be used if possible. If not, the WDL tables will be used
 * during the search.
 * @param b
 * @return
 */
Move Search::bestMove(Board* b, TimeManager* timeman, int threadId) {
    UCI_ASSERT(b);
    UCI_ASSERT(timeman);
    
    // set the depth to max depth, correct the depth if a depth limit is set
    Depth maxDepth = MAX_PLY;
    if (timeman->depth_limit.enabled)
        maxDepth = std::min((Depth)MAX_PLY, timeman->depth_limit.depth);
    
    // if the main thread calls this function, we need to generate the search data for all the threads
    // first
    if (threadId == 0) {
        
        // store the time manager locally
        // also dtz probing will use the time manager
        this->timeManager = timeman;
        
        // if there is a dtz move available, do not start any threads or search at all. just do the
        // dtz move
        Move dtzMove = this->probeDTZ(b);
        if (dtzMove != 0)
            return dtzMove;

        if (polyglot::book.enabled) {
            Move bookmove = polyglot::book.probe(*b);
            if (bookmove)
                return bookmove;
        }
        
        // we need to reset the hash between searches
        this->table->incrementAge();

        // for each thread, we will reset the thread data like node counts, tablebase hits etc.
        for (size_t i = 0; i < tds.size(); i++) {
            // reseting the thread data
            this->tds[i].threadID = i;
            this->tds[i].tbhits   = 0;
            this->tds[i].nodes    = 0;
            this->tds[i].seldepth = 0;
        }

        // we will call this function for the other threads which will skip this part and jump
        // straight to the part below
        for (int n = 1; n < threadCount; n++) {
            this->runningThreads.emplace_back(&Search::bestMove, this, b, timeman, n);
        }
    }

    // the thread id starts at 0 for the first thread
    ThreadData* td = &this->tds[threadId];
    // initialise the score outside the loop tp keep track of it during iterations.
    // This is required for aspiration windows
    Score score     = 0;
    Score prevScore = 0;
    // we will create a copy of the board object which will be used during search
    // This is relevant as multiple threads can clearly not use the same object.
    // Also, its relevant because if we stop the search even if the search has not finished, the board
    // object will have a random position from the tree. Using this would lead to an illegal/not
    // existing pv
    Board       searchBoard {*b};
    Board       printBoard {*b};
    // dropout means that we stopped the search. It is important to reset this before we
    // start searching.
    td->dropOut = false;
    // start the main iterative deepening loop
    Depth depth;
    for (depth = 1; depth <= maxDepth; depth++) {
        for (uint16_t& len : td->pvLen) {
            len = 0;
        }
        // do not use aspiration windows if we are in the first few operations since they will be
        // done very quick anyway
        if (depth < 6) {
            score = this->pvSearch(&searchBoard, -MAX_MATE_SCORE, MAX_MATE_SCORE, depth, 0, td, 0, 2);
            prevScore = score;
        } else {
            // initial window size
            Score window = 10;
            // lower and upper bounds
            Score alpha  = score - window;
            Score beta   = score + window;
            Depth sDepth = depth;
            // widen the window as long as time is left
            while (this->timeManager->isTimeLeft()) {
                sDepth = sDepth < depth - 3 ? depth - 3 : sDepth;
                score  = this->pvSearch(&searchBoard, alpha, beta, sDepth, 0, td, 0, 2);
                window += window;
                // dont widen the window above a size of 500
                if (window > 500)
                    window = MIN_MATE_SCORE;
                // adjust the alpha/beta bound based on which side has failed
                if (score >= beta) {
                    beta += window;
                    sDepth--;
                } else if (score <= alpha) {
                    beta = (alpha + beta) / 2;
                    alpha -= window;
                } else {
                    break;
                }
            }
        }
        // compute a score which puts the nodes we spent looking at the best move
        // in relation to all the nodes searched so far (only thread local)
        int timeManScore = td->searchData.spentEffort[getSquareFrom(td->searchData.bestMove)]
                                                     [getSquareTo  (td->searchData.bestMove)]
                           * 100 / td->nodes;

        int evalScore    = prevScore - score;
        
        // print the info string if its the main thread
        if (threadId == 0) {
            this->printInfoString(depth, score, td->pv[0], td->pvLen[0]);
        }

        // if the search finished due to timeout, we also need to stop here
        if (!this->timeManager->rootTimeLeft(timeManScore, evalScore))
            break;
    }

    // if the main thread finishes, we will record the data of this thread
    if (threadId == 0) {
        // tell all other threads if they are running to stop the search
        timeman->stopSearch();
        for (std::thread& th : this->runningThreads) {
            th.join();
        }
        this->runningThreads.clear();

        // retrieve the best move from the search
        Move best = td->searchData.bestMove;

        // collect some information which can be used for benching
        this->searchOverview.nodes = this->totalNodes();
        this->searchOverview.depth = depth;
        this->searchOverview.score = score;
        this->searchOverview.time  = timeman->elapsedTime();
        this->searchOverview.move  = best;

        // return the best move if it's the main thread
        return best;
    }
    // return nothing (doesn't matter)
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
Score Search::pvSearch(Board* b, Score alpha, Score beta, Depth depth, Depth ply, ThreadData* td,
                       Move skipMove, int behindNMP, Depth* lmrFactor) {
    UCI_ASSERT(b);
    UCI_ASSERT(td);
    UCI_ASSERT(beta > alpha);
    UCI_ASSERT(ply >= 0);

    // increment the node counter for the current thread
    td->nodes++;

    // force a stop when enough nodes have been searched
    if (   timeManager->node_limit.enabled
        && timeManager->node_limit.nodes <= td->nodes) {
        this->timeManager->stopSearch();
    }

    // check if a stop is forced
    if (timeManager->force_stop) {
        td->dropOut = true;
        return beta;
    }

    // if the time is over, we fail hard to stop the search. We don't want to call the system clock
    // too often for speed reasons, so we only apply this when the depth is larger than 6.
    if ((depth > 6 && !timeManager->isTimeLeft(&td->searchData))) {
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
        if (b->getCurrent50MoveRuleCount() >= 50 && b->isInCheck(b->getActivePlayer())) {
            MoveList mv {};
            generatePerftMoves(b, &mv);
            for (int i = 0; i < mv.getSize(); i++) {
                if (b->isLegal(mv.getMove(i)))
                    return 8 - (td->nodes & MASK<4>);
            }
            return -MAX_MATE_SCORE + ply;
        }
        return 8 - (td->nodes & MASK<4>);
    }

    // check if the active player is in check. used for various pruning decisions.
    bool inCheck = b->isInCheck(b->getActivePlayer());
    
    // beside keeping track of the nodes, we need to keep track of the selective depth for this
    // thread.
    if (ply > td->seldepth) {
        td->seldepth = ply;
    }

    // depth > MAX_PLY means that it overflowed because depth is unsigned.
    if (depth == 0 || depth > MAX_PLY || ply > MAX_PVSEARCH_PLY) {
        return qSearch(b, alpha, beta, ply, td, inCheck);
    }

    // we extract a lot of information about various things.
    SearchData* sd            = &td->searchData;
    U64         key           = b->zobrist();
    bool        pv            = (beta - alpha) != 1;
    Score       originalAlpha = alpha;
    Score       highestScore  = -MAX_MATE_SCORE;
    Score       score         = -MAX_MATE_SCORE;
    Move        bestMove      = 0;
    Move        hashMove      = 0;
    Score       staticEval;
    Score       ownThreats    = 0;
    Score       enemyThreats  = 0;
    Square      mainThreat    = 0;

    // ***********************************************************************************************
    // transposition table probing:
    // we probe the transposition table and check if there is an entry with the same zobrist key as
    // the current position. First, we adjust the static evaluation and second, we might be able to
    // return the tablebase score if the depth of that entry is larger than our current depth.
    // ***********************************************************************************************
    Entry en = table->get(key);
    bb::Score ttScore = scoreFromTT(en.score, ply);

    if (en.zobrist == key >> 32 && !skipMove) {
        hashMove = en.move;

        staticEval = en.eval;

        // We treat child nodes of null moves differently. The reason a null move
        // search has to be searched to great depth is to make sure that we dont
        // cut in an unsafe way. Well if the nullmove search fails high, we dont cut anything,
        // we still do a normal search. Thus the standard of proof required is different.
        if (!pv && en.depth + (!b->getPreviousMove() && ttScore >= beta) * 100 >= depth) {
            if (en.type == PV_NODE) {
                return ttScore;
            } else if (en.type == CUT_NODE) {
                if (ttScore >= beta) {
                    return ttScore;
                }
            } else if (en.type & ALL_NODE) {
                if (ttScore <= alpha) {
                    return ttScore;
                }
            }
        }
    } else {
        if (inCheck)
            staticEval = -MAX_MATE_SCORE + ply;
        else {
            staticEval = b->evaluate();
        }
    }

    if (!inCheck) {
        getThreats(b, sd, ply);
        ownThreats   = sd->threatCount[ply][ b->getActivePlayer()];
        enemyThreats = sd->threatCount[ply][!b->getActivePlayer()];
        mainThreat   = sd->mainThreat [ply];
        
        if (ply > 0 && b->getPreviousMove() != 0) {
            if (sd->eval[!b->getActivePlayer()][ply - 1] > -TB_WIN_SCORE_MIN) {
                int improvement = -staticEval - sd->eval[!b->getActivePlayer()][ply - 1];
                sd->maxImprovement[getSquareFrom(b->getPreviousMove())]
                                  [getSquareTo  (b->getPreviousMove())] = improvement;
            }
        }
    }

    // we check if the evaluation improves across plies.
    sd->setHistoricEval(staticEval, b->getActivePlayer(), ply);
    bool  isImproving = inCheck ? false : sd->isImproving(staticEval, b->getActivePlayer(), ply);

    if (en.zobrist == key >> 32) {
        // adjusting eval
        if (   (en.type == PV_NODE)
            || (en.type == CUT_NODE && staticEval < ttScore)
            || (en.type  & ALL_NODE && staticEval > ttScore)) {
            staticEval = ttScore;
        }
    } 

    // ***********************************************************************************************
    // tablebase probing:
    // search the wdl table if we are not at the root and the root did not use the wdl table to sort
    // the moves
    // ***********************************************************************************************
    if (useTB && ply > 0) {
        Score res = this->probeWDL(b);

        // TB_FAILED is used for no result
        if (res != TB_FAILED) {
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
        if (depth <= 3 && staticEval + RAZOR_MARGIN * depth < beta) {
            score = qSearch(b, alpha, beta, ply, td);
            if (score < beta)
                return score;
        }
        // *******************************************************************************************
        // static null move pruning:
        // if the static evaluation is already above beta with a specific margin, assume that the we
        // will definetly be above beta and stop the search here and fail soft. Also reuse information
        // from eval to prevent pruning if the oponent has multiple threats.
        // *******************************************************************************************
        if (   depth        <= 7
            && staticEval   >= beta + (depth - (isImproving && !enemyThreats)) * FUTILITY_MARGIN
            && staticEval   <  MIN_MATE_SCORE)
            return staticEval;

        if (depth == 1 && staticEval > beta + (isImproving ? 0 : 30) && !enemyThreats)
            return beta;

        // *******************************************************************************************
        // null move pruning:
        // if the evaluation from a very shallow search after doing nothing is still above beta, we
        // assume that we could achieve beta, so we can return early. Don't do nmp when the oponent
        // has threats or the position or we don't have non-pawn material.
        // *******************************************************************************************
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
                // dont return mate/tb scores
                if (score >= TB_WIN_SCORE_MIN)
                    score = beta;

                return score;
            }
        }
    }

    // we reuse movelists for memory reasons.
    moveGen* mGen   = &td->generators[ply];

    // ***********************************************************************************************
    // probcut was first implemented in StockFish by Gary Linscott. See
    // https://www.chessprogramming.org/ProbCut. apart from only doing probcut when we have threats,
    // this is based on other top engines.
    // ***********************************************************************************************

    Score     betaCut = beta + 130;
    if (!inCheck && !pv && depth > 4 && !skipMove && ownThreats
        && !(hashMove && en.depth >= depth - 3 && ttScore < betaCut)) {
        mGen->init(sd, b, ply, 0, 0, 0, Q_SEARCH, 0);
        Move m;
        while ((m = mGen->next())) {
            if (!m)
                break;

            if (!b->isLegal(m))
                continue;

            b->move<true>(m, table);

            Score qScore = -qSearch(b, -betaCut, -betaCut + 1, ply + 1, td);

            if (qScore >= betaCut)
                qScore = -pvSearch(b, -betaCut, -betaCut + 1, depth - 4, ply + 1, td, 0, behindNMP);

            b->undoMove();

            if (qScore >= betaCut) {
                table->put(key, scoreToTT(qScore, ply), m, CUT_NODE, depth - 3, sd->eval[b->getActivePlayer()][ply]);
                return betaCut;
            }
        }
    }

    // ***********************************************************************************************
    // internal iterative deepening by Ed Schröder::
    // http://talkchess.com/forum3/viewtopic.php?f=7&t=74769&sid=64085e3396554f0fba414404445b3120
    // ***********************************************************************************************
    if (depth >= 4 && !hashMove)
        depth--;

    // ***********************************************************************************************
    // mate distance pruning:
    // ***********************************************************************************************
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
    
    Square      kingSq     = bitscanForward(b->getPieceBB(!b->getActivePlayer(), KING));
    U64         occupiedBB = b->getOccupiedBB();
    U64         kingCBB    = attacks::lookUpBishopAttacks(kingSq, occupiedBB) 
                           | attacks::lookUpRookAttacks(kingSq, occupiedBB) 
                           | KNIGHT_ATTACKS[kingSq];
    mGen->init(sd, b, ply, hashMove, b->getPreviousMove(), b->getPreviousMove(2),
               PV_SEARCH, mainThreat, kingCBB);
    // count the legal and quiet moves.
    int         legalMoves      = 0;
    int         quiets          = 0;
    U64         prevNodeCount   = td->nodes;
    U64         bestNodeCount   = 0;

    Move m;
    // loop over all moves in the movelist
    while ((m = mGen->next())) {
        // if the move is the move we want to skip, skip this move (used for extensions)
        if (sameMove(m, skipMove))
            continue;

        if (pv && td->threadID == 0)
            td->pvLen[ply + 1] = 0;

        // check if the move gives check and/or its promoting
        bool givesCheck  = ((ONE << getSquareTo(m)) & kingCBB) ? b->givesCheck(m) : 0;
        bool isPromotion = move::isPromotion(m);
        bool quiet       = !isCapture(m) && !isPromotion && !givesCheck;

        if (ply > 0 && legalMoves >= 1 && highestScore > -MIN_MATE_SCORE) {
            Depth moveDepth = std::max(1, 1 + depth - lmrReductions[depth][legalMoves]);

            if (quiet) {
                quiets++;
                // ***********************************************************************************
                // late move pruning:
                // if the depth is small enough and we searched enough quiet moves, dont consider this
                // move
                // ***********************************************************************************
                if (mGen->shouldSkip())
                    continue;
                
                if (depth <= 7 && quiets >= lmp[isImproving][depth]) {
                    mGen->skip();
                }

                // prune quiet moves that are unlikely to improve alpha
                if (!inCheck
                    && moveDepth <= 7
                    && sd->maxImprovement[getSquareFrom(m)][getSquareTo(m)]
                               + moveDepth * FUTILITY_MARGIN + 100
                               + sd->eval[b->getActivePlayer()][ply]
                           < alpha)
                    continue;

                // ***********************************************************************************
                // history pruning:
                // if the history score for a move is really bad at low depth, dont consider this
                // move.
                // ***********************************************************************************
                if (!inCheck
                    && sd->getHistories(m, b->getActivePlayer(), b->getPreviousMove(),
                                        b->getPreviousMove(2), mainThreat)
                           < std::min(140 - 30 * (depth * (depth + isImproving)), 0)) {
                    continue;
                }
            }

            // ***************************************************************************************
            // static exchange evaluation pruning (see pruning):
            // if the depth we are going to search the move at is small enough and the static exchange
            // evaluation for the given move is very negative, dont consider this quiet move as well.
            // ***************************************************************************************
            if (moveDepth <= 5 + quiet * 3
                && (getCapturedPieceType(m)) < (getMovingPieceType(m))
                && (isCapture(m) ? mGen->lastSee : b->staticExchangeEvaluation(m)) <= (quiet ? -40 * moveDepth : -100 * moveDepth))
                continue;
        }

        // dont search illegal moves
        if (!b->isLegal(m)) {
            quiets -= quiet;
            continue;
        }

        if (ply == 0 && depth == 1) {
            sd->spentEffort[getSquareFrom(m)][getSquareTo(m)] = 0;
        }

        // compute the static exchange evaluation if the move is a capture
        Score staticExchangeEval = isCapture(m) ? mGen->lastSee : 1;

        // keep track of the depth we want to extend by
        int extension = 0;

        // *******************************************************************************************
        // singular extensions
        // standard implementation apart from the fact that we cancel lmr of parent node in-case the
        // node turns out to be singular. Also standard multi-cut.
        // *******************************************************************************************
        if (depth >= 8
            && !skipMove
            && !inCheck
            &&  sameMove(m, hashMove)
            &&  legalMoves == 0
            &&  ply        >  0
            &&  en.depth   >= depth - 3
            &&  abs(ttScore) < MIN_MATE_SCORE
            && (   en.type == CUT_NODE
                || en.type == PV_NODE)) {
            // compute beta cut value
            betaCut = std::min(static_cast<int>(ttScore - SE_MARGIN_STATIC - depth * 2), static_cast<int>(beta));
            // get the score from recursive call
            score   = pvSearch(b, betaCut - 1, betaCut, depth >> 1, ply, td, m, behindNMP);
            if (score < betaCut) {
                if (lmrFactor != nullptr) {
                    depth += *lmrFactor;
                    *lmrFactor = 0;
                }
                extension++;
            } else if (score >= beta) {
                return score;
            } else if (ttScore >= beta) {
                score = pvSearch(b, beta - 1, beta, (depth >> 1) + 3, ply, td, m, behindNMP);
                if (score >= beta)
                    return score;
            }
            mGen->init(sd, b, ply, hashMove, b->getPreviousMove(),
                       b->getPreviousMove(2), PV_SEARCH, mainThreat, kingCBB);
            m = mGen->next();
        }

        if (pv) {
            sd->sideToReduce = !b->getActivePlayer();
            sd->reduce       = false;
            if (legalMoves == 0) {
                sd->reduce = true;
            }
        } else if (depth < 8
               && !skipMove
               && !inCheck
               &&  sameMove(m, hashMove)
               &&  ply > 0
               &&  sd->eval[b->getActivePlayer()][ply] < alpha - 25
               &&  en.type == CUT_NODE) {
            extension = 1;
        }

        U64   nodeCount = td->nodes;

        
        // compute the lmr based on the depth, the amount of legal moves etc.
        // we dont want to reduce if its the first move we search, or a capture with a positive see
        // score or if the depth is too small. furthermore no queen promotions are reduced
        Depth lmr       = (legalMoves < 2 - (hashMove != 0) + pv || depth <= 2
                     || (isCapture(m) && staticExchangeEval > 0)
                     || (isPromotion && (getPromotionPieceType(m) == QUEEN)))
                              ? 0
                              : lmrReductions[depth][legalMoves];

        // increase reduction if we are behind a null move, depending on which side we are looking at.
        // this is a sound reduction in theory.
        if (legalMoves > 0 && depth > 2 && b->getActivePlayer() == behindNMP)
            lmr++;

        if (lmr) {
            int history = sd->getHistories(m, b->getActivePlayer(), b->getPreviousMove(),
                                           b->getPreviousMove(2), mainThreat);
            lmr         = lmr - history / 150;
            lmr += !isImproving;
            lmr -= pv;
            if (!sd->targetReached) 
                lmr++;
            if (sd->isKiller(m, ply, b->getActivePlayer()))
                lmr--;
            if (sd->reduce && sd->sideToReduce != b->getActivePlayer())
                lmr++;
            lmr += std::min(2, std::abs(staticEval - alpha) / 350);
            lmr -= bitCount(getNewThreats(b, m));
            if (lmr > MAX_PLY) {
                lmr = 0;
            }
            if (lmr > depth - 2) {
                lmr = depth - 2;
            }
            if (history > 256*(2-isCapture(m)))
                lmr = 0;
        }

        // doing the move
        b->move<true>(m, table);

        // adjust the extension policy for checks.
        if (extension == 0 && depth > 4 && b->isInCheck(b->getActivePlayer()))
            extension = 1;

        if (sameMove(hashMove, m) && !pv && en.type > ALL_NODE)
            extension = 1;

        // principal variation search recursion.
        if (legalMoves == 0) {
            score = -pvSearch(b, -beta, -alpha, depth - ONE_PLY + extension, ply + ONE_PLY, td, 0,
                              behindNMP);
        } else {
            if (ply == 0 && lmr) {
                sd->reduce       = true;
                sd->sideToReduce = !b->getActivePlayer();
            }
            // reduced search.
            score = -pvSearch(b, -alpha - 1, -alpha, depth - ONE_PLY - lmr + extension, ply + ONE_PLY,
                              td, 0, lmr != 0 ? b->getActivePlayer() : behindNMP, &lmr);
            if (pv)
                sd->reduce = true;
            if (ply == 0) {
                sd->sideToReduce = b->getActivePlayer();
            }

            if (lmr && score > alpha)
                score = -pvSearch(b, -alpha - 1, -alpha, depth - ONE_PLY + extension,
                                  ply + ONE_PLY, td, 0, behindNMP);    // re-search
            if (score > alpha && score < beta)
                score = -pvSearch(b, -beta, -alpha, depth - ONE_PLY + extension, ply + ONE_PLY,
                                  td, 0, behindNMP);    // re-search
        }

        // undo the move
        b->undoMove();

        if (ply == 0) {
            sd->spentEffort[getSquareFrom(m)][getSquareTo(m)] += td->nodes - nodeCount;
        }

        mGen->addSearched(m);

        // if we got a new best score for this node, update the highest score and keep track of the
        // best move
        if (score > highestScore) {
            highestScore = score;
            bestMove     = m;
            if (ply == 0 && (timeManager->isTimeLeft() || depth <= 2) && td->threadID == 0) {
                // Store bestMove for bestMove
                sd->bestMove = m;
                alpha        = highestScore;
            }
            if (pv && td->threadID == 0) {
                td->pv[ply][0] = m;
                memcpy(&td->pv[ply][1], &td->pv[ply + 1][0], sizeof(move::Move) * td->pvLen[ply + 1]);
                td->pvLen[ply] = td->pvLen[ply + 1] + 1;
            }
            bestNodeCount = td->nodes - nodeCount;
        }

        // beta -cutoff
        if (score >= beta) {
            if (!skipMove && !td->dropOut) {
                // put the beta cutoff into the perft_tt
                table->put(key, scoreToTT(score, ply), m, CUT_NODE, depth, sd->eval[b->getActivePlayer()][ply]);
            }
            // also set this move as a killer move into the history
            if (!isCapture(m) && !isPromotion)
                sd->setKiller(m, ply, b->getActivePlayer());

            // update history scores
            mGen->updateHistory(depth + (staticEval < alpha));

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
    if (   timeManager->match_time_limit.enabled
        && ply          == 0
        && legalMoves   == 1
        && td->threadID == 0) {
        // save best move
        sd->bestMove = bestMove;
        timeManager->stopSearch();
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
            table->put(key, scoreToTT(highestScore, ply), bestMove, PV_NODE, depth,
                       sd->eval[b->getActivePlayer()][ply]);
        } else {
            if (hashMove && en.type == CUT_NODE) {
                bestMove = en.move;
            } else if (highestScore == alpha && !sameMove(hashMove, bestMove)) {
                bestMove = 0;
            }

            if (depth > 7 && bestMove && (td->nodes - prevNodeCount) / 2 < bestNodeCount) {
                table->put(key, scoreToTT(highestScore, ply), bestMove, FORCED_ALL_NODE, depth,
                           sd->eval[b->getActivePlayer()][ply]);
            } else {
                table->put(key, scoreToTT(highestScore, ply), bestMove, ALL_NODE, depth,
                           sd->eval[b->getActivePlayer()][ply]);
            }
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
Score Search::qSearch(Board* b, Score alpha, Score beta, Depth ply, ThreadData* td, bool inCheck) {
    UCI_ASSERT(b);
    UCI_ASSERT(td);
    UCI_ASSERT(beta > alpha);

    // increase the nodes for this thread
    td->nodes++;

    // extract information like search data (history tables), zobrist etc
    SearchData* sd         = &td->searchData;
    U64         key        = b->zobrist();
    Entry       en         = table->get(b->zobrist());
    NodeType    ttNodeType = ALL_NODE;
    bb::Score ttScore = scoreFromTT(en.score, ply);

    Score stand_pat;
    Score bestScore = -MAX_MATE_SCORE;

    // ***********************************************************************************************
    // transposition table probing:
    // we probe the transposition table and check if there is an entry with the same zobrist key as
    // the current position. As we have no information about the depth, we will allways use the
    // perft_tt entry.
    // ***********************************************************************************************

    if (en.zobrist == key >> 32) {

        if (en.type == PV_NODE) {
            return ttScore;
        } else if (en.type == CUT_NODE) {
            if (ttScore >= beta - 25) {
                return ttScore;
            }
        } else if (en.type & ALL_NODE) {
            if (ttScore <= alpha + 25) {
                return ttScore;
            }
        }
        stand_pat = bestScore = en.eval;
    } else {
        stand_pat = bestScore = inCheck ? -MAX_MATE_SCORE + ply : b->evaluate();
    }

    // we can also use the tt entry to adjust the evaluation.
    if (en.zobrist == key >> 32) {
        // adjusting eval
        if (   (en.type == PV_NODE)
            || (en.type == CUT_NODE && stand_pat < ttScore)
            || (en.type  & ALL_NODE && stand_pat > ttScore)) {
            // save as best score
            bestScore = ttScore;
        }
    }

    if (bestScore >= beta || ply >= MAX_INTERNAL_PLY)
        return bestScore;
    if (alpha < bestScore)
        alpha = bestScore;

    
    moveGen* mGen   = &td->generators[ply];
    mGen->init(sd, b, ply, 0, b->getPreviousMove(), b->getPreviousMove(2), Q_SEARCH + inCheck, 0);

    // keping track of the best move for the transpositions
    Move        bestMove = 0;
    Move        m;

    while ((m = mGen->next())) {
        // do not consider illegal moves
        if (!b->isLegal(m))
            continue;

        // *******************************************************************************************
        // static exchange evaluation pruning (see pruning):
        // if the depth is small enough and the static exchange evaluation for the given move is very
        // negative, dont consider this quiet move as well.
        // *******************************************************************************************
        Score see =
            (!inCheck && (isCapture(m) || isPromotion(m))) ? mGen->lastSee : 0;
        if (see < 0)
            continue;
        if (see + stand_pat > beta + 200)
            return beta;
        

        b->move<true>(m, table);

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
                table->put(key, scoreToTT(bestScore, ply), m, ttNodeType, !inCheckOpponent, stand_pat);
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
        table->put(key, scoreToTT(bestScore, ply), bestMove, ttNodeType, 0, stand_pat);
    return bestScore;
}

void Search::init(int hashsize) {
    if (table != nullptr)
        delete table;
    table = new TranspositionTable(hashsize);
    initLMR();

    setThreads(1);
}
void Search::cleanUp() {
    delete table;
    table = nullptr;

    tds.clear();
}
U64 Search::totalNodes() const {
    U64 total = 0;
    for (const auto &td : tds) {
        total += td.nodes;
    }
    return total;
}
int Search::selDepth() const {
    int maxSd = 0;
    for (const auto &td : tds) {
        maxSd = std::max(td.seldepth, maxSd);
    }
    return maxSd;
}
U64 Search::tbHits() const {
    int total = 0;
    for (const auto &td : tds) {
        total += td.tbhits;
    }
    return total;
}

move::MoveList Search::legals(Board* board) const {
    // create a movelist to store all moves and one to only store the legal ones
    MoveList ml{};
    MoveList ml_legal{};
    generatePerftMoves(board, &ml);
    // go through each move and check the move for legality
    for(int i = 0; i < ml.getSize();i++){
        // increase the legal count if the move is legal
        if(board->isLegal(ml.getMove(i))) {
            ml_legal.add(ml.getMove(i));
        }
    }
    return ml_legal;
}

SearchOverview Search::overview() const { return this->searchOverview; }
void           Search::enableInfoStrings() { this->printInfo = true; }
void           Search::disableInfoStrings() { this->printInfo = false; }
void           Search::useTableBase(bool val) { this->useTB = val; }
void           Search::clearHistory() {
    for (auto &td : tds) {
        memset(&td.searchData.th, 0, 2*64*4096*4);
        memset(&td.searchData.captureHistory, 0, 2*4096*4);
        memset(&td.searchData.cmh, 0, 384*2*384*4);
        memset(&td.searchData.fmh, 0, 384*2*384*4);
        memset(&td.searchData.killer, 0, 2*257*2*4);
        memset(&td.searchData.maxImprovement, 0, 64*64*4);
    }
}
void Search::clearHash() { this->table->clear(); }
void Search::setThreads(int threads) {
    int processor_count = static_cast<int>(std::thread::hardware_concurrency());
    if (processor_count == 0)
        processor_count = MAX_THREADS;
    if (processor_count < threads)
        threads = processor_count;
    if (threads < 1)
        threads = 1;
    if (threads > MAX_THREADS)
        threads = MAX_THREADS;
    threadCount = threads;
    tds.clear();
    for (int i = 0; i < threadCount; i++) {
        tds.emplace_back();
    }
}
void Search::setHashSize(int hashSize) {
    if (table)
        table->setSize(hashSize);
}
void Search::stop() {
    if (timeManager)
        timeManager->stopSearch();
}
void Search::printInfoString(Depth depth, Score score, Move* pv, uint16_t pvLen) {

    if (!printInfo)
        return;

    // extract nodes, seldepth and nps
    U64 nodes       = totalNodes();
    U64 sel_depth   = selDepth();
    U64 tb_hits     = tbHits();
    U64 nps         = static_cast<U64>(nodes * 1000) /
                      static_cast<U64>(timeManager->elapsedTime() + 1);

    // print basic info string including depth and seldepth
    std::cout << "info"
              << " depth "          << static_cast<int>(depth)
              << " seldepth "       << static_cast<int>(sel_depth);

    // print the score. if its a mate score, show mate xx instead of cp xx
    if (abs(score) > MIN_MATE_SCORE) {
        std::cout << " score mate " << (MAX_MATE_SCORE - abs(score) + 1) / 2 * (score > 0 ? 1 : -1);
    } else {
        std::cout << " score cp "   << score;
    }
    // show tablebase hits if tablebase has been hit
    if (tb_hits != 0) {
        std::cout << " tbhits "     << tb_hits;
    }
    // show remaining information (nodes, nps, time, hash usage)
    std::cout << " nodes "          << nodes
              << " nps "            << nps
              << " time "           << timeManager->elapsedTime()
              << " hashfull "       << static_cast<int>(table->usage() * 1000);

    // print "pv" to shell
    std::cout << " pv";
    // go through each move
    for (int i = 0; i < pvLen; i++) {
        // transform move to a string and append it
        std::cout << " " << toString(pv[i]);
    }
    // new line
    std::cout << std::endl;
}
/**
 * probes the wdl tables if tablebases can be used.
 */
Score Search::probeWDL(Board* board) {
    UCI_ASSERT(board);

    if (!useTB)
        return MAX_MATE_SCORE;
    // we cannot prove the tables if there are too many pieces on the board
    if (bitCount(board->getOccupiedBB()) > (signed) TB_LARGEST)
        return MAX_MATE_SCORE;
    
    U64 w_occ   = board->getTeamOccupiedBB()[WHITE];
    U64 b_occ   = board->getTeamOccupiedBB()[BLACK];
    U64 pawns   = board->getPieceBB<WHITE, PAWN  >() | board->getPieceBB<BLACK, PAWN  >();
    U64 knights = board->getPieceBB<WHITE, KNIGHT>() | board->getPieceBB<BLACK, KNIGHT>();
    U64 bishops = board->getPieceBB<WHITE, BISHOP>() | board->getPieceBB<BLACK, BISHOP>();
    U64 rooks   = board->getPieceBB<WHITE, ROOK  >() | board->getPieceBB<BLACK, ROOK  >();
    U64 queens  = board->getPieceBB<WHITE, QUEEN >() | board->getPieceBB<BLACK, QUEEN >();
    U64 kings   = board->getPieceBB<WHITE, KING  >() | board->getPieceBB<BLACK, KING  >();

    U64    fifty_mr     =   board->getBoardStatus()->fiftyMoveCounter;
    bool   any_castling = !!(board->getBoardStatus()->castlingRights & MASK<4>);
    Square ep_square    = std::max((Square) 0, board->getEnPassantSquare());  // board uses -1 as e.p.
    Color  whiteToMove  = board->getActivePlayer() == WHITE;
    
    // use the given files to prove the tables using the information from the board.
    unsigned res = tb_probe_wdl(w_occ, b_occ, kings, queens, rooks, bishops, knights, pawns, fifty_mr,
                                any_castling, ep_square, whiteToMove);

    // if the result failed, we return the max_mate_score internally. This is not used within the
    // search and will be catched later.
    if (res == TB_RESULT_FAILED) {
        return TB_FAILED;
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
    return TB_FAILED;
}

/**
 * probes the dtz table. If an entry is being found, it also displays the info string.
 * The displayed depth is usually the distance to zero which is the distance until the 50-move rule is
 * reset.
 */
Move Search::probeDTZ(Board* board) {
    UCI_ASSERT(board);
    
    if (!useTB)
        return 0;
    
    if (bitCount(board->getOccupiedBB()) > (signed) TB_LARGEST)
        return 0;

    U64 w_occ   = board->getTeamOccupiedBB()[WHITE];
    U64 b_occ   = board->getTeamOccupiedBB()[BLACK];
    U64 pawns   = board->getPieceBB<WHITE, PAWN  >() | board->getPieceBB<BLACK, PAWN  >();
    U64 knights = board->getPieceBB<WHITE, KNIGHT>() | board->getPieceBB<BLACK, KNIGHT>();
    U64 bishops = board->getPieceBB<WHITE, BISHOP>() | board->getPieceBB<BLACK, BISHOP>();
    U64 rooks   = board->getPieceBB<WHITE, ROOK  >() | board->getPieceBB<BLACK, ROOK  >();
    U64 queens  = board->getPieceBB<WHITE, QUEEN >() | board->getPieceBB<BLACK, QUEEN >();
    U64 kings   = board->getPieceBB<WHITE, KING  >() | board->getPieceBB<BLACK, KING  >();
    
    U64    fifty_mr     =   board->getBoardStatus()->fiftyMoveCounter;
    bool   any_castling = !!(board->getBoardStatus()->castlingRights & MASK<4>);
    Square ep_square    = std::max((Square) 0, board->getEnPassantSquare());  // board uses -1 as e.p.
    Color  whiteToMove  = board->getActivePlayer() == WHITE;

    unsigned result = tb_probe_root(w_occ, b_occ, kings, queens, rooks, bishops, knights, pawns,
                                          fifty_mr, any_castling, ep_square, whiteToMove, NULL);

    // if the result failed for some reason or the game is over, dont do anything
    if (   result == TB_RESULT_FAILED
        || result == TB_RESULT_CHECKMATE
        || result == TB_RESULT_STALEMATE)
        return 0;

    // we need the wdl and the dtz values to get the score.
    int dtz = TB_GET_DTZ(result);
    int wdl = TB_GET_WDL(result);

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
    PieceType promo  = 5 - TB_GET_PROMOTES(result);

    // gets the square from and square to for the move which should be played
    Square    sqFrom = TB_GET_FROM(result);
    Square    sqTo   = TB_GET_TO(result);

    // we generate all pseudo legal moves and check for equality between the moves to make sure the
    // bits are correct.
    MoveList mv {};
    generatePerftMoves(board, &mv);

    for (int i = 0; i < mv.getSize(); i++) {
        // get the current move from the movelist
        Move m = mv.getMove(i);

        // check if it's the same.
        if (getSquareFrom(m) == sqFrom && getSquareTo(m) == sqTo) {
            if (   (    promo == 5
                    && !isPromotion(m))
                || (isPromotion(m)
                    && promo < 5
                    && getPromotionPieceType(m) == promo)) {
                std::cout << "info"
                          << " depth "      << static_cast<int>(dtz)
                          << " seldepth "   << static_cast<int>(selDepth());
                std::cout << " score cp "   << s;

                if (tbHits() != 0) {
                    std::cout << " tbhits " << 1;
                }

                std::cout << " nodes "      << 1
                          << " nps "        << 1
                          << " time "       << timeManager->elapsedTime()
                          << " hashfull "   << static_cast<int>(table->usage() * 1000);
                std::cout << std::endl;

                return m;
            }
        }
    }

    return 0;
}

bb::Score scoreToTT(bb::Score s, int plies)
{
    return (s >= TB_WIN_SCORE_MIN ? s + plies : s <= -TB_WIN_SCORE_MIN ? s - plies : s);
}

bb::Score scoreFromTT(bb::Score s, int plies)
{
    return (s >= TB_WIN_SCORE_MIN ? s - plies : s <= -TB_WIN_SCORE_MIN ? s + plies : s);
}

ThreadData::ThreadData(int threadId) : threadID(threadId) {}
ThreadData::ThreadData() {}