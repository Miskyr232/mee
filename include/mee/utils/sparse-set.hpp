#include <cstddef>
#include <vector>
template <typename T, typename Handle>
class SparseSet {
    std::vector<size_t> generations;
    std::vector<size_t> freeIndices;
    std::vector<T> dense;
    std::vector<size_t> sparse;
    std::vector<size_t> denseToSparse;

    static constexpr size_t nopos = std::numeric_limits<size_t>::max();

public:
    bool isValid(Handle handle) const {
        return handle.index < generations.size() &&
               generations[handle.index] == handle.generation;
    }

    template <typename... Args>
    Handle create(Args&&... args) {
        size_t index;
        if (!freeIndices.empty()) {
            index = freeIndices.back();
            freeIndices.pop_back();
        } else {
            index = generations.size();
            generations.push_back(0);
        }

        if (index >= sparse.size()) {
            sparse.resize(index + 1, nopos);
        }

        size_t denseIndex = dense.size();
        sparse[index] = denseIndex;
        denseToSparse.push_back(index);
        dense.emplace_back(std::forward<Args>(args)...);

        return Handle{ index, generations[index] };
    }

    void destroy(Handle handle) {
        if (!isValid(handle)) return;

        size_t denseIndex = sparse[handle.index];
        size_t lastDenseIndex = dense.size() - 1;

        if (denseIndex != lastDenseIndex) {
            dense[denseIndex] = std::move(dense[lastDenseIndex]);
            size_t movedSlotIndex = denseToSparse[lastDenseIndex];
            denseToSparse[denseIndex] = movedSlotIndex;
            sparse[movedSlotIndex] = denseIndex;
        }

        dense.pop_back();
        denseToSparse.pop_back();
        sparse[handle.index] = nopos;

        generations[handle.index]++;
        freeIndices.push_back(handle.index);
    }

    T* get(Handle handle) {
        if (!isValid(handle)) return nullptr;
        size_t denseIndex = sparse[handle.index];
        if (denseIndex == nopos) return nullptr;
        return &dense[denseIndex];
    }
};
