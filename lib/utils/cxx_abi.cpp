#include <stdint.h>

extern "C" {
void* __dso_handle = 0;

int __cxa_atexit(void (*)(void*), void*, void*) { return 0; }

int __cxa_guard_acquire(int64_t* guard)
{
	return (*(char*)guard == 0);
}

void __cxa_guard_release(int64_t* guard)
{
	*(char*)guard = 1;
}

void __cxa_guard_abort(int64_t*) {}
}