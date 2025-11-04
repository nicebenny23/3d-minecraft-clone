#include "../util/type_index.h"
#include "../util/dependency.h"
#include "../util/dynamicarray.h"
#include <functional>
#pragma once
namespace event {
    struct Event {
        virtual ~Event() = default;
    };


    using ListenerFn = std::function<void(const Event&)>;

    struct EventSubsystem{
        template<typename EventType, typename Listener>
            void push() {
                auto listen = Listener();
                ListenerFn wrapper = [listen](const Event& event) mutable {
                listen(static_cast<const EventType&>(event));};
                listeners.push(wrapper);
               dependenciesList.push<Listener>();
                }
        void Call(const Event& event) {
            for (size_t index : dependenciesList.sortedActive) {
                listeners[index](event);
            }
        }
        stn::array<ListenerFn> listeners;
        Depends::DependencySystem dependenciesList;
    };
    struct EventBus
    {

        template<typename EventType,typename Listener>
            requires std::invocable<Listener, const EventType&>&&std::is_class_v<Listener>
        void Add_listener() {
            size_t index = event_indexer.insert<EventType>().first.id;
            event_subsystem[index].push<EventType, Listener>();
        }
        template<typename EventType>
        void Call(const EventType& event) {
            size_t index = event_indexer.insert<EventType>().first.id;
            event_subsystem[index].Call(event);
        }
        stn::array<EventSubsystem> event_subsystem;
        type_map::type_indexer<> event_indexer;
        EventBus() {
        }
    };
}