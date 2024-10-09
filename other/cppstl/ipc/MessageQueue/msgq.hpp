#ifndef MSGQ_HPP_
#define MSGQ_HPP_

struct message {
    long mtype;
    char mtext[100];
};

#endif