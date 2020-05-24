//
// Created by finne on 5/15/2020.
//

#ifndef CHESSCOMPUTER_UTIL_H
#define CHESSCOMPUTER_UTIL_H

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <cmath>
#include <chrono>

constexpr char const* ws_t = " \t\n\r\f\v";

// trim from end of string (right)
std::string& rtrim(std::string& s, const char* t = ws_t);

// trim from beginning of string (left)
std::string& ltrim(std::string& s, const char* t = ws_t);

// trim from both ends of string (right then left)
std::string& trim(std::string& s, const char* t = ws_t);

/**
 * https://thispointer.com/find-and-replace-all-occurrences-of-a-sub-string-in-c/
 * @param data
 * @param toSearch
 * @param replaceStr
 * @return
 */
std::string& findAndReplaceAll(std::string & data, std::string toSearch, std::string replaceStr);

std::vector<std::string> & splitString(const std::string &txt, std::vector<std::string> &strs, char ch);


void startMeasure();

void stopMeasure();

#endif //CHESSCOMPUTER_UTIL_H
