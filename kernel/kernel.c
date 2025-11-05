#include "kernel.h"

void kernel_main(void) {
    char *video = (char*)0xB8000;
    video[0] = 'O';
    video[1] = 0x07;
    video[2] = 'K';
    video[3] = 0x07;
    while(1);
}