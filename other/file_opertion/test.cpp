#include <fstream>
#include <iostream>
#include <cstring>  // 用于 memset

#pragma pack(push, 1)  // 按 1 字节对齐，消除填充
struct Student {
    int id;
    double gpa;
    char name[32];
};
#pragma pack(pop)

int main() {
    // 写入二进制数据
    Student s1{101, 3.8, "Alice"};
    std::ofstream ofs("students.bin", std::ios::binary);
    if (!ofs) {
        std::cerr << "无法创建输出文件！" << std::endl;
        return 1;
    }
    ofs.write(reinterpret_cast<const char*>(&s1), sizeof(s1));
    ofs.close();  // 显式关闭确保写入完成

    // 读取二进制数据
    Student s2;
    memset(&s2, 0, sizeof(s2));  // 初始化结构体
    std::ifstream ifs("students.bin", std::ios::binary);
    if (!ifs) {
        std::cerr << "无法打开输入文件！" << std::endl;
        return 1;
    }
    ifs.seekg(0, std::ios::beg);  // 确保指针在文件头
    ifs.read(reinterpret_cast<char*>(&s2), sizeof(s2));

    if (ifs.gcount() != sizeof(s2)) {  // 检查实际读取的字节数
        std::cerr << "读取失败：文件不完整！" << std::endl;
        return 1;
    }

    std::cout << "ID: " << s2.id 
              << ", GPA: " << s2.gpa 
              << ", Name: " << s2.name << std::endl;
    return 0;
}