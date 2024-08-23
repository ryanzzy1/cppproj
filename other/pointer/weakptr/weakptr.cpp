#include <iostream>
#include <memory>

std::weak_ptr<int> gw;

void observe()
{
    std::cout << "gw.use_count() == " << gw.use_count() << ";";

    if (std::shared_ptr<int> spt = gw.lock())
        std::cout << "*spt == " << *spt << "\n";
    else
        std::cout << "gw expired.\n";
}

int main()
{
    {
        auto sp = std::make_shared<int>(42);
        gw = sp;

        observe();
    }

    observe();
}