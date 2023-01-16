
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

#ifndef CHESSCOMPUTER_UTIL_CPP
#define CHESSCOMPUTER_UTIL_CPP

#include "util.h"
#include <iterator>

/**
 * Trims leading and trailing whitespace characters from a given string
 * @param s The input string to be trimmed
 * @return The trimmed string
 */
std::string& trim(std::string& s, const char* t) {
    s.erase(0, s.find_first_not_of(t));
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

/**
 * splits the string into subparts
 * @param fen
 * @return
 */
std::vector<std::string> splitString(const std::string &fen) {
    std::stringstream fen_stream(fen);
    std::vector<std::string> seglist;
    std::copy(std::istream_iterator<std::string>(fen_stream),
              std::istream_iterator<std::string>(),
              std::back_inserter(seglist));
    return seglist;
}



#endif    // CHESSCOMPUTER_UTIL_CPP
