#include "ecs/ecs.h"
#include "../util/data_format.h"
#include "../util/fileloader.h"
#include <concepts>
#pragma once
namespace ecs {

	struct serializer {
		ecs::Ecs& world;
		stn::file_handle file;
		template<stn::Writable<serializer> T>
		void write_bytes(stn::span <const std::byte> bytes){
			file.write_bytes(bytes);
		}
		 stn::array<std::byte> read_bytes(size_t count) {
			return file.read_bytes(count);
		}
	};

	struct IComponentSerializer {
		virtual ecs::obj load(serializer& serializer) = 0;
		virtual void unload(ecs::obj object, serializer& serializer) = 0;
	};
	template<typename T>
	concept ComponentSerializerType = std::derived_from<T, IComponentSerializer>;
	
	

	/*using component_serializer_id = stn::typed_id<IComponentSerializer>;
	struct LoadedSerializers:ecs::resource {
		stn::array<stn::box<IComponentSerializer>> serializers;
		template<ComponentSerializerType T> load() {
			if () {

			}
		}

	};
	*/



}