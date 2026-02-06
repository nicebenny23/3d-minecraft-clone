#include "meshStorage.h"
#include "Uniform.h"
#include "../../util/Id.h"
#include "RenderProperties.h"
#include "../../game/ecs/game_object.h"
#pragma once
namespace renderer{
	struct order_key :ecs::component {
		float order;
		order_key(float priority) :order(priority) {

		}
	
	};
	struct material_component:ecs::component {
		material_handle mat_id;
		material_component(material_handle mat) :mat_id(mat) {
		}
		void start() {
		}
	};
	struct mesh_component:ecs::component {
		stn::Option<mesh_id> msh;
		mesh_component(mesh_id id) :msh(id) {
		}

		void start() {
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
	struct renderable_overides : ecs::component {
		stn::array< renderer::uniform> overides;
		renderable_overides():overides() {

		}

		void set(const renderer::uniform& value) {
			for (auto& val : overides) {
				if (val.name == value.name) {
					if (val.current_type()!=value.current_type()) {
						stn::throw_logic_error("unform may not change type");
					}
					val = value;
					return;
				}
			}
			overides.push(value);
		}
		stn::span<const renderer::uniform> view() const {
			return overides.span();
		}

	};

	struct renderable_marker :ecs::component {
	

	};
	struct renderable_recipe :ecs::Recipe {
		void apply(ecs::obj& object) {
			object.add_component<is_enabled>();
			object.add_component<order_key>(0);
			object.add_component<renderable_marker>();
			object.add_component<renderable_overides>();
		}

	};
	//todo remove
	struct renderable{
		renderable(ecs::obj temp) :object(temp) {

		}
		ecs::obj object;
		material_handle material() {
			return get_component<material_component>().mat_id;
		}
		void set(const renderer::uniform& value) {
			get_component<renderable_overides>().set(value);
		}
		
		template<ecs::ComponentType T>
		const T& get_component() const{
			return object.get_component<T>();
		}
		template<ecs::ComponentType T>
		T& get_component() {
			return object.get_component<T>();
		}
		template<ecs::ComponentType T>
		bool has_component() const{
			return object.has_component<T>();
		}
		template<ecs::ComponentType T, typename ...Args>
			requires std::constructible_from<T, Args&&...>
		T& ensure_component(Args&&... args) {
			return object.ensure_component<T>(std::forward<Args>(args)...);
		}

		void set_order(float key) {
			ensure_component<order_key>(key).order=key;
		}
		float get_order() const{
			return get_component<order_key>().order;
		}
		bool should_render() {
			return get_component<is_enabled>().enabled;
		}
		void enable() {
			get_component<is_enabled>().enable();
		}
		void disable() {
			get_component<is_enabled>().disable();
		}
	

		mesh_id& get_mesh(){
			return get_component<mesh_component>().msh.expect("mesh must exist");
		}
		mesh_id get_mesh() const {
			return get_component<mesh_component>().msh.expect("mesh must exist");
		}
		stn::span<const renderer::uniform> view_overides() const {
			return get_component<renderable_overides>().view();
		}
		void set_material(material_handle mat_id) {
		object.ensure_component<material_component>(mat_id).mat_id=mat_id;
		}
		void set_mesh(renderer::mesh_id msh_id) {
			object.ensure_component<mesh_component>(msh_id).msh = msh_id;
		}
		bool operator==(const renderable& other) const = default;
	};

	

}