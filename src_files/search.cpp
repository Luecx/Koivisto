//
// Created by finne on 5/30/2020.
//

#include "search.h"




MoveList **moves;
TranspositionTable *table;


int _nodes;
int _maxTime;
auto _startTime = std::chrono::system_clock::now();




/**
 * =================================================================================
 *                              S E A R C H
 *                             H E L P E R S
 * =================================================================================
 */


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
    return elapsedTime() < _maxTime;
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
    moves = new MoveList*[MAX_PLY];
    for(int i = 0; i < MAX_PLY; i++){
        moves[i] = new MoveList();
    }
    table = new TranspositionTable(hashSize);
}

/**
 * called at the exit of the program
 */
void search_cleanUp() {
    for(int i = 0; i < MAX_PLY; i++){
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
 * @param b
 * @param mvList
 */
void extractPV(Board *b, MoveList* mvList){
    
    U64 zob = b->zobrist();
    if(table->get(zob) != nullptr){
        Move mov = table->get(zob)->move;
        
        if(!b->isLegal(mov)) return;
        
        mvList->add(mov);
        b->move(table->get(zob)->move);
        extractPV(b, mvList);
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
void printInfoString(Board *b, Depth d, Score score, int time){
    
    
    int nps = (int) (_nodes) / (int) (time+1) * 1000;
    
    std::cout << "info"
                 " score cp " << score;
    
    if(abs(score) > MIN_MATE_SCORE){
        std::cout << " mate " << (MAX_MATE_SCORE-abs(score)+1)/2;
    }
    
    std::cout <<
                 " depth " << (int)d <<
                 " nodes " << _nodes <<
                 " nps " << nps <<
                 " time " << elapsedTime() <<
                 " hashfull " << (int)(table->usage() * 1000);
    
    MoveList* em = moves[0];
    em->clear();
    extractPV(b, em);
    std::cout << " pv";
    for(int i = 0; i < em->getSize(); i++){
        std::cout << " " << toString(em->getMove(i)) ;
    }
    
    
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
    
    _startTime = std::chrono::system_clock::now();
    _maxTime = maxTime;
    
    for(Depth d = 1; d <= maxDepth; d++){
    
        //start measure for time this iteration takes
        startMeasure();
        _nodes = 0;
        Score score = pvSearch(b, -MAX_MATE_SCORE, MAX_MATE_SCORE, d, 0, false);
        printInfoString(b, d, score, stopMeasure());
       
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
Score pvSearch(Board *b, Score alpha, Score beta, Depth depth, Depth ply, bool expectedCut) {
    
    
    _nodes++;
    
    if(!isTimeLeft()){
        return beta;
    }
    
    if( depth <= 0 ) return qSearch(b, alpha, beta, ply);
    
    
    U64 zobrist                 = b->zobrist();
    bool pv                     = (beta-alpha) != 1;
    Score originalAlpha         = alpha;
    Move bestMove               = 0;
    
    
    /*
     * null move pruning
     */
    if ( !pv && !b->isInCheck(b->getActivePlayer())) {
        b->move_null();
        Score score = -pvSearch(b, 1-alpha,-alpha,depth-3*ONE_PLY, ply + ONE_PLY,false);
        b->undoMove_null();
        if ( score >= beta ) {
            return beta;
        }
    }
    
    
    MoveList *mv = moves[ply];
    b->getPseudoLegalMoves(mv);
    
    
    //count the legal moves
    int legalMoves = 0;
    
    for(int i = 0; i < mv->getSize(); i++){
        
        Move m = mv->getMove(i);
        
        if(!b->isLegal(m)) continue;
        
        bool givesCheck = b->givesCheck(m);
        
        
        b->move(m);
        
        //verify that givesCheck is correct
        assert(givesCheck == b->isInCheck(b->getActivePlayer()));
        
        Score score;
        if (legalMoves == 0 && pv) {
            score = -pvSearch(b, -beta, -alpha, depth - ONE_PLY, ply + ONE_PLY, false);
        } else {
            score = -pvSearch(b, -alpha-1, -alpha, depth - ONE_PLY, ply+ONE_PLY,false);
            if ( score > alpha ) // in fail-soft ... && score < beta ) is common
                score = -pvSearch(b, -beta, -alpha, depth - ONE_PLY, ply + ONE_PLY, false); // re-search
        }
        
        
        b->undoMove();
    
        
    
        if( score >= beta )
            return beta;
        if( score > alpha ) {
            alpha = score;
            bestMove = m;
        }
    
    
    
        legalMoves ++;
    }
    
    //if there are no legal moves, its either stalemate or checkmate.
    if(legalMoves == 0){
        if(b->isDraw()){
            return 0;
        }else{
            return  -MAX_MATE_SCORE + ply;
        }
    }
    
    
    if(alpha > originalAlpha){
        table->put(zobrist, alpha, bestMove,PV_NODE,depth);
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
    Score stand_pat = evaluate(b) * ((b->getActivePlayer() == WHITE) ? 1:-1);
    
    //shall we count qSearch nodes?
    _nodes ++;
    
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
    
    for(int i = 0; i < mv->getSize(); i++){
        
        Move m = mv->getMove(i);
        
        if(!b->isLegal(m)) continue;
        
        b->move(m);
        
        //verify that givesCheck is correct
       // assert(givesCheck == b->isInCheck(b->getActivePlayer()));
        
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


