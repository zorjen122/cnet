#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

// 共享的计数器，用于跟踪线程切换的数量
// std::atomic<int> counter(0);

// 本地函数，模拟执行任务
void local_function()
{
    for (int i = 0; i < 5000000; ++i)
    {
        // counter.fetch_add(1, std::memory_order_relaxed);
        std::this_thread::yield(); // 主动让出 CPU，促使线程切换
    }
}

// 线程函数，模拟执行任务
void thread_function()
{
    for (int i = 0; i < 10000000; ++i)
    {
        // counter.fetch_add(1, std::memory_order_relaxed);
        std::this_thread::yield(); // 主动让出 CPU，促使线程切换
    }
}

int main()
{
    // 创建并启动两个线程
    // std::thread t1(local_function);  // 第一个线程，调用本地函数
    std::thread t2(thread_function); // 第二个线程，调用线程函数

    // 记录开始时间
    auto start = std::chrono::high_resolution_clock::now();

    // 等待两个线程完成
    // t1.join();
    t2.join();

    // 记录结束时间
    auto end = std::chrono::high_resolution_clock::now();

    // 计算并输出总的执行时间
    std::chrono::duration<double> duration = end - start;
    std::cout << "Execution time: " << duration.count() << " seconds\n";
    // std::cout << "Counter value (should be 20,000,000): " << counter.load() << "\n";

    return 0;
}
