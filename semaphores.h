#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

/* SEMAPHORE OPERATIONS */

sem_t* sem_create(const char *name, int oflag, mode_t mode, unsigned int value); //Creates a semaphore
sem_t* opensem(const char* name, int oflag);                                     //Opens an already existing semaphore
int sem_p(sem_t *sem);                                                           // Performs sem_wait() on given semaphore 
int sem_v(sem_t *sem);                                                           //Performs sem_post() on given semaphore
int sem_cl(sem_t *sem);                                                          //Closes given semaphore
int sem_delete(const char* name);                                                //Destroys given semaphore



#endif