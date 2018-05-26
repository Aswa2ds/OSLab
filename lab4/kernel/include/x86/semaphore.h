#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

typedef int32_t sem_t;

struct Semaphore {
    int value;
    int waiting;
};

struct Semaphore semaphore;


#endif
