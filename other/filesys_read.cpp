
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

int main() {
    // create file path
    fs::path myFile("myfile1.txt");

    // openfile and read content
    fs::filesystem ifs(myFie);
    if (ifs.is_open()) {
        string line;
        while (getline(ifs, line)) {
            cout << line << endl;
        }
        ifs.close();
    } else {
        cout << "Error opening file." << endl;
    }

    return 0;
}
