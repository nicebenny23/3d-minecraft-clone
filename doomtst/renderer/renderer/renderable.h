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
	struct MaterialComponent:ecs::component {
		MaterialHandle mat_id;
		stn::array< renderer::uniform> overides;
		MaterialComponent(MaterialHandle mat) :mat_id(mat) {
		}
		
		void set(const renderer::uniform& value) {
			size_t check =mat_id->shader->uniformlocation(value.name.c_str());
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
	struct color_component :ecs::component {
		colors::Color mesh_color;
		color_component(colors::Color color) :mesh_color(color){
		}

	};
	
	struct ColorSetter:ecs::System{
		void run(ecs::Ecs& world) override{
			ecs::View<color_component, MaterialComponent> renderable_iter(world);
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
		MaterialHandle material;
		renderable_recipe(MaterialHandle mat):material(mat){

		};
		void apply(ecs::obj& object) const{
			object.add_component<is_enabled>();
			object.add_component<order_key>(0);
			object.add_component<MaterialComponent>(material);
			object.add_component<mesh_component>();
		}

	};
	//todo remove
	using  renderable = ecs::Constrained<is_enabled, order_key,mesh_component, MaterialComponent>;
	

}