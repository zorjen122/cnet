#include <iostream>
#include "../../src/coroutine.h"
#include "../../src/scheduler.h"
#include <ctime>
#include <chrono>
void foo()
{
    for (unsigned long long _ = 0; _ < 100000; ++_)
    {
        Scheduler::getThis()->push(Coroutine::GetThis());
        Coroutine::GetThis()->yield();
        std::cout << "run!\n";
    }
}

void data1()
{
    Scheduler sch(10);

    for (int i = 0; i < 100; ++i)
    {
        Coroutine::GetThis();
        Coroutine::SharedPtr co(new Coroutine(&foo));
        sch.push(co);
    }
    std::cout << "[start]\n";
    std::clock_t start = std::clock();
    sch.run();
    std::clock_t end = std::clock();
    double s = ((double)end - (double)start) / CLOCKS_PER_SEC;
    std::cout << "[second]: " << s << "\n";
    sch.stop();
}
void goo()
{
    for (unsigned long long _ = 0; _ < 5000000; ++_)
        Coroutine::GetThis()->yield();
}
void data2()
{

    Coroutine::GetThis();
    Coroutine::SharedPtr co(new Coroutine(&goo));
    std::cout << "[start....]\n";
    auto start = std::chrono::high_resolution_clock::now();

    for (unsigned long long _ = 0; _ < 5000000; ++_)
        co->resume();
    auto end = std::chrono::high_resolution_clock::now();

    // 计算并输出总的执行时间
    std::chrono::duration<double, std::milli> duration = end - start;
    // double s = ((double)end - (double)start) / CLOCKS_PER_SEC;
    std::cout << "[ms]: " << duration.count() << "\n";
}

int main()
{
    data2();
}