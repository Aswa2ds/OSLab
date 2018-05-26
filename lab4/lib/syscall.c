#include "lib.h"
#include <stdarg.h>

/*
 * io lib here
 * 库函数写在这
 */
int32_t syscall(int num, uint32_t ebx, uint32_t ecx)
{
	int32_t ret = 0;

	/*内嵌汇编 保存 num, a1, a2, a3, a4, a5 至通用寄存器*/

	asm volatile("int $0x80" : "=a"(ret): "a"(num), "b"(ebx), "c"(ecx));

	return ret;
}


void printf(const char *format,...){
	char choice;
	char c;
	uint32_t x;
	int d;
	char *s;
	va_list ap;
	va_start(ap, format);
	if(!format)
		return;
	while(*format != '\0'){
		if(*format != '%')
			syscall(4, (uint32_t)format, 1);
		else{
			choice = *++format;
			switch(choice){
				case 's':{
					int len;
					s = va_arg(ap, char*);
					for(len = 0; s[len] != '\0'; ++len);
					syscall(4, (uint32_t)s, len);
					break;
				}
				case 'c':{
					c = va_arg(ap, int);
					syscall(4, (uint32_t)&c, 1);
					break;
				}
				case 'x':{
					x = va_arg(ap, int);
					unsigned char index[10];
					int i = 0;
					if(x == 0){
						c = '0';
						syscall(4, (uint32_t)&c, 1);
					}
					while(x > 0){
						unsigned temp = x % 16;
						if(temp < 10)
							index[i++] = temp + '0';
						else
							index[i++] = temp - 10 + 'a';
						x = x / 16;
					}
					for(i = i - 1; i >= 0; --i)
						syscall(4, (uint32_t)(index + i), 1);
					break;
				}
				case 'd':{
					d = va_arg(ap, int);
					unsigned char index[15];
					if(d == 0){
						c = '0';
						syscall(4, (uint32_t)&c, 1);
					}
					if(d < 0){
						if(d == 0x80000000){
							s = "-2147483648";
							syscall(4, (uint32_t)s, 11);
						}
						else{
							c = '-';
							syscall(4, (uint32_t)&c, 1);
							d = -d;
						}
					}
					int i = 0;
					while(d > 0){
						unsigned temp = d % 10;
						index[i++] = temp + '0';
						d = d / 10;
					}
					for(i = i - 1; i >= 0; --i)
						syscall(4, (uint32_t)(index + i), 1);
					break;
				}
				default: return;
			}
		}
		format++;
	}
	va_end(ap);
	return;
}

int fork(){
	return syscall(2, 0, 0);
}

void sleep(uint32_t seconds){
	syscall(200, seconds, 0);
}
void exit(){
	syscall(1, 0, 0);
}
