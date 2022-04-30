#include "Lerps.h"

float Lerp(float a, float b, float t)
{
	//assert(t <= 1.0f);
	//assert(t >= 0.0f);
	return (1.0f - t) * a + t * b;
}

float InvLerp(float a, float b, float v)
{
	//assert(b!=a)
	return (v - a) / (b - a);
}

float Remap(float inMin, float inMax, float outMin, float outMax, float inValue)
{
	float t = InvLerp(inMin, inMax, inValue);
	return Lerp(outMin, outMax, t);
}