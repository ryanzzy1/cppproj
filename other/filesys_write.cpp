// using <filesystem> library to write file
// since c++ 17 statdard
// g++ version >=11.1

#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::filesytem;

int main() {
    // create file path
    fs::path myFile("myfile1.txt");

    // open file and write text 
    fs::ofstream ofs(myFile);
    if (ofs.is_open()) {
        ofs << "This is a line of text in my file." << endl;
        ofs.close();
    } else {
        cout << "Error opening file." << endl;
    }

    return 0;
}