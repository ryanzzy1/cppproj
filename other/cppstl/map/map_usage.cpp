#include <iostream>
#include <map>
#include <string>

int main()
{
    // create map container to store info of employees
    std::map<std::string, int> employees;
    
    // insert info
    employees["Alice"] = 30;
    employees["Bob"] = 20;
    employees["Charlie"] = 35;

    // search all employees and print info
    for (std::map<std::string, int>::iterator it = employees.begin(); it != employees.end(); ++it) {
        std::cout << it->first << " is " << it->second << " years old." << std::endl;
    }

    // other usage 
    // find one element
    if (employees.find("Bob") != employees.end()) {
        std::cout << "Bob is in the map." << std::endl;
    }

    // erase element
    if (employees.find("Charlie") != employees.end()) {
        std::cout << "Charlie is there. will be erased." << std::endl;
        // erase and add new
        employees.erase("Charlie");
        if (employees.find("Charlie") == employees.end()) {
            std::cout << "Charlie already erased.\n";
        }

        // check size of map
        size_t size = employees.size();
        std::cout << "employees' size: " << size << std::endl;

        // reinsert new element
        employees["Tina"] = 40;
        std::cout << "Now employees are: \n";
        for (std::map<std::string, int>::iterator it = employees.begin(); it != employees.end(); ++it) {
            std::cout << it->first<< " is " << it->second << " years old." << std::endl;
        }

    }
    


    return 0;
}



