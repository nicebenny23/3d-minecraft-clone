
#pragma once
#include "../util/owner.h"
#include "../util/dynamicarray.h"
#include "../debugger/debug.h"
#include "../game/Core.h"
#define uisize 400
using namespace stn;
using namespace Cptr;
namespace ui {

	struct uielement;
	struct uistate {
		bool hovered;
		bool rightclicked;
		bool leftclicked;
		bool enabled;
		cptr<uielement> parent;
	};

	struct uielement{
		virtual void customdestroy();
		void destroy();
		int id;
		int priority;
		uistate state;
		void updateparent() {

			if (state.parent.ptr() != nullptr) {


				state.parent->state.rightclicked |= state.rightclicked;
				state.parent->state.leftclicked |= state.leftclicked;
				state.parent->state.hovered |= state.hovered;
				state.parent->updateparent();
			}
		}
		void totalupdate() {
			if (state.enabled) {
				update();
				updateparent();
				if (state.parent.ptr() == nullptr) {
					render();
				}
			}

		}

		virtual void update() {

		};
		virtual void render() {
		};
	};
	
	extern array<cptr< uielement>>uielemlist;
	void createuilist();
	void updateui();
	template <class T, typename... types>
	inline cptr<T> createuielement(types&& ...initval) {
		cptr<T> obj = cptr<T>(new T(std::forward<types>(initval)...));

		for (int i = 0; i < uisize; i++) {
			if (uielemlist[i] == nullptr) {

				uielemlist[i].set<T>(obj);
				obj->id = i;
				obj->state.parent = cptr<uielement>(nullptr);
				return obj;

			}
		}
		Assert("not enoght room to init a new ui element");
	}

	int compareuielem(const void* b, const void* a);

	struct updateui :ecs::System {
		void run(ecs::Ecs& world) {
			array<int> indexerlist;
			for (int i = 0; i < uisize; i++) {
				if (uielemlist[i].ptr() != nullptr) {

					if (uielemlist[i]->state.enabled) {

						indexerlist.push(i);
					}

				}
			}
			qsort(indexerlist.data(), indexerlist.length(), sizeof(int), compareuielem);
			for (int i = 0; i < indexerlist.length(); i++) {
				uielemlist[indexerlist[i]]->totalupdate();
			}
		}
	};
	struct UiPlugin :Core::Plugin {
		void Build(Core::App& app) {
			app.emplace_system<updateui>();
		}

	};
}
