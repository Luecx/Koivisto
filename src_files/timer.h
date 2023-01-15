//
// Created by Luecx on 15.01.2023.
//

#ifndef KOIVISTO_TIMER_H
#define KOIVISTO_TIMER_H

#include <chrono>

class Timer {
    public:
    // starts the timer
    void tick();
    // stops the timer
    void tock();
    // returns the elapsed time in milliseconds
    [[nodiscard]] uint64_t elapsed() const;
    private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
};

void Timer::tick() {
    start_time = std::chrono::high_resolution_clock::now();
}

void Timer::tock() {
    end_time = std::chrono::high_resolution_clock::now();
}

uint64_t Timer::elapsed() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
}


#endif    // KOIVISTO_TIMER_H
