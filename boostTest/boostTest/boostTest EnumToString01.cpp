#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

enum EPacketType
{
	ECLGS_VERIFY_ACCOUNT_REQ,
	ECLGS_LOGIN,
};

struct KPacketVerifyAccount
{
	std::string         m_login;
	int                 m_id;
};

template <typename Archive>
void serialize(Archive& ar, KPacketVerifyAccount& a, const unsigned int version)
{
	ar & a.m_login;
	ar & a.m_id;
}

struct KPacketLogin
{
	std::string         m_login;
	std::string         m_password;
	int                 m_id;
	int                 m_age;
};

template <typename Archive>
void serialize(Archive& ar, KPacketLogin& a, const unsigned int version)
{
	ar & a.m_login;
	ar & a.m_password;
	ar & a.m_id;
	ar & a.m_age;
}

void main()
{
	KPacketLogin        login;
	{
		login.m_login = "jintaeks";
		login.m_password = "hello world";
		login.m_id = 99;
		login.m_age = 48;
	}
}//main()
