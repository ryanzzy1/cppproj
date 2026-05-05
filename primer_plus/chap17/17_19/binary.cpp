// binary.cpp
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>

inline void eatline() {while(std::cin.get() != '\n') continue;}
struct planet
{
    char name[20];
    double population;
    double g;
};

const char * file = "planets.dat";

int main()
{
    using namespace std;
    planet pl;
    cout << fixed << right;

    // 输出初始化内容
    ifstream fin;
    fin.open(file, ios_base::in | ios_base::binary);
    if (fin.is_open())
    {
        cout << "Here are the contents of the " << file << " file:" << endl;
        while(fin.read((char *) &pl, sizeof pl))
        {
            cout << setw(20) << pl.name << ": "
                << setprecision(0) << setw(12) << pl.population
                << setprecision(2) << setw(6) << pl.g << endl;
        }
        fin.close();
    }

    // add new data

    ofstream fout(file,
             ios_base::out | ios_base::app | ios_base::binary);
    if(!fout.is_open())
    {
        cerr << "Can't open " << file << " file for output.\n";
        exit(EXIT_FAILURE);
    }

    cout << "输入planet 名字:(空行退出) \n";
    cin.get(pl.name, 20);
    while(pl.name[0] != '\0')
    {
        eatline();
        cout << "输入planet 人口数量：\n";
        cin >> pl.population;
        cout << "输入planet 重力加速度：\n";
        cin >> pl.g;
        eatline();
        fout.write((char *) &pl, sizeof pl);
        cout << "输入planet 名字:(空行退出) \n";
        cin.get(pl.name, 20);

    }
    fout.close();

    // 显示修改的名字
    fin.clear();
    fin.open(file, ios_base::in | ios_base::binary);
    if (fin.is_open())
    {
        cout << "文件 " << file << " 的新内容如下：" << endl;
        while(fin.read((char *) &pl, sizeof pl))
        {
            cout << setw(20) << pl.name << ": "
                 << setprecision(0) << setw(12) << pl.population
                 << setprecision(2) << setw(6) << pl.g << endl;
        }
        fin.close();
    }

    cout << "结束！" << endl;

    return 0;
} 