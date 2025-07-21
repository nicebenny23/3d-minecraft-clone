#pragma once
#include "../util/type_index.h"
#include "../util/dynamicarray.h"
#include <stdexcept>
#include <typeinfo>

struct ResourceManager {
    template<typename T>
    void push(T& representative) {
        auto [id, inserted] = resources_ids.insert<T>();
        if (inserted) {
            // New type inserted: store pointer at dense id index
            if (id.id >= resources.length) {
                // resize array to fit new index
                resources.resize(id.id + 1, nullptr);
            }
            resources[id.id] = &representative;
        }
        else {
            resources[id.id] = &representative;
        }
    }

    template<typename T>
    T& get() {
        Opt::Option<Ids::Id> comp_id = resources_ids.get_opt<T>();

        if (!comp_id) {
            throw std::invalid_argument(std::string(typeid(T).name()) + " has not been initialized yet");
        }
        auto id = resources_ids.get<T>();
        void* ptr = nullptr;
        if (id.id < resources.length) {
            ptr = resources[id.id];
        }
        if (!ptr) {
            throw std::runtime_error(std::string(typeid(T).name()) + " resource pointer is null");
        }
        return *static_cast<T*>(ptr);
    }

private:
    stn::array<void*> resources;
    type_id::type_indexer resources_ids;
};