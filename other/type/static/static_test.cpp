#include <iostream>

using namespace std;

class point{
public:
    
    point(){
        cout << "point ctor" << endl;
        m_x = 3;
    }

    ~point(){
        cout << "point dtor" << endl;
    }

    void init(){
        
        // m_y = 0;
        cout << "m_y:" << m_y << endl;
    }

    static void output(){
        cout << "point output" << endl;
    }

    int get_x(){
        return m_x;
    }

    int get_y(){
        return m_y;
    }

private:
    int m_x;
    static int m_y;
};

int point::m_y = 12;    // only assignment here.

int main(){
    // int point::m_y = 12; // wrong operation
    cout << "sizeof(point):" << sizeof(point) << endl;

    point pt;

    pt.init();
    cout << "after ctor sizeof(point):" << sizeof(pt) << endl;
    // pt.output();
    point::output();
    cout << "x:" << pt.get_x() << " y:" << pt.get_y() << endl;    

    return 0;
}