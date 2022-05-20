#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdbool.h>

//#define SHM_TAG (key_t)1357
#define PERMS 0600

/* SHARED MEMORY SEGMENT OPERATIONS */

static int shm_id = 0;
static int get_shared_block(char* filename, int size, int shm_id);
char* attach_memory_block(char *filename, int size, int shm_id);
bool detach_memory_block(char* block);
bool destroy_memory_block(char* filename, int shm_id);

#define FILENAME "p1.c"
#define SHM_SIZE 150

//semaphores
#define SEM_PRODUCER "/producer1"
#define SEM_CONSUMER "/consumer1"
#define SEM_PRODUCER2 "/producer2"
#define SEM_PRODUCER3 "/producer3"
#define SEM_PRODUCER4 "/producer4"
#define SEM_REV1 "/reverse1"
#define SEM_REV2 "/reverse2"
#define SEM_REV3 "/reverse3" 
#define SEM_REV4 "/reverse4"


#endif