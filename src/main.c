#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

static int load_test_program(RL78_CPU* cpu)
{
    FILE* file = fopen("./example_program/test.bin", "rb");
    if (file == NULL)
    {
        return 0;
    }
    fread(cpu->memory, sizeof(uint8_t), ROM_SIZE, file);
    fclose(file);
    return 1;
}

int main(void)
{
    RL78_CPU *cpu = malloc(sizeof(RL78_CPU));
    cpu_init(cpu);

    if (!load_test_program(cpu))
    {
        printf("Couldn't load test.bin\n");
        return 1;
    }

    for (;;) {
        getchar();
        cpu_cycle(cpu);
    }
     
    free(cpu);
    return 0;
}
