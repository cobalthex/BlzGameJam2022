#pragma once

#include "Types.hh"

struct ISystem
{
	virtual void Update(const TimeStep& time) = 0;
};