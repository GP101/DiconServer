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

#define EXPAND_ENUM( x )			x
#define PRN_ENUM_TEXT_NARG(...)		EXPAND_ENUM( PRN_ENUM_TEXT_NARG_(__VA_ARGS__, PRN_ENUM_TEXT_RSEQ_N()) )
#define PRN_ENUM_TEXT_NARG_(...)	EXPAND_ENUM( PRN_ENUM_TEXT_ARG_N(__VA_ARGS__) )
#define PRN_ENUM_TEXT_ARG_N(_1, _2, _3, _4, _5, N, ...) N
#define PRN_ENUM_TEXT_RSEQ_N()		5, 4, 3, 2, 1, 0

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
	enum EResult
	{
		SUCCESS,
		ERROR_INVALID_PASSWORD,
		ERROR_DUPLICATE_CONNECTION
	};

	static const char* EResultToString(int e)
	{
		if (e == SUCCESS)
			return "SUCCESS";
		else if (e == ERROR_INVALID_PASSWORD)
			return "ERROR_INVALID_PASSWORD";
		else if (e == ERROR_DUPLICATE_CONNECTION)
			return "ERROR_DUPLICATE_CONNECTION";
		return "";
	}

	std::string         m_login;
	std::string         m_password;
	int                 m_id;
	int                 m_age;
	int					m_result;
};

template <typename Archive>
void serialize(Archive& ar, KPacketLogin& a, const unsigned int version)
{
	ar & a.m_login;
	ar & a.m_password;
	ar & a.m_id;
	ar & a.m_age;
	ar & a.m_result;
}

void main()
{
	KPacketLogin        login;
	{
		login.m_login = "jintaeks\0hello";
		login.m_password = "hello world";
		login.m_id = 99;
		login.m_age = 48;
		login.m_result = KPacketLogin::ERROR_INVALID_PASSWORD;
	}

	std::cout << KPacketLogin::EResultToString(login.m_result) << std::endl;

	int i = PRN_ENUM_TEXT_NARG(A, B, HELLO);
	printf("%i\r\n", i);
	//  A,  B, HELLO,  5,  4, 3, 2, 1, 0
	// _1, _2,    _3, _4, _5, N, ...
	// N == 3
	i = PRN_ENUM_TEXT_NARG(A, B, HELLO, C);
	printf("%i\r\n", i);
	//  A,  B, HELLO,  C,  5, 4, 3, 2, 1, 0
	// _1, _2,    _3, _4, _5, N, ...
	// N == 4
}//main()
