#pragma once

#include <stddef.h>

inline void* memset(void* ptr, const int value, size_t num)
{
	auto p = static_cast<unsigned char*>(ptr);
	while (num--)
	{
		*p++ = static_cast<unsigned char>(value);
	}
	return ptr;
}