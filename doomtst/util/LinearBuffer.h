#pragma once
namespace stn {
	struct Linear_Buffer{
		char* data = nullptr;
		size_t capacity = 0; 
		size_t size = 0; 
		size_t element_size = 0;
	
		untyped_linear_buffer() {


		}
		void resize();
		void append();
		void cross();

	};
}