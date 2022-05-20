#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "shared_memory.h"
#include "semaphores.h"

#define MESSAGE_SIZE 100

int main(int argc, char *argv[]){
    
    char *input = NULL;
    size_t len; 
    unsigned int str_length;
    int read;
    if(argc != 2){
        printf("You have to put 2 arguments in command line!");
        return -1;
    }

    /* THIS IS THE SERVER PROCESS, IT CREATES ALL NECESSARY SEMAPHORES AND SHARED MEMORY SEGMENTS BETWEEN PROCESSES */

    sem_t *sem_prod1 = sem_create(SEM_PRODUCER, O_CREAT, 0600, 0); 
    sem_t *sem_cons1 = sem_create(SEM_CONSUMER, O_CREAT, 0600, 1);   
    sem_t *sem_rev1 = sem_create(SEM_REV1, O_CREAT, 0600, 0);

    sem_t *sem_prod2 = sem_create(SEM_PRODUCER2, O_CREAT, 0600, 0);
    sem_t *sem_rev2 = sem_create(SEM_REV2, O_CREAT, 0600, 0);
    
    sem_t *sem_prod3 = sem_create(SEM_PRODUCER3, O_CREAT, 0600, 0);
    sem_t *sem_rev3 = sem_create(SEM_REV3, O_CREAT, 0600, 0);

    sem_t *sem_prod4 = sem_create(SEM_PRODUCER4, O_CREAT, 0600, 0);
    sem_t *sem_rev4 = sem_create(SEM_REV4, O_CREAT, 0600, 0);

    char* block_p1_enc1 = attach_memory_block(FILENAME, SHM_SIZE, 1);       //SHARED MEMORY BETWEEN P1-ENC1
    char* block_enc1_channel = attach_memory_block(FILENAME, SHM_SIZE, 2);  //SHARED MEMORY BETWEEN ENC1-CHANNEL
    char* block_channel_enc2 = attach_memory_block(FILENAME, SHM_SIZE, 3);  //SHARED MEMORY BETWEEN CHANNEL-ENC2
    char* block_enc2_p2 = attach_memory_block(FILENAME, SHM_SIZE, 4);       //SHARED MEMORY BETWEEN ENC2-P2
    char* possibility = attach_memory_block(FILENAME, sizeof(float), 5);    //GIVEN AS SECOND ARGUMENT, POSSIBILITY OF CHANGING A CHAR OF TRANSMITTED WORD
    
    float pos = atof(argv[1]);
    memcpy(possibility, &pos, sizeof(float));


    while(true){
        /* PROGRAM RUNS APPROPRIATELY WITHOUT USING sem_cons1, BUT WE USE IT FOR SAFETY */
        sem_p(sem_cons1);

        memset(block_p1_enc1, '\0', SHM_SIZE);    

        //WRITES A MESSAGE TO P2 VIA ENC1, CHANNEL AND ENC2
        printf("Write a message: ");
        read = getline(&input, &len, stdin);                  
        strtok(input, "\n");
        printf("\n");                                  
        str_length = (unsigned int)strlen(input);             
        
        if(str_length != 0 && str_length < MESSAGE_SIZE){
           
            strncpy(block_p1_enc1, input, str_length);              //SENDS MESSAGE TO ENC1
            free(input);      
            input = NULL;
            sem_v(sem_prod1);
            sem_p(sem_rev1);                                        //RECEIVES MESSAGE FROM ENC1

            printf("Reading: \"%s\"\n\n", block_p1_enc1);                                            
            bool complete1 = (strcmp(block_p1_enc1, "TERM") == 0);
            if(complete1){
                printf("Communication terminated!\n");
                break;
            }

        }
        else{
            printf("Size of input must be between 1 and 100 characters! Try again!\n");
            break;
        }

    }

    /* AS A SERVER PROCESS, IT CLOSES ALL SEMAPHORES THAT IT HAS CREATED */      
    sem_cl(sem_prod1);
    sem_cl(sem_cons1);
    sem_cl(sem_rev1);
    sem_cl(sem_prod2);
    sem_cl(sem_rev2);
    sem_cl(sem_prod3);
    sem_cl(sem_rev3);
    sem_cl(sem_prod4);
    sem_cl(sem_rev4);

    /* REMOVES THEM */
    sem_delete(SEM_PRODUCER);
    sem_delete(SEM_CONSUMER);
    sem_delete(SEM_REV1);
    sem_delete(SEM_PRODUCER2);
    sem_delete(SEM_REV2);
    sem_delete(SEM_PRODUCER3);
    sem_delete(SEM_REV3);
    sem_delete(SEM_PRODUCER4);
    sem_delete(SEM_REV4);

    /* DETACHES FROM ALL THE SHARED MEMORY BLOCKS */
    detach_memory_block(block_p1_enc1);
    detach_memory_block(block_enc1_channel);
    detach_memory_block(block_channel_enc2);
    detach_memory_block(block_enc2_p2);
    detach_memory_block(possibility);

    /* AND DESTROYS THEM */
    destroy_memory_block(FILENAME, 1);
    destroy_memory_block(FILENAME, 2);
    destroy_memory_block(FILENAME, 3);
    destroy_memory_block(FILENAME, 4);
    destroy_memory_block(FILENAME, 5);

    

    return 0;
}