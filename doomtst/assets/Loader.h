#pragma once
#include "Descriptor.h"
namespace assets {
	//A loader takes in a descritor and turns it into an asset
	struct Assets;
	struct IAssetLoader {

		virtual stn::box<Asset> load(const BoxedLoadDescriptor& load_info, Assets& assets) = 0;
		virtual void unload(stn::box<Asset>&& asset) = 0;
		virtual bool immortal() const = 0;
		virtual ~IAssetLoader() = default;
	};

	
	
	template<typename Loader>
	concept HasLoadDescriptor =
		requires {
		typename Loader::load_descriptor;
		requires LoadDescriptorType<typename Loader::load_descriptor>;
	};

	template<HasLoadDescriptor L>
	using LoaderDescriptor = typename L::load_descriptor;
	
	template<typename Loader> 
	concept LoadsWithoutAssets=
		HasLoadDescriptor<Loader> &&
		requires(Loader l, const LoaderDescriptor<Loader> & d) {
			{
				l.load(d)
			} -> std::same_as<stn::box<DescriptorAssetType<LoaderDescriptor<Loader>>>>;
	};

	template<typename Loader>
	concept LoadsWithAssets =
		HasLoadDescriptor<Loader> &&
		requires(Loader l, const LoaderDescriptor<Loader> &d,Assets& assets) {
			{
				l.load(d, assets)
			} -> std::same_as<stn::box<DescriptorAssetType<LoaderDescriptor<Loader>>>>;
	};

	template<typename Loader>
	concept LoaderType = (LoadsWithAssets<Loader>||LoadsWithoutAssets<Loader>) &&
		requires(Loader l, stn::box<DescriptorAssetType<LoaderDescriptor<Loader>>> asset) {
			{
				l.unload(std::move(asset))
			} -> std::same_as<void>;
	};


	//the descriptor of a loader
	
	//loaders which do not deload, note i cannot get the actual check to work so even if immortal=false it will count as immortal
	template<typename L>
	concept ImmortalLoader = LoaderType<L> && requires{{L::immortal}->std::convertible_to<bool>; };
	//the asset of a loader
	template<LoaderType L>
	using LoaderAsset = DescriptorAssetType<LoaderDescriptor<L>>;//A loaders asset must match its descriptors
	
	//forwards the Descriptor so when creating a loader you do not have to deal with type erasure
	template<LoaderType Loader>
	struct AssetLoaderForwarder : IAssetLoader {
		using LoaderDescriptor = LoaderDescriptor<Loader>;
		using LoaderAsset = LoaderAsset<Loader>;

		template<typename... Args>
			requires std::constructible_from<Loader, Args&&...>
		AssetLoaderForwarder(Args&&... args)
			: loader(std::forward<Args>(args)...) {
		}
		stn::box<Asset> load(const BoxedLoadDescriptor& load_info,Assets& assets) override {
			
			const auto& desc = load_info.template descriptor_unchecked<LoaderDescriptor>();
			if constexpr (LoadsWithAssets<Loader>) {
				return loader.load(desc,assets).template upcast<Asset>();
			}
			else {
				return loader.load(desc).template upcast<Asset>();
			}
		}
		bool immortal() const override {
			return ImmortalLoader<Loader>;
		}
		void unload(stn::box<Asset>&& asset) override {
			loader.unload(std::move(asset).template downcast_unchecked<LoaderAsset>());
		}
	private:
		
		Loader loader;
	};
	//stores an assets loaders
	struct AssetSlot {
		stn::box<Asset> asset;
		BoxedLoadDescriptor descriptor;
		size_t count;

		AssetSlot(stn::box<Asset>&& asset_box, BoxedLoadDescriptor&& asset_description) :count(0), asset(std::move(asset_box)), descriptor(std::move(asset_description)) {
		}
	};
	struct Loaders {

		template<LoaderType U, typename ...Args> requires std::constructible_from<U, Args&&...>
		void emplace_loader(Args&&... args) {
			if (descriptors.contains<LoaderDescriptor<U>>()) {
				stn::throw_logic_error("May not have a duplicate descriptor loader");
			}
			descriptors.insert<LoaderDescriptor<U>>();
			loaders.emplace(stn::construct_derived <AssetLoaderForwarder<U>>(), std::forward<Args>(args)...);
		}

		stn::Option<stn::box<Asset>> load(const BoxedLoadDescriptor& descriptor,Assets& assets) {
			if (!descriptors.contains(descriptor.type_id())) {
				return stn::None;
			}
			return loaders[descriptors.get(descriptor.type_id()).id]->load(descriptor,assets);
		}
		void unload(AssetSlot& asset) {
			loaders[descriptors.get(asset.descriptor.type_id()).id]->unload(std::move(asset.asset));
		}
		bool is_immortal(const AssetSlot& asset) {
			return loaders[descriptors.get(asset.descriptor.type_id()).id]->immortal();
		}
	private:
		stn::array<stn::box<IAssetLoader>> loaders;
		stn::type_indexer<descriptor_id> descriptors;
	};
}