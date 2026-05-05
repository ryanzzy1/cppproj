#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

const char * file = "guests.txt";
const int LIM = 20;
struct planet
{
    char name[LIM];
    double population;
    double g;
};


int main()
{
    using namespace std;
    char ch;
    planet pl;
    
    ifstream fin(file);
    if(fin.is_open())
    {
        cout << "文件内容如下："<< endl;
        while (fin.get(ch))
            cout << ch;
        cout << "从文件读取内容完毕！" << endl;
        fin.close();
    }
    /*
    if(!fin.is_open())
    {
        cerr << "Could not open " << file << endl;
        exit(EXIT_FAILURE);
    } else{
        cout << "文件内容如下："<< endl;
        while (fin.get(ch))
            cout << ch;
        cout << "从文件读取内容完毕！" << endl;
        fin.close();
    }*/

    // 添加新文件名
    ofstream fout(file, ios_base::out | ios::app);
    if (!fout.is_open())
    {
        cerr << "Can't open " << file << " file for output.\n";
        exit(EXIT_FAILURE);
    }

    cout << "ENTER Guest names (输入一个空行退出):" << endl;
    string name;
    while (getline(cin, name) && name.size() > 0)
    {
        fout << name << endl;
    }
    fout.close();

    // 显示文件内容
    fin.clear();
    fin.open(file);
    if(fin.is_open())
    {
        cout << "文件"<< file << "的内容如下:" << endl;
        while(fin.get(ch))
            cout << ch;
        cout << "从文件读取内容完毕！" << endl;
        fin.close();
    }

    ofstream fout2("palnets.dat", ios_base::out | ios_base::app);
    fout2 << pl.name << " " << pl.population << " " << pl.g << endl;
    fout2.close();

    cout << "演示完毕！" << endl;

    return 0;
}