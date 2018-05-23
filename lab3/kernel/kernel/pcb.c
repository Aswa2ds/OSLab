#include "x86.h"

void init_pcb(){
    int i = 0;
    for(; i < MAX_PCB_NUM - 1; ++i)
        pcb[i]->next = &pcb[i+i];
    pcb[MAX_PCB_NUM - 1]->next = NULL;
}
