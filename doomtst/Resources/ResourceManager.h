#pragma once
#include "../util/type_index.h"
#include "../util/dynamicarray.h"
#include <stdexcept>
#include <typeinfo>

struct ResourceManager {
    template<typename T>
    void push(T& representative) {
        auto [id, is_new] = resources_ids.insert<T>();
        if (is_new) {
            // New type inserted: store pointer at dense id index
            if (id.id >= Resources.length()) {
                // resize array to fit new index
                Resources.reserve(id.id + 1, nullptr);
            }
            Resources[id.id] = &representative;
        }
        else {
            Resources[id.id] = &representative;
        }
    }

    template<typename T>
    T& get() {
        stn::Option<stn::Id> comp_id = resources_ids.get_opt<T>();

        if (!comp_id) {
            throw std::invalid_argument(std::string(typeid(T).name()) + " has not been initialized yet");
        }
        auto id = resources_ids.get<T>();
        void* ptr = nullptr;
        if (id.id < Resources.length()) {
            ptr = Resources[id.id];
        }
        if (!ptr) {
            throw std::runtime_error(std::string(typeid(T).name()) + " resource pointer is null");
        }
        return *static_cast<T*>(ptr);
    }

private:
    stn::array<void*> Resources;
    stn::type_indexer<> resources_ids;
};