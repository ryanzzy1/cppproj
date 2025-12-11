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
};

class StrategyA : public Strategy
{
public:
    void AlgorithmInterface()
    {
        cout << "It's StrategyA implementation." << endl;
    }
};

class StrategyB : public Strategy
{
public:
    void AlgorithmInterface()
    {
        cout << "It's StrategyB implementation." << endl;
    }
};

class StrategyC : public Strategy
{
public:
    void AlgorithmInterface()
    {
        cout << "It's StrategyC implementation." << endl;
    }
};

class Context
{
public:
    Context(Strategy *pStrategyArg) : pStrategy(pStrategyArg){}

    void ContextInterface()
    {
        pStrategy->AlgorithmInterface();
    }

private:
    Strategy *pStrategy;
};

int main()
{
    Strategy *pStrategyA = new StrategyA;

    Strategy *pStrategyB = new StrategyB;

    Strategy *pStrategyC = new StrategyC;

    Context *pContextA = new Context(pStrategyA);

    Context *pContextB = new Context(pStrategyB);

    Context *pContextC = new Context(pStrategyC);

    pContextA->ContextInterface();

    pContextB->ContextInterface();

    pContextC->ContextInterface();

    if (pStrategyA) delete pStrategyA;

    return 0;

}