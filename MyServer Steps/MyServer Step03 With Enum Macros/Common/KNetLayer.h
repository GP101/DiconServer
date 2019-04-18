#pragma once
#include <KSocketAccepterThread.h>
#include "KBaseSingleton.h"
#include "KPacket.h"


class KNetLayer : public KBaseSingleton<KNetLayer>
{
    friend class KBaseSingleton<KNetLayer>;

protected:
    CONSTRUCTOR         KNetLayer();

public:
    virtual             ~KNetLayer();

    virtual bool        Initialize( DWORD dwParam_ );
    virtual void        Finalize();

    void                SetPort( unsigned short usPort_ ) { m_usDefaultPort = usPort_; }

    unsigned short      GetPort() const { return m_usDefaultPort; }

public:
    unsigned short      m_usDefaultPort;

protected:
    KSocketAccepterThread
                        m_kSocketAccepterThread;
};

#define _KNetLayer      KNetLayer::Instance()


enum EPacketReceiverType
{
    SNDTO_DB,
    SNDTO_SERVER,
    SNDTO_CENTER,
    SNDTO_CLIENT,
    SNDTO_USER,             // to
    SNDTO_REMOTE_DB,        // from
    SNDTO_REMOTE_USER,      // from, to
    SNDTO_REMOTE_SERVER,    // from, to(serveruid)
    SNDTO_REMOTE_CLIENT     // serveruid, useruid
};

void NetSendPacket( int nFlag, LONGLONG nFrom, LONGLONG nTo, KPacketPtr spPacket );

template<typename T>
void NetSendPacket( int nFlag_, LONGLONG nFrom_, LONGLONG nTo_, u_short usPacketId_, const T& data_ )
{
    KPacketPtr spPacket( new KPacket );
    {
        spPacket->m_nSenderUid = nFrom_;
        spPacket->SetData( usPacketId_, data_ );
    }

    NetSendPacket( nFlag_, nFrom_, nTo_, spPacket );
}

template<typename T>
void NetSendTo( int nFlag_, LONGLONG nFrom, u_short usPacketId, const T& data )
{
    NetSendPacket( nFlag_, nFrom, 0, usPacketId, data );
}

inline void NetSendTo( int nFlag_, LONGLONG nFrom, u_short usPacketId )
{
    NetSendPacket( nFlag_, nFrom, 0, usPacketId, char() );
}
