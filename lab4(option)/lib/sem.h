#include "types.h"
#ifndef __sem_h__
#define __sem_h__

typedef int32_t sem_t;

int sem_init(sem_t *, int);
void sem_wait(sem_t *);
void sem_post(sem_t *);
void sem_destroy(sem_t *);

#endif
