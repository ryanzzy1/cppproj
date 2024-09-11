#include <iostream>
#include <sstream>

using namespace std;

int main(int argc, char **argv) {
    uint32_t cycle = 1000; // default 1s

    std::string cycle_arg("--cycle");

    for (int i = 1; i < argc; i++) {
        if (cycle_arg == argv[i] && i + 1 < argc) {
            cout << "cycle_arg: "<< cycle_arg << "\n"
                << "argv[" << i << "] : " << argv[i] << "\n"
                << "cycle_argv == argv["<< i << "] : " << (cycle_arg == argv[i]) << "\n"
                << "cycle_arg == argv[" <<  i << "] && " << i  << " : "<< (cycle_arg == argv[i] && i) << "\n"
                << "cycle_arg == argv[" <<  i << "] && " << i <<" + 1 : " 
                << (cycle_arg == argv[i] && i + 1) << endl;
            i++;
            std::stringstream converter;
            converter << argv[i];
            converter >> cycle;
            cout << "argv["<< i << "] :" << argv[i] << "\ncycle : "<< cycle << endl;
        }
    }
}