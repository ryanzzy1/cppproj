// queuetp.h -- queue template with a nested class
#ifndef QUEUETP_H_
#define QUEUETP_H_ 

template <class Item>
class QueueTP
{
private:
    enum {Q_SIZE = 10};
    // Node is a nested class definition
    class Node
    {
    public:
        Item item;
        Node * next;
        Node(const Item & i) : item(i), next(0) {}
    };
    Node * front;
    Node * rear;
    int items;
    const int qsize;
    QueueTP(const QueueTP & q) : qsizee(0) {}
    QueueTP & operator=(const QueueTP & q) {return *this;}
public:
    QueueTP(int qs = Q_SIZE);
    ~QueueTP();
};




#endif 
