
#include "shared_memory.h"

/*RETURNS THE IDENTIFIER OF SHARED MEMORY ASSOCIATED WITH KEY VALUE, ELSE RETURNS -1*/
static int get_shared_block(char* filename, int size, int shm_id){
    key_t key;

    key = ftok(filename, shm_id);
    if(key == -1){
        return -1;
    }

    return shmget(key, size, PERMS | IPC_CREAT);
}

/*CALLS get_shared_block() TO GET SHARED MEMORY IDENTIFIER AND ATTACHES TO IT, PRODUCES ERROR IF FAILS*/
char* attach_memory_block(char* filename, int size, int shm_id) {
    int shared_block_id = get_shared_block(filename, size, shm_id);
    char* attach;
    
    if(shared_block_id == -1){
        fprintf(stderr, "shm_get() failed, errno:%d\n", errno);
        exit(EXIT_FAILURE);
    }
    
    attach = shmat(shared_block_id, NULL, 0);
    if(attach == (char*) -1){
        fprintf(stderr, "shm_at() failed, errno:%d\n", errno);
        exit(EXIT_FAILURE);
    }

    return attach;
}

/*DETACHES GIVEN SHARED MEMORY BLOCK*/
bool detach_memory_block(char* block){
    return (shmdt(block));
}


/*DESTROYS GIVEN SHARED MEMORY BLOCK*/
bool destroy_memory_block(char *filename, int shmid) {
    int shared_block_id = get_shared_block(filename,0,shmid);

    if(shared_block_id == -1){
        return NULL;
    }
    return (shmctl(shared_block_id, IPC_RMID, NULL));
}