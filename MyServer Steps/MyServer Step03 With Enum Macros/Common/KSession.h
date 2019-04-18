#pragma once
#include "KGen.h"
#include "KWorkQueue.h"
#include "KThread.h"
#include "KSessionSocket.h"


class KSession
    : public KServerObject
{
    friend class KSessionSocket;
    friend class KIocpWorkerThread;

public:
    enum
    { 
        SECURE_KEY_WAITING_TIME = 3000, // waiting time to get authentification key before connection.
    };

    CONSTRUCTOR         KSession();
    virtual             ~KSession();

    virtual bool        Initialize( DWORD dwParam_ );
    virtual void        Finalize();

    virtual void        Update(DWORD dwElapsedTime_); // process packet queue, session closing and heart bit.
    virtual void        OnPacket( IN KPacket* pkPacket ) = 0;

    void                QueueingPacket( const KPacketPtr& spPacket )
                        {
                            m_workQueue.QueueingPacket( spPacket );
                        }

    void                ReserveDestroy() { m_bDestroyReserved = true; }

    /// check invalid reference counter
    bool                IsRefCountLessThan( int nCount_ ) const;

    template<typename T> 
    bool                SendPacket( unsigned short usPacketId, T& data, bool bLogging = true );
    bool                SendPacket( IN const KPacket& packet );

    bool                IsConnected() const { return m_kSessionSocket.IsConnected(); }
    unsigned int        GetIp() const { return m_kSessionSocket.GetIp(); }
    const char*         GetIpStr() const { return m_kSessionSocket.GetIpStr(); }
    unsigned short      GetPort() const { return m_kSessionSocket.GetPort(); }
    KSocket*            GetKSocket() { return &m_kSessionSocket; }

    /// @brief  immediately called after Acception, at here you can create authentification key and set it to client
    virtual void        OnAcceptConnection();

protected:
    void                OnReceiveData( IN std::vector<char>& buffer_ );
    /// will be called if there is a socket error in the Iocp
    virtual void        OnCloseSocket();
    virtual void        OnDestroy(); ///< must thread safe

    KSessionSocket      m_kSessionSocket;
    KWorkQueue          m_workQueue;
    bool                m_bDestroyReserved;
};

template<typename T> 
bool KSession::SendPacket( unsigned short usPacketId_, T& data_, bool bLogging_ )
{
    KPacket packet;
    packet.m_usPacketId = usPacketId_;
    packet.m_nSenderUid = GetUid();

    PacketToBuffer( data_, OUT packet.m_buffer );

    // send packet
    if( KSession::SendPacket( packet ) == false )
        return false;

    if( bLogging_ ) {
        BEGIN_LOG( cerr, L"[s] " )
            << packet.GetIdWstr()
            << L" (name:" << m_wstrName << L")"
            << std::endl;
    }

    return true;
}
