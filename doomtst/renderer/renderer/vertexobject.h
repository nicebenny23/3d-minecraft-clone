#pragma once
#include "../../util/dynamicarray.h"
#include "../../util/Id.h"
#include <glad/glad.h>

namespace renderer {

	/*

			//fills data unnasotided
			void fillbuffer(void* data,size_t size) {
				if (data!=nullptr)
				{
					glBufferData(type, size, data, GL_STATIC_DRAW);
				}

			}
				//use when data is of one type
			template<class T>
			void fillbuffer(stn::array<T>& data) {
				fillbuffer(data.data(), data.length() * sizeof(T));
			}
			*/

	struct ebo_tag {

	};
	using Ebo = stn::typed_id<ebo_tag>;

	struct vbo_tag {
	};
	using Vbo = stn::typed_id<vbo_tag>;
}

