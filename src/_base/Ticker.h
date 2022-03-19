#pragma once
#include "framework.h"

class Ticker
{
	using clock = std::chrono::steady_clock;
	using ms = std::chrono::duration<float,std::milli>;
	clock::time_point stepstart;
	clock::time_point fullstart;
public:
	Ticker() :stepstart{ clock::now() }
	{

	}
	void reset() {
		stepstart = clock::now();
	}
	
	float delta()
	{
		clock::time_point last = stepstart;
		stepstart = clock::now();
		ms gone = stepstart - last;
		return gone.count();
	}
	float elapsed()
	{
		return ms(clock::now() - fullstart).count();
	}
	~Ticker() = default;
};

