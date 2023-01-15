
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

#ifndef CHESSCOMPUTER_UTIL_H
#define CHESSCOMPUTER_UTIL_H

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <sstream>

constexpr char const*     ws_t = " \t\n\r\f\v";

/**
 * trim from both ends of string (right then left)
 * @param s
 * @param t
 * @return
 */
std::string& trim(std::string& s, const char* t = ws_t);

/**
 * splits the string into subparts
 * @param fen
 * @return
 */
std::vector<std::string> splitString(const std::string &fen);

#endif    // CHESSCOMPUTER_UTIL_H
