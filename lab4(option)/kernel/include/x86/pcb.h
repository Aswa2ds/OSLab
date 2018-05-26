#ifndef __X86_PCB_H__
#define __X86_PCB_H__

#define MAX_STACK_SIZE (16 << 10)

enum { BLOCKED, DEAD, RUNNING, RUNNABLE };

struct ProcessTable {

    union{
        uint8_t stack[MAX_STACK_SIZE]; // 内核堆栈
        struct {
            uint8_t pad0[MAX_STACK_SIZE - sizeof(struct TrapFrame)];
            struct TrapFrame tf;
        };
    };
    int state;
    int timeCount;
    int sleepTime;
    uint32_t pid;
};

struct ProcessTable pcb[2];
int current;
int ProcessNum;

/*void init_pcb();
void insert(struct ProcessTable*);
struct ProcessTable *create_pcb();*/

void schedule();

#endif
