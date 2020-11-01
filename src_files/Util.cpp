
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

#ifndef CHESSCOMPUTER_UTIL_CPP
#define CHESSCOMPUTER_UTIL_CPP

#include "Util.h"

/**
 * trim from end of string (right)
 * @param s
 * @param t
 * @return
 */
std::string& rtrim(std::string& s, const char* t) {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

/**
 * trim from beginning of string (left)
 * @param s
 * @param t
 * @return
 */
std::string& ltrim(std::string& s, const char* t) {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

/**
 * trim from both ends of string (right then left)
 * @param s
 * @param t
 * @return
 */
std::string& trim(std::string& s, const char* t) {
    return ltrim(rtrim(s, t), t);
}

/**
 * https://thispointer.com/find-and-replace-all-occurrences-of-a-sub-string-in-c/
 * @param data
 * @param toSearch
 * @param replaceStr
 * @return
 */
std::string& findAndReplaceAll(std::string& data, std::string toSearch,
                               std::string replaceStr) {
    // Get the first occurrence
    size_t pos = data.find(toSearch);

    // Repeat till end is reached
    while (pos != std::string::npos) {
        // Replace this occurrence of Sub String
        data.replace(pos, toSearch.size(), replaceStr);
        // Get the next occurrence from the current position
        pos = data.find(toSearch, pos + replaceStr.size());
    }
    return data;
}

/**
 * splits the string into subparts at the specified char.
 * @param txt
 * @param strs
 * @param ch
 * @return
 */
std::vector<std::string>& splitString(const std::string&        txt,
                                      std::vector<std::string>& strs, char ch) {
    size_t pos        = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while (pos != std::string::npos) {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    // Add the last one
    strs.push_back(
        txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

    return strs;
}

/**
 * returns a loading bar as a string. Usually used together with '\r'.
 * @param count
 * @param max
 * @param msg
 * @return
 */
std::string loadingBar(int count, int max, std::string msg) {
    std::stringstream ss {};
    double            p = count / (double) max;

    ss << (int) (100 * p) << "% [";
    for (int i = 0; i < 50 * p; i++) {
        ss << "=";
    }
    ss << ">";
    for (int i = 0; i < 50 * (1 - p); i++) {
        ss << " ";
    }
    ss << "] ";
    ss << count << "/" << max << " " << msg;
    return ss.str();
}

auto start = std::chrono::system_clock::now();

/**
 * starts the time measurement.
 * Note that this Tool is not used during search but rather for internal profilings and debugging.
 */
void startMeasure() {
    // std::cout << "starting measurement!\n";
    start = std::chrono::system_clock::now();
}

/**
 * stops the time measurement and returns the elapsed milliseconds.
 * @return
 */
int stopMeasure() {

    auto                          end  = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;

    return round(diff.count() * 1000);
    // std::cout << "measurement finished! [" << round(diff.count() * 1000) << "
    // ms]" << std::endl;
}

#endif    // CHESSCOMPUTER_UTIL_CPP
