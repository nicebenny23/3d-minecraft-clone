#include "../util/sharedptr.h"
#include "../util/dynamicarray.h"
#ifndef ui_HPP
#define ui_HPP

#define uisize 400
using namespace dynamicarray;
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

	struct uielement
	{

		virtual void customdestroy();
		void destroy();
		int id;
		int priority;
		uistate state;
		void updateparent() {

			if (state.parent.ptr()!=nullptr)
			{
				

				state.parent->state.rightclicked|= state.rightclicked;
				state.parent->state.leftclicked|= state.leftclicked;
				state.parent->state.hovered |= state.hovered;
				state.parent->updateparent();
			}
		}
		void totalupdate() {
			if (state.enabled)
			{
				update();
				updateparent();
				if (state.parent.ptr()==nullptr)
				{
					render();
				}	
			}

		}
		
		virtual void update() {
		
		};
		virtual void render() {};
	};
	extern array<cptr< uielement>> uielemlist;
	void createuilist();
	void updateui();
	template <class T, typename... types>
	inline cptr<T> createuielement(types&& ...initval)
	{
		cptr<T> obj= cptr<T>(new T(std::forward<types>(initval)...));
		
		for (int i = 0; i < uisize; i++)
		{
			if (uielemlist[i] == nullptr) {
				uielemlist[i].cset<T>(obj);
				obj->id = i;
				obj->state.parent =cptr<uielement>( nullptr);
				return obj;

			}
		}
		Assert("not enoght room to init a new ui element");
			}

	int compareuielem(const void* b, const void* a);
	
inline	void updateui() {
		array<int> indexerlist;
		for (int i = 0; i < uisize; i++)
		{
			if (uielemlist[i].ptr() != nullptr) {

				if (uielemlist[i]->state.enabled) {

					indexerlist.append(i);
				}

			}
		}
		qsort(indexerlist.list,indexerlist.length, sizeof(int), compareuielem);
		for (int i = 0; i < indexerlist.length; i++)
		{
			uielemlist[indexerlist[i]]->totalupdate();
		}
		indexerlist.destroy();
	}
}

#endif