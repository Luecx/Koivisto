
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

#ifndef KOIVISTO_UCI_H
#define KOIVISTO_UCI_H

#include "Bitboard.h"
#include "Board.h"
#include "Move.h"
#include "Perft.h"
#include "Util.h"
#include "search.h"

#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <thread>
#include <unistd.h>

namespace uci {

void mainloop(bool bench);

void processCommand(std::string str);

void go_match(int wtime, int btime, int winc, int binc, int movesToGo, int depth);
void go_perft(int depth, bool hash);
void go_depth(int depth);
void go_nodes(int nodes);
void go_time(int movetime);
void go_infinite();
void go_mate(int depth);

void stop();

void set_option(std::string& name, std::string& value);

void isReady();

void debug(bool mode);

void uci();

void position_fen(std::string fen, std::string moves);
void position_startpos(std::string moves);

void bench();

void quit();

}    // namespace uci

#endif    // KOIVISTO_UCI_H
