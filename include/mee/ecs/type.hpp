#include <atomic>

namespace mee::detail {
    inline std::atomic<std::size_t> nextTypeId{0};

    template<typename T>
    std::size_t typeId(){
        static const std::size_t id =
            nextTypeId.fetch_add(1, std::memory_order_relaxed);
        return id;
    }
}
