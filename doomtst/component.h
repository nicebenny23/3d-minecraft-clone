#include "dynamicarray.h"
#include <unordered_map>

#ifndef component_HPP
#define component_HPP
using namespace dynamicarray;
namespace components
{



	extern std::unordered_map<char*, int> stringtoint;
	extern int curid;
	inline void initmap() {



		stringtoint = std::unordered_map<char*, int>();
		curid = 0;
	}

	inline	int idfromnameadd(char* name) {
		if (stringtoint.count(name) == 0)
		{

			stringtoint.emplace(name, curid);
			curid++;
		}

		return stringtoint[name];
	}


	inline int idfromname(char* name) {
		if (stringtoint.count(name) == 0)
		{

			return -1;
		}

		return stringtoint[name];
	}
	struct component
	{
		component() = default;
		virtual void start();

		virtual void update();



		int id;

	};








}

#endif // !coponenet_HPP#pragma once
