#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#define synchronized( statement_ ) \
    if (int iBlockScope = 0) {} \
    else \
    for (guard<std::mutex> mtxHelper(statement_); iBlockScope == 0; iBlockScope = 1)


const int g_numLoop = 100000;
int g_value = 0;
std::mutex  g_m;

template<typename T>
struct guard
{
    T& m;
    guard(T& m_) : m(m_) {
        m.lock();
    }
    ~guard() {
        m.unlock();
    }
};

void WorkerThread1(int param)
{
    for (int i = 0; i < g_numLoop; ++i) {
        //guard<std::mutex> guard(g_m);
        synchronized(g_m)
        {
            //g_m.lock();
            g_value += 1;
            //g_m.unlock();
        }
    }
}

void WorkerThread2(int param)
{
    for (int i = 0; i < g_numLoop; ++i) {
        //guard<std::mutex> guard(g_m);
        synchronized(g_m)
        {
            //g_m.lock();
            g_value -= 1;
            //g_m.unlock();
        }
    }
}

int main()
{
    std::thread worker1(WorkerThread1, 1);
    std::thread worker2(WorkerThread2, 11);

    worker1.join();
    worker2.join();
    std::cout << g_value << std::endl;
}
