#include "semaphores.h"

/*CREATES A SEMAPHORE, PRODUCES ERROR IF IT FAILS TO DO SO*/
sem_t* sem_create(const char *name, int oflag, mode_t mode, unsigned int value){
    sem_t* sem;
    sem = sem_open(name, oflag, mode, value);

    if(sem == SEM_FAILED){
        fprintf(stderr, "sem_open() failed, errno:%d\n", errno);
        exit(EXIT_FAILURE);
    }

    return sem;
}

/*OPENS AN EXISTING SEMAPHORE, PRODUCES ERROR IF IT FAILS TO DO SO*/
sem_t* opensem(const char* name, int oflag){
    sem_t* sem;
    sem = sem_open(name, oflag);

    if(sem == SEM_FAILED){
        fprintf(stderr, "sem_open() failed, errno:%d\n", errno);
        exit(EXIT_FAILURE);
    }

    return sem;
}

/*PERFORMS SEM_WAIT, ELSE PRODUCES ERROR*/
int sem_p(sem_t* sem){
    if(sem_wait(sem) == -1){
        fprintf(stderr, "sem_wait() failed, errno:%d\n", errno);
        exit(EXIT_FAILURE);
    }

    return 0;
}

/*PERFORMS SEM_POST, ELSE PRODUCES ERROR*/
int sem_v(sem_t* sem){
    if(sem_post(sem) == -1){
        fprintf(stderr, "sem_post() failed, errno:%d\n", errno);
        exit(EXIT_FAILURE);
    }

    return 0;
}

/*CLOSES A SEMAPHORE, PRODUCES ERROR IF IT FAILS TO DO SO*/
int sem_cl(sem_t* sem){
    if(sem_close(sem) == -1){
        fprintf(stderr, "sem_close() failed, errno:%d\n", errno);
        exit(EXIT_FAILURE);
    }

    return 0;
}

/*REMOVES NAMED SEMAPHORE, PRODUCES ERROR IF IT FAILS*/
int sem_delete(const char* name){
    if(sem_unlink(name) == -1){
        fprintf(stderr, "sem_unlink() failed, errno:%d\n", errno);
        exit(EXIT_FAILURE);
    }

    return 0;
}
