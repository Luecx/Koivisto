//
// Created by finne on 5/15/2020.
//

#ifndef CHESSCOMPUTER_UTIL_CPP
#define CHESSCOMPUTER_UTIL_CPP


#include "Util.h"

// trim from end of string (right)
std::string &rtrim(std::string &s, const char *t) {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
std::string &ltrim(std::string &s, const char *t) {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
std::string &trim(std::string &s, const char *t) {
    return ltrim(rtrim(s, t), t);
}

/**
 * https://thispointer.com/find-and-replace-all-occurrences-of-a-sub-string-in-c/
 * @param data
 * @param toSearch
 * @param replaceStr
 * @return
 */
std::string &findAndReplaceAll(std::string &data, std::string toSearch, std::string replaceStr) {
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

std::vector<std::string> &splitString(const std::string &txt, std::vector<std::string> &strs, char ch) {
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
    strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));
    
    return strs;
}


std::string loadingBar(int count, int max, std::string msg) {
    std::stringstream ss{};
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
 */
void startMeasure() {
    //std::cout << "starting measurement!\n";
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
    //std::cout << "measurement finished! [" << round(diff.count() * 1000) << " ms]" << std::endl;
}


#endif //CHESSCOMPUTER_UTIL_CPP
