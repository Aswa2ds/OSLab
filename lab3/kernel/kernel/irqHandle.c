#include "x86.h"
#include "device.h"

void syscallHandle(struct TrapFrame *tf);

void timerHandle(struct TrapFrame *tf);

void GProtectFaultHandle(struct TrapFrame *tf);

void irqHandle(struct TrapFrame *tf) {
	/*
	 * 中断处理程序
	 */
	/* Reassign segment register */
	asm volatile("movl %0, %%eax" ::"r"(KSEL(SEG_KDATA)));
    asm volatile("movw %ax, %ds");
    asm volatile("movw %ax, %fs");
    asm volatile("movw %ax, %es");
    asm volatile("movl %0, %%eax" ::"r"(KSEL(SEG_VIDEO)));
    asm volatile("movw %ax, %gs");

	switch(tf->irq) {
		case -1:
			break;
		case 0xd:
			GProtectFaultHandle(tf);
			break;
		case 0x20:
			//panic_s("int 0x20");
			timerHandle(tf);
			break;
		case 0x80:
			panic_s("int 0x80");
			syscallHandle(tf);
			break;
		default:
			panic_s("GP!!!!!!!!!!!!!");
			assert(0);
	}
}

void video_print(int row, int col, char c) {
	unsigned int p1, p2;
	p1 = 0xb8000 + (80 * row + col) * 2;
	p2 = p1 + 1;
	*(unsigned char*)p1 = c;
	*(unsigned char*)p2 = 0x0c;
}

void sys_write(struct TrapFrame *tf) {
	asm volatile("movl %0, %%eax":: "r"(KSEL(SEG_VIDEO)));
	asm volatile("movw %ax, %gs");
	static int row = 0, col = 0;
	int i;
	for(i = 0; i < tf->ecx; ++i){
		char ch = *(char*)(tf->ebx+i);
		putChar(ch);
		if(ch == '\n'){
			row++;
			col = 0;
			continue;
		}
		if(col == 80){
			row++;
			col = 0;
		}
		video_print(row, col++, ch);
	}
	tf->eax = 1;
}

void sys_fork(struct TrapFrame *tf){
	struct ProcessTable *p = &pcb[1];
	struct ProcessTable *q = &pcb[0];

	int i;
	for(i = 0; i < MAX_STACK_SIZE; ++i)
		p->stack[i] = q->stack[i];

	int src = 0x200000;
	int dest = 0x210000;
	for(i = 0; i < 10000; ++i)
		*(uint8_t*)(dest + i) = *(uint8_t*)(src + i);
	for(i = 0; i < sizeof(struct TrapFrame); ++i)
		p->tf_block[i] = q->tf_block[i];
	p->pid = q->pid + 1;
	p->sleepTime = 0;
	p->timeCount = 16;
	p->state = RUNNABLE;
	ProcessNum++;
	schedule();
}

void syscallHandle(struct TrapFrame *tf) {
	/* 实现系统调用*/
	switch(tf->eax){
		case 2:
			sys_fork(tf);
			break;
		case 4:
			sys_write(tf);
			break;
		default:
			assert(0);
	}

}

void timerHandle(struct TrapFrame *tf){
	if(pcb[0].sleepTime > 0)
		if(--pcb[0].sleepTime == 0)
			pcb[0].state = RUNNABLE;

	if(pcb[0].sleepTime > 0)
		if(--pcb[0].sleepTime == 0)
			pcb[0].state = RUNNABLE;

	if(current == -1){
		schedule();
		return ;
	}

	pcb[current].timeCount -= 1;
	if(pcb[current].timeCount == 0){
		pcb[current].state = RUNNABLE;
        pcb[current].timeCount = 16;
        panic_i(pcb[current].timeCount);
        schedule();
	}
	return ;
}

void GProtectFaultHandle(struct TrapFrame *tf){
	assert(0);
	return;
}
