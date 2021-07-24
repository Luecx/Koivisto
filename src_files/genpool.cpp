#include "genpool.h"
#include "game.h"
#include <chrono>

GeneratorPool::GeneratorPool(int nThreads)
    : m_NThreads(nThreads)
{}

void GeneratorPool::runGames(std::string_view bookPath, int nGames)
{
    std::srand(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    std::ofstream outputBook(bookPath.data());

    if (!outputBook)
        throw std::invalid_argument("Couldn't open outputBook");
    
    Game game(outputBook);

    for(int i = 0;i < nGames;i++)
    {
        game.reset();
        game.run();
        m_TotalGamesRun++;
        m_TotalFens += game.totalGenerated();
    }
    outputBook.close();
}

void GeneratorPool::run(int nGames)
{
    m_TotalGamesRun = ATOMIC_VAR_INIT(0);

    auto computationBegin = std::chrono::system_clock::now();
    auto startTime = std::chrono::system_clock::to_time_t(computationBegin);

    std::cout << "Began computation at " << std::ctime(&startTime) << std::endl;

    int chunk = nGames / m_NThreads;

    for(int i = 0;i < m_NThreads;i++)
    {
        std::string bookName = "generated_" + std::to_string(i) + ".txt";
        m_Workers.emplace_back(&GeneratorPool::runGames, this, bookName, chunk);
    }

    while(m_TotalGamesRun < (chunk * m_NThreads))
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\rGenerating... [GAMES=" << m_TotalGamesRun <<"] "
                                  << "[FENS=" << m_TotalFens << "]";
    }

    auto computationEnd = std::chrono::system_clock::now();
    auto endTime = std::chrono::system_clock::to_time_t(computationEnd);
    
    std::cout << "\nFinished computation at " << std::ctime(&endTime) << '\n';
}