//
// Created by finne on 5/30/2020.
//

#include "search.h"
#include "History.h"


MoveList **moves;
TranspositionTable *table;


int _nodes;
int _maxTime;
int _selDepth;
auto _startTime = std::chrono::system_clock::now();
bool _forceStop = false;

/*
 * Lmr table
 */
int lmrReductions[256][256];
Evaluator evaluator{};


int RAZOR_MARGIN = 198;
int FUTILITY_MARGIN = 92;
int SE_MARGIN_STATIC = 22;
int LMR_DIV = 192;

void initLmr()
{
    int d, m;
    
    for (d = 0; d < 256; d ++)
        for (m = 0; m < 256; m ++)
            lmrReductions[d][m] = log(d) * log(m) * 100 / LMR_DIV;
}
int lmp[2][11] = {
        { 0, 2, 3, 5, 9, 13, 18, 25, 34, 45, 55 },
        { 0, 5, 6, 9, 14, 21, 30, 41, 55, 69, 84 }
};

/**
 * =================================================================================
 *                              S E A R C H
 *                             H E L P E R S
 * =================================================================================
 */

#define posMargin(x) (200 + (x-1) * 50)

/**
 * stops the search
 */
void search_stop() {
    _forceStop = true;
}

/**
 * sets the start time
 */
void setStartTime(){
    _startTime = std::chrono::system_clock::now();
}

/**
 * returns the amount of elapsed time since _startTime
 * @return
 */
int elapsedTime(){
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end-_startTime;
    return round(diff.count() * 1000);
}

/**
 * checks if there is time left and the search should continue.
 * @return
 */
bool isTimeLeft(){
    return elapsedTime()+1 < _maxTime && !_forceStop;
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
    moves = new MoveList*[MAX_INTERNAL_PLY];
    for(int i = 0; i < MAX_INTERNAL_PLY; i++){
        moves[i] = new MoveList();
    }
    table = new TranspositionTable(hashSize);
    initLmr();
}

/**
 * called at the exit of the program
 */
void search_cleanUp() {
    for(int i = 0; i < MAX_INTERNAL_PLY; i++){
        delete moves[i];
        moves[i] = nullptr;
    }
    delete moves;
    moves = nullptr;
    delete table;
    table = nullptr;
}

/**
 * extracts the pv for the given board using the transposition table.
 * It stores the moves recursively in the given list.
 * It does not clear the list so this has to be done beforehand.
 * to avoid infinite sequences, this search is limited by the depth
 * @param b
 * @param mvList
 */
void extractPV(Board *b, MoveList* mvList, Depth depth){
    
    if(depth <= 0) return;
    
    
    U64 zob = b->zobrist();
    if(table->get(zob) != nullptr){
        
        //extract the move from the table
        Move mov = table->get(zob)->move;
        
        //get a movelist which can be used to store all pseudo legal moves
        MoveList* mvStorage = new MoveList();
        //extract pseudo legal moves
        b->getPseudoLegalMoves(mvStorage);
        
        bool moveContained = false;
        //check if the move is actually valid for the position
        for(int i = 0; i < mvStorage->getSize(); i++){
            
            Move stor = mvStorage->getMove(i);
            
            if(sameMove(stor,mov)){
                moveContained = true;
            }
        }
        
        delete mvStorage;
        
        
        //return if the move doesnt exist for this board
        if(!moveContained) return;
        
        //check if its also legal
        if(!b->isLegal(mov)) return;
        
        mvList->add(mov);
        b->move(table->get(zob)->move);
        extractPV(b, mvList, depth -1);
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
void printInfoString(Board *b, Depth d, Score score){
    
    
    int nps = (int) (_nodes) / (int) (elapsedTime()+1) * 1000;
    
    std::cout << "info"
                 " depth " << (int)d <<
              " seldepth " << (int)_selDepth <<
              " score cp " << score;
    
    if(abs(score) > MIN_MATE_SCORE){
        std::cout << " mate " << (MAX_MATE_SCORE-abs(score)+1)/2 * (score > 0 ? 1:-1);
    }
    
    std::cout <<
    
              " nodes " << _nodes <<
              " nps " << nps <<
              " time " << elapsedTime() <<
              " hashfull " << (int)(table->usage() * 1000);
    
    MoveList* em = new MoveList();
    em->clear();
    extractPV(b, em, _selDepth);
    std::cout << " pv";
    for(int i = 0; i < em->getSize(); i++){
        std::cout << " " << toString(em->getMove(i)) ;
    }
    
    delete em;
    
    
    std::cout << std::endl;
}


/**
 * =================================================================================
 *                                M A I N
 *                              S E A R C H
 * =================================================================================
 */





/**
 * returns the best move for the given board.
 * the search will stop if either the max depth of max time is reached
 * @param b
 * @return
 */
Move bestMove(Board *b, Depth maxDepth, int maxTime) {
    
    if(maxDepth > MAX_PLY) maxDepth = MAX_PLY;
    
    _maxTime = maxTime;
    _forceStop = false;
    _nodes = 0;
    _selDepth = 0;
    table->clear();
    setStartTime();
    
    SearchData sd;
    
    
    for(Depth d = 1; d <= maxDepth; d++){
        
        //start measure for time this iteration takes
//        Score score =
//        //printInfoString(b, d, score);
        pvSearch(b, -MAX_MATE_SCORE, MAX_MATE_SCORE, d, 0, false,&sd, 0);
        
        if(!isTimeLeft()) break;
    }
    
    Move best = table->get(b->zobrist())->move;
    return best;
}

/**
 * main search for both full-windows and null-windows.
 * @param b
 * @param alpha
 * @param beta
 * @param depth
 * @param ply
 * @param expectedCut
 * @return
 */
Score pvSearch(Board *b, Score alpha, Score beta, Depth depth, Depth ply, bool expectedCut,SearchData *sd, Move skipMove) {


    _nodes++;

    if (!isTimeLeft()) {
        return beta;
    }

    if (b->isDraw() && ply > 0) {
        return 0;
    }

    if (ply > _selDepth) {
        _selDepth = ply;
    }

    //depth > MAX_PLY means that it overflowed because depth is unsigned.
    if (depth == 0 || depth > MAX_PLY) {
        //Don't drop into qsearch if in check
        if (b->isInCheck(b->getActivePlayer())) {
            depth++;
        } else {
            return qSearch(b, alpha, beta, ply);
        }
    }


    U64 zobrist = b->zobrist();
    bool pv = (beta - alpha) != 1;
    bool inCheck = b->isInCheck(b->getActivePlayer());
    Score staticEval = evaluator.evaluate(b) * ((b->getActivePlayer() == WHITE) ? 1 : -1);
    sd->setHistoricEval(staticEval, b->getActivePlayer(), ply);
    Score originalAlpha = alpha;
    Score highestScore = -MAX_MATE_SCORE;
    Score score = -MAX_MATE_SCORE;
    Move bestMove = 0;
    Move hashMove = 0;


    /**************************************************************************************
     *                  T R A N S P O S I T I O N - T A B L E   P R O B E                 *
     **************************************************************************************/
    Entry *en = table->get(zobrist);
    if (en != nullptr && !skipMove) {
        hashMove = en->move;

        if (!pv && en->depth >= depth) {
            if (en->type == PV_NODE && en->score >= alpha) {
                return en->score;
            } else if (en->type == CUT_NODE) {
                if (en->score >= beta) {
                    return beta;
                }

            } else if (en->type == ALL_NODE) {
                if (en->score <= alpha) {
                    return alpha;
                }

            }
        }

    }

    if (!inCheck && !pv) {
        /**************************************************************************************
     *                              R A Z O R I N G                                       *
     **************************************************************************************/
        if (depth <= 3 && staticEval + RAZOR_MARGIN < beta) {
            score = qSearch(b, alpha, beta, ply);
            if (score < beta) return score;
        }
        /**************************************************************************************
     *                      F U T I L I T Y   P R U N I N G                               *
     **************************************************************************************/
        if (depth <= 6 && staticEval >= beta + depth * FUTILITY_MARGIN)
            return staticEval;

        /**************************************************************************************
         *                  N U L L - M O V E   P R U N I N G                                 *
         **************************************************************************************/
        if (depth >= 2 && staticEval >= beta) {
            b->move_null();

            score = -pvSearch(b, -beta, 1 - beta, depth - (depth / 4 + 3) * ONE_PLY, ply + ONE_PLY, false, sd, 0);
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
    if (depth >= 6 && pv && !hashMove && !skipMove ) {
        pvSearch(b, alpha, beta, depth - 2, ply, false, sd, 0);
        en = table->get(zobrist);
        if (en != nullptr) {
            hashMove = en->move;
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


    MoveList *mv = moves[ply];
    b->getPseudoLegalMoves(mv);

    MoveOrderer moveOrderer{};
    moveOrderer.setMovesPVSearch(mv, hashMove, sd, b, ply);

    //count the legal moves
    int legalMoves = 0;
    int quiets = 0;

    while (moveOrderer.hasNext()) {

        Move m = moveOrderer.next();

        if (!b->isLegal(m)) continue;

        if (sameMove(m, skipMove))continue;

        bool givesCheck = b->givesCheck(m);
        bool isPromotion = move::isPromotion(m);
//        bool isQueenPromotion = move::promotionPiece(m) % 6 == QUEEN && isPromotion;

        if (!pv && ply>0 && legalMoves >= 1)
        {
            if (!isCapture(m) && !isPromotion && !givesCheck)
            {
                quiets++;
                // LMP
                if (depth <= 10 && quiets > lmp[sd->isImproving(staticEval, b->getActivePlayer(), ply)][depth])
                {
                    continue;
                }
            }
        }

        Score staticExchangeEval = 0;
        if (isCapture(m)) {
            staticExchangeEval = b->staticExchangeEvaluation(m);
        }


        int extension = 0;

        if (b->givesCheck(m) && b->staticExchangeEvaluation(m) >= 0) {
            extension = 1;
        }

        // singular extensions
        if (!extension &&
            depth >= 8 &&
            !skipMove &&
            legalMoves == 0 &&
            sameMove(m, hashMove) &&
            ply>0 &&
            en!=nullptr &&
            abs(en->score)<MIN_MATE_SCORE &&
            en->type == CUT_NODE&&
            en->depth >= depth - 3)
        {
            Score betaCut = en->score - SE_MARGIN_STATIC -depth*2;
            score = pvSearch(b, betaCut-1, betaCut, depth>>1, ply, false, sd, m);
            if (score < betaCut)
                extension++;
            b->getPseudoLegalMoves(mv);
            moveOrderer.setMovesPVSearch(mv, hashMove, sd, b, ply);
            m = moveOrderer.next();
        }

        b->move(m);

        //verify that givesCheck is correct
        //assert(givesCheck == b->isInCheck(b->getActivePlayer()));

        Depth lmr = (pv || legalMoves == 0 || givesCheck || depth < 2 || staticExchangeEval > 0 || isPromotion) ? 0:lmrReductions[depth][legalMoves];

        if (legalMoves == 0 && pv) {
            score = -pvSearch(b, -beta, -alpha, depth - ONE_PLY + extension, ply + ONE_PLY, false, sd, 0);
        } else {
            score = -pvSearch(b, -alpha - 1, -alpha, depth - ONE_PLY - lmr + extension, ply + ONE_PLY, false, sd, 0);
            if (lmr && score > alpha)
                score = -pvSearch(b, -alpha - 1, -alpha, depth - ONE_PLY + extension, ply + ONE_PLY, false, sd,
                                  0); // re-search
            if (score > alpha && score < beta)
                score = -pvSearch(b, -beta, -alpha, depth - ONE_PLY + extension, ply + ONE_PLY, false, sd,
                                  0); // re-search

        }


        b->undoMove();


        if (score >= beta) {
            if (!skipMove) {
                table->put(zobrist, beta, m, CUT_NODE, depth);
                sd->setKiller(m, ply, b->getActivePlayer());
                if (!isCapture(m)){
                    sd->addHistoryScore(m, depth, mv, b->getActivePlayer());
                    sd->addCounterMoveHistoryScore(b->getPreviousMove(), m, depth, mv);
                }
            }
            return beta;
        }

        if (score > highestScore) {
            highestScore = score;
            bestMove = m;
        }
        if (score > alpha) {

            if (!skipMove && ply == 0 && isTimeLeft()) {
                //we need to put the transposition in here so that printInfoString displays the correct pv
                table->put(zobrist, alpha, bestMove, PV_NODE, depth);
                printInfoString(b, depth, score);
            }

            alpha = score;
            bestMove = m;
        }


        legalMoves++;
    }

    //if there are no legal moves, its either stalemate or checkmate.
    if (legalMoves == 0) {
        if (!b->isInCheck(b->getActivePlayer())) {
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
Score qSearch(Board *b, Score alpha, Score beta, Depth ply) {
    
    
    
    //shall we count qSearch nodes?
    _nodes ++;
    
    if(b->isDraw()) return 0;
    
//    if(evaluator.probablyDrawByMaterial(b)){
//        return 0;
//    }
    
    
    Score stand_pat = evaluator.evaluate(b) * ((b->getActivePlayer() == WHITE) ? 1:-1);
   
    
    if( stand_pat >= beta )
        return beta;
    if( alpha < stand_pat )
        alpha = stand_pat;
    
    
   
    
    /**
     * extract all:
     *  - captures (including e.p.)
     *  - promotions
     *
     *  moves that give check are not considered non-quiet in
     *  getNonQuietMoves() allthough they are not quiet.
     */
    MoveList *mv = moves[ply];
    b->getNonQuietMoves(mv);
    
    MoveOrderer moveOrderer{};
    moveOrderer.setMovesQSearch(mv);
    
    
    
    
    for(int i = 0; i < mv->getSize(); i++){
        
        Move m = moveOrderer.next();
        
        if(!b->isLegal(m)) continue;
        
        if(b->staticExchangeEvaluation(m) < 0) continue;
        
        
        
        b->move(m);
        
        Score score = -qSearch(b, -beta, -alpha, ply + ONE_PLY);
        
        
        
        b->undoMove();
        
        
        
        if( score >= beta )
            return beta;
        if( score > alpha )
            alpha = score;
        
        
    }
    return alpha;

//    return 0;
}


