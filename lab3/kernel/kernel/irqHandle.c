#include "x86.h"
#include "device.h"

void syscallHandle(struct TrapFrame *tf);

void GProtectFaultHandle(struct TrapFrame *tf);

void irqHandle(struct TrapFrame *tf) {
	/*
	 * 中断处理程序
	 */
	/* Reassign segment register */
	switch(tf->irq) {
		case -1:
			break;
		case 0xd:
			GProtectFaultHandle(tf);
			break;
		csse 0x20:
			syscallHandle(tf);
		case 0x80:
			syscallHandle(tf);
			break;
		default:assert(0);
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

void sys_fork(tf){
	struct ProcessTable *p = new struct ProcessTable;
	
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

void GProtectFaultHandle(struct TrapFrame *tf){
	assert(0);
	return;
}
