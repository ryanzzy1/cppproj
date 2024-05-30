#include <cassert>
#include <iostream>
#include <thread>
#include <utility>
/*
void f(std::unique_ptr<int> p) {std::cout << *p << std::endl;}

int main(){
  std::unique_ptr<int>p{new int(42)};
  std::thread t{f, std::move(p)};
  t.join();
}
*/
/*
class A {
  public:
    void f(int i) {std::cout << i << std::endl;}
    
};

int main(){
  A a;
  std::thread t1{&A::f, &a, 42};
  std::thread t2{&A::f, a, 42};
  t1.join();
  t2.join();
}*/

void f() {}
void g() {}

int main() {
  std::thread a{f};
  std::thread b = std::move(a);
  assert(!a.joinable());
  assert(b.joinable());

  a = std::thread{g};
  assert(a.joinable());
  assert(!b.joinable());
  a.join();


}
