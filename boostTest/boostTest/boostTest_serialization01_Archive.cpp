#include <sstream>
#include <memory>

struct KData;
typedef std::shared_ptr<KData> KDataPtr;
struct KData
{
    char    m_cData;
    float   m_fData;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    }
};

class KOutArchive
{
public:
    explicit KOutArchive(std::stringstream& ss)
        : ss_(ss)
    {
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

int main()
{
    KData d;
    d.m_cData = 1;
    d.m_fData = 2.3f;

    std::stringstream ss;
	KOutArchive oa(ss);
    oa << d;
    return 0;
}