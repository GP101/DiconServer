#pragma once
#include "KGen.h"
#include "KSocket.h"


class KSession;
class KSessionSocket : public KSocket
{
public:
    virtual void        OnReceiveCompleted( DWORD dwTransferred_ ) override;
    virtual void        OnAcceptConnection() override;
    virtual void        OnCloseSocket() override;

    KSession*           m_pkSession; // qff. must be std::weak_ptr<KSession>
};
