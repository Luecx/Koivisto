//
// Created by finne on 7/11/2020.
//

#include "TimeManager.h"

#include "Board.h"

auto startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

/**
 * for exact timings
 */
TimeManager::TimeManager(int moveTime) {
    isSafeToStop = true;
    ignorePV     = true;
    forceStop    = false;
    
    timeToUse      = moveTime;
    upperTimeBound = moveTime;
    mode           = MOVETIME;
    
    startTime      = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

/**
 * for depth, infinite search etc.
 */
TimeManager::TimeManager() {
    isSafeToStop = true;
    ignorePV     = true;
    forceStop    = false;

    timeToUse      = 1 << 30;
    upperTimeBound = 1 << 30;
    mode           = DEPTH;

    startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
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
TimeManager::TimeManager(int white, int black, int whiteInc, int blackInc, int movesToGo, Board* board) {
    moveHistory  = new Move[256];
    scoreHistory = new Score[256];
    depthHistory = new Depth[256];
    historyCount = 0;
    isSafeToStop = true;
    ignorePV     = false;
    forceStop    = false;
    mode         = TOURNAMENT;

    double division = 55;

    timeToUse = board->getActivePlayer() == WHITE ? (int(white / division) + whiteInc) - 10
                                                  : (int(black / division) + blackInc) - 10;

    upperTimeBound = board->getActivePlayer() == WHITE ? (int(white / 30) + whiteInc) - 10
                                                  : (int(black / 30) + blackInc) - 10;

    startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

int TimeManager::elapsedTime() {
    auto                       end  = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    auto                       diff = end - startTime;

    return diff;
    // std::cout << "measurement finished! [" << round(diff.count() * 1000) << " ms]" << std::endl;
}

TimeManager::~TimeManager() {

    if (ignorePV)
        return;

    delete[] moveHistory;
    delete[] scoreHistory;
    delete[] depthHistory;
}

void TimeManager::updatePV(Move move, Score score, Depth depth) {

    // dont keep track of pv changes if timing doesnt matter
    if (ignorePV)
        return;

    // store the move,score,depth in the arrays
    moveHistory[historyCount]  = move;
    scoreHistory[historyCount] = score;
    depthHistory[historyCount] = depth;

    historyCount++;
}

void TimeManager::stopSearch() { forceStop = true; }

bool TimeManager::isTimeLeft() {

    int elapsed = elapsedTime();

    // stop the search if requested
    if (forceStop)
        return false;

    // if we are above the maximum allowed time, stop
    if (elapsed >= upperTimeBound)
        return false;

    return true;
}

bool TimeManager::rootTimeLeft(){
    int elapsed = elapsedTime();
    
    // stop the search if requested
    if (forceStop)
        return false;

    // if we are above the maximum allowed time at root, stop
    if (elapsed >= timeToUse)
        return false;

    return true;
}

TimeMode TimeManager::getMode() const { return mode; }
