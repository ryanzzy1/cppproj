/**
 *  文法规则：
 *  expression ::= direction action distance | composite
 *  composite ::= expression 'and' expression
 *  direction ::= 'up' | 'down' | 'left' | 'right'
 *  action ::= 'move' | 'walk'
 *  distance ::= an integer
 */
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cwchar>

using namespace std;

#define MAX_SIZE 256

#define SAFE_DELETE(p) if(p) {delete p; p = nullptr;}

const wchar_t *const DOWN = L"down";
const wchar_t *const UP = L"up";
const wchar_t *const LEFT = L"left";
const wchar_t *const RIGHT = L"right";

const wchar_t *const MOVE = L"move";
const wchar_t *const WALK = L"walk";

class AbstractNode {
public:
    virtual wchar_t *Interpret() = 0;
};

class AndNode : public AbstractNode {
private:
    AbstractNode *m_pLeft;
    AbstractNode *m_pRight;
public:
    AndNode(AbstractNode *left, AbstractNode *right) : m_pLeft(left), m_pRight(right) {}

    wchar_t *Interpret() override {
        // wchar_t *pResult = new wchar_t[MAX_SIZE];
        // memset(pResult, 0, MAX_SIZE * sizeof(wchar_t));
        wchar_t *pLeft = m_pLeft->Interpret();
        wchar_t *pRight = m_pRight->Interpret();
        std::wstring result = std::wstring(pLeft) + std::wstring(pRight);
        wchar_t *pResult = new wchar_t[result.size() + 1];
        std::wcscpy(pResult, result.c_str());

        SAFE_DELETE(pLeft);
        SAFE_DELETE(pRight);

        return pResult;
    }
};

class SentenceNode : public AbstractNode {
private:
    AbstractNode *m_pDirection;
    AbstractNode *m_pAction;
    AbstractNode *m_pDistance;
public:
    SentenceNode(AbstractNode *direction, AbstractNode *action, AbstractNode *distance)
        : m_pDirection(direction), m_pAction(action), m_pDistance(distance) {}
    
    wchar_t *Interpret() override {
        wchar_t *pDirection = m_pDirection->Interpret();
        wchar_t *pAction = m_pAction->Interpret();
        wchar_t *pDistance = m_pDistance->Interpret();
        std::wstring result = std::wstring(pDirection) + std::wstring(pAction) + std::wstring(pDistance);
        wchar_t *pResult = new wchar_t[result.size() + 1];
        std::wcscpy(pResult, result.c_str());

        SAFE_DELETE(pDirection);
        SAFE_DELETE(pAction);
        SAFE_DELETE(pDistance);

        return pResult;
    }

};

class DirectionNode : public AbstractNode {
private:
    wchar_t *m_pDirection;
public:
    DirectionNode(wchar_t *direction) : m_pDirection(direction) {}

    wchar_t *Interpret() override {

        std::wstring out;
    
        if (!wcscasecmp(m_pDirection, DOWN))
        {
            out = L"向下";
        }
        else if (!wcscasecmp(m_pDirection, UP))
        {
            out = L"向上";
        }
        else if (!wcscasecmp(m_pDirection, LEFT))
        {
            out = L"向左";
        }
        else if (!wcscasecmp(m_pDirection, RIGHT))
        {
            out = L"向右";
        } 
        else 
        {
            out = L"未知方向";
        }

        wchar_t *pResult = new wchar_t[out.size() + 1];
        std::wcscpy(pResult, out.c_str());

        SAFE_DELETE(m_pDirection);
        return pResult;
    }
};

class ActionNode : public AbstractNode {
private:
    wchar_t *m_pAction;
public:
    ActionNode(wchar_t *action) : m_pAction(action) {}

    wchar_t *Interpret() override {
        
        std::wstring out;        
        if (!wcscasecmp(m_pAction, MOVE))
        {
            out = L"移动";
        }
        else if (!wcscasecmp(m_pAction, WALK))
        {
            out = L"行走";
        }
        else 
        {
            out = L"未知动作";
        }

        wchar_t *pResult = new wchar_t[out.size() + 1];
        std::wcscpy(pResult, out.c_str());

        SAFE_DELETE(m_pAction);
        return pResult;
    }
};

class DistanceNode : public AbstractNode {
private:
    wchar_t *m_pDistance;
public:
    DistanceNode(wchar_t *distance) : m_pDistance(distance) {}

    wchar_t *Interpret() override {
        std::wstring out;
        
        out = std::wstring(m_pDistance) + L"米";

        wchar_t *pResult = new wchar_t[out.size() + 1];
        std::wcscpy(pResult, out.c_str());

        SAFE_DELETE(m_pDistance);
        return pResult;
    }
};

class InstructionHandler {
private:
    wchar_t *m_pInstruction;
    AbstractNode *m_pTree;
    void SplitInstruction(wchar_t **&instruction, int &size);
public:
    InstructionHandler(wchar_t *instruction) : m_pInstruction(instruction), m_pTree(nullptr) {}

    void Handle();
    void Output();
};

void InstructionHandler::Handle()
{
    AbstractNode *pLeft = nullptr;
    AbstractNode *pRight = nullptr;
    AbstractNode *pDirection = nullptr;
    AbstractNode *pAction = nullptr;
    AbstractNode *pDistance = nullptr;

    vector<AbstractNode *> node;

    wchar_t **InstructionArray = nullptr;
    int size = 0;
    SplitInstruction(InstructionArray, size);
    for (int i = 0; i < size; ++i)
    {
        if(!wcscasecmp(InstructionArray[i], L"and"))
        {
            wchar_t *pDirectionStr = InstructionArray[++i];
            pDirection = new DirectionNode(pDirectionStr);

            wchar_t *pActionStr = InstructionArray[++i];
            pAction = new ActionNode(pActionStr);

            wchar_t *pDistanceStr = InstructionArray[++i];
            pDistance = new DistanceNode(pDistanceStr);

            pRight = new SentenceNode(pDirection, pAction, pDistance);
            
            node.push_back(new AndNode(pLeft, pRight));
        }
        else
        {
            wchar_t *pDirectionStr = InstructionArray[i];
            pDirection = new DirectionNode(pDirectionStr);

            wchar_t *pActionStr = InstructionArray[++i];
            pAction = new ActionNode(pActionStr);

            wchar_t *pDistanceStr = InstructionArray[++i];
            pDistance = new DistanceNode(pDistanceStr);

            pLeft = new SentenceNode(pDirection, pAction, pDistance);
            node.push_back(pLeft);
        }
    }
    m_pTree = node[node.size() - 1];
}

void InstructionHandler::Output()
{
    wchar_t *pResult = m_pTree->Interpret();
   
    setlocale(LC_ALL, "");
    wcout << L"指令解释结果: " << pResult << endl;
    
    SAFE_DELETE(pResult);
    
}

void InstructionHandler::SplitInstruction(wchar_t **&instruction, int &size)
{
    instruction = new wchar_t *[10];
    memset(instruction, 0, 10 * sizeof(wchar_t *));

    for (int i = 0; i < 10; ++i)
    {
        instruction[i] = new wchar_t[10];
        memset(instruction[i], 0, 10 * sizeof(wchar_t));
    }

    size = 0;
    int n = 0;
    while (*m_pInstruction != L'\0')
    {
        if (*m_pInstruction == L' ')
        {
            size++;
            m_pInstruction++;
            n = 0;
            continue;
        }

        instruction[size][n++] = *m_pInstruction++;       
    }
    size++;
}

int main()
{
    wchar_t *pInstructionStr = L"up move 5 and down walk 10";

    InstructionHandler *pInstructionHandler = new InstructionHandler(pInstructionStr);
    pInstructionHandler->Handle();
    pInstructionHandler->Output();

    SAFE_DELETE(pInstructionHandler);

    return 0;
}