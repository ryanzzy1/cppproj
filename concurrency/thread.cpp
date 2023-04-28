#include <iostream>
#include <thread>

void f() {std::cout << "helloworld!\n";}

int main(){

	std::thread t{f};
	t.join();
}
