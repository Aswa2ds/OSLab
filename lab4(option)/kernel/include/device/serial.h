#ifndef __SERIAL_H__
#define __SERIAL_H__

void initSerial(void);
void putChar(char);
unsigned getKeyCode();
#define SERIAL_PORT  0x3F8

#endif
