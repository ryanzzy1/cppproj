#include <iostream>

class Command {
public:
    virtual ~Command() {}
    virtual void execute() const = 0;
};

class SimpleCommand : public Command {
public:
    SimpleCommand(const std::string& payload) : payload_(payload) {}
    
    void execute() const override {
        std::cout << "SimpleCommand: 执行简单命令，负载: " << payload_ << std::endl;
    }
private:
    std::string payload_;
};

class Receiver {
public:
    void DoSomething(const std::string& a) {
        std::cout << "Receiver: 正在处理 " << a << std::endl;
    }

    void DoSomethingElse(const std::string& b) {
        std::cout << "Receiver: 正在处理其他事情 " << b << std::endl;
    }
};

class ComplexCommand : public Command {
private:
    Receiver* receiver_;
    std::string a_;
    std::string b_;
public:
    ComplexCommand(Receiver* receiver, const std::string& a, const std::string& b)
        : receiver_(receiver), a_(a), b_(b) {}
    
    void execute() const override {
        std::cout << "ComplexCommand: 执行复杂命令，委托给接收者。" << std::endl;
        receiver_->DoSomething(a_);
        this->receiver_->DoSomethingElse(this->b_);
    }
};

class Invoker {
private:
    Command* onStart_;
    Command* onFinish_;
public:
    ~Invoker() {
        delete onStart_;
        delete onFinish_;
    }

    void setOnStart(Command* command) {
        this->onStart_ = command;
    }

    void setOnFinish(Command* command) {
        this->onFinish_ = command;
    }

    void DoSomethingImportant() {
        std::cout << "Invoker: 准备开始工作。" << std::endl;
        if (this->onStart_) {
            this->onStart_->execute();
        }

        std::cout << "Invoker: ...正在做重要的事情..." << std::endl;

        std::cout << "Invoker: 工作完成。" << std::endl;
        if (this->onFinish_) {
            this->onFinish_->execute();
        }
    }
};

int main()
{
    Invoker* invoker = new Invoker;
    invoker->setOnStart(new SimpleCommand("Say Hi!"));
    
    Receiver* receiver = new Receiver;
    invoker->setOnFinish(new ComplexCommand(receiver, "Send email", "Save report"));

    invoker->DoSomethingImportant();

    delete invoker;
    delete receiver;

    return 0;
}