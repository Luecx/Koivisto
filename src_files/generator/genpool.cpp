

#include "genpool.h"
#include "game.h"
#include <chrono>
#include <random>

#ifdef GENERATOR

GeneratorPool::GeneratorPool(int nThreads)
    : m_nThreads(nThreads)
{}

void GeneratorPool::runGames(std::string_view bookPath, int nGames, unsigned int ID) {
    std::ofstream outputBook(bookPath.data());

    if (!outputBook)
        throw std::invalid_argument("Couldn't open outputBook");

    std::mt19937 generator(ID);
    Game         game(outputBook, generator);

    for (int i = 0; i < nGames; i++) {
        game.reset();
        game.run();
        m_totalGamesRun++;
        m_totalFens += game.totalGenerated();
    }
    outputBook.close();
}

template <
    class result_t   = std::chrono::seconds ,
    class clock_t    = std::chrono::steady_clock,
    class duration_t = std::chrono::seconds
    >
auto since(std::chrono::time_point<clock_t, duration_t> const& start)
{
    return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}

void GeneratorPool::run(int nGames)
{
    std::random_device rd;
    m_totalGamesRun = ATOMIC_VAR_INIT(0);

    auto computationBegin = std::chrono::system_clock::now();
    auto startTime = std::chrono::system_clock::to_time_t(computationBegin);

    std::cout << "Began computation at " << std::ctime(&startTime) << std::endl;

    int chunk = nGames / m_nThreads;

    for(int i = 0;i < m_nThreads;i++)
    {
        std::string bookName = "generated_" + std::to_string(i) + ".txt";
        m_workers.emplace_back(&GeneratorPool::runGames, this, bookName, chunk, rd());
    }

    while(m_totalGamesRun < (chunk * m_nThreads))
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto elapsed = since(computationBegin).count();
        auto games_per_second = m_totalGamesRun / elapsed;
        
        if(elapsed > 0 && games_per_second > 0){
        
            std::cout << "\rGenerating... [GAMES=" << m_totalGamesRun <<"] "
                      << "[FENS=" << m_totalFens << "] "
                      << "[FENS/sec=" << m_totalFens / elapsed << "]"
                      << " estimated time left: " << (nGames - m_totalGamesRun) / games_per_second << std::flush;
        }
    }

    for(auto& worker : m_workers) worker.join();

    auto computationEnd = std::chrono::system_clock::now();
    auto endTime = std::chrono::system_clock::to_time_t(computationEnd);
    
    std::cout << "\nFinished computation at " << std::ctime(&endTime) << '\n';
}

#endif