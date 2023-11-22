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

#pragma pack( push, 1 )
class KPacket;
typedef std::shared_ptr<KPacket>  KPacketPtr;
class KPacket
{
public:
	template <class T>
	void SetData(unsigned int nSenderUID, unsigned short usPacketId, const T& data);

	unsigned int        m_nSenderUid;
	unsigned short      m_usPacketId;
	std::vector<char>   m_buffer;
};//class KPacket
#pragma pack( pop )

template <class T>
void KPacket::SetData(unsigned int nSenderUID, unsigned short usPacketId, const T& data_)
{
	m_nSenderUid = nSenderUID;
	m_usPacketId = usPacketId;

	std::stringstream   ss;
	cereal::BinaryOutputArchive oa(ss);
	oa << data_;

	std::string& str = ss.str();
	m_buffer.reserve(str.size());
	m_buffer.assign(str.begin(), str.end());
}//KPacket::SetData()


template <typename Archive>
void serialize(Archive& ar, KPacket& a, const unsigned int version)
{
	ar & a.m_nSenderUid;
	ar & a.m_usPacketId;
	ar & a.m_buffer;
}//serialize()

template <typename T>
void BufferToPacket(IN std::stringstream& ss_, OUT T& packet_)
{
	cereal::BinaryInputArchive ia(ss_);
	ia >> packet_;
}//BufferToPacket()

template <typename T>
void BufferToPacket(IN std::vector<char>& buffer, OUT T& data)
{
	std::stringstream ss;
	std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<char>(ss));
	cereal::BinaryInputArchive ia(ss);
	ia >> data;
}//BufferToPacket()

template<typename T>
void PacketToBuffer(IN T& packet_, OUT std::stringstream& ss_)
{
	cereal::BinaryOutputArchive oa(ss_);
	oa << packet_;
}//PacketToBuffer()

template<typename T>
void PacketToBuffer(IN T& packet_, OUT std::vector<char>& buffer_)
{
	std::stringstream   ss;
	cereal::BinaryOutputArchive oa(ss);
	oa << packet_;

	// set [out] parameter
	std::string& str = ss.str();
	buffer_.reserve(str.size());
	buffer_.assign(str.begin(), str.end());
}//PacketToBuffer()

void main()
{
	KPacket     packet;

	KPacketLogin        login;
	{
		login.m_login = "jintaeks\0hello";
		login.m_password = "hello world";
		login.m_id = 99;
		login.m_age = 48;
		login.m_result = KPacketLogin::ERROR_INVALID_PASSWORD;
	}
	packet.SetData(0, ECLGS_LOGIN, login);

	// write to archive
	//
	std::stringstream ss;
	PacketToBuffer(IN packet, OUT ss);

	// read from archive
	//
	KPacket     packetNew;
	BufferToPacket(IN ss, OUT packetNew);

	if (packetNew.m_usPacketId == ECLGS_LOGIN)
	{
		KPacketLogin    loginNew;
		BufferToPacket(IN packetNew.m_buffer, OUT loginNew);
		std::cout << KPacketLogin::EResultToString(loginNew.m_result) << std::endl;
	}
}//main()
