#pragma once
#include "game.h"

#include <array>
#include <atomic>
#include <fstream>
#include <thread>
#include <vector>

class GeneratorPool {
    public:
    GeneratorPool(int);

    void runGames(std::string_view bookPath, int nGames, unsigned int ID);
    void run(int nGames);

    private:
    int                      m_nThreads;
    std::vector<std::thread> m_workers;
    std::atomic_uint64_t     m_totalGamesRun = ATOMIC_VAR_INIT(0);
    std::atomic_uint64_t     m_totalFens     = ATOMIC_VAR_INIT(0);
};