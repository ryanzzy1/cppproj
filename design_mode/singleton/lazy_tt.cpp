
// lazy-mode singleton pattern
/*class Singleton {
public:
    static Singleton* getInstance() {
        if (instance == nullptr) {
            instance = new Singleton();
        }
        return instance;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    Singleton() = default;
    static Singleton* instance;
};

Singleton* Singleton::instance = nullptr;
*/
// Meyers' Singleton

#include <iostream>
#include <string>

class Logger {

public:

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void log(const std::string& message)
    {
        std::cout << "Log: " << message << std::endl;
        logCount++;
    }

    int getLogCount() const {return logCount;}

    Logger(const Logger&) = delete;

    Logger& operator=(const Logger&) = delete;

private:

    Logger() : logCount(0) 
    {
        std::cout << "Logger initialized." << std::endl;
    }

    ~Logger() {
        std::cout << "Logger destroyed. Total logs: " << logCount << std::endl;
    }

    int logCount;
};

int main() 
{
    Logger::getInstance().log("App created.");
    Logger::getInstance().log("Processing data...");


}

void processData() 
{
    Logger::getInstance().log("Data processed.");
}