#include "../util/sharedptr.h"
#include "../game/ecs/resources.h"
#include <concepts>
#include "Asset.h"
#include "../util/Name.h"
#include "../renderer/HandleMap.h"
#pragma once
namespace assets {


	
	struct descriptor_id_tag {
	};
	using descriptor_id = stn::typed_id<descriptor_id_tag>;
	//a load descriptor describes to the loader what to load
	template<typename T>
	concept LoadDescriptorType =
		stn::Namable<T>
		&& handle::Mappable<T>
		&& requires{typename T::asset_type; };//a descriptor must have an asset type as otherwise it could load diffrent Asset Types depdending on the collections

	template<LoadDescriptorType T>
	using DescriptorAssetType = typename T::asset_type;


	struct ILoadDescriptor {
		virtual ~ILoadDescriptor() = default;
		virtual std::string_view name() const = 0;
		virtual stn::type_id type_id() const = 0;
		virtual bool equals(const ILoadDescriptor& other) const = 0;
		virtual std::size_t hash() const = 0;
		// required to allow copy constructablity
		virtual stn::box<ILoadDescriptor> clone() const = 0;
	};
	//implmentation of a load descriptor for a specific type
	template<LoadDescriptorType T>
	struct ConcreteDescriptor : ILoadDescriptor {
		T description;
	private:
		stn::type_id id;

	public:
		template<typename ...Args>
		ConcreteDescriptor(Args&&... args) requires std::constructible_from<T, Args&&...>
			: description(std::forward<Args>(args)...)
			, id(stn::type_id::make_type_id<T>()) {
		}
		std::string_view name() const override {
			return stn::name(description);
		}

		stn::type_id type_id() const override {
			return id;
		}

		bool equals(const ILoadDescriptor& other) const override {
			if (id == other.type_id()) {
				return static_cast<const ConcreteDescriptor<T>&>(other).description == description;
			}
			return false;
		}
		stn::box<ILoadDescriptor> clone() const override {
			return stn::box<ILoadDescriptor>(stn::construct_derived<ConcreteDescriptor<T>>(), *this);
		}
		std::size_t hash() const override {
			return std::hash<T>{}(description);
		}
	};
//stores a type erased LoadDescriptor 
	struct BoxedLoadDescriptor {
		stn::box<ILoadDescriptor> descriptor;
		BoxedLoadDescriptor(const BoxedLoadDescriptor& other)
			: descriptor(other.descriptor->clone()) {
		}
		BoxedLoadDescriptor& operator=(const BoxedLoadDescriptor& other) {
			if (this != &other) {
				descriptor = other.descriptor->clone();
			}
			return *this;
		}

		BoxedLoadDescriptor(BoxedLoadDescriptor&&) noexcept = default;
		BoxedLoadDescriptor& operator=(BoxedLoadDescriptor&&) noexcept = default;

		template<typename... Args> requires std::constructible_from<stn::box<ILoadDescriptor>, Args&&...>
		BoxedLoadDescriptor(Args&&... args)
			: descriptor(std::forward<Args>(args)...) {
		}

		bool operator==(const BoxedLoadDescriptor& other) const {
			return descriptor->equals(*other.descriptor);
		}

		bool operator!=(const BoxedLoadDescriptor& other) const {
			return !(*this == other);
		}

		std::size_t hash() const {
			return descriptor->hash();
		}

		std::string_view name() const {
			return descriptor->name();
		}

		stn::type_id type_id() const {
			return descriptor->type_id();
		}
	
		template<typename T>
		T& descriptor_unchecked() {
			return descriptor.ref_as_unchecked<ConcreteDescriptor<T>>().description;
		}

		template<typename T>
		const T& descriptor_unchecked() const {
			return descriptor.ref_as_unchecked<ConcreteDescriptor<T>>().description;
		}
	};
}
namespace std {
	template<>
	struct hash<assets::BoxedLoadDescriptor> {
		size_t operator()(const assets::BoxedLoadDescriptor& box) const noexcept {
			return box.hash();
		}

		hash() = default;
	};
}