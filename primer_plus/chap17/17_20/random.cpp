// random.cpp
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
const int LIM = 20;
struct planet
{
    char name[LIM];
    double population;
    double g;
};
const char * file = "planets.dat";
inline void eatline() {while (std::cin.get() != '\n') continue;}

int main()
{
    using namespace std;
    planet pl;
    cout << fixed;
    
    // show initial contents
    fstream finout;
    finout.open(file, ios_base::in | ios_base::out | ios_base::binary);
    int ct = 0;
    if (finout.is_open())
    {
        finout.seekg(0);    // 移动输入文件指针到文件起始位置
        cout << "以下为当前文件" << file << "内容：\n";
        while (finout.read((char *) &pl, sizeof pl))
        {
            cout << ct++ << ": " << setw(LIM) << pl.name << ": "
                << setprecision(0) << setw(12) << pl.population
                << setprecision(2) << setw(6) << pl.g << endl;
        }
        if (finout.eof())
            finout.clear();
        else
        {
            cerr << "Error in reading " << file << ".\n";
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        cerr << "can't open " << file << ".\n";
        exit(EXIT_FAILURE);
    }

    // 改变记录
    cout << "输入想要修改的记录序号: ";
    long rec;
    cin >> rec;
    eatline();
    if (rec < 0 || rec >= ct)
    {
        cerr << "记录序号无效，退出。\n";
        exit(EXIT_FAILURE);
    }
    streampos place = rec * sizeof pl;
    finout.seekg(place);    // 移动输入指针到指定位置文件记录
    if (finout.fail())
    {
        cerr << "Error on attempted seek.\n";
        exit(EXIT_FAILURE);
    }

    finout.read((char*) &pl, sizeof pl);
    cout << "你的选择如下：" << endl;
    cout << rec << ": " << setw(LIM) << pl.name << ": "
        << setprecision(0) << setw(12) << pl.population 
        << setprecision(2) << setw(6) << pl.g << endl;
    if (finout.eof())
        finout.clear();     // 清除flag
    
    cout << "键入新的名字：";
    cin.get(pl.name, LIM);
    eatline();
    cout << "键入新的人口数量：";
    cin >> pl.population;
    cout << "键入新的重力加速度：";
    cin >> pl.g;
    finout.seekp(place);    // 返回到指定文件位置
    finout.write((char *)& pl, sizeof pl);  // 将finout对象作为输出流对象写入sizeof pl字节到文件
    if (finout.fail())
    {
        cerr << "Error on attempted write.\n";
        exit(EXIT_FAILURE);
    }

    // 显示修改后的内容
    ct = 0;
    finout.seekg(0);    // 移动文件指针到文件起始位置
    cout << "以下为修改后的文件" << file << "内容：\n";
    while (finout.read((char *) &pl, sizeof pl))
    {
        cout << ct++ << ": " << setw(LIM) << pl.name << ": "
            << setprecision(0) << setw(12) << pl.population
            << setprecision(2) << setw(6) << pl.g << endl;  
    }
    finout.close();
    cout << "结束！" << endl;

    return 0;
}