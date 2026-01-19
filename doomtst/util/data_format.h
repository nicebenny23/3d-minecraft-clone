#pragma once
#include "dynamicarray.h"
#include <tuple>
#include "type_tag.h"
namespace stn {
	template<typename Ctx>
	concept ByteContext = requires(Ctx ctx, stn::span<const std::byte> data, size_t n) {
		{
			ctx.write_bytes(data)
		};
		{
			ctx.read_bytes(n)
		} -> std::convertible_to<stn::array<std::byte>>;
	};
	struct IByteContext {
		virtual void write_bytes(stn::span<const std::byte> data) = 0;
		virtual stn::array<std::byte> read_bytes(size_t count) = 0;
		virtual ~IByteContext() = default;
	};
	template<ByteContext Ctx>
	struct ByteContextAdapter : IByteContext {
		Ctx& ctx;
		void write_bytes(stn::span<const std::byte> data) override {
			ctx.write_bytes(data);
		}
		stn::array<std::byte> read_bytes(size_t count) override {
			return ctx.read_bytes(count);
		}
	};
	template<typename T>
	struct Serializer;


	template<typename T>
	concept Readable = requires(IByteContext & context, const T & format) {
		Serializer<T>::read(context);
	};
	template<typename T>
	concept Writable = requires(IByteContext & context, const T & value) {
		//1. write exists
		{
			Serializer<T>::write(value, context)
		}->std::same_as<void>;
	};

	template<typename T>
	concept Serializable = Readable<T> && Writable<T> ;

	template<typename T> requires std::is_trivially_constructible_v<T>
	struct Serializer<T> {
		static void write(const T& val, IByteContext& ctx) {
			
			//ctx.write_bytes(stn::span<std::byte>(static_cast<std::byte*>(&val), sizeof(T)));
		}

		static T read(IByteContext& ctx) {
			stn::array<std::byte> bytes=ctx.read_bytes(sizeof(T));
			return *reinterpret_cast<T*>(bytes.data());
			
		}
	};


	template<typename T>
	struct Serializer<stn::array<T>> {
		static void write(const stn::array<T>& arr, IByteContext& ctx) requires Writable<T>{
			Serializer<size_t>::write(arr.length(),ctx);
			for (const auto& element: arr) {
				Serializer<T>::write(element, ctx);
			}
		}

		static stn::array<T> read(IByteContext& ctx)  requires Readable<T> {
			size_t length= Serializer<size_t>::read(ctx);
			stn::array<T> elements;
			for (size_t i = 0; i <length;i++) {
				elements.push(Serializer<T>::read(ctx));
			}
			return elements;
		}
	};


	template<typename T>
	struct Serializer<stn::Option<T>> {
		static void write(const Option<T>& opt, IByteContext& ctx) requires Writable<T> {
			Serializer<bool>::write(opt.is_some(), ctx);
			if (opt.is_some()) {
				Serializer<T>().write(opt.unwrap_unchecked(), ctx);
			}
		}

		static stn::Option<T> read(IByteContext& ctx)  requires Readable<T> {
			if (Serializer<bool>::read(ctx)) {
				return Serializer<T>().read(ctx);
			}
			return stn::None;
		}
	};
	
	template<typename E>
		requires std::is_enum_v<E>
	struct Serializer<E> {
		using Underlying = std::underlying_type_t<E>;
		static void write(E value, IByteContext& ctx) {
			Serializer<Underlying>::write(static_cast<Underlying>(value), ctx);
		}
		static E read(IByteContext& ctx) {
		return static_cast<E>(Serializer<Underlying>::read(ctx));
		}
	};
	template<>
	struct Serializer<std::string> {
		static void write(const std::string& str, IByteContext& ctx) {
			Serializer<size_t>::write(str.size(), ctx);
			ctx.write_bytes(stn::span<const std::byte>(
				reinterpret_cast<const std::byte*>(str.data()),
				str.size()
			));
		}

		static std::string read(IByteContext& ctx) {
			size_t len = Serializer<size_t>::read(ctx);
			auto bytes = ctx.read_bytes(len);
			return std::string(reinterpret_cast<const char*>(bytes.data()), len);
		}
	};
}