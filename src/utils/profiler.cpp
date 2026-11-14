#include "mee/utils/profiler.hpp"
#include <vector>

const std::vector<std::pair<std::string, ProfiledFunc>>& Profiler::getData(){
    return toOut;
}
