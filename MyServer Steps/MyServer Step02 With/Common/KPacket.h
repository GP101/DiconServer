#pragma once
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <memory>
#include <sstream>
#include "NetUtil.h"
#include "KGen.h"
#include "KPool.h"

enum EPacketIds
{
    //// Packet Id
    ECLGS_HEART_BIT = 0,
    EGSCL_ACCEPT_CONNECTION_NOT, // server -> client connection
    EDB_ON_START_REQ, // for onetime Db processing when server initialization
    EDB_ON_START_ACK, 
    EDB_PERIODIC_UPDATE, // periodic Db update
    EGSCN_DIRECT_TO_CENTER_DB, 
    EGSCN_DIRECT_TO_USER, // reserved for remote user and remote server
    EGSCN_DIRECT_TO_CENTER, 
    EGSCL_KICK_USER_NOT, 
    ECNGS_CONNECTION_LOST_NOT, 
    EGSCN_DIRECT_TO_RSERVER, 

    //// general Packets
    EGSCN_VERIFY_ACCOUNT_REQ, 
    EGSCN_VERIFY_ACCOUNT_ACK, 

    ECLGS_VERIFY_ACCOUNT_REQ, 
    ECLGS_VERIFY_ACCOUNT_ACK, 

    EGSCN_CHECK_UUID_REQ,  // game server to center server: ask Uid
    EGSCN_CHECK_UUID_ACK, 

    EGSCN_USER_AUTHENTICATE_REQ,  // game server to center server: ask authentification
    EGSCN_USER_AUTHENTICATE_ACK, 

    EGSCN_USER_DISCONNECT_NOT,  // game server to center server: notify user disconnection
    EDB_UPDATE_USER_INFO_REQ, 
    EDB_UPDATE_USER_INFO_FAILED_ACK, 

    ECLGS_USER_CHAT, 

    ECLGS_LEAVE_PLAYER_REQ, 
    ECLGS_LEAVE_PLAYER_NOT, 

    ECLGS_P2PINFO_NOT, // test for p2p connection

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
class KPacket : public KPool<KPacket>
{
public:
                        KPacket();
                        ~KPacket();
    KPacket&            operator=( const KPacket& right );

    template<typename T>
    void                SetData( unsigned short usPacketId, const T& data );

    template<typename Archive>
    void                serialize( Archive& ar, const unsigned int version )
                        {
                            ar & m_nSenderUid;
                            ar & m_usPacketId;
                            ar & m_buffer;
                        }//serialize()

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
    boost::archive::text_oarchive oa{ ss };
    oa << data_;

    std::string& str = ss.str();
    m_buffer.reserve( str.size() );
    m_buffer.assign( str.begin(), str.end() );
}//KPacket::SetData()


template<typename T>
bool BufferToPacket( IN std::vector<char>& buffer, OUT T& data )
{
    if( buffer.empty() == true )
        return false;

    // alternative (slow) implementation. jintaeks on 2017-08-24_20-08
    //std::stringstream ss;
    //std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<char>(ss));
    //boost::archive::text_iarchive ia{ ss };
    //ia >> data;

    vectorwrapbuf<char> databuf( buffer );
    std::istream is( &databuf );
    boost::archive::text_iarchive ia{ is };
    ia >> data;
    return true;
}//BufferToPacket()


template<typename T>
void BufferToPacket( IN std::stringstream& ss_, OUT T& packet_ )
{
    boost::archive::text_iarchive ia{ ss_ };
    ia >> packet_;
}//BufferToPacket()


template<typename T>
void PacketToBuffer( IN T& packet_, OUT std::vector<char>& buffer_ )
{
    std::stringstream   ss;
    boost::archive::text_oarchive oa{ ss };
    oa << packet_;

    // set [out] parameter
    std::string& str = ss.str();
    buffer_.reserve( str.size() );
    buffer_.assign( str.begin(), str.end() );
}//PacketToBuffer()


template<typename T>
void PacketToBuffer( IN T& packet_, OUT std::stringstream& ss_ )
{
    boost::archive::text_oarchive oa{ ss_ };
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
