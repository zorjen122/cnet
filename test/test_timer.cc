#include "timer.h"
#include "iomanager.h"
#include <iostream>
#include <assert.h>

void example()
{
    static thread_local int i = 0;
    std::cout << "i=" << i++ << "\n";
}

IOManager iom(1, false);

void test()
{
    std::cout << "[[1-TEST-start!]]\n";

    for (int i = 0; i < 3; ++i)
        iom.push(example);
}

void test2()
{
    std::cout << "[[2-TEST-start!]]\n";

    for (int i = 0; i < 3; ++i)
        iom.push(example);
}

void func()
{
    iom.addTimer(3000, &test, false);
    iom.addTimer(3000, &test, false);
}

void counter()
{
    int s = 1;
    int m = 0;
    while (1)
    {
        sleep(1);
        if (s == 60)
            m++, s = 0;
        std::cout << "\t\t\t\t\t\a[" << m << ":" << s++ << "]\n";
    }
}

int main()
{
    std::thread t(counter);
    Coroutine::GetThis();
    iom.push(func);
    iom.run();

    t.join();
    std::cout << "iom-run over!\n";
    getchar();
    return 0;
}