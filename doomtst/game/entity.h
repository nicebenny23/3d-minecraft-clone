#include "gameobject.h"
#include "../util/Option.h"
#include "../util/counter.h"
#include <stdexcept>
#pragma once

namespace gameobject {
    namespace Observe {
        //weak ptr to a game_object
        struct SlotTracker;
        struct entity_slot {


            entity_slot();
            entity_slot(gameobject::obj& object);
            entity_slot(entity_slot&& other);
            entity_slot& operator=(entity_slot&& other);
            ~entity_slot();


            entity_slot(const entity_slot& other) = delete;
            entity_slot& operator=(const entity_slot& other) = delete;
            bool empty() const {
                return !static_cast<bool>(held);
            }

            explicit operator bool() const {
                return static_cast<bool>(held);
            }

            gameobject::obj& get() {
                return (*held);
            }

            gameobject::obj& operator*() {
                return (*held);
            }
            void swap(entity_slot& other);
        
            
            void reset();
        private:
            friend struct SlotTracker;

            stn::Option<gameobject::obj> held;

        };

        struct SlotTracker : gameobject::component {
        private:

            friend struct entity_slot;

            void destroy_hook() {
                for (auto& obj : refs) {
                    obj->held = stn::None;
                }
            }
            void swapSlots(entity_slot* old, entity_slot* location) {

                for (entity_slot*& refren : refs)
                {
                    if (refren == old) {
                        refren = location;
                    }
                }
            }
            void registerSlot(entity_slot* old) {
                refs.push(old);
            };
            void unregisterSlot(entity_slot* old) {
                stn::Option<uint32_t>  loc = refs.find(old);
                if (!loc)
                {
                    throw std::logic_error("Unable to remove a watcher from a gameobject it is not viewing");
                }
                refs.remove_at(loc());
            };

        private:
            array<entity_slot*> refs;
        };

    }
}