#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <openssl/md5.h>

#include "shared_memory.h"
#include "semaphores.h"

#define MESSAGE_SIZE 100

int main(){

    char *message;                                  //MESSAGE RECEIVED FROM P1              
    char *message2;                                 //MESSAGE RECEIVED FROM CHANNEL
    unsigned int str_length;
    unsigned char initial_hash[MD5_DIGEST_LENGTH];  //HASH OF INITIAL MESSAGE
    unsigned char hash[MD5_DIGEST_LENGTH];          //HASH OF MESSAGE AFTER "NOISE" FROM CHANNEL
    unsigned char hash2[MD5_DIGEST_LENGTH];         
    char *count;
    char *count2;
    int attempts = 1;
    int i;

    /* OPENS SEMAHORES THAT ENC1 PROCESS NEEDS */
    sem_t *sem_prod1 = opensem(SEM_PRODUCER, 0);
    sem_t *sem_cons1 = opensem(SEM_CONSUMER, 0);
    sem_t *sem_rev1 = opensem(SEM_REV1, 0);

    sem_t *sem_prod2 = opensem(SEM_PRODUCER2, 0);
    sem_t *sem_rev2 = opensem(SEM_REV2, 0);

    /* AND OPENS NEEDED SHARED MEMORY BLOCKS */
    char* block_p1_enc1 = attach_memory_block(FILENAME, SHM_SIZE,1);
    char* block_enc1_channel = attach_memory_block(FILENAME, SHM_SIZE,2);


    while(true){
        
        /* P1 --> ENC1 --> CHANNEL COMMUNICATION */
        sem_p(sem_prod1);

        //CLEAR BLOCK
        memset(block_enc1_channel,'\0',SHM_SIZE);

        //READ DATA FROM ENC1 
        str_length = (unsigned int)strlen(block_p1_enc1);
        message = strdup(block_p1_enc1);                                      
        printf("message is %s\n", message);

        //WRITE MESSAGE TO CHANNEL
        str_length = (unsigned int)strlen(message);             
        strncpy(block_enc1_channel, message, str_length);

        if(strcmp(message, "TERM") != 0){

            //HASH THE RECEIVED MESSAGE
            MD5((unsigned char *)message, str_length, hash);

            //ADD HASH IN BLOCK
            count = block_enc1_channel + MESSAGE_SIZE;                       
            memcpy(count, hash, MD5_DIGEST_LENGTH);
        }
                

        sem_v(sem_prod2);

        while(true){
            /* CHANNEL --> ENC1 --> P1 COMMUNICATION */

            sem_p(sem_rev2);

            /* IF MESSAGE FROM CHANNEL IS REDO, SEND INITIAL MESSAGE AND ITS HASH TO CHANNEL AGAIN */
            while(strcmp(block_enc1_channel, "REDO") == 0){

                //CLEAR BLOCK
                memset(block_enc1_channel,'\0',SHM_SIZE);

                //WRITE MESSAGE TO CHANNEL
                str_length = (unsigned int)strlen(message);             
                strncpy(block_enc1_channel, message, str_length);

                //ADD HASH IN BLOCK
                count = block_enc1_channel + 100;                       
                memcpy(count, hash, MD5_DIGEST_LENGTH);

                //POST PROD2 SEMAPHORE FOR CHANNEL TO READ, THEN WAIT
                sem_v(sem_prod2);
                sem_p(sem_rev2);
            }


            //CLEAR BLOCK
            memset(block_p1_enc1, '\0', SHM_SIZE);

            //READ DATA FROM CHANNEL  
            str_length = (unsigned int)strlen(block_enc1_channel);
            message2 = strdup(block_enc1_channel);

            if(strcmp(message2, "TERM") != 0){
                
                //HASH THE RECEIVED MESSAGE
                MD5((unsigned char *)message2, str_length, hash2);

                //CHECK IF NEW HASH IS THE SAME WITH HASH GIVEN INITIALLY
                count2 = block_enc1_channel + 100;
                memcpy(initial_hash, count2, MD5_DIGEST_LENGTH);

                //IF NEW HASH AND INITIAL HASH ARE THE SAME, SEND MESSAGE TO P1 FOR VISUALIZATION
                if(memcmp(initial_hash, hash2, MD5_DIGEST_LENGTH) == 0){
                    printf("Reading: \"%s\"\n", block_enc1_channel);
                    printf("SUCCESS! Transmission successful after %d attempts!\n\n", attempts);
                    attempts = 1;
                    strncpy(block_p1_enc1, message2, str_length);
                    break;
                }
                else{       //ELSE SIGNAL THAT MESSAGE SHOULD BE RESENT BY SENDING "REDO" TO CHANNEL
                    memset(block_enc1_channel, '\0', SHM_SIZE);
                    strcpy(block_enc1_channel, "REDO");
                    attempts++;
                    sem_v(sem_prod2);
                }
            }else{
                strncpy(block_p1_enc1, message2, str_length);
                break;
            }

        }
        sem_v(sem_cons1);
        sem_v(sem_rev1);
        if(strcmp(message2, "TERM") == 0){break;}

    }

    
    sem_cl(sem_prod1);
    sem_cl(sem_cons1);
    sem_cl(sem_rev1);

    sem_cl(sem_prod2);
    sem_cl(sem_rev2);

    detach_memory_block(block_p1_enc1);
    detach_memory_block(block_enc1_channel);


    return 0;
}