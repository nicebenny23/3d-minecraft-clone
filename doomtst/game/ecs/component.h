#include "component_pages.h"
#include "component_id.h"
#include "../../util/exception.h"
#include <concepts>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "../../util/bitset.h"
#include "../../util/dynamicarray.h"
#include "../../util/pair.h"
#include "../../util/Span.h"
#include "EntityAllocator.h"
#include "../../util/type_index.h"
#include "../../util/unique.h"
#pragma once
namespace ecs {
	struct obj;
	struct Ecs;
	template<typename T>
	struct component_storage;


	struct component {
		obj owner() const;
		component() :ecs(nullptr) {
		}
		component(const component& other) :ecs(other.ecs), ent() {

		}
		virtual ~component() = default;

		virtual void start() {
		}
		virtual void update() {
		}
		virtual void destroy_hook() {
		};
		Ecs& world() {
			return *ecs;
		}
		const Ecs& world() const {
			return *ecs;
		}
		entity owning_entity() const {
			return ent;
		}
	private:
		Ecs* ecs;
		entity ent;
		template<typename T>
		friend struct component_storage;
		friend struct component_type;

	};
	template<typename T>
	concept ComponentType = std::derived_from<T, component>;

	struct ComponentInfo {
		int priority = 0;
		bool updates = true;
	};
	template<typename T>
	inline constexpr ComponentInfo ComponentTraits{};
	struct IComponentIndexer {
		virtual	bool has(entity_id ent) const = 0;
		virtual	component& unchecked_at(entity_id entity) = 0;
		virtual	const component& unchecked_at(entity_id entity) const = 0;

		virtual	component& at(entity_id entity) = 0;
		virtual	const component& at(entity_id entity) const = 0;
		template<typename T>
		T& get_as_unchecked(entity_id e) {
			return static_cast<T&>(unchecked_at(e));
		}
		template<typename T>
		const T& get_as_unchecked(entity_id e) const {
			return static_cast<const T&>(unchecked_at(e));
		}
		virtual void remove_at_unchecked(entity_id ent) = 0;
		void remove_at(entity_id ent) {
			if (has(ent)) {
				throw std::logic_error("emplace<T> cannot overwrite an existing component");
			}
			remove_at_unchecked(ent);
		}

		virtual ~IComponentIndexer() {

		}
	};
	template<typename T>
	struct component_storage :IComponentIndexer {

		component_storage() :pages() {

		}


		component_storage(component_storage&& other) noexcept
			: pages(std::move(other.pages)) {
		}

		bool has(entity_id ent) const override {
			return pages.contains(ent.id);
		}

		T& at(entity_id entity)	override {
			return pages[entity.id];
		}
		const T& at(entity_id entity) const override {
			return pages[entity.id];
		}

		T& unchecked_at(entity_id entity)	override {
			return pages.unchecked_at(entity.id);
		}
		const T& unchecked_at(entity_id entity) const override {
			return pages.unchecked_at(entity.id);
		}


		template<typename... Args>
		T& emplace_unchecked(entity_id ent, Args&&... args)  requires std::constructible_from<T, Args&&...> {
			return pages.insert_at_unchecked(ent.id, std::forward<Args>(args)...);
		}
		template<typename...Args>
		stn::insertion<T&> emplace(entity_id ent, Args&&... args)  requires std::constructible_from<T, Args&&...> {
			return pages.insert_at(ent.id, std::forward<Args>(args)...);
		}

		void remove_at_unchecked(entity_id ent) override {
				static_cast<component&>(pages.unchecked_at(ent.id)).destroy_hook();
			pages.remove_at_unchecked(ent.id);
		}
		template<typename...Args>
		stn::insertion<T&> set_at(entity_id ent, Args&&... args)  requires std::constructible_from<T, Args&&...> {

			if (has(ent)) {
				static_cast<component&>(pages[ent.id]).destroy_hook();
				return stn::insertion<T&>(pages.replace_at_unchecked(ent.id, std::forward<Args>(args)...), false);
			}
			return stn::insertion<T&>(emplace_unchecked(ent, std::forward<Args>(args)...), true);
		}
	private:
		

		component_pages<T, ecs::page_size> pages;

	};



};