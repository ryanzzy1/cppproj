#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <locale>
#include <memory>
#include <stdexcept>

// polymorphism
struct B
{
    virtual ~B() = default;
    virtual void bar() {std::cout << "B::bar\n";}
};

struct D : B
{
    D() {std::cout << "D::D\n";}
    ~D() {std::cout << "D::~D\n";}

    void bar() override {std::cout << "D::bar\n";}
};

std::unique_ptr<D> pass_through(std::unique_ptr<D> p)
{
    p->bar();
    return p;
}

void close_file(std::FILE* fp)
{
    std::fclose(fp);
}

struct List
{
    struct Node
    {
        int data;
        std::unique_ptr<Node> next;
    };

    std::unique_ptr<Node> head;

    ~List()
    {
        while (head)
        {
            auto next = std::move(head->next);
            head = std::move(next);
        }
    }

    void push(int data)
    {
        head = std::unique_ptr<Node>(new Node{data, std::move(head)});
    }
};

int main()
{
    std::cout << "1) owner semantic show\n";
    {
        // create owner resource
        std::unique_ptr<D> p = std::make_unique<D>();

        std::unique_ptr<D> q = pass_through(std::move(p));

        assert(!p);
    }

    std::cout << "\n2) polymorphism demenstration: \n";
    {
        std::unique_ptr<B> p = std::make_unique<D>();

        p->bar();
    }

    std::cout << "\n 3) userdefined deleter demenstration\n";
    std::ofstream("demo.txt") << 'x';
    {
        using unique_file_t = std::unique_ptr<std::FILE, decltype(&close_file)>;
        unique_file_t fp(std::fopen("demo.txt", "r"), &close_file);
        if (fp)
            std::cout << char(std::fgetc(fp.get())) << '\n';
    }

    std::cout << "\n 4) usedefined lambda expression deleter and exception securiy demenstration\n";
    try{
        std::unique_ptr<D,void(*)(D*)> p(new D, [](D* ptr)
        {
            std::cout << "userdefined deleter destructor...\n";
            delete ptr;
        });
        
        throw std::runtime_error("");
    }
    catch (const std::exception&) {
        std::cout << "captured exception.\n";
    }

    std::cout << "\n 5) array form unique_ptr demenstration:\n";
    {
        std::unique_ptr<D[]> p(new D[3]);
    }

    std::cout << "\n 6) list demenstrator\n";
    {
        List wall;
        const int enough{1'000'000};
        for (int beer = 0; beer != enough; ++beer)
            wall.push(beer);

        std::cout.imbue(std::locale("en_US.UTF-8"));
        std::cout << "On the wall, there ara " << enough << " bottles beer.\n";
    }
}