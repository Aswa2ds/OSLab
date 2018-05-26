#include "common.h"
#include "x86.h"
#include "device.h"

static char *i2A(int a) {
	static char buf[30];
	int nag = 0;
	char *p = buf + sizeof(buf) - 1;
	if(a < 0){
		a = -a;
		nag = 1;
	}
	do {
		*--p = '0' + a % 10;
	} while (a /= 10);
	if(nag)
		*--p = '-';
	return p;
}

static void append(char **p, const char *str) {
	while (*str) {
		*((*p) ++) = *str ++;
	}
}

/* 将文件名和assert fail的行号显示在屏幕上 */
#define BLUE_SCREEN_TEXT "Assertion failed: "
static void displayMessage(const char *file, int line) {
	static char buf[256] = BLUE_SCREEN_TEXT;
	char *p = buf + sizeof(BLUE_SCREEN_TEXT) - 1;

	append(&p, file);
	append(&p, ":");
	append(&p, i2A(line));
	append(&p, "\n");

	for (p = buf; *p; p ++) {
		putChar(*p);
	}
}

int abort(const char *fname, int line) {
	disableInterrupt();
	displayMessage(fname, line);
	while (TRUE) {
		waitForInterrupt();
	}
}

#define PANIC_TEXT "PPPPPPANIC: "
void panic_i(int num){
	static char buf[256] = PANIC_TEXT;
	char *p = buf + sizeof(PANIC_TEXT) - 1;
	append(&p, i2A(num));
	append(&p, "\n");
	for(p = buf; *p; p ++) {
		putChar(*p);
	}
}
void panic_s(const char* line){
	static char buf[256] = PANIC_TEXT;
	char *p = buf + sizeof(PANIC_TEXT) - 1;
	append(&p, line);
	append(&p, "\n");
	for(p = buf; *p; p ++) {
		putChar(*p);
	}
}
