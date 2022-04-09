
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
 * trim from end of string (right)
 * @param s
 * @param t
 * @return
 */
std::string&              rtrim(std::string& s, const char* t = ws_t);

/**
 * trim from beginning of string (left)
 * @param s
 * @param t
 * @return
 */
std::string&              ltrim(std::string& s, const char* t = ws_t);

/**
 * trim from both ends of string (right then left)
 * @param s
 * @param t
 * @return
 */
std::string&              trim(std::string& s, const char* t = ws_t);

/**
 * returns a loading bar as a string. Usually used together with '\r'.
 * @param count
 * @param max
 * @param msg
 * @return
 */
std::string               loadingBar(int count, int max, const std::string& msg);

/**
 * https://thispointer.com/find-and-replace-all-occurrences-of-a-sub-string-in-c/
 * @param data
 * @param toSearch
 * @param replaceStr
 * @return
 */
std::string&              findAndReplaceAll(std::string& data, const std::string& toSearch, const std::string& replaceStr);



/**
 * splits the string into subparts at the specified char.
 * @param txt
 * @param strs
 * @param ch
 * @return
 */
std::vector<std::string>& splitString(const std::string& txt, std::vector<std::string>& strs, char ch);

/**
 * starts the time measurement.
 * Note that this Tool is not used during search but rather for internal profilings and debugging.
 */
void                      startMeasure();

/**
 * stops the time measurement and returns the elapsed milliseconds.
 * @return
 */
int                       stopMeasure();

#endif    // CHESSCOMPUTER_UTIL_H
