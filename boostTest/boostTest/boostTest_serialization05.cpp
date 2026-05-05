#include <stdio.h>
#include <cereal/archives/binary.hpp>
#include <sstream>
#include <memory>

#pragma pack(push,1)
struct KData;
typedef std::shared_ptr<KData> KDataPtr;
struct KData
{
    char    m_cData;
    float   m_fData;
};
#pragma pack(pop)

template <typename Archive>
void serialize(Archive& ar, KData& a, const unsigned int version)
{
    ar& a.m_cData;
    ar& a.m_fData;
}

void main()
{
    KData d;
    d.m_cData = 1;
    d.m_fData = 2.3f;

    std::stringstream ss;
    cereal::BinaryOutputArchive oa(ss);
    oa << d;

    //std::cout << ss.str() << std::endl;

    KData d2;
    cereal::BinaryInputArchive ia(ss);
    ia >> d2;
}
