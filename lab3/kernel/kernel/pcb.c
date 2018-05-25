#include "device.h"
#include "x86.h"

/*void init_pcb(){
    int i = 0;
    for(; i < MAX_PCB_NUM - 1; ++i)
        pcb[i]->next = &pcb[i+i];
    pcb[MAX_PCB_NUM - 1]->next = NULL;
    next_pcb = &pcb[0];
    now_pcb = NULL;
    head_pcb = NULL;
}*/

/*void insert(struct ProcessTable *p){
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
}*/

extern SegDesc gdt[NR_SEGMENTS];
extern TSS tss;

void IDLE() {
    asm volatile("movl %0, %%esp;" ::"i"(0x200000));
    asm volatile("sti");
    while(1)
        waitForInterrupt();
}

void schedule(){
    if(current == -1){
        if(pcb[0].state == RUNNABLE)
            current = 0;
        else if(pcb[1].state == RUNNABLE)
            current = 1;
    }
    else{
        if(current == 0 && pcb[1].state == RUNNABLE)
            current = 1;
        else if(current == 1 && pcb[0].state == RUNNABLE)
            current = 0;
    }

    if(current != -1)
        pcb[current].state = RUNNING;

    //panic_i(current);
    if(current == -1)
        IDLE();
    else{
        tss.esp0 = (uint32_t)&(pcb[current].stack[MAX_STACK_SIZE]);
        tss.ss0 = KSEL(SEG_KDATA);

        gdt[SEG_UCODE] = SEG(STA_X | STA_R, current * (1 << 16), 0xffffffff, DPL_USER);
        gdt[SEG_UDATA] = SEG(STA_W,         current * (1 << 16), 0xffffffff, DPL_USER);

        asm volatile("pushl %eax"); // save eax
        asm volatile("movl %0, %%eax" ::"r"(USEL(SEG_UDATA)));
        asm volatile("movw %ax, %ds");
        asm volatile("movw %ax, %es");
        asm volatile("popl %eax");

        // restore process info
        //panic_i(pcb[current].tf.eip);
        asm volatile("movl %0, %%esp" ::"r"(&pcb[current].tf));
        asm volatile("popl %gs");
        asm volatile("popl %fs");
        asm volatile("popl %es");
        asm volatile("popl %ds");
        asm volatile("popal");  // Attention! will change all registers
        asm volatile("addl $4, %esp");
        asm volatile("addl $4, %esp");

        // return to user space
        asm volatile("iret");
    }
}
