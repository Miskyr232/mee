#pragma once

#include <functional>
#include <utility>
#include <vector>

using ConnectionID = int;

template<typename... Args>
class Signal{
public:
    using Callback = std::function<void(Args...)>;
private:
    ConnectionID next_id = 0;
    std::vector<std::pair<ConnectionID, Callback>> callbacks;
public:
    Signal() = default;
    Signal(Callback initialCallback){
        connect(initialCallback);
    }
    ConnectionID connect(Callback callback){
        ConnectionID id = next_id++;
        callbacks.emplace_back(id, std::move(callback));
        return id;
    }
    void disconnect(ConnectionID id){
        for(auto it = callbacks.begin(); it < callbacks.end(); ++it){
            if(it->first == id){
                callbacks.erase(it);
                break;
            }
        }
    }
    template<typename... UArgs>
    void emit(UArgs&&... args){
        auto current_callbacks = callbacks;
        for(auto& [id, callback] : current_callbacks){
            if(callback)
                callback(std::forward<UArgs>(args)...);
        }
    }
    void clear(){
        callbacks.clear();
    }
};
