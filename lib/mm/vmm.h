#pragma once

#include <stdint.h>

struct PTE
{
	uint32_t present : 1;
	uint32_t rw : 1;
	uint32_t user : 1;
	uint32_t write_through : 1;
	uint32_t cache_disable : 1;
	uint32_t accessed : 1;
	uint32_t dirty : 1;
	uint32_t pat : 1;
	uint32_t global : 1;
	uint32_t available : 3;
	uint32_t frame : 20;
};

struct PageTable
{
	PTE pages[1024];
};

struct PDE
{
	uint32_t present : 1;
	uint32_t rw : 1;
	uint32_t user : 1;
	uint32_t write_through : 1;
	uint32_t cache_disable : 1;
	uint32_t accessed : 1;
	uint32_t zero : 1;
	uint32_t page_size : 1;
	uint32_t ignored : 1;
	uint32_t available : 3;
	uint32_t table_addr : 20;
};

struct PageDirectory
{
	PDE tables[1024];
};