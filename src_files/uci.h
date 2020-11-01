
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
#include <stdio.h>
#include <string>
#include <thread>
#include <unistd.h>

void uci_loop(bool bench);

void uci_processCommand(std::string str);

void uci_go_match(int wtime, int btime, int winc, int binc, int movesToGo, int depth);
void uci_go_perft(int depth, bool hash);
void uci_go_depth(int depth);
void uci_go_nodes(int nodes);
void uci_go_time(int movetime);
void uci_go_infinite();
void uci_go_mate(int depth);

void uci_stop();

void uci_set_option(std::string& name, std::string& value);

void uci_isReady();

void uci_debug(bool mode);

void uci_uci();

void uci_position_fen(std::string fen, std::string moves);
void uci_position_startpos(std::string moves);

void uci_bench();

void uci_quit();

#endif    // KOIVISTO_UCI_H
