#pragma once
#include "../../util/type_index.h"
#include "../../util/queue.h"
#include "resources.h"
#include <algorithm>
namespace ecs {
	struct event_tag {
	};
	using event_count = stn::typed_count<event_tag >;
	struct event_reader_tag {};
	using reader_id = stn::typed_id<event_reader_tag>;

	template<typename T>
	struct Events:resource{
		
		Events() {}
		template<typename ...Args>
		void write(Args&&... args) requires std::constructible_from<T,Args&&...>{
			if (being_read_from()) {
				stn::throw_logic_error("you may not write to a Events with {} current readers", current_readers.length());
			}
			constexpr size_t max_events = 2 << 18;
			if (max_events <active_events().count) {
				stn::throw_logic_error("you may not write with {} active events",active_events());
			}
			if (counts.nonempty()) {
				queue.emplace(std::forward<Args>(args)...);
			}

		}
		//returns the total number of active events
		event_count active_events() const {
			return event_count(queue.length());
		}
		//returns whether it is being read from
		bool being_read_from() const {
			return current_readers.nonempty();
		}
		struct EventRange {
			using iterator = stn::queue<T>::iterator;
			iterator begin() {
				size_t offset = events.counts[id.id].count;
				return events.queue.begin() + offset;
			}
			iterator end() {
				return events.queue.end();
			}
			~EventRange() {
				events.recieve(id);
			}

		private:
			Events& events;
			reader_id id;
			EventRange(Events& buffer, reader_id read_id) : events(buffer), id(read_id) {};
			friend struct Events;
		
		};

		struct EventReader {
			EventReader() = delete;
			bool operator==(const EventReader& other)const {
				return other.id == id;
			}
			bool operator!=(const EventReader& other)const {
				return !((*this) == other);
			} 
			EventReader(const EventReader&) = delete;            
			EventReader& operator=(const EventReader&) = delete; 
			
			EventReader(EventReader&& other):id(other.id),events(other.events) {
				
			}
			EventRange read(){
				return events.create_range(*this);
			}
		private:
			EventReader(Events& event_system, reader_id read_id) :events(event_system), id(read_id) {};
			//returns if not in a move only state
			Events& events;
			reader_id id;
			friend struct Events;
		};
		
		
		EventReader make_reader() {
			reader_id id = reader_id(counts.length());
			counts.push(active_events());
			return	EventReader(*this, id);
		}
	private:
		EventRange create_range(const EventReader& reader) {
			if (current_readers.contains(reader.id)) {
				stn::throw_logic_error("You may not Read from Events with the same reader block_id {} while it is currently being read", reader.id.id);
			}
			current_readers.push(reader.id);
			return EventRange(*this, reader.id);
		}
		void recieve(reader_id reader) {
			if (!current_readers.contains(reader)) {
				stn::throw_logic_error("Invariant violated: Event Reader must be a current_reader");
			}
			size_t index = reader.id;
			counts[index] = active_events();
			current_readers.remove_at(index);
			if (!being_read_from()) {
				event_count all_read_upto = active_events();
				for (event_count& count : counts) {
					all_read_upto = std::min(count, all_read_upto);
				}
				for (event_count& count : counts) {
					count -= all_read_upto;
				}
				queue.drop_many(all_read_upto.count);
			}

		}
		friend struct EventRange;
		stn::array<reader_id> current_readers;
		stn::array<event_count> counts;
		stn::queue<T> queue;

	};
	template <typename T>
	using EventReader = Events<T>::EventReader;
}