#include <sstream>
#include <memory>
#include <iostream>

struct KData;
typedef std::shared_ptr<KData> KDataPtr;
struct KData
{
    char    m_cData;
    float   m_fData;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& m_cData;
        ar& m_fData;
    }
};

class KOutArchive
{
public:
    explicit KOutArchive(std::stringstream& ss)
        : ss_(ss)
    {
    }

    KOutArchive& operator&(char& v)
    {
        ss_ << static_cast<int>(v) << ' ';
        return *this;
    }

    KOutArchive& operator&(float& v)
    {
        ss_ << v << ' ';
        return *this;
    }

    template <class T>
    KOutArchive& operator<<(T& obj)
    {
        obj.serialize(*this, 0);
        return *this;
    }

private:
    std::stringstream& ss_;
};

class KInArchive
{
public:
    explicit KInArchive(std::stringstream& ss)
        : ss_(ss)
    {
    }

    KInArchive& operator&(char& v)
    {
        int tmp = 0;
        ss_ >> tmp;
        v = static_cast<char>(tmp);
        return *this;
    }

    KInArchive& operator&(float& v)
    {
        ss_ >> v;
        return *this;
    }

    template <class T>
    KInArchive& operator>>(T& obj)
    {
        obj.serialize(*this, 0);
        return *this;
    }

private:
    std::stringstream& ss_;
};

int main()
{
    KData d;
    d.m_cData = 1;
    d.m_fData = 2.3f;

    std::stringstream ss;
    KOutArchive oa(ss);
    oa << d;

    std::cout << "serialized:\n" << ss.str() << "\n";

    KData d2{};
    KInArchive ia(ss);
    ia >> d2;

    std::cout << "deserialized:\n"
              << "  m_cData=" << static_cast<int>(d2.m_cData) << "\n"
              << "  m_fData=" << d2.m_fData << "\n";
    return 0;
}