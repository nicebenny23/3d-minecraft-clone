
#pragma once
#include "entity.h"
namespace gameobject {
    namespace Observe {

        void entity_slot::swap(entity_slot& other)  {
            using std::swap;
            // Unregister both watchers first to avoid dangling references
            if (held) {
                get().addcomponent<SlotTracker>()->unregisterSlot(this);
            }
            if (other.held) {
                other.get().addcomponent<SlotTracker>()->unregisterSlot(&other);
            }

            // Swap the held gameobject::obj (your Option wrapper)
            swap(held, other.held);

            // Register watchers again if not empty
            if (held) {
                get().addcomponent<SlotTracker>()->registerSlot(this);
            }
            if (other.held) {
                other.get().addcomponent<SlotTracker>()->registerSlot(&other);
            }
        }
        entity_slot::entity_slot() = default;


        entity_slot::entity_slot(gameobject::obj& object)
            : held({ object })
        {
            object.addcomponent<SlotTracker>()->registerSlot(this);
        }


        entity_slot::entity_slot(entity_slot&& oth) {
            (*oth).addcomponent<SlotTracker>()->swapSlots(&oth, this);
            held = std::move(oth.held);
            oth.held = stn::None;
        }


        entity_slot& entity_slot::operator=(entity_slot&& oth) {
            if (this == &oth) {
                return *this;
            }

            clear();
            (*oth).addcomponent<SlotTracker>()->swapSlots(&oth, this);
            held = std::move(oth.held);
            oth.held = stn::None;
            return *this;
        }


        entity_slot::~entity_slot() {
            clear();
        }


        void entity_slot::clear() {
            if (held) {
                get().addcomponent<SlotTracker>()->unregisterSlot(this);
                held = stn::None;
            }
        }

    }
}