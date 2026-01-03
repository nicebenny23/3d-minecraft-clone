#include "Loader.h"
#pragma once
namespace assets {
	

	struct Assets;
	template<AssetType T>
	struct AssetHandle {

		AssetHandle() = delete;

		AssetHandle(asset_id id, Assets* assets)
			: id(id), AssetManager(assets) {
			inc();
		}

		AssetHandle(const AssetHandle& other)
			: id(other.id), AssetManager(other.AssetManager) {
			inc();
		}
		bool operator==(const AssetHandle& other) const noexcept {
			return id == other.id && AssetManager == other.AssetManager;
		}

		bool operator!=(const AssetHandle& other) const noexcept {
			return !(*this == other);
		}
		AssetHandle& operator=(const AssetHandle& other) {
			if (id != other.id) {
				dec();
				id = other.id;
				AssetManager = other.AssetManager;
				inc();
			}

			return *this;
		}

		const T& operator*() const {
			return (*AssetManager)[id].asset.ref_as_unchecked<T>();
		}
		T& operator*() {
			return (*AssetManager)[id].asset.ref_as_unchecked<T>();
		}

		const T* operator->() const {
			return (*AssetManager)[id].asset.get_as_unchecked<T>();
		}
		T* operator->() {
			return (*AssetManager)[id].asset.get_as_unchecked<T>();
		}

		~AssetHandle() {
			dec();
		}
		asset_id handle_id() const noexcept {
			return id;
		}
	private:

		asset_id id;
		Assets* AssetManager;
		void dec() {
			AssetManager->dec(id);
		}
		void inc() {
			AssetManager->inc(id);
		}
	};
	struct Assets :ecs::resource {
		template<LoadDescriptorType T>
		stn::Option<AssetHandle<DescriptorAssetType<T>>> load(const T& info) {
			stn::Option<asset_id> id = map.get_or_try(BoxedLoadDescriptor(stn::construct_derived<ConcreteDescriptor<T>>(), info),
				[this](const BoxedLoadDescriptor& load_description, const asset_id& mapped_id) -> stn::Option<AssetSlot> {
					names.emplace(std::string(stn::name(load_description)), mapped_id);
					stn::Option<stn::box<Asset>> mabye_asset = loaders.load(load_description,*this);
					if (mabye_asset) {
						return AssetSlot(std::move(mabye_asset.unwrap()), BoxedLoadDescriptor(load_description));
					}
					else {
						return stn::None;
					}
				});
			if (id) {
				map.element_of(id.unwrap());
				return AssetHandle<DescriptorAssetType<T>>(id.unwrap(), this);
			}
			return stn::None;
		}
		template<LoadDescriptorType T, typename ...Args>
		AssetHandle<DescriptorAssetType<T>>  load_emplace(Args&&...args)
			requires std::constructible_from<T, Args&&...> {
			return load(T(std::forward<Args>(args)...));
		}
		template<AssetType T>
		stn::Option<AssetHandle<T>> from_name(const std::string& name) {
			auto it = names.find(name);
			if (it == names.end()) {
				return stn::None;
			}
			if (!map.element_of(it->second).asset.is<T>()) {
				return stn::None;
			}
			return AssetHandle<T>(it->second, this);
		}
		template<LoaderType U, typename ...Args> requires std::constructible_from<U, Args&&...>
		void emplace_loader(Args&&... args) {
			loaders.emplace_loader<U>(std::forward<Args>(args)...);
		}

	private:

		const AssetSlot& operator[](asset_id id) const {
			return map.element_of(id);
		}
		AssetSlot& operator[](asset_id id) {
			return map.element_of(id);
		}

		void inc(asset_id id) {
			map.element_of(id).count++;

		}
		void dec(asset_id id) {
			AssetSlot& slot = map.element_of(id);
			if (--slot.count == 0) {
				remove_element(id);
			}
		}
		void remove_element(asset_id id) {
			AssetSlot& slot = map.element_of(id);
			if (!loaders.is_immortal(slot)) {
				loaders.unload(slot);
				names.erase(std::string(slot.descriptor.name()));
				map.remove_id(id);
			}
		}
		template<AssetType U>
		friend struct AssetHandle;
		Loaders loaders;
		handle::HandleMap< BoxedLoadDescriptor, AssetSlot, asset_id> map;
		std::unordered_map<std::string, asset_id> names;
	};
	//use this for things that benifit from an asset registry naming but do not need an external loader
	template<typename T> requires AssetType<T>&&LoadDescriptorType<T>&& std::same_as<DescriptorAssetType<T>,T>
	struct SelfDescriptorLoader {
		using load_descriptor = T; 
		using asset_type = T;
		bool immortal = true;
		SelfDescriptorLoader() {

		}
		stn::box<T> load(const T& asset) {
			return stn::box<T>(asset);
		}

		void unload(stn::box<T>&& asset) {
		}
	};
}
namespace std {
	template <assets::AssetType T>
	struct hash<assets::AssetHandle<T>> {
		size_t operator()(const assets::AssetHandle<T>& handle) const noexcept {
			return std::hash<assets::asset_id>{}(handle.handle_id());
		}
	};
}