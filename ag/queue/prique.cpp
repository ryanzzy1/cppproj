#include <iostream>
#include <queue>

struct compare {
    bool operator()(int a, int b) {
        return a > b;
    }
};

int main()
{
    std::priority_queue<int, std::vector<int>, compare> pq;
    pq.push(5);
    pq.push(1);
    pq.push(3);

    if (pq.empty()) {
        std::cout << "Priority queue is empty." << std::endl;
    } else {
        std::cout << "Priority queue is not empty." << std::endl;
    }

    while (!pq.empty()) {
        std::cout << pq.top() << " ";
        pq.pop();
    }
    std::cout << std::endl;

    return 0;
}