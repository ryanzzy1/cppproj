// fileio.cpp
#include <iostream>
#include <fstream>
#include <string>

int main()
{
    using namespace std;
    string filename;

    cout << "输入文件名: \n";
    cin >> filename;

    // create output stream object for the new file and call it fout
    ofstream fout(filename.c_str());

    fout << "For your eyes only!\n";
    cout << "输入你的密钥："<< endl;
    float secret;
    cin >> secret;

    fout << "Your secret number is " << secret << endl;
    fout.close();

    // 为上述新文件创建一个输入流对象，并通过fin对象调用它
    ifstream fin(filename.c_str());
    cout << "以下为文件的输入内容：" << filename << endl;
    char ch;
    while(fin.get(ch))
        cout << ch;
    cout << "从文件读取内容完毕！" << endl;

    fin.close();

    return 0;
}