#pragma once

#include <deque>
#include <string>
#include <unordered_map>

template <typename Resource, typename ID = size_t>
class ResourceManager {
    std::unordered_map<std::string, ID> pathToID;
    std::deque<Resource> resources;
public:

    template<typename LoaderFunc>
    ID load(const std::string& path, LoaderFunc loader){
        auto it = pathToID.find(path);
        if(it != pathToID.end()){
            return it->second;
        }

        Resource res = loader(path.c_str());

        resources.push_back(res);
        ID id = resources.size() - 1;
        pathToID[path] = id;

        return id;
    }
    const Resource& get(ID id) const{
        return resources[id];
    }
    template<typename DeleteFunc>
    void clear(DeleteFunc deleter){
        for(auto& resource : resources){
            deleter(resource);
        }
        resources.clear();
        pathToID.clear();
    }
};
