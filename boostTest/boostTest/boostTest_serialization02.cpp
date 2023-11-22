#include <stdio.h>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <sstream>

#pragma pack(push,1)

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

#pragma pack(pop)

void main()
{
	KData d;
	d.m_cData = 1;
	d.m_fData = 2.3f;

	std::stringstream ss;
	cereal::BinaryOutputArchive oa(ss);
	oa << d;

	std::cout << ss.str() << std::endl;

	KData d2;
	cereal::BinaryInputArchive ia(ss);
	ia >> d2;
}
