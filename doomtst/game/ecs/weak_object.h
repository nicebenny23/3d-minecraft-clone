#include "game_object.h"
#pragma once
namespace ecs {
	struct weak_object {
		weak_object(ecs::obj object) :element(object) {

		}

		stn::Option<ecs::obj> get(){
			if (element.exists()) {
				return element;

			}
			return stn::None;
		}
		stn::Option<const ecs::obj> get() const {
			if (element.exists()) {
				return element;

			}
			return stn::None;
		}
		bool alive() const {
			return element.exists();

		}
		explicit operator bool() const {
			return alive();
		}
		void set(ecs::obj object) {
			element = object;
		}

	private:
		ecs::obj element;

	};


}