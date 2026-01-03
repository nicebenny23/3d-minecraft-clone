#pragma once
#include <concepts>
#include "../util/Id.h"
namespace assets {

	struct Asset {
		virtual ~Asset() = default;
	};


	template<typename T>
	concept AssetType = std::derived_from<T, Asset>;

	using asset_id = stn::typed_id<Asset>;

}