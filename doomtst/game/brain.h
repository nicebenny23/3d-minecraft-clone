#include "ecs/query.h"
#include "ecs/ecs.h"
#include "../util/dynamicarray.h"
#include "Core.h"
#pragma once

namespace ai {
	struct BrainAiTag{ };
	using BrainNodeId = stn::typed_id<BrainAiTag>;
	struct Brain:ecs::component {
		stn::array<stn::Option<double>> utilities;
		stn::Option<BrainNodeId> active_node;
		stn::type_indexer<BrainNodeId> node_map;
		template<typename T>
		bool active() const{
			return active_node == node_map.get_opt<T>();
		}
		template<typename T>
		void set(double utility){
			utilities.reach(node_map.insert<T>().value.id,stn::None) = utility;
		}
		template<typename T> 
		void relinquish_control() {
			if (active<T>()) {
				active_node = stn::None;
			}
		}
		stn::Option<double&> operator[](BrainNodeId id) {
			return utilities[id.id].as_ref();
		}
		stn::Option<const double&> operator[](BrainNodeId id) const{
			return utilities[id.id].as_ref();
		}
	};

	struct BrainSystem:ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View<Brain> brain_query(world);
			
			for (auto&& [brain]:brain_query) {
				if (brain.active_node == stn::None) {
					continue;
				}
				for (size_t i = 0; i < brain.utilities.length();i++) {
					stn::Option<double> utility= brain.utilities[i];
					if (!utility) {
						continue;
					}
					if (brain.active_node.is_none_or([&](BrainNodeId id) {return brain[id].copied()<utility;})) {
						brain.active_node = BrainNodeId(i);
					}
				}

				brain.utilities.clear();
			}


		}
	};

	struct BrainPlugin{
		void operator()(Core::App& world) {
			world.emplace_system< BrainSystem>();
		}
	};
}