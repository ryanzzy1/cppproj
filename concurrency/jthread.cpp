// c++20 jthread to handle join in the destruct function
//
#include <iostream>
#include <thread>

int main(){
/*	std::thread t([] {});
	try{
	  throw 0;
	} catch (int x) {
	  t.join();
	  throw x;
	}
	t.join();*/
	std::jthread t([] {});
}
