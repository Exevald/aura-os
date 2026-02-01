#pragma once

#include "registers.h"

class InterruptHandler
{
public:
	virtual ~InterruptHandler() = default;
	virtual void HandleInterrupt(Registers* regs) = 0;
};