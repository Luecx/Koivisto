//
// Created by Luecx on 15.01.2023.
//

#ifndef KOIVISTO_TIMER_H
#define KOIVISTO_TIMER_H

#include <chrono>

class Timer {
    public:
    // starts the timer
    void tick(){
        start_time = std::chrono::high_resolution_clock::now();
    }
    // stops the timer
    void tock(){
        end_time = std::chrono::high_resolution_clock::now();
    }
    // returns the elapsed time in milliseconds
    [[nodiscard]] uint64_t elapsed() const{
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    }
    private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
};

#endif    // KOIVISTO_TIMER_H
