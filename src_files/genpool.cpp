#include "genpool.h"
#include "game.h"
#include <vector>

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
    }
}

void GeneratorPool::run(int nGames)
{
    int chunk = nGames / m_NThreads;
    std::vector<std::thread> workers;

    for(int i = 0;i < m_NThreads;i++)
    {
        std::string bookName = "generated_" + std::to_string(i) + ".txt";

        workers.emplace_back(&GeneratorPool::runGames, this, bookName, chunk);
    }

    for(auto& worker : workers) 
        worker.join();
}