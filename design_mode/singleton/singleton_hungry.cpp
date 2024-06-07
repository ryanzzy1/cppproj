
// hungry mode: 单例类定义的时候就行了实例化

class singleton{
private:
    singleton(){}
    static singleton *p;
public:
    static singleton *instance();
};

singleton *singleton::p = new singleton();
singleton* singleton::instance()
{
    return p;
}
