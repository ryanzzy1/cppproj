// 共享指针

#include <memory>
#include <iostream>

class ShareData {
public:
    std::string message = "Hello, World!";
};  

class Sender {
    std::shared_ptr<ShareData> data;
public:
    Sender(std::shared_ptr<ShareData> d) : data(d) {}
    void send() {
        data->message += " from Sender";
        std::cout << "Sender: " << data->message << std::endl;
    }
};

class Receiver {
    std::shared_ptr<ShareData> data;
public:
    Receiver(std::shared_ptr<ShareData> d) : data(d) {}
    void receive() {
        std::cout << "Receiver: " << data->message << std::endl;
    }
};

int main() {
    std::shared_ptr<ShareData> sharedData = std::make_shared<ShareData>();

    Sender sender(sharedData);
    Receiver receiver(sharedData);

    sender.send();
    receiver.receive();

    return 0;
}   