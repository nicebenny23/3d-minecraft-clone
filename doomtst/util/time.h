#include <chrono>
#include "dynamicarray.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "mathutil.h"
using namespace dynamicarray;
using namespace std::chrono;
#ifndef timehpp
#define timehpp
namespace timename {
	extern float fps;
	extern float dt;
	extern float gametime;
	extern float tfps;
	extern float averagedt;
	extern float smoothdt;
	void inittime(); 

	inline void calcfps() {

		auto currtime = glfwGetTime();
		dt = currtime - gametime;
		//to prevent erros in debug
		if (averagedt==0)
		{
			averagedt = 1/60.f;
		}
		averagedt = averagedt * .95f+ Min( dt,.2f)* .05f;
		smoothdt =Min(averagedt, .05f);
		
		fps = 1000 / smoothdt;

		gametime = currtime;
	}

	extern int currtimer;

	struct  timer
	{

		bool decreseonceonframe;//whether it goes by frame
		float value;//the current time on the timer
		timer();
		
		bool isenabled();
		void decrement(float amt);
		void remove();
		bool running;//on iff the timer is running at the molment
		void enable(float time);
		bool finishedthisframe() {
			return value == 0;
		}
	private:int timeid;


	};
	extern array<timer*> timerlist;


	inline timer::timer() {
		decreseonceonframe = false;

		running = false;

		timeid = currtimer;
		timerlist.append(this);
		currtimer++;


	}

	inline bool timer::isenabled()
	{
		//ifrunning will activate if being ran
		if (value == 0 || !running)
		{
			return false;
		}
		return true;

	}

	inline void timer::decrement(float amt)
	{

		if (value > 0 && running)
		{


			if (value == 0)
			{
				amt = 1;
			}
			value -= amt;
			if (value < 0)
			{
				running = false;

				value = 0;
			}

		}


	}

	inline void timer::remove()
	{
		timerlist.deleteind(timeid);
		currtimer--;
	}

	inline void timer::enable(float time)
	{

		value = time;
		running = true;
	}

	extern int currwatch;

	struct  watch
	{
		bool running;
		float currtime;
		watch(float time, bool isrunningatstart);

		void increment(float amt);
		void removefromlist();

		void start();
	private:
		int watchid;



	};
	extern array<watch*> watchlist; 


	inline watch::watch(float time, bool isrunningatstart) {


		currtime = time;
		watchid = currtimer;
		watchlist.append(this);
		currwatch++;
		running = isrunningatstart;

	}


	inline void watch::increment(float amt)
	{
		if (running)
		{
			currtime += amt;

		}

	}

	inline void watch::removefromlist()
	{
		watchlist.deleteind(watchid);
		currwatch--;
	}

inline 	void watch::start()
	{
		currtime = 0;
		running = true;
	}


	inline  void calctimers() {


		for (int i = 0; i < watchlist.length; i++)
		{
			watchlist[i]->increment(dt);
		}
		for (int i = 0; i < timerlist.length; i++)
		{
			timerlist[i]->decrement(dt);
		}
	}
}
#endif // !timehpp#pragma once
