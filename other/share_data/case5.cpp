// 依赖注入框架

#include <iostream>
#include <memory>

class DatabaseService {
public:
    std::string fetchData() {
        return "Data from Database";
    }
};

class ReportGenerator {
    // std::shared_ptr<DatabaseService> dbService; 
    DatabaseService& db;
public:
    ReportGenerator(DatabaseService& srv) : db(srv) {}
    void generate() {
        std::cout << "db.fetchData(): " << db.fetchData() << std::endl;
    }
};

int main()
{
    // 使用依赖注入
    DatabaseService dbService;
    ReportGenerator reportGen(dbService);
    
    reportGen.generate();

    return 0;
}