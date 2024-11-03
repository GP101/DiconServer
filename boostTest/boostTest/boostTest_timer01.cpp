#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    const auto start{ std::chrono::steady_clock::now() };
    std::this_thread::sleep_for( std::chrono::seconds(1) );
    const auto end{ std::chrono::steady_clock::now() };
    const std::chrono::duration<double> elapsed_seconds{ end - start };

    std::cout << elapsed_seconds.count() << "s\n"; // Before C++20
}