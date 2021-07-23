#include <thread>
#include <fstream>

class GeneratorPool 
{
public:
    GeneratorPool(int);

    void runGames(std::string_view bookPath, int nGames);
    void run(int nGames);

private:
    int m_NThreads;
};