#include <mee/utils/random.hpp>
#include <random>

Random::Random() : gen(rd()){

}

float Random::getRandom0to1(){
    std::uniform_real_distribution<float> dis0to1;
    return dis0to1(gen);
}

int Random::getRandomInt(int min, int max){
    std::uniform_int_distribution<int> disInt(min, max);
    return disInt(gen);
}
