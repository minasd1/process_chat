#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <openssl/md5.h>

#include "shared_memory.h"
#include "semaphores.h"

int main(){

    char *message;                
    char *message2;
    unsigned int str_length;
    unsigned char initial_hash[MD5_DIGEST_LENGTH];  //HASH OF INITIAL MESSAGE
    unsigned char hash[MD5_DIGEST_LENGTH];          //HASH OF MESSAGE AFTER "NOISE" FROM CHANNEL
    unsigned char hash2[MD5_DIGEST_LENGTH];
    char *count;
    char *count2;
    int attempts = 1;
    int i;

    /* OPEN EXISTING SEMAPHORES NEEDED */
    sem_t *sem_prod3 = opensem(SEM_PRODUCER3, 0);
    sem_t *sem_prod4 = opensem(SEM_PRODUCER4, 0);

    sem_t *sem_rev3 = opensem(SEM_REV3, 0);
    sem_t *sem_rev4 = opensem(SEM_REV4, 0);

    /* AND ATTACH TO APPROPRIATE SHARED MEMORY BLOCKS */
    char* block_channel_enc2 = attach_memory_block(FILENAME, SHM_SIZE, 3);
    char* block_enc2_p2 = attach_memory_block(FILENAME, SHM_SIZE, 4);

    while(true){
        
        while(true){
            sem_p(sem_prod3);
            /* CHANNEL --> ENC2 --> P2 COMMUNICATION */

            /* IF MESSAGE FROM CHANNEL IS REDO, SEND MESSAGE AND ITS HASH TO CHANNEL AGAIN */
            while(strcmp(block_channel_enc2, "REDO") == 0){  
                //CLEAR BLOCK
                memset(block_channel_enc2, '\0', SHM_SIZE);

                //TRANSFER MESSAGE TO CHANNEL
                str_length = (unsigned int)strlen(message2);             
                strncpy(block_channel_enc2, message2, str_length);

                //ADD HASH
                count2 = block_channel_enc2 + 100;                       
                memcpy(count, hash2, MD5_DIGEST_LENGTH);

                //POST REV3 SEMAPHORE FOR CHANNEL TO READ, THEN WAIT
                sem_v(sem_rev3);
                sem_p(sem_prod3);
            }

            //CLEAR BLOCK
            memset(block_enc2_p2, '\0', SHM_SIZE);

            //READ DATA FROM CHANNEL  
            str_length = (unsigned int)strlen(block_channel_enc2);
            message = strdup(block_channel_enc2);

            if(strcmp(message, "TERM") != 0){

                //HASH THE RECEIVED MESSAGE
                MD5((unsigned char *)message, str_length, hash);

                //CHECK IF IT IS THE SAME WITH HASH GIVEN INITIALLY
                count = block_channel_enc2 + 100;
                memcpy(initial_hash, count, MD5_DIGEST_LENGTH);

                //IF NEW HASH AND INITIAL HASH ARE THE SAME, SEND MESSAGE TO P2 FOR VISUALIZATION
                if(memcmp(initial_hash, hash, MD5_DIGEST_LENGTH) == 0){
                    printf("Reading: \"%s\"\n", block_channel_enc2);
                    printf("SUCCESS! Transmission successful after %d attempts!\n\n", attempts);
                    attempts = 1;
                    strncpy(block_enc2_p2, message, str_length);
                    break;
                }
                else{           //ELSE SIGNAL THAT MESSAGE SHOULD BE RESENT BY SENDING "REDO" TO CHANNEL                                                          
                    memset(block_channel_enc2, '\0', SHM_SIZE);
                    strcpy(block_channel_enc2, "REDO");
                    attempts++;
                    sem_v(sem_rev3);
                }
            }else{
                strncpy(block_enc2_p2, message, str_length);
                break;
            }

        }   
        sem_v(sem_prod4);
        sem_p(sem_rev4);                               //READ FROM P2, MAKE HASH AND WRITE TO CHANNEL
            /* P2 --> ENC2 --> CHANNEL COMMUNICATION */

            //CLEAR BLOCK
            memset(block_channel_enc2, '\0', SHM_SIZE);

            //READ INPUT FROM P2  
            str_length = (unsigned int)strlen(block_enc2_p2);
            message2 = strdup(block_enc2_p2);
            printf("message is %s\n", message2);                                 

            //TRANSFER MESSAGE TO CHANNEL
            str_length = (unsigned int)strlen(message2);             
            strncpy(block_channel_enc2, message2, str_length);
            
            if(strcmp(message2, "TERM") != 0){

                //HASH INPUT FROM P2
                MD5((unsigned char *)message2, str_length, hash2);

                //ADD HASH
                count2 = block_channel_enc2 + 100;                       
                memcpy(count, hash2, MD5_DIGEST_LENGTH);
            }
            else{
                sem_v(sem_rev3);
                break;
            }

        sem_v(sem_rev3);

    
    }


    sem_cl(sem_prod3);
    sem_cl(sem_rev3);
    sem_cl(sem_prod4);
    sem_cl(sem_rev4);

    detach_memory_block(block_channel_enc2);
    detach_memory_block(block_enc2_p2);

    return 0;


}