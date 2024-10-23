#pragma once
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <memory>
#include <sstream>
#include "NetUtil.h"
#include "KGen.h"

enum EPacketIds
{
    //// Packet Id
    ECLGS_HEART_BIT = 0,
    EGSCL_ACCEPT_CONNECTION_NOT, // server -> client connection

    //// general Packets
    ECLGS_VERIFY_ACCOUNT_REQ, 
    ECLGS_VERIFY_ACCOUNT_ACK, 

    PACKETID_SENTINEL, // end of packets
};

/// @see    https://stackoverflow.com/questions/8815164/c-wrapping-vectorchar-with-istream
template<typename CharT, typename TraitsT = std::char_traits<CharT> >
class vectorwrapbuf : public std::basic_streambuf<CharT, TraitsT>
{
public:
                        vectorwrapbuf( IN std::vector<CharT>& vec )
                        {
                            setg( vec.data(), vec.data(), vec.data() + vec.size() );
                        }
};//class vectorwrapbuf


#pragma pack( push, 1 )

class KPacket;
typedef std::shared_ptr<KPacket>  KPacketPtr;
class KPacket// : public KPool<KPacket>
{
public:
                        KPacket();
                        ~KPacket();
    KPacket&            operator=( const KPacket& right );

    template<typename T>
    void                SetData( unsigned short usPacketId, const T& data );

public:
    LONGLONG            m_nSenderUid;
    unsigned short      m_usPacketId;
    std::vector<char>   m_buffer;
};//class KPacket

#pragma pack( pop )


template<typename T>
void KPacket::SetData( unsigned short usPacketId, const T& data_ )
{
    //m_nSenderUid = nSenderUid;
    m_usPacketId = usPacketId;

    std::stringstream   ss;
    cereal::BinaryOutputArchive oa(ss); // Create an output archive
    oa << data_;

    std::string& str = ss.str();
    m_buffer.reserve(str.size());
    m_buffer.assign(str.begin(), str.end());
}//KPacket::SetData()

template <typename Archive>
void serialize(Archive& ar, KPacket& a, const unsigned int version)
{
    ar& a.m_nSenderUid;
    ar& a.m_usPacketId;
    ar& a.m_buffer;
}//serialize()

template<typename T>
bool BufferToPacket( IN std::vector<char>& buffer, OUT T& data )
{
    if( buffer.empty() == true )
        return false;

    // alternative (slow) implementation. jintaeks on 2017-08-24_20-08
    //std::stringstream ss;
    //std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<char>(ss));
    //cereal::BinaryInputArchive ia(ss);
    //ia >> data;

    vectorwrapbuf<char> databuf(buffer);
    std::istream is(&databuf);
    cereal::BinaryInputArchive ia(is);
    ia >> data;
    return true;
}//BufferToPacket()


template<typename T>
void BufferToPacket( IN std::stringstream& ss_, OUT T& packet_ )
{
    cereal::BinaryInputArchive ia(ss_);
    ia >> packet_;
}//BufferToPacket()


template<typename T>
void PacketToBuffer( IN T& packet_, OUT std::vector<char>& buffer_ )
{
    std::stringstream   ss;
    cereal::BinaryOutputArchive oa(ss);
    oa << packet_;

    // set [out] parameter
    std::string& str = ss.str();
    buffer_.reserve(str.size());
    buffer_.assign(str.begin(), str.end());
}//PacketToBuffer()


template<typename T>
void PacketToBuffer( IN T& packet_, OUT std::stringstream& ss_ )
{
    cereal::BinaryOutputArchive oa(ss_);
    oa << packet_;
}//PacketToBuffer()

#define DECLARE_PACKET( id )      struct K##id

#define CASE_PACKET(id, packetType_) \
    case id: \
    { \
        packetType_ kPacket; \
        if( BufferToPacket( IN pkPacket_->m_buffer, OUT kPacket ) == false ) \
        { \
            BEGIN_LOG( cerr, L"deserialze failed." L#id L" - " L#packetType_ ); \
        } \
        else On_##id( pkPacket_->m_nSenderUid, kPacket); \
    } \
    break

#define DECLARE_ON_PACKET(id, packet)    void On_##id( LONGLONG, packet& )
