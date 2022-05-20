#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "shared_memory.h"
#include "semaphores.h"

int main(int argc, char *argv[]){

    char *input = NULL;         //INPUT FROM USER 
    size_t len;
    unsigned int str_length;
    int read;

    sem_t *sem_prod4 = opensem(SEM_PRODUCER4, 0);
    sem_t *sem_rev4 = opensem(SEM_REV4, 0);
    
    char* block_enc2_p2 = attach_memory_block(FILENAME, SHM_SIZE, 4);


    while(true){
        

        sem_p(sem_prod4);                                       //WAIT UNTIL PRODUCER PRODUCES SOMETHING

        //READ MESSAGE FROM P1 VIA ENC2
        printf("Reading: \"%s\"\n\n", block_enc2_p2);                                            
        bool complete = (strcmp(block_enc2_p2, "TERM") == 0);   //TERMINATE COMMUNICATION
        if(complete){
            sem_v(sem_rev4);
            break;
        }

        //CLEAR BLOCK AND COMPOSE A MESSAGE TO P1
        memset(block_enc2_p2, '\0', SHM_SIZE);
        printf("Write a message: ");
        read = getline(&input, &len, stdin);
        if(read == -1){
            printf("Received no input\n");
        }
        strtok(input, "\n");
        printf("\n");
        str_length = (unsigned int)strlen(input);
        if(str_length != 0 && str_length < 100){
            strncpy(block_enc2_p2, input, str_length);
            bool complete2 = (strcmp(input, "TERM") == 0);  
            if(complete2){

                //SENDS "TERM" MESSAGE TO ENC2 AND TERMINATES 
                sem_v(sem_rev4);
                free(input);
                input = NULL;
                break;
            }
            free(input);
            input = NULL;
        }    
        else{
            printf("Size of input must be between 1 and 100 characters!\n");
            memset(block_enc2_p2, '\0', SHM_SIZE);
            strcpy(block_enc2_p2, "TERM");
            sem_v(sem_rev4);
            break;
        }

        sem_v(sem_rev4);
    }


    sem_cl(sem_prod4);
    sem_cl(sem_rev4);

    detach_memory_block(block_enc2_p2);

    return 0;
}