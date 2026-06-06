#pragma once
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <memory>
#include <sstream>
#include "NetUtil.h"
#include "KGen.h"


#undef  _ENUM
#define _ENUM( id ) id,
enum EPacketId
{ 
    #include "Packet.inc" 
};

struct VectorStreamBuf : std::streambuf
{
    std::vector<char>& out;

    explicit VectorStreamBuf(std::vector<char>& v) : out(v) {}

    std::streamsize xsputn(const char* s, std::streamsize n) override
    {
        out.insert(out.end(), s, s + n);
        return n;
    }

    int overflow(int ch) override
    {
        if (ch != traits_type::eof())
            out.push_back(static_cast<char>(ch));
        return ch;
    }
};

struct VectorIStreamBuf : std::streambuf
{
    explicit VectorIStreamBuf(const std::vector<char>& v)
    {
        // cereal::BinaryInputArchive reads bytes; treat as raw memory.
        char* begin = const_cast<char*>(v.data());
        char* end = begin + v.size();
        setg(begin, begin, end);
    }
};


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

    const wchar_t*      GetIdWstr() const { return GetIdWstr( m_usPacketId ); }
    static const wchar_t*   
                        GetIdWstr( const unsigned short usPacketId );

public:
    LONGLONG            m_nSenderUid;
    unsigned short      m_usPacketId;
    std::vector<char>   m_buffer;

protected:
    static const wchar_t*   
                        ms_szPacketId[];
};//class KPacket

#pragma pack( pop )


template <class T>
void KPacket::SetData(unsigned short usPacketId, const T& data_)
{
    m_nSenderUid = 0;// nSenderUID;
    m_usPacketId = usPacketId;

    m_buffer.clear();

    VectorStreamBuf vb(m_buffer);
    std::ostream os(&vb);

    cereal::BinaryOutputArchive oa(os); // Create an output archive
    oa(data_);
}//KPacket::SetData()

template <typename Archive>
void serialize(Archive& ar, KPacket& a, const unsigned int version)
{
    ar& a.m_nSenderUid;
    ar& a.m_usPacketId;
    ar& a.m_buffer;
}//serialize()

template <typename T>
void BufferToPacket(IN std::stringstream& ss_, OUT T& packet_)
{
    cereal::BinaryInputArchive ia(ss_); // Create an input archive
    ia(packet_);
}//BufferToPacket()

template <typename T>
bool BufferToPacket(IN std::vector<char>& buffer, OUT T& data)
{
    if( buffer.empty() )
        return false;
    VectorIStreamBuf vb(buffer);
    std::istream is(&vb);
    cereal::BinaryInputArchive ia(is); // Create an input archive
    ia(data);
    return true;
}//BufferToPacket()

template<typename T>
void PacketToBuffer(IN T& packet_, OUT std::stringstream& ss_)
{
    cereal::BinaryOutputArchive oa(ss_); // Create an output archive
    oa(packet_);
}//PacketToBuffer()

template<typename T>
void PacketToBuffer(IN T& packet_, OUT std::vector<char>& buffer_)
{
    buffer_.clear();
    VectorStreamBuf vb(buffer_);
    std::ostream os(&vb);

    cereal::BinaryOutputArchive oa(os); // Create an output archive
    oa(packet_);
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
