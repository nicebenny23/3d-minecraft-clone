#include "archtype.h"
#pragma once
namespace ecs {
	struct Ecs;
	struct obj {
		
		size_t id() const{
			return ent.id();
		}
		size_t generation() const{
			return ent.generation();
		}
		entity inner() const{
			return ent;
		}
		obj(entity inner, Ecs* ecs) :ecs(ecs) {}
		
	private:
		entity ent;
		Ecs* ecs;
	};
}