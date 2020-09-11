//
// Created by finne on 5/31/2020.
//

#ifndef KOIVISTO_UCI_H
#define KOIVISTO_UCI_H

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include "Bitboard.h"
#include "Board.h"
#include "Move.h"
#include "Perft.h"
#include "Util.h"
#include "search.h"

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

void uci_set_option(std::string &name, std::string &value);

void uci_isReady();

void uci_debug(bool mode);

void uci_uci();

void uci_position_fen(std::string fen, std::string moves);
void uci_position_startpos(std::string moves);

void uci_bench();

void uci_quit();

#endif  // KOIVISTO_UCI_H
