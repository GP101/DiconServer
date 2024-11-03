#include <chrono>
#include <iostream>
#include <thread>

class KTimer
{
private:
    std::chrono::steady_clock::time_point _start;
    std::chrono::steady_clock::time_point _end;
    std::chrono::duration<double> _elapsed_seconds;

public:
    KTimer() {
        restart();
    }

    void restart() {
        _start = std::chrono::steady_clock::now();
        _end = _start;
        _elapsed_seconds = _end - _start;
    }

    double elapsed() {
        _end = std::chrono::steady_clock::now();
        _elapsed_seconds = _end - _start;
        _start = _end;
        return _elapsed_seconds.count();
    }
};

int main()
{
    KTimer t0;
    int i = 0;
    while (i < 5)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << t0.elapsed() << std::endl;
        i += 1;
    }
}
