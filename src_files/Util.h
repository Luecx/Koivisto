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
#include <iomanip>

constexpr char const *ws_t = " \t\n\r\f\v";

// trim from end of string (right)
std::string &rtrim(std::string &s, const char *t = ws_t);

// trim from beginning of string (left)
std::string &ltrim(std::string &s, const char *t = ws_t);

// trim from both ends of string (right then left)
std::string &trim(std::string &s, const char *t = ws_t);

// prints a loading bar
std::string loadingBar(int count, int max, std::string msg);


/**
 * https://thispointer.com/find-and-replace-all-occurrences-of-a-sub-string-in-c/
 * @param data
 * @param toSearch
 * @param replaceStr
 * @return
 */
std::string &findAndReplaceAll(std::string &data, std::string toSearch, std::string replaceStr);

std::vector<std::string> &splitString(const std::string &txt, std::vector<std::string> &strs, char ch);


//static void printArray(float* data, int count){
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
//                    std::cout << std::setprecision(2) << std::setw(11) << data[index];
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
//static void printArrayBinary(float* data, int count){
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
//                    std::cout << (data[index] > 0 ? '+' : data[index] < 0 ? '-':'.');
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



void startMeasure();

int stopMeasure();

#endif //CHESSCOMPUTER_UTIL_H
