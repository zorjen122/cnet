#include "hook.h"
#include "iomanager.h"

#include <iostream>

void example()
{

    IOManager::getThis()->push([]()
                               {
    hook::Sleep(3);

    std::cout << "Hello!\n"; });

    IOManager::getThis()->push([]()
                               {
    hook::Sleep(3);

    std::cout << "World "; });
    std::cout << "example over!\n";
}

void example2()
{
    hook::Sleep(4);

    std::cout << "Hello, World!\n";
}

void test()
{

    hook::enableHook = true;

    IOManager iom(1, false);
    iom.push(&example);

    iom.run();
    getchar();
}

int main()
{
    test();
}
