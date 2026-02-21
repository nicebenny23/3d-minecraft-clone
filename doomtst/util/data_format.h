#pragma once
#include "dynamicarray.h"
#include <tuple>
namespace stn {
	
	struct Context {

	};


	template<typename Ctx>
	concept ByteContext = requires(Ctx ctx, stn::span<const std::byte> data, size_t n) {
		{
			ctx.write_bytes(data)
		};
		{
			ctx.read_bytes(n)
		} -> std::convertible_to<stn::array<std::byte>>;
	};
	
	template<ByteContext Ctx,typename T>
	struct Serializer;


	template<typename Ctx,typename T>
	concept Readable = ByteContext<Ctx>&&requires(Ctx & context) {
		Serializer<Ctx,T>::read(context);
	};
	template<typename Ctx, typename T>
	concept Writable = ByteContext<Ctx> && requires(Ctx & context, const T & value) {
		//1. write exists
		{
			Serializer<Ctx,T>::write(value, context)
		}->std::same_as<void>;
	};

	template<typename Ctx,typename T>
	concept Serializable = Readable<Ctx,T> && Writable<Ctx, T> ;

	template<ByteContext Ctx, typename T> requires std::is_trivially_copyable_v<T>
	struct Serializer<Ctx,T> {
		static void write(T val, Ctx& ctx) {
			stn::span<std::byte> data(std::bit_cast<std::byte*>(&val), sizeof(T));
			ctx.write_bytes(data);
		}

		static T read(Ctx& ctx) {
			stn::array<std::byte> bytes=ctx.read_bytes(sizeof(T));
			return *reinterpret_cast<T*>(bytes.data());
			
		}
	};


	template<ByteContext Ctx, typename T>
	struct Serializer<Ctx,stn::array<T>> {
		static void write(const stn::array<T>& arr, Ctx& ctx) requires Writable<Ctx,T>{
			Serializer<Ctx, size_t>::write(arr.length(),ctx);
			for (const auto& element: arr) {
				Serializer<Ctx, T>::write(element, ctx);
			}
		}

		static stn::array<T> read(Ctx& ctx)  requires Readable<Ctx,T> {
			size_t length= Serializer<Ctx,size_t>::read(ctx);
			stn::array<T> elements;
			for (size_t i = 0; i <length;i++) {
				elements.push(Serializer<T>::read(ctx));
			}
			return elements;
		}
	};


	template<ByteContext Ctx, typename T>
	struct Serializer<Ctx,stn::Option<T>> {
		static void write(const Option<T>& opt, Ctx& ctx) requires Writable<Ctx,T> {
			Serializer<Ctx,bool>::write(opt.is_some(), ctx);
			if (opt.is_some()) {
				Serializer<Ctx, T>().write(opt.unwrap_unchecked(), ctx);
			}
		}

		static stn::Option<T> read(Ctx& ctx)  requires Readable<Ctx,T> {
			if (Serializer<Ctx, bool>::read(ctx)) {
				return Serializer<Ctx, T>().read(ctx);
			}
			return stn::None;
		}
	};
	
	template<ByteContext Ctx, typename E>
		requires std::is_enum_v<E>
	struct Serializer<Ctx,E> {
		using Underlying = std::underlying_type_t<E>;
		static void write(E value, Ctx& ctx) {
			Serializer<Ctx, Underlying>::write(static_cast<Underlying>(value), ctx);
		}
		static E read(Ctx& ctx) {
		return static_cast<E>(Serializer<Ctx, Underlying>::read(ctx));
		}
	};
	template<ByteContext Ctx>
	struct Serializer<Ctx,std::string> {
		static void write(const std::string& str, Ctx& ctx) {
			Serializer<Ctx, size_t>::write(str.size(), ctx);
			ctx.write_bytes(stn::span<const std::byte>(
				reinterpret_cast<const std::byte*>(str.data()),
				str.size()
			));
		}

		static std::string read(Ctx& ctx) {
			size_t len = Serializer<Ctx, size_t>::read(ctx);
			auto bytes = ctx.read_bytes(len);
			return std::string(reinterpret_cast<const char*>(bytes.data()), len);
		}
	};
	
}