#include "x86.h"

void init_pcb(){
    int i = 0;
    for(; i < MAX_PCB_NUM - 1; ++i)
        pcb[i]->next = &pcb[i+i];
    pcb[MAX_PCB_NUM - 1]->next = NULL;
    next_pcb = &pcb[0];
    now_pcb = NULL;
    head_pcb = NULL;
}

void insert(struct ProcessTable *p){
    if(head_pcb == NULL)
        head_pcb = p;
    else{
        struct ProcessTable *q = head_pcb;
        for(; q->next != NULL; p = p->next);
        q->next = p;
    }
}

struct ProcessTable *create_pcb(){
    struct ProcessTable *p = next_pcb;
    next_pcb = next_pcb->next;
    p->sleepTime = 0;
    p->timeCount = 16;
    p->pid = 2000 + (p - pcb);
    p->state = RUNNABLE;
    p->next = NULL;
    insert(p);
    return p;
}
