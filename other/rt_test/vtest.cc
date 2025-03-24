#include<vector>
#include<iostream>
#include<array>
#include<iterator>
#include<deque>
#include<set>
#include<map>
#include<string>


int main(){
    std::vector<int> v0;
    v0.reserve(3);
    v0.push_back(1);
    v0.push_back(2);
    v0.push_back(3);
    v0.push_back(4);
    v0.push_back(5);
    v0.push_back(6);
    v0.push_back(7);
    
    std::vector<int> v1 = std::move(v0);
    std::array<int, 3> v2;

    std::deque<int> v3(10, 1 );
    std::deque<int> v4(v3);

    std::set<int> v5;
    std::set<int> available;
    int y;
    std::set<int>::iterator it = available.lower_bound(y);

   std::map<std::string, int> mp;
   mp.insert(std::pair<std::string, int>("a", 1));
   mp.insert(std::pair<std::string, int>("b", 2));
   mp.insert(std::pair<std::string, int>("c", 3));

   using si = std::map<std::string, int>::iterator;
   for (si it = mp.begin(); it != mp.end(); it++){
       std::cout << "map: first " << it->first << " second " << it->second << std::endl;
    }

        
    #define DEBUG
    #ifdef DEBUG

    if(it == available.end()){
        std::cout << "no available" << std::endl;
    }else{
        std::cout << "available" << std::endl;
        available.erase(it);
    }

    for (std::map<std::string, int>::iterator iter = mp.begin(); iter != mp.end(); iter++){
        std::cout << "map: first " << iter->first << " second " << iter->second << std::endl;
    }
    std::map<std::string, int>::iterator it1 = mp.find("a");
    std::map<std::string,int>::iterator it2 = mp.find("b");
    
    if (mp.empty()){
        std::cout << "mp is empty" << std::endl;
    }else{
        std::cout << "mp size: " << mp.size() << std::endl;
        mp.find("a");
        mp.erase(it1, it2);
        std::cout << "after erase ite mp size: " << mp.size() << std::endl;
        std::cout << "cleared." << std::endl;
        mp.clear();
    }
    
    v4.push_front(1);
    v4.push_back(1);
    std::cout << "front: " << v4.front() << std::endl;
    std::cout << "back: " << v4.back() << std::endl;
    v4.pop_front();
    v4.pop_back();
    std::cout << "pop front: " << v4.front() << std::endl;
    std::cout << "pop back: " << v4.back() << std::endl;
    std::cout << "deque size: " << v4.size() << std::endl;
    std::cout << "deque empty:" << v4.empty() << std::endl;
    v4.clear();

    std::cout << v1.data() <<" " << v1.size() << " " << v1.capacity()<< std::endl;
    // v1.shrink_to_fit();
    std::cout << v1[0] << " " << &v1[0] << std::endl;
    std::cout << std::distance(v1.begin(), v1.end()) << std::endl;

    std::vector<int> v = {0, 1, 2, 3, 4, 5};
    auto n = v.size();
    std::cout << "loop range:" << n << '\n';
    for (auto i : v)
        std::cout << "c++20 range based loop:" << i << '\n';
    #endif

    #ifndef DEBUG
    for(std::vector<int>::iterator iter = v1.begin(); iter != v1.end(); iter++){
        std::cout << *iter << " " << *iter << std::endl;
    }
    #endif
    return 0;

}