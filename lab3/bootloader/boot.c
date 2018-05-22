#include "boot.h"

#define SECTSIZE 512

void bootMain(void) {
	/* 加载内核至内存，并跳转执行 */
	ELFHeader *elf;
	ProgramHeader *ph, *eph;

	uint8_t *buf = (uint8_t*)0x1000000;
	for(int i = 0; i < 200; i++)
		readSect((void*)(buf+512*i), i+1);

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
	void (*elf_entry)(void);
	elf_entry = (void*)(elf->entry);
	elf_entry();

}

void waitDisk(void) { // waiting for disk
	while((inByte(0x1F7) & 0xC0) != 0x40);
}

void readSect(void *dst, int offset) { // reading a sector of disk
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
