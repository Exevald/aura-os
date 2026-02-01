#pragma once

#include "../../drivers/ata_driver.h"
#include "../console/console.h"

namespace Command
{
void Touch(const char* filename, Console* console);
void Cat(const char* filename, Console* console);
void Grep(const char* pattern, const char* filename, Console* console);
void Ls(Console* console);
void Echo(const char* text, const char* filename, Console* console);
void Run(int argc, char** argv, Console* console);
void Clear(Console* console);
} // namespace Command
