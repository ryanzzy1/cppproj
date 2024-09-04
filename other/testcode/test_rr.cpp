  #include <cstdio>
  #include <thread>
  #include <stdlib.h>

  void inc(int& x, int id) {
    id = id + 1; //这一行有点多余，主要是为了展示rr的reverse-continue 功能
    if (x == 2 && id == 3) {
      abort();
    }
    ++x;
    printf("x=%d\n", x);
  }

  int main () {
    int x = 0;
    std::thread t1(inc, std::ref(x), 1);
    std::thread t2(inc, std::ref(x), 2);
    std::thread t3(inc, std::ref(x), 3);
    std::thread t4(inc, std::ref(x), 4);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
  }