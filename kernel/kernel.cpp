#include "../lib/console/console.h"

extern "C" void _cxa_pure_virtual() {
    while (true);
}

extern "C" void kernel_main() {
    Console console;
    console.Init();

    console.Write("Aura OS Kernel (C++ Edition)\n");
    console.SetColor(Console::LightGreen, Console::Blue);
    console.Write("System status: OK\n");
    console.SetColor(Console::Red, Console::Blue);
}
