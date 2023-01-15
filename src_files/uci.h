
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

// mainloop - This function is the entry point of the program and it is responsible for processing
// input commands and calling appropriate functions.
void mainloop(int argc, char* argv[]);

// processCommand - This function takes a string as input, and processes the command by calling
// appropriate functions based on the command.
void processCommand(std::string str);

// go_perft - This function runs a perft test with a given depth and hash usage.
void go_perft(int depth, bool hash);

// go - This function starts the search for a move. The function takes in a split vector of strings
// and a string as input.
void go(const std::vector<std::string>& split, const std::string& str);

// stop - This function stops the search.
void stop();

// set_option - This function sets the value of a given option.
void set_option(const std::string& name, const std::string& value);

// isReady - This function sends the "readyok" message to the GUI, indicating that the engine is ready
// to receive commands.
void isReady();

// debug - This function enables/disables the debug mode.
void debug(bool mode);

// uci - This function sends the "id" and "option" messages to the GUI, identifying the engine and its
// options.
void uci();

// eval - This function sends the evaluation of the current position to the GUI.
void eval();

// position_fen - This function sets up the position from a given fen string and a list of moves.
void position_fen(const std::string& fen, const std::string& moves);

// position_startpos - This function sets up the position from the start position and a list of moves.
void position_startpos(const std::string& moves);

// bench - This function runs a benchmark test.
void bench();

// quit - This function exits the program.
void quit();
}    // namespace uci

#endif    // KOIVISTO_UCI_H
