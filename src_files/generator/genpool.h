#include <thread>
#include <fstream>
#include <atomic>
#include <array>
#include <vector>

class GeneratorPool 
{
public:
    GeneratorPool(int);

    void runGames(std::string_view bookPath, int nGames);
    void run(int nGames);

private:
    int m_NThreads;
    std::vector<std::thread> m_Workers;
    std::atomic_uint64_t m_TotalGamesRun = ATOMIC_VAR_INIT(0);
    std::atomic_uint64_t m_TotalFens = ATOMIC_VAR_INIT(0);
};