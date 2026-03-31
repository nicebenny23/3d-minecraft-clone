#include "ecs/query.h"
#include "ecs/ecs.h"
#include "../util/dynamicarray.h"
#include "Core.h"
#pragma once

namespace ai {
	struct BrainAiTag{ };
	using brain_node_id = stn::typed_id<BrainAiTag>;
	struct Brain:ecs::component {
		stn::array<stn::Option<double>> utilities;
		stn::Option<brain_node_id> active_node;
		stn::type_indexer<brain_node_id> node_map;
		template<typename T>
		bool active() const{
			return active_node == node_map.get_opt<T>();
		}
		template<typename T>
		void set(double utility){
			utilities.reach(node_map.insert<T>(),stn::None) = utility;
		}

		stn::Option<double&> operator[](brain_node_id id) {
			return utilities[id.id].as_ref();
		}
		stn::Option<const double&> operator[](brain_node_id id) const{
			return utilities[id.id].as_ref();
		}
	};

	struct BrainSystem:ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View<Brain> brain_query(world);
			
			for (auto&& [brain]:brain_query) {
				brain.active_node = stn::None;
				for (size_t i = 0; i < brain.utilities.length();i++) {
					stn::Option<double> utility= brain.utilities[i];
					if (!utility) {
						continue;
					}
					if (brain.active_node.is_none_or([&](brain_node_id id) {return brain[id].copied()<utility;})) {
						brain.active_node = brain_node_id(i);
					}
				}

				brain.utilities.clear();
			}


		}
	};

	struct BrainPlugin:Core::App {
		void build(Core::App& world) {
			world.emplace_system< BrainSystem>();
		}
	};
}