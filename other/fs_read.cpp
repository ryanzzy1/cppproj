// reading from an file
#include <iostream>
#include <fstream>

using namespace std;

int main(){
    // create ifstream object and open file
    ifstream myFile("myfile.txt");

    // chech if the file is open or not
    if (myFile.is_open()){
        string line;

        // read file by line
        while (getline(myFile, line)) {
            cout << line << endl;
        }

        // close file
        myFile.close();
    } else {
        cout << "Error opening file." << endl;
    }

    return 0;
}