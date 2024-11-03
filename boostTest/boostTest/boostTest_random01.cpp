#include <algorithm>
#include <array>
#include <iostream>
#include <random>

auto dice()
{
    static std::uniform_int<unsigned int> distr{ 1, UINT_MAX };
    static std::random_device device;
    static std::mt19937 engine{ device() };
    return distr(engine);
}

template<typename T>
void print(std::string comment, const T& v)
{
    std::cout << comment;
    for (unsigned int i : v)
        std::cout << i << ' ';
    std::cout << '\n';
}

int main()
{
    std::array<unsigned int, 8> v;
    std::generate(v.begin(), v.end(), dice);
    print("dice: ", v);
}
