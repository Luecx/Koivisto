
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

#ifndef CHESSCOMPUTER_UTIL_H
#define CHESSCOMPUTER_UTIL_H

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

constexpr char const* ws_t = " \t\n\r\f\v";

/**
 * trim from end of string (right)
 * @param s
 * @param t
 * @return
 */
std::string& rtrim(std::string& s, const char* t = ws_t);

/**
 * trim from beginning of string (left)
 * @param s
 * @param t
 * @return
 */
std::string& ltrim(std::string& s, const char* t = ws_t);

/**
 * trim from both ends of string (right then left)
 * @param s
 * @param t
 * @return
 */
std::string& trim(std::string& s, const char* t = ws_t);

/**
 * returns a loading bar as a string. Usually used together with '\r'.
 * @param count
 * @param max
 * @param msg
 * @return
 */
std::string loadingBar(int count, int max, std::string msg);

/**
 * https://thispointer.com/find-and-replace-all-occurrences-of-a-sub-string-in-c/
 * @param data
 * @param toSearch
 * @param replaceStr
 * @return
 */
std::string& findAndReplaceAll(std::string& data, std::string toSearch, std::string replaceStr);

/**
 * splits the string into subparts at the specified char.
 * @param txt
 * @param strs
 * @param ch
 * @return
 */
std::vector<std::string>& splitString(const std::string& txt, std::vector<std::string>& strs, char ch);

// static void printArray(float* data, int count){
//    int blocks = count / 64 + (count % 64 != 0 ? 1:0);
//    int wboxes = blocks > 4 ? 4:blocks;
//    int hboxes = 1+ (blocks-1) / 4;
//
//
//    bool indexOut = false;
//
//    for(int h = 0; h < hboxes; h++){
//        for(int row = 0; row < 8; row++){
//
//            for(int w = 0; w < wboxes; w++){
//                for(int column = 0; column < 8; column++){
//                    int index = row * 8 + column + 64 * (h * wboxes + w);
//                    if(index >= count) {
//                        indexOut = true;
//                        continue;
//                    }
//
//                    std::cout << std::setprecision(2) << std::setw(11) <<
//                    data[index];
//                }
//                std::cout << " ";
//            }
//
//
//            std::cout << "\n";
//
//            if(indexOut) return;
//        }
//        std::cout << "\n";
//    }
//}
//
// static void printArrayBinary(float* data, int count){
//    int blocks = count / 64 + (count % 64 != 0 ? 1:0);
//    int wboxes = blocks > 12 ? 12:blocks;
//    int hboxes = 1+ (blocks-1) / 12;
//
//
//    bool indexOut = false;
//
//    for(int h = 0; h < hboxes; h++){
//        for(int row = 0; row < 8; row++){
//
//            for(int w = 0; w < wboxes; w++){
//                for(int column = 0; column < 8; column++){
//                    int index = row * 8 + column + 64 * (h * wboxes + w);
//                    if(index >= count) {
//                        indexOut = true;
//                        continue;
//                    }
//
//                    std::cout << (data[index] > 0 ? '+' : data[index] < 0 ?
//                    '-':'.');
//                }
//                std::cout << " ";
//            }
//
//
//            std::cout << "\n";
//
//            if(indexOut) return;
//        }
//        std::cout << "\n";
//    }
//}

/**
 * starts the time measurement.
 * Note that this Tool is not used during search but rather for internal profilings and debugging.
 */
void startMeasure();

/**
 * stops the time measurement and returns the elapsed milliseconds.
 * @return
 */
int stopMeasure();

#endif    // CHESSCOMPUTER_UTIL_H
