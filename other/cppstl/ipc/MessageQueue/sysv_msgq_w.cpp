#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "msgq.hpp"


// send message to queue

int main()
{
    key_t key = ftok("queuefile", 65);  // gen unique key

    int msgid = msgget(key, 0666 | IPC_CREAT);  // create message queue

    struct message msg;

    msg.mtype = 1; // set msg type

    sprintf(msg.mtext, "Hello World!"); // msg content

    msgsnd(msgid, &msg, sizeof(msg.mtext), 0); //send msg

    printf("Sent message: %s\n", msg.mtext);

    return 0;
}