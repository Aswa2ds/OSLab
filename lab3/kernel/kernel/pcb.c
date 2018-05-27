#include "device.h"
#include "x86.h"

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

        asm volatile("pushl %eax");
        asm volatile("movl %0, %%eax" ::"r"(USEL(SEG_UDATA)));
        asm volatile("movw %ax, %ds");
        asm volatile("movw %ax, %es");
        asm volatile("popl %eax");

        asm volatile("movl %0, %%esp" ::"r"(&pcb[current].tf));
        asm volatile("popl %gs");
        asm volatile("popl %fs");
        asm volatile("popl %es");
        asm volatile("popl %ds");
        asm volatile("popal");
        asm volatile("addl $4, %esp");
        asm volatile("addl $4, %esp");

        asm volatile("iret");
    }
}
