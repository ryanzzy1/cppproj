#include <iostream>
#include <thread>

class A {
	public:
		A(int& x):x_(x) {}

		void operator() () const{
			for(int i = 0; i < 1000000; ++i) {
				call(x_);
			}
		}
	private:
		void call(int& x){};
	private:
		int& x_;

};

void f() {
int x = 0;
A a{x};
std::thread t{a};
t.detach();

}

int main(){
	std::thread t{f};
	t.join();
}
