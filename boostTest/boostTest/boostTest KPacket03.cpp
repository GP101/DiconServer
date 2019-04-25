#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>

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
	KPacketVerifyAccount    verifyAccount;
	{
		verifyAccount.m_login = "jintaeks";
		verifyAccount.m_id = 48;
	}

	KPacketLogin        login;
	{
		login.m_login = "jintaeks\0hello";
		login.m_password = "hello world";
		login.m_id = 99;
		login.m_age = 48;
	}
}//main()
