#include <iostream>

// 抽象类：文件处理流程
class FileProcessor {
public:
    // 模板方法：定义文件处理的流程骨架
    void processFile(const std::string& fileName) {
        openFile(fileName);
        readFile();
        processData();
        writeFile();
        closeFile();
    }

    // 便捷方法：不带文件名的调用，使用默认文件名
    void process() {
        processFile("default_file");
    }

    virtual ~FileProcessor() = default;

protected:
    virtual void openFile(const std::string& fileName) {
        std::cout << "Opening file: " << fileName << std::endl;
    }

    virtual void readFile() = 0;      // 原语操作，由子类实现
    virtual void processData() {      // 提供默认实现，子类可覆盖
        std::cout << "Processing data" << std::endl;
    }
    virtual void writeFile() {        // 提供默认实现，子类可覆盖
        std::cout << "Writing file" << std::endl;
    }

    virtual void closeFile() {
        std::cout << "Closing file" << std::endl;
    }
};

class TextFileProcessor : public FileProcessor {
protected:
    void readFile() override {
        std::cout << "Reading text file" << std::endl;
    }
};

class ImageFileProcessor : public FileProcessor {
protected:
    void readFile() override {
        std::cout << "Reading image file" << std::endl;
    }
};

class AudioFileProcessor : public FileProcessor {
protected:
    void readFile() override {
        std::cout << "Reading audio file" << std::endl;
    }
};

class VideoFileProcessor : public FileProcessor {
protected:
    void readFile() override {
        std::cout << "Reading video file" << std::endl;
    }
};

class ExcelFileProcessor : public FileProcessor {
protected:
    void readFile() override {
        std::cout << "Reading excel file" << std::endl;
    }
};

class WordFileProcessor : public FileProcessor {
protected:
    void readFile() override {
        std::cout << "Reading word file" << std::endl;
    }
};
int main() {
    // 示例1：处理文本文件
    TextFileProcessor textProcessor;
    std::cout << "=== 处理文本文件 ===" << std::endl;
    textProcessor.process();

    std::cout << std::endl;

    // 示例2：处理图片文件
    ImageFileProcessor imageProcessor;
    std::cout << "=== 处理图片文件 ===" << std::endl;
    imageProcessor.process();

    std::cout << std::endl;

    // 示例3：处理音频文件
    AudioFileProcessor audioProcessor;
    std::cout << "=== 处理音频文件 ===" << std::endl;
    audioProcessor.process();

    std::cout << std::endl;

    // 示例4：处理视频文件
    VideoFileProcessor videoProcessor;
    std::cout << "=== 处理视频文件 ===" << std::endl;
    videoProcessor.process();

    std::cout << std::endl;

    // 示例5：处理Excel文件
    ExcelFileProcessor excelProcessor;
    std::cout << "=== 处理Excel文件 ===" << std::endl;
    excelProcessor.process();

    std::cout << std::endl;

    // 示例6：处理Word文件
    WordFileProcessor wordProcessor;
    std::cout << "=== 处理Word文件 ===" << std::endl;
    wordProcessor.process();

    return 0;
}

/*
class CaffeineBeverage {
public:
    // 模板方法：定义算法骨架
    void prepareRecipe() {
        boilWater();
        brew();              // 由子类实现
        pourInCup();
        if (customerWantsCondiments()) {  // Hook，子类可选择重写
            addCondiments();  // 由子类实现
        }
    }

    virtual ~CaffeineBeverage() = default;

protected:
    void boilWater() {
        std::cout << "Boiling water\n";
    }

    void pourInCup() {
        std::cout << "Pouring into cup\n";
    }

    // 原语操作：子类必须实现
    virtual void brew() = 0;
    virtual void addCondiments() = 0;

    // Hook：提供缺省实现，子类可选择覆盖
    virtual bool customerWantsCondiments() {
        return true;
    }
};

// 具体类：茶
class Tea : public CaffeineBeverage {
protected:
    void brew() override {
        std::cout << "Steeping the tea\n";
    }

    void addCondiments() override {
        std::cout << "Adding lemon\n";
    }
};

// 具体类：咖啡
class Coffee : public CaffeineBeverage {
protected:
    void brew() override {
        std::cout << "Dripping coffee through filter\n";
    }

    void addCondiments() override {
        std::cout << "Adding sugar and milk\n";
    }

    // 覆盖 Hook：不加调料
    bool customerWantsCondiments() override {
        return false;  // 比如某些场景下不加糖和牛奶
    }
};

int main() {
    std::cout << "Prepare tea:\n";
    Tea tea;
    tea.prepareRecipe();

    std::cout << "\nPrepare coffee:\n";
    Coffee coffee;
    coffee.prepareRecipe();

    return 0;
}*/