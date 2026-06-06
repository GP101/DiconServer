#include <iostream>
#include <thread>
#include <windows.h>

std::thread*    pthread1;

void Test(float ratio)
{
    auto a = [=](float value) // unsafe when captured by reference 
    {
        std::cout << ratio << std::endl;
        Sleep(2000);
        std::cout << ratio << std::endl;
        return value * ratio;
    };

    pthread1 = new std::thread(a, 200.f);
    Sleep(1000);
}

void main()
{
    Test(0.1f);

    pthread1->join();
    delete pthread1;
}