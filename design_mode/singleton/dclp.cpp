#include <iostream>
#include <mutex>
#include <atomic>

class DatabaseConnection {
public:
    static DatabaseConnection* getInstance() {
        DatabaseConnection* tmp = instance.load(std::memory_order_acquire);
        if (tmp == nullptr) {
            std::lock_guard<std::mutex> lock(mutex);
            tmp = instance.load(std::memory_order_relaxed);
            if (tmp == nullptr) {
                tmp = new DatabaseConnection();
                instance.store(tmp, std::memory_order_release);
            }
        }
        return tmp;
    }

    void connect() {
        if (!connected) {
            std::cout << "Connecting to database..." << std::endl;
            connected = true;
        }
    }

    void executeQuery(const std::string& query) {
        if (connected) {
            std::cout << "Executing query: " << query << std::endl;
        } else {
            std::cout << "Database not connected!" << std::endl;
        }
    }

    static void destroy() {
        DatabaseConnection* tmp = instance.load(std::memory_order_acquire);
        if (tmp != nullptr) {
            delete tmp;
            instance.store(nullptr, std::memory_order_release);
        }
    }

    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

private:
    DatabaseConnection() : connected(false) {
        std::cout << "DatabaseConnection created." << std::endl;
    }

    ~DatabaseConnection() {
        std::cout << "DatabaseConnection destroyed." << std::endl;
    }

    bool connected;
    static std::atomic<DatabaseConnection*> instance;
    static std::mutex mutex;
};

std::atomic<DatabaseConnection*> DatabaseConnection::instance{nullptr};
std::mutex DatabaseConnection::mutex;

int main()
{
    DatabaseConnection::getInstance()->connect();
    DatabaseConnection::getInstance()->executeQuery("SELECT * FROM users;");
    DatabaseConnection::getInstance()->executeQuery("UPDATE users SET active = 1 WHERE id = 42;");
    
    DatabaseConnection::destroy();

    return 0;
}