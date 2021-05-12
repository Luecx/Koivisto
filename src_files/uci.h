
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

#ifndef KOIVISTO_H
#define KOIVISTO_H

#include "Bitboard.h"
#include "Board.h"
#include "Move.h"
#include "Perft.h"
#include "Util.h"
#include "search.h"

#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <thread>
#include <unistd.h>
namespace uci {
std::string versionString(bool addAuthorName);
void        mainloop(bool bench);

void        processCommand(std::string& str);

void        goMatch(int wtime, int btime, int winc, int binc, int movesToGo, int depth);
void        goPerft(int depth, bool hash);
void        goDepth(int depth);
void        goNodes(int nodes);
void        goMoveTime(int movetime);
void        goInfinite();
void        goMate(int depth);

void        stop();

void        searchAndPrint(Depth maxDepth, TimeManager* p_timeManager);

std::string getValue(std::vector<std::string>& vec, const std::string& key);
void        setOption(const std::string& name, const std::string& value);

void        isReady();

void        debug(bool mode);

void        uci();

void        positionFEN(const std::string& fen, const std::string& moves);
void        positionStartPos(const std::string& moves);

void        bench();

void        quit();
}    // namespace uci
#endif    // KOIVISTO_H
