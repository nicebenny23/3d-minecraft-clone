#include "meshStorage.h"
#include "Uniform.h"
#include "../../util/Id.h"
#include "RenderProperties.h"
#include "../../game/ecs/game_object.h"
#pragma once
namespace renderer{
	struct renderable_tag;
	
	using renderable_id = stn::typed_id<renderable_tag>;
	struct order_key :ecs::component {
		float order;
		order_key(float priority) :order(priority) {

		}
	
	};
	struct material_component:ecs::component {
		material_handle mat_id;
		material_component(material_handle mat) :mat_id(mat) {
			int l = 3;
		}
		void start() {
			int l = 3;
		}
	};
	struct is_enabled : ecs::component {
		bool enabled;
		is_enabled(bool v = true) : enabled(v) {
		}
		void enable() {
			enabled = true;
		}
		void disable() {
			enabled = false;
		}
	};
	struct renderable{
		ecs::obj object;
		renderable_id id;
		renderer::mesh_id mesh;
		material_handle material() {
			return object.get_component<material_component>().mat_id;
		}

		template <typename T>
		void set(const char* shader_alias, T& value) {

			for (auto& val : overides)
			{
				if (val.name == shader_alias)
				{
					val.id = value;
					return;
				}

			}
			overides.push(uniforms::uniform(shader_alias, value));

		}
		void set_order(float key) {
			object.add_component<order_key>(key);
		}
		float get_order() const{
			return object.get_component<order_key>().order;
		}
		bool should_render() {
			return object.get_component<is_enabled>().enabled;
		}
		void enable() {
			object.get_component<is_enabled>().enable();
		}
		void disable() {
			object.get_component<is_enabled>().disable();
		}
		renderable(renderable_id id);

		
		stn::array<uniforms::uniform> overides;
		~renderable() {
		}
		void set_material(material_handle mat_id) {
			material_handle id =object.ensure_component<material_component>(mat_id).mat_id;
			int l = 2;
		}
		void set_mesh(renderer::mesh_id msh_id) {
			mesh = msh_id;
		}
		
	};
}