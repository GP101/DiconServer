#pragma once
#include "KBaseServer.h"


class KMyGameServer;
typedef std::shared_ptr<KMyGameServer>    KMyGameServerPtr;

class KMyGameServer : public KBaseServer
{
public:
    CONSTRUCTOR         KMyGameServer();
    virtual             ~KMyGameServer();

    void                Dump() {}
    virtual bool        Initialize( DWORD dwParam_ ) override;
    virtual void        Finalize() override;

    virtual bool        DestroyAllSession();

    /// override OnPacket() in the KBaseServer
    virtual void        OnPacket( IN KPacket* pkPacket ) override;

    virtual void        Update(DWORD dwElapsedTime_) override;
};

#define _KMyGameServer  std::static_pointer_cast<KMyGameServer, KBaseServer>( KBaseServer::Instance() )
