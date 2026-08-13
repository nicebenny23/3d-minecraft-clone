#include "ecs/query.h"
#include "ecs/ecs.h"
#include "../util/dynamicarray.h"
#include "Core.h"
#pragma once

namespace ai {
	struct BrainAiTag{ };
	using BrainNodeId = stn::typed_id<BrainAiTag>;
	
	struct Brain:ecs::component {
		stn::array<double> priorities;
		stn::Option<BrainNodeId> next_active;
		stn::Option<BrainNodeId> active_node;
		stn::Option<BrainNodeId> last_active;

		stn::type_indexer<BrainNodeId> node_map;
		template<typename T>
		bool active() const{
			return active_node == node_map.get<T>();
		}
		template<typename T>
		bool becoming_active() const {
			BrainNodeId id = node_map.get<T>();
			return active_node==id&& last_active!=id;
		}
		template<typename T>
		bool becoming_inactive() const {
			BrainNodeId id = node_map.get<T>();
			return active_node != id && last_active == id;
		}
		template<typename T>
		void allow(){
			BrainNodeId id = node_map.get<T>();
			if (next_active==stn::None) {
				next_active = id;
			}
			else if (priorities[next_active.unwrap().id]<= priorities[id.id]) {
				next_active = id;
			}
			
		}
		template<typename T>
		void add_behavior(double priority) {
			priorities.reach(node_map.insert<T>().value.id)=priority;
		}


	};

	struct BrainSystem:ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View<Brain> brain_query(world);
			
			for (auto&& [brain]:brain_query) {
				brain.last_active = brain.active_node;
				brain.active_node = brain.next_active;
				brain.next_active = stn::None;
			}


		}
	};

	struct BrainPlugin{
		void operator()(core::App& world) {
			world.emplace_system< BrainSystem>();
		}
	};
}