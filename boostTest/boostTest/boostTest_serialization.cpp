#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <sstream>

class animal
{
public:
    animal() = default;
    animal(int legs) : legs_{ legs } {}
    int legs() const { return legs_; }

private:
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version);

    int legs_;
};

template <typename Archive>
void animal::serialize(Archive &ar, const unsigned int version)
{
    ar & legs_;
}

int main()
{
    std::stringstream ss;

    boost::archive::text_oarchive oa{ ss };
    animal a{ 4 };
    oa << a;

    boost::archive::text_iarchive ia{ ss };
    animal a2;
    ia >> a2;
    std::cout << a2.legs() << '\n';
}