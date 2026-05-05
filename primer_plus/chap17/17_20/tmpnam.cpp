#include <cstdio>
#include <iostream>

int main()
{
    using namespace std;
    cout << "当前系统可以生成 " << TMP_MAX << " 个临时文件名,为 " <<L_tmpnam <<" 个字符." << endl;

    char pszName[L_tmpnam] = {'\0'};
    cout << "以下是系统生成的临时文件名称：" << endl;
    for (int i = 0; 10 > i; i++)
    {
        tmpnam(pszName);
        cout << pszName << endl;
    }

    return 0;
}