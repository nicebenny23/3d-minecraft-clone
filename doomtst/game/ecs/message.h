#pragma once
#include "../../util/queue.h"

#include "../../util/dynpool.h"
namespace ecs {
	struct Event {

	};
	template<typename T>
	concept EventType =std::derived_from<T,Event>;
	using event_count = stn::typed_index<Event>;
	struct event_reader_tag {};
	using event_reader_id = stn::typed_id<event_reader_tag>;

	template<typename T>
	struct EventReader {
		EventReader() = delete;
	private:
		event_reader_id reader_id;
		friend struct EventBuffer;
	};
	using event_reader_count = stn::typed_index<EventReader>;
	template<typename T>
	struct EventBuffer {
		EventBuffer() : amount_read() {}
		template<typename ...Args>
		void write(Args&&... args) {
			if (reading_count!=0)
			{
				stn::throw_logic_error("you may not write to a EventBuffer with {} current readers", reading_count);
			}
			queue.emplace(std::forward<Args>(args)...);

		}

		size_t event_count() const {
			return queue.length();
		}
		

		event_count max_count() const{
			return amount_read() + event_count();
		}
		event_range read(EventReader reader) {
			reading_count++;
			counts[reader.reader_id] = max_count();
		
		}

		struct event_range {
			event_range(EventBuffer& buffer) : events(buffer) {
				
			}
			using iterator = events.queue::iterator;
			iterator begin() {
				//add offset 
				return events.queue.begin();
			}
			iterator end() {
				return events.queue.end();
			}

			~event_range() {
				//events.counts
			}
			iterator
			EventBuffer& events;


		};
	private:
		void recieve(EventReader id,event_count total_read) {

			size_t original_count = counts[id.reader_id];
			counts[id.reader_id] = total_read;
			reading_count--;
			if (reading_count==0)
			{
				event_count all_read = max_count();
				for (event_count& count : counts)
				{
					all_read = std::min(count, all_read);
				}
			}
			queue.drop_many(all_read - original_count);
		}
		friend struct event_range;
		event_reader_count reading_count;
		stn::array<event_count> counts;
		stn::queue<T> queue;
		event_count amount_read;
	
	};
	
}