#include "x86.h"
#include "device.h"

SegDesc gdt[NR_SEGMENTS];
TSS tss;

#define SECTSIZE 512

void waitDisk(void) {
	while((inByte(0x1F7) & 0xC0) != 0x40);
}

void readSect(void *dst, int offset) {
	int i;
	waitDisk();

	outByte(0x1F2, 1);
	outByte(0x1F3, offset);
	outByte(0x1F4, offset >> 8);
	outByte(0x1F5, offset >> 16);
	outByte(0x1F6, (offset >> 24) | 0xE0);
	outByte(0x1F7, 0x20);

	waitDisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = inLong(0x1F0);
	}
}

void initSeg() {
	gdt[SEG_KCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_KERN);
	gdt[SEG_KDATA] = SEG(STA_W,         0,       0xffffffff, DPL_KERN);
	gdt[SEG_UCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_USER);
	gdt[SEG_UDATA] = SEG(STA_W,         0,       0xffffffff, DPL_USER);
	gdt[SEG_TSS] = SEG16(STS_T32A,      &tss, sizeof(TSS)-1, DPL_KERN);
	gdt[SEG_TSS].s = 0;
	gdt[SEG_VIDEO] = SEG(STA_W,  0x0b8000,       0xffffffff, DPL_KERN);
	setGdt(gdt, sizeof(gdt));

	/*
	 * 初始化TSS
	 */
	//tss.esp0 = 0x200000;   // set kernel esp to 0x200,000
	tss.esp0 = (uint32_t)&pcb[0].stack[KERNEL_STACK_SIZE];
	tss.ss0  = KSEL(SEG_KDATA);
	asm volatile("ltr %%ax":: "a" (KSEL(SEG_TSS)));

	/*设置正确的段寄存器*/
	asm volatile("movl %0, %%eax":: "r"(KSEL(SEG_KDATA)));
	asm volatile("movw %ax, %ds");
	asm volatile("movw %ax, %es");
	asm volatile("movw %ax, %ss");
	asm volatile("movw %ax, %fs");
	asm volatile("movl %0, %%eax":: "r"(KSEL(SEG_VIDEO)));
	asm volatile("movw %ax, %gs");
	lLdt(0);

}

void enterUserSpace(uint32_t entry) {
	asm volatile("movl %0, %%eax":: "r"(USEL(SEG_UDATA)));
	asm volatile("movw %ax, %ds");
	asm volatile("movw %ax, %es");
	asm volatile("movw %ax, %fs");
	/*
	 * Before enter user space
	 * you should set the right segment registers here
	 * and use 'iret' to jump to ring3
	*/
	asm volatile("pushl %0":: "r"(USEL(SEG_UDATA)));	// %ss
	asm volatile("pushl %0":: "r"(128 << 20));			// %esp 128MB
	asm volatile("pushfl");								// %eflags
	asm volatile("pushl %0":: "r"(USEL(SEG_UCODE)));	// %cs
	asm volatile("pushl %0":: "r"(entry));				// %eip

	asm volatile("iret");
}

void loadUMain(void) {

	/*加载用户程序至内存*/
	/*加载用户程序至内存*/
	ELFHeader *elf;
	ProgramHeader *ph, *eph;

	uint8_t *buf = (uint8_t*)0x2000000;
	for(int i = 0; i < 200; i++)
		readSect((void*)(buf+512*i), i+201);
	elf = (ELFHeader*)buf;
	ph = (ProgramHeader*)((uint32_t)elf+elf->phoff);
	eph = ph + elf->phnum;
	for(; ph < eph; ph++){
		if(ph->type == 1){
			uint32_t a = ph->vaddr;
			uint32_t b = (uint32_t)elf + ph->off;
			while(a < ph->vaddr + ph->filesz){
				*(uint8_t*)a = *(uint8_t*)b;
				a++;
				b++;
			}
			while(a < ph->vaddr + ph->memsz){
				*(uint8_t*)a = 0;
				a++;
				b++;
			}
		}
	}
	enterUserSpace(elf->entry);
}
