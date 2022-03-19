#pragma once
constexpr const bool isDebug()
{
#ifndef NDEBUG
	return true;
#else
	return false;
#endif
}

constexpr const UINT GetDebugValue(UINT onTrue, UINT onFalse)
{
	if (isDebug())
	{
		return onTrue;
	}
	return onFalse;
}