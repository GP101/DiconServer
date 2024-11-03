#include <algorithm>
#include <array>
#include <iostream>
#include <iomanip>
#include <random>
#include <string_view>
#include <map>

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
    //std::array<unsigned int, 8> v;
    //std::generate(v.begin(), v.end(), dice);
    //print("dice: ", v);

// Seed with a real random value, if available
    std::random_device r;

    // Choose a random mean between 1 and 6
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(1, 6);
    int mean = uniform_dist(e1);
    std::cout << "Randomly-chosen mean: " << mean << '\n';

    // Generate a normal distribution around that mean
    std::seed_seq seed2{ r(), r(), r(), r(), r(), r(), r(), r() };
    std::mt19937 e2(seed2);
    std::normal_distribution<> normal_dist(mean, 2);

    std::map<int, int> hist;
    for (int n = 0; n != 10000; ++n)
        ++hist[std::round(normal_dist(e2))];

    std::cout << "Normal distribution around " << mean << ":\n"
        << std::fixed << std::setprecision(1);
    for (auto v : hist)
        std::cout << std::setw(2) << v.first << ' ' << std::string(v.second / 200, '*') << '\n';
    /**
    Randomly-chosen mean: 4
    Normal distribution around 4:
    -4
    -3
    -2
    -1
     0 *
     1 ***
     2 ******
     3 ********
     4 *********
     5 ********
     6 ******
     7 ***
     8 *
     9
    10
    11
    12
    */
}
