#include "utils.h"

extern "C" void* memcpy(void* dest, const void* src, size_t n)
{
	auto* d = static_cast<uint8_t*>(dest);
	const auto* s = static_cast<const uint8_t*>(src);
	while (n--)
	{
		*d++ = *s++;
	}
	return dest;
}

extern "C" void* memset(void* s, int c, size_t n)
{
	auto* p = static_cast<uint8_t*>(s);
	while (n--)
	{
		*p++ = static_cast<uint8_t>(c);
	}
	return s;
}