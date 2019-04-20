#include <stdio.h>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <sstream>

#pragma pack(push,1)

struct KData
{
	char    m_cData;
	float   m_fData;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & m_cData;
		ar & m_fData;
	}
};

#pragma pack(pop)

void main()
{
	KData d;
	d.m_cData = 1;
	d.m_fData = 2.3f;

	std::stringstream ss;
	boost::archive::binary_oarchive oa(ss);
	oa << d;

	std::cout << ss.str() << std::endl;

	KData d2;
	boost::archive::binary_iarchive ia(ss);
	ia >> d2;

}
