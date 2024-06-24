// creating and writing text to file

#include <iostream>
#include <fstream>

using namespace std;

int main(){
    // create ofstream object and open file
    ofstream myFile("myfile.txt");

    // check if file open or not
    if(myFile.is_open()){
        // write txt in the file
        myFile << "This is a line of text in my file." << endl;

        // close file
        myFile.close();
        cout << "File written successfully." << endl;
    } else {
        cout << "Error opening file." << endl;
    }

    return 0;
}