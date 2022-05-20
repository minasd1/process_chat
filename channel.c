#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

#include "shared_memory.h"
#include "semaphores.h"

#define MESSAGE_SIZE 100

int main(){

    char* count1;
    char* count2;
    char* count3;
    char* count4;
    unsigned char hash[MD5_DIGEST_LENGTH];
    unsigned int str_length;
    float pos;                              //POSSIBILITY THAT PASSED AS ARGUMENT FROM P1
    float random;                           //A RANDOM NUMBER BETWEEN 0 AND 1
    int i;

    /* OPEN EXISTING SEMAPHORES NECESSARY */
    sem_t *sem_prod2 = opensem(SEM_PRODUCER2, 0);
    sem_t *sem_rev2 = opensem(SEM_REV2, 0);

    sem_t *sem_prod3 = opensem(SEM_PRODUCER3, 0);
    sem_t *sem_rev3 = opensem(SEM_REV3, 0);

    /* AND ATTACH TO THE SHARED MEMORY BLOCKS NEEDED */
    char* block_enc1_channel = attach_memory_block(FILENAME, SHM_SIZE,2);
    char* block_channel_enc2 = attach_memory_block(FILENAME, SHM_SIZE,3);
    char* possibility = attach_memory_block(FILENAME, sizeof(float), 5);

    while(true){

        sem_p(sem_prod2);
        /* ENC1 --> CHANNEL --> ENC2 COMMUNICATION */

        //CLEAR BLOCK AND MESSAGE
        memset(block_channel_enc2, '\0', SHM_SIZE);

        //READ MESSAGE FROM ENC1  
        unsigned int str_length = (unsigned int)strlen(block_enc1_channel);
        char message[str_length];
        memset(message, '\0', str_length);
        strcpy(message, block_enc1_channel);

        str_length = (unsigned int)strlen(message);

        //TRANSFER MESSAGE TO ENC2
        if(strcmp(message, "REDO") != 0 && strcmp(message, "TERM") != 0){

            //FILTER GIVEN MESSAGE USING POSSIBILITY ARGUMENT FROM COMMAND LINE
            memcpy(&pos, possibility, sizeof(float));
            str_length = (unsigned int)strlen(message);
            for(i = 0; i < str_length; i++){
                random = ((float)rand()/RAND_MAX);
                if(random < pos){
                    message[i] = message[i] + 1;
                }
            }

            //RETRIEVE HASH FROM ENC1 WITHOUT CHANGING IT
            count1 = block_enc1_channel + 100;
            memcpy(hash, count1, MD5_DIGEST_LENGTH);

            //ADD HASH IN BLOCK
            count2 = block_channel_enc2 + 100;                       
            memcpy(count2, hash, MD5_DIGEST_LENGTH);

        }
        //IF MESSAGE FROM ENC1 IS REDO OR TERM, SEND MESSAGE ONLY
        strncpy(block_channel_enc2, message, str_length);

        sem_v(sem_prod3);
        sem_p(sem_rev3);                                    
        /* ENC2 --> CHANNEL --> ENC1 COMMUNICATION */

        //CLEAR BLOCK AND MESSAGE
        memset(block_enc1_channel, '\0', SHM_SIZE);
   
        //READ MESSAGE FROM ENC2  
        str_length = (unsigned int)strlen(block_channel_enc2);
        char message2[str_length];
        memset(message2, '\0', str_length);
        strcpy(message2, block_channel_enc2);

        str_length = (unsigned int)strlen(message2);  

        //TRANSFER MESSAGE TO ENC1
        if(strcmp(message2, "REDO") != 0 && strcmp(message2, "TERM") != 0){

            //FILTER GIVEN MESSAGE USING POSSIBILITY ARGUMENT FROM COMMAND LINE
            memcpy(&pos, possibility, sizeof(float));
            for(i = 0; i < str_length; i++){
                random = ((float)rand()/RAND_MAX);
                if(random < pos){
                    message2[i] = message2[i] + 1;
                }
            }
            
            //RETRIEVE HASH FROM ENC2 WITHOUT CHANGING IT
            count3 = block_channel_enc2 + 100;
            memcpy(hash, count3, MD5_DIGEST_LENGTH);

            //ADD HASH IN BLOCK
            count4 = block_enc1_channel + 100;                       
            memcpy(count4, hash, MD5_DIGEST_LENGTH);
        }
        //IF MESSAGE FROM ENC2 IS REDO OR TERM, SEND MESSAGE ONLY
        strncpy(block_enc1_channel, message2, str_length);

        sem_v(sem_rev2);
        if(strcmp(message2, "TERM") == 0){break;}       

    }


    sem_cl(sem_prod2);
    sem_cl(sem_rev2);

    sem_cl(sem_prod3);
    sem_cl(sem_rev3);

    detach_memory_block(block_enc1_channel);
    detach_memory_block(block_channel_enc2);

    return 0;
}