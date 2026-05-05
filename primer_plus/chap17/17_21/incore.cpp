// strout.cpp
#include <iostream>
#include <sstream>
#include <string>

int main()
{
    using namespace std;
    ostringstream outstr;

    string hdisk;
    cout << "硬盘名称是什么？\n";
    getline(cin, hdisk);
    int cap;
    cout << "硬盘容量是多少GB？\n";
    cin >> cap;
    outstr << "The hard disk " << hdisk << "容量是：" << cap << "GB.\n";

    string result = outstr.str();   // 保存result字符串
    cout << result;    // 输出result字符串

    return 0;

}