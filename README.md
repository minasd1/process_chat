# process_chat
Interprocess communication using shared memory and semaphores

Compilation - Implementation guide:

After using "make all" command for compilation, we start execution from process p1. This must be done because p1 is the server of the communication, as it establishes all the shared memory segments between the processes and the semaphores needed so that the communication will be successful.

Having said that, the first execution command must be: ./p1 x, where x is a real number between 0 and 1 and it represents the probability of the message receiving noise while transmitted to and from the channel process.

After that, it will be instructed to enter a message. Having entered one, we open a second terminal and we execute the enc1 process, then execute the channel process in a third terminal and so on. 

When the message reaches process p2, it will be instructed to write a message back to process p1 and then vice versa. If a "TERM" message is sent by either of these processes, the communication will be terminated, which means that all the allocated memory will be freed, all the semaphores will close and all the shared memory segments will be destroyed.
