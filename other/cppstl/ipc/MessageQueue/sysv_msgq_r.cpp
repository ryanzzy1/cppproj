#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "msgq.hpp"

// recied message

int main()
{
    key_t key = ftok("queuefile", 65);  // create unique key

    int msgid = msgget(key, 0666 | IPC_CREAT); // create message queue

    struct message msg;

    msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0);

    printf("Received message: %s\n", msg.mtext);

    msgctl(msgid, IPC_RMID, NULL);   // delete message queue    
    
    return 0;
}
