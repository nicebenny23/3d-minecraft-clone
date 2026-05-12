#include "meshStorage.h"
#include "Uniform.h"
#include "../../util/Id.h"
#include "RenderProperties.h"
#include "../../game/ecs/game_object.h"
#include "../color.h"
#include "../../game/ecs/ecs.h"
#include "../../game/ecs/system.h"
#include "../../game/ecs/query.h"
#include "../../game/ecs/filtered_object.h"
#pragma once
namespace renderer{
	struct order_key :ecs::component {
		double order;
		order_key(double priority) :order(priority) {

		}
	
	};
	struct material_component:ecs::component {
		material_handle mat_id;
		material_component(material_handle mat) :mat_id(mat) {
		}
		void start() {
		}
	};
	struct color_component :ecs::component {
		colors::Color mesh_color;
		color_component(colors::Color color) :mesh_color(color){
		}

	};
	struct renderable_overides : ecs::component {
		stn::array< renderer::uniform> overides;
		renderable_overides() :overides() {

		}

		void set(const renderer::uniform& value) {
			size_t check=
			owner().get_component<material_component>().mat_id->Shader->uniformlocation(value.name.c_str());
			for (auto& val : overides) {
				if (val.name == value.name) {
					if (val.value.index() != value.value.index()) {
						stn::throw_logic_error("unform may not change type");
					}
					val = value;
					return;
				}
			}
			overides.push(value);
		}
		stn::span<renderer::uniform> view() {
			return overides.span();
		}

	};
	struct ColorSetter:ecs::System{
		void run(ecs::Ecs& world) override{
			ecs::View<color_component, renderable_overides> renderable_iter(world);
			for (auto&& [color, overrides] : renderable_iter) {
				overrides.set(uniform(color.mesh_color.glm(), "color"));
			}
		}
	};
	struct mesh_component:ecs::component {
		stn::Option<MeshId> msh;
		mesh_component(MeshId id) :msh(id) {
		}
		mesh_component() :msh() {
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
	

	struct renderable_recipe {
		material_handle material;
		renderable_recipe(material_handle mat):material(mat){

		};
		void apply(ecs::obj& object) const{
			object.add_component<is_enabled>();
			object.add_component<order_key>(0);
			object.add_component<material_component>(material);
			object.add_component<renderable_overides>();
			object.add_component<mesh_component>();
		}

	};
	//todo remove
	using  renderable = ecs::Constrained<is_enabled, order_key, renderable_overides,mesh_component, material_component>;
	

}