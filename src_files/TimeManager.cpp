//
// Created by finne on 7/11/2020.
//

#include "TimeManager.h"
#include "Board.h"



auto startTime = std::chrono::system_clock::now();


/**
 * for exact timings
 */
TimeManager::TimeManager(int moveTime) {
    m_isSafeToStop = true;
    m_ignorePV = true;
    m_forceStop = false;
    
    m_timeToUse = moveTime;
    m_upperTimeBound = moveTime;
    startTime = std::chrono::system_clock::now();
}

/**
 * for depth, infinite search etc.
 */
TimeManager::TimeManager() {
    m_isSafeToStop = true;
    m_ignorePV = true;
    m_forceStop = false;
    
    m_timeToUse = 1 << 30;
    m_upperTimeBound = 1 << 30;
    
    startTime = std::chrono::system_clock::now();
}

/**
 * for timecontrol
 * @param white
 * @param black
 * @param whiteInc
 * @param blackInc
 * @param movesToGo
 * @param board
 */
TimeManager::TimeManager(int white, int black, int whiteInc, int blackInc, int movesToGo, Board* board){
    m_moveHistory = new Move[256];
    m_scoreHistory = new Score[256];
    m_depthHistory = new Depth[256];
    m_historyCount = 0;
    m_isSafeToStop = true;
    m_ignorePV = false;
    m_forceStop = false;
    
    
    double _phase =
            (18 - bitCount(
                        board->getPieces()[WHITE_BISHOP] |
                            board->getPieces()[BLACK_BISHOP] |
                            board->getPieces()[WHITE_KNIGHT] |
                            board->getPieces()[BLACK_KNIGHT] |
                            board->getPieces()[WHITE_ROOK] |
                            board->getPieces()[BLACK_ROOK]) -
             3*bitCount(
                     board->getPieces()[WHITE_QUEEN] |
                      board->getPieces()[BLACK_QUEEN])) / 18.0;
    
    if(_phase > 1) _phase = 1;
    
    double division = movesToGo - 30 + 50 * _phase;
    division = 40;
    
    m_timeToUse = board->getActivePlayer() == WHITE ?
            (int(white/division) + whiteInc)-10 :
            (int(black/division) + blackInc)-10;
    
    
    int difference = board->getActivePlayer() == WHITE ? (white-black) : (black-white);
    difference = 0;
    
    m_timeToUse += int(difference / division);
    m_upperTimeBound = m_timeToUse * 3;
    
    if(m_upperTimeBound > (board->getActivePlayer() == WHITE ? white/10:black/10)){
        m_upperTimeBound = (board->getActivePlayer() == WHITE ? white/10:black/10);
    }
    
    if(m_timeToUse > m_upperTimeBound) m_timeToUse = m_upperTimeBound / 3;
    
    
    
    startTime = std::chrono::system_clock::now();
    
}




int TimeManager::elapsedTime(){
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end-startTime;
    
    return round(diff.count() * 1000);
    //std::cout << "measurement finished! [" << round(diff.count() * 1000) << " ms]" << std::endl;
}


TimeManager::~TimeManager() {
    
    if(m_ignorePV) return;
    
    delete m_moveHistory;
    delete m_scoreHistory;
    delete m_depthHistory;
}

void TimeManager::updatePV(Move move, Score score, Depth depth) {
    
    //dont keep track of pv changes if timing doesnt matter
    if(m_ignorePV) return;
    
    //store the move,score,depth in the arrays
    m_moveHistory[m_historyCount] = move;
    m_scoreHistory[m_historyCount] = score;
    m_depthHistory[m_historyCount] = depth;
    
    //compute the safety to stop the search
    if(m_historyCount > 0)
        computeSafetyToStop();
    
    m_historyCount ++;
    
}

void TimeManager::stopSearch() {
    m_forceStop = true;
}

bool TimeManager::isTimeLeft() {
    
//    return true;
    

    int elapsed = elapsedTime();
    
//    std::cout << elapsed << " | " << forceStop << " | " << isSafeToStop << " | " << timeToUse << " | "<< upperTimeBound << " | " <<std::endl;
    
    //stop the search if requested and its safe
    if(m_forceStop && m_isSafeToStop) return false;
    
    //if we have safe time left, continue searching
    if(elapsed < m_timeToUse) return true;
    
    //if we are above the maximum allowed time, stop
    if(elapsed >= m_upperTimeBound) return false;
    
    //dont stop the search if its not safe
    if(!m_isSafeToStop) return true;
    
    return false;
    
}

void TimeManager::computeSafetyToStop() {
    //check if the pv didnt change but the score dropped heavily
    if(m_moveHistory[m_historyCount] == m_moveHistory[m_historyCount-1] && m_scoreHistory[m_historyCount] < m_scoreHistory[m_historyCount-1]-60){
        m_isSafeToStop = false;
        return;
    }
    
    //if the pv changed multiple times during this iteration, search deeper.
    Depth depth = m_depthHistory[m_historyCount];
    int index = m_historyCount-1;
    int changes = 0;
    while(index >= 0){
        if(m_depthHistory[index] != depth) break;
        
        changes ++;
        
        index --;
    }
    
    if(changes >= 1){
        m_isSafeToStop = false;
    }else{
        m_isSafeToStop = true;
    }
    
}



