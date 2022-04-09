#pragma once
#include "framework.h"

class Ticker
{
	using clock = std::chrono::steady_clock;
	using ms = std::chrono::duration<float,std::milli>;
	clock::time_point stepstart;
	clock::time_point fullstart;
	float _delta = 0.0f;
	float _elapsed = 0.0f;
	int customFrameCount = 60;
	int currentFrameCount = 0;
	float _sumofDeltas = 0.0f;
	float _lastFrameDelta = 0.0f;

	void stepTime()
	{
		clock::time_point last = stepstart;
		
		stepstart = clock::now();
		ms gone = stepstart - last;
		_delta= gone.count();
		_elapsed= ms(stepstart - fullstart).count();
	}
	
	void updateFrameCount()
	{
		currentFrameCount++;
		_sumofDeltas += _delta;
		if (currentFrameCount >= customFrameCount)
		{
			_lastFrameDelta = _sumofDeltas;
			_sumofDeltas = 0.0f;
			currentFrameCount = 0;
		}
	}
public:
	Ticker(int _customCounter) :stepstart{ clock::now() }, customFrameCount(_customCounter)
	{

	}
	void reset() {
		stepstart = clock::now();
	}
	

	float GetElapsed() const
	{
		return _elapsed;
	}
	float GetDelta() const
	{
		return _delta;
	}
	float GetFramesDelta()
	{
		return _lastFrameDelta;
	}

	void Tick()
	{
		this->stepTime();
		this->updateFrameCount();
	}
	~Ticker() = default;
};

