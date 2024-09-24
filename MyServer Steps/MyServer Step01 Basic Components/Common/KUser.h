#pragma once
#include "KSession.h"
#include "KFsm.h"


class KUser;
typedef std::shared_ptr<KUser> KUserPtr;

class KUser
    : public KSession
    , public KFsmBase
{
public:
    CONSTRUCTOR         KUser() {}
    virtual             ~KUser() {}

    virtual void        OnPacket( IN KPacket* pkPacket )
                        {
                        }
};
