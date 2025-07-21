#include <iostream>

class AppConfig {
    static AppConfig* instance;
    std::string theme = "Light";

    AppConfig() {} // 私有构造函数
public:
    static AppConfig* getInstance() {
        if (!instance) {
            instance = new AppConfig();
        }
        return instance;
    }
    
    std::string getTheme() const {
        return theme;
    }

    void setTheme(const std::string& newTheme) {
        theme = newTheme;
    }
};


AppConfig* AppConfig::instance = nullptr;

int main(){
    AppConfig* config = AppConfig::getInstance();
    std::cout << "Current theme: " << config->getTheme() << std::endl;

    config->setTheme("Dark");
    std::cout << "Updated theme: " << config->getTheme() << std::endl;

    return 0;
}
// 单例模式
void anyFunction() {
    AppConfig::getInstance()->setTheme("Dark");
    std::cout << "Theme set to: " << AppConfig::getInstance()->getTheme() << std::endl;
}