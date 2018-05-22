#ifndef __X86_PCB_H__
#define __X86_PCB_H__

#define MAX_STACK_SIZE 16 << 8
#define MAX_PCB_NUM 20

struct ProcessTable {
    uint32_t stack[MAX_STACK_SIZE]; // 内核堆栈
    struct TrapFrame tf;
    int state;
    int timeCount;
    int sleepTime;
    uint32_t pid;
    struct ProcessTable *next;
};

struct ProcessTable pcb[MAX_PCB_NUM];


#endif
