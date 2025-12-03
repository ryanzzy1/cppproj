#include <iostream>
using namespace std;

// gramar analysis
class CsyntaxParser
{
public:
    void SyntaxParser()
    {
        cout << "Syntax Parser" << endl;
    }
};

// middleware genenate subsystem
class CGenMidCode
{
public:
    void GenMidCode()
    {
        cout << "Generate middleware code" << endl;
    }
};

// assemble code subsystem
class CGenAssemblyCode
{
public:
    void GenAssemblyCode()
    {
        cout << "Generate assembly code" << endl;
    }
};

// link end exe subsystem
class CLinkSystem
{
public:
    void LinkSystem()
    {
        cout << "Link System" << endl;
    }
};

class Facade
{
public:
    void Compile()
    {
        CsyntaxParser syntaxParser;
        CGenMidCode genMidCode;
        CGenAssemblyCode genAssemblyCode;
        CLinkSystem linkSystem;
        syntaxParser.SyntaxParser();
        genMidCode.GenMidCode();
        genAssemblyCode.GenAssemblyCode();
        linkSystem.LinkSystem();
    }
};


// client
int main()
{
    Facade facade;
    facade.Compile();
    return 0;
}