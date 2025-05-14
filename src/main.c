#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

static void load_test_program(RL78_CPU* cpu)
{
    cpu->memory[0x0000] = 0x00;
    cpu->memory[0x0001] = 0x0C;
    cpu->memory[0x0002] = 0x20;
    cpu->memory[0x0003] = 0x16;
    cpu->memory[0x0004] = 0xF7;
}

int main(void)
{
    RL78_CPU *cpu = malloc(sizeof(RL78_CPU));
    cpu_init(cpu);

    load_test_program(cpu);

    for (;;) {
        getchar();
        cpu_cycle(cpu);
    }

    free(cpu);
}
