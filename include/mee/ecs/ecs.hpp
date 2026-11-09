#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>
#include <type_traits>

inline constexpr std::size_t nopos = static_cast<std::size_t>(-1);

struct Entity{
    std::uint32_t index;
    std::uint32_t generation;
    bool operator==(const Entity&) const = default;
    static constexpr Entity Invalid() {
        return {
            std::numeric_limits<uint32_t>::max(),
            0
        };
    }
};

struct Slot{
    std::uint32_t generation = 0;
    bool alive = true;
};

class StorageBase{
public:
    virtual ~StorageBase() = default;
    virtual void erase(Entity e) = 0;
    virtual std::size_t size() const = 0;
    virtual const std::vector<Entity>& getOwners() const = 0;
};

template<typename T>
class Storage : public StorageBase{
public:
    std::vector<T> dense;
    std::vector<Entity> owners;
    std::vector<std::size_t> sparce;

    void add(Entity e, T value){
        if(has(e)){
            dense[sparce[e.index]] = std::move(value);
            return;
        }
        if(e.index >= sparce.size()){
            sparce.resize(static_cast<std::size_t>(e.index + 1), nopos);
        }
        sparce[e.index] = dense.size();
        dense.push_back(std::move(value));
        owners.push_back(e);
    }
    bool has(Entity e) const{
        return e.index < sparce.size()
            && sparce[e.index] != nopos
            && owners[sparce[e.index]] == e;
    }
    T& get(Entity e){
        assert(has(e));
        return dense[sparce[e.index]];
    }
    void erase(Entity e) override{
        if(!has(e))
            return;
        auto pos = sparce[e.index];
        auto last = dense.size() - 1;
        if(pos != last){
            dense[pos] = std::move(dense[last]);
            owners[pos] = owners[last];
            sparce[owners[pos].index] = pos;
        }
        dense.pop_back();
        owners.pop_back();
        sparce[e.index] = nopos;
    }
    std::size_t size() const override{
        return dense.size();
    }
    const std::vector<Entity>& getOwners() const override{
        return owners;
    }
};

class ECS{
    std::vector<Slot> slots;
    std::vector<uint32_t> freeList;

    std::unordered_map<std::type_index, std::unique_ptr<StorageBase>> storages;

    template<typename T>
    Storage<T>& storage(){
        auto& ptr = storages[typeid(T)];
        if(!ptr) ptr = std::make_unique<Storage<T>>();
        return *static_cast<Storage<T>*>(ptr.get());
    }
    public:
    template<typename... Components>
    Entity createEntity(Components&&... comps) {
        Entity e = createEntity();
        (addComponent(e, std::forward<Components>(comps)), ...);
        return e;
    }
    Entity createEntity();
    void removeEntity(Entity e);
    bool isAlive(Entity e) const;

    template<typename T>
    void addComponent(Entity e, T c){
        if(e == Entity::Invalid())
            return;
        storage<T>().add(e, std::move(c));
    }

    template<typename T>
    void addComponent(Entity e){
        T value;
        storage<T>().add(e, std::move(value));
    }

    template<typename T>
    T& getComponent(Entity e){
        return storage<T>().get(e);
    }

    template<typename T>
    bool hasComponent(Entity e){
        return storage<T>().has(e);
    }

    template<typename T>
    void removeComponent(Entity e){
        storage<T>().erase(e);
    }

    template<typename... Components, typename F>
    void view(F&& f) {
        static_assert(sizeof...(Components) >= 1);
        StorageBase* sets[] = { &storage<Components>()... };

        std::size_t min = 0;
        for (std::size_t k = 1; k < sizeof...(Components); ++k)
            if (sets[k]->size() < sets[min]->size()) min = k;

        for (Entity e : sets[min]->getOwners())
            if ((hasComponent<Components>(e) && ...))
                if constexpr (std::is_invocable_v<F, Entity, Components&...>) {
                    f(e, getComponent<Components>(e)...);
                }else if constexpr (std::is_invocable_v<F, Entity>) {
                    f(e);
                }
    }
};
