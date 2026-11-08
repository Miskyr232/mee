#include <random>

class Random{
    std::random_device rd;
    std::mt19937 gen;
public:
    Random();
    float getRandom0to1();
    int getRandomInt(int min, int max);
};
