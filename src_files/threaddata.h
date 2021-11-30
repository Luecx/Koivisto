
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

//
// Created by Luecx on 30.11.2021.
//

#ifndef KOIVISTO_THREADDATA_H
#define KOIVISTO_THREADDATA_H

#include "History.h"
#include "newmovegen.h"
/**
 * data about each thread
 */
struct ThreadData {
    int        threadID = 0;
    U64        nodes    = 0;
    int        seldepth = 0;
    int        tbhits   = 0;
    bool       dropOut  = false;
    SearchData searchData {};
    moveGen    generators[MAX_INTERNAL_PLY] {};
    ThreadData();
    
    ThreadData(int threadId);
} __attribute__((aligned(4096)));

#endif    // KOIVISTO_THREADDATA_H
