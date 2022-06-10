
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

#include "bitboard.h"
#include "board.h"
#include "move.h"
#include "perft.h"
#include "util.h"
#include "search.h"

#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <thread>
#include <unistd.h>

namespace uci{
void mainloop(int argc, char* argv[]);

void processCommand(std::string str);

void go_perft(int depth, bool hash);
void go(const std::vector<std::string>& split, const std::string& str);

void stop();

void set_option(const std::string& name, const std::string& value);

void isReady();

void debug(bool mode);

void uci();

void eval();

void position_fen(const std::string& fen, const std::string& moves);
void position_startpos(const std::string& moves);

void bench();

void quit();
}

#endif    // KOIVISTO_UCI_H
