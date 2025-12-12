/**
 *  1. 相关类行为有异，“策略”模式提供了一种用多个行为中的一个来配置一个类的方法；
 * 
 *  2. 需要使用一个算法的不同变体；
 * 
 *  3. 算法使用客户不应该知道的数据。可使用策略模式以避免暴露复杂的、与算法相关的数据结构；
 * 
 *  4. 一个类定义了多种行为，并且这些行为在这个类的操作中以多个条件语句的形式出现。
 *      将相关的条件分支移入它们各自的Strategy类中，
 * 
 */

#include <iostream>
using namespace std;

typedef enum StrategyType
{
    StrategyA,
    StrategyB,
    StrategyC
}STRATEGYTYPE;

class Strategy
{
public:
    virtual void AlgorithmInterface() = 0;

    virtual ~Strategy() = 0;
};

Strategy::~Strategy(){}

class ccStrategyA : public Strategy
{
public:
    void AlgorithmInterface()
    {
        cout << "It's ccStrategyA implementation." << endl;
    }

    ~ccStrategyA(){}
};

class ccStrategyB : public Strategy
{
public:
    void AlgorithmInterface()
    {
        cout << "It's ccStrategyB implementation." << endl;
    }

    ~ccStrategyB(){}
};

class ccStrategyC : public Strategy
{
public:
    void AlgorithmInterface()
    {
        cout << "It's ccStrategyC implementation." << endl;
    }

    ~ccStrategyC(){}
};

class Context
{
public:
    Context(STRATEGYTYPE strategyType)
    {
        switch (strategyType)
        {
        case StrategyA:
            pStrategy = new ccStrategyA;
            break;

        case StrategyB:
            pStrategy = new ccStrategyB;
            break;

        case StrategyC: 
            pStrategy = new ccStrategyC;
            break;

        default:
            break;
        }
    }

    ~Context()
    {
        if(pStrategy) delete pStrategy;
    }

    void ContextInterface()
    {
        if(pStrategy){
            pStrategy->AlgorithmInterface();
        }
    }

private:
    Strategy *pStrategy;
};

int main()
{
    Context *pContext = new Context(StrategyA);

    pContext->ContextInterface();

    if (pContext) delete pContext;

    return 0;

}