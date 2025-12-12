#pragma once
#include "ecs.h"
namespace ecs {
	// Views iterate over entities containing the specified Components.
	template<typename... Components>
	struct View {
		Ecs& world() {
			return ecs;
		}
		stn::span<archetype_id> view_archetypes() {
			return archetypes.span();
		}
		stn::span<component_id> view_indices() {
			return positions.span();
		}
		struct Iterator {
			View& owner;
			//actual location

			size_t archetype_list_index;
			archetype_index arch_index;
			Iterator(View& vw, size_t list_index ,archetype_index arch_ind)
				:owner(vw), archetype_list_index(list_index), arch_index(arch_ind){
			}
			
			//finds the next valid/final entry, returning whether it was a valid entry
			bool find_next_entry() {
				while (owner.archetypes.contains_index(archetype_list_index)) {
					Archetype& arch = owner.ecs.archetypes.archetype_at(owner.archetypes[archetype_list_index]);
					if (arch.contains_index(arch_index)) {
						return true;
					}
					//reset to next archetype
					arch_index= archetype_index(0);
					archetype_list_index++;
				}
				return false;
			}

			Iterator& operator++() {
				arch_index++;
				if (!find_next_entry())
				{
					//because we are at end
					arch_index = archetype_index(0);
				}
				return *this;
			}

			bool operator==(const Iterator& other) const {
				return archetype_list_index ==other.archetype_list_index&&arch_index==other.arch_index;
			}
			
			bool operator!=(const Iterator& other) const {
				return !(*this==other);
			}
			
			auto operator*() {
				if (!owner.archetypes.contains_index(archetype_list_index))
				{
					stn::throw_logic_error("Cannot dereference iterator: reached end of View");
				}
				space_id ent= owner.ecs.archetypes.archetype_at(owner.archetypes.unchecked_at(archetype_list_index))[arch_index];
				return owner.ecs.get_tuple_unchecked<Components...>(ent, owner.view_indices());
			}

		};
		
		Iterator end() {
			//[total_archetypes,0] is the sentinel state
			return Iterator(*this, total_archetypes(),archetype_index(0));
		}

		Iterator begin() {
			if (archetypes.empty())
			{
				return end();
			}
			Iterator it(*this, 0, archetype_index(0));
			it.find_next_entry();
			return it;
		}

		View(Ecs& world) :ecs(world), archetypes(), positions() {
			component_ids bitlist = ecs.components.insert_ids<Components...>();
			positions =ecs.components.insert_id_list<Components...>();
			archetypes = ecs.archetypes.archetypes_passing(ecs::archetype_predicate(component_ids(), bitlist)); 
		}
		
		//returns the archetype at index in archetypes
		Archetype& archetype_at(size_t index) {
			return ecs.archetypes.archetype_at(archetypes[index]);
		}
		
		//returns the total number of archetypes in this view
		size_t total_archetypes() const{
			return archetypes.length();
		}
		
	private:
		Ecs& ecs;
		friend struct Iterator;
		stn::array<component_id> positions;
		stn::array<archetype_id> archetypes;
	};

}