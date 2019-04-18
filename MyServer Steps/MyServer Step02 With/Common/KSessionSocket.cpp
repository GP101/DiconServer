#include "KGen.h"
#include "KSessionSocket.h"
#include "KSession.h"


VIRTUAL void KSessionSocket::OnReceiveCompleted( DWORD dwTransferred_ )
{
    if( dwTransferred_ == 0 ) {
        BEGIN_LOG( cout, L"closed socket: " )
            << LOG_NAMEVALUE( dwTransferred_ )
            << END_LOG;
        VIRTUAL OnCloseSocket();
        return;
    }

    USHORT usPacketLen = 0;

    m_ovlReceive.m_dwRemained += dwTransferred_;
    
    //if( m_ovlReceive.m_dwRemained >= MAX_PACKET_SIZE )
    //{
    //    BEGIN_LOG( cwarn, "Recv Buffer Full. " )
    //        << LOG_NAMEVALUE( MAX_PACKET_SIZE )
    //        << LOG_NAMEVALUE( m_ovlReceive.m_dwRemained )
    //        << LOG_NAMEVALUE( m_wstrName );
    //    return;
    //} 
    
    // if we can read packet size
    while( m_ovlReceive.m_dwRemained >= sizeof(usPacketLen) ) {
        // get packet length
        memcpy( OUT &usPacketLen, m_ovlReceive.m_pBuffer, sizeof(usPacketLen) );

        // if we can build complete packet, then process it
        if( m_ovlReceive.m_dwRemained >= usPacketLen ) {
            std::vector<char>   buffer;
            buffer.insert( buffer.end(), m_ovlReceive.m_pBuffer + sizeof( usPacketLen )
                           , m_ovlReceive.m_pBuffer + usPacketLen - sizeof( usPacketLen ) );

            if( m_pkSession != nullptr )
                m_pkSession->OnReceiveData( buffer );

            // buffer management
            m_ovlReceive.m_dwRemained -= usPacketLen;
            memmove(&m_ovlReceive.m_pBuffer[0], &m_ovlReceive.m_pBuffer[usPacketLen], m_ovlReceive.m_dwRemained );            
        }
        else {
            break; // incomplete data to build a packet.
        }//if.. else..
    }

    // start next receiving.
    KSocket::ReceiveData();
}

VIRTUAL void KSessionSocket::OnAcceptConnection()
{
    if( m_pkSession != nullptr )
        VIRTUAL m_pkSession->OnAcceptConnection();
}

VIRTUAL void KSessionSocket::OnCloseSocket()
{
    if( m_pkSession != nullptr )
        VIRTUAL m_pkSession->OnCloseSocket();
}
