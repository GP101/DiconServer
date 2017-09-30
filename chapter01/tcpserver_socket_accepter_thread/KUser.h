#pragma once
#include "KGen.h"
#include "KSocket.h"


class KUser;
typedef boost::shared_ptr<KUser> KUserPtr;
class KUser
{
public:
    KSocket*            GetKSocket() { return &m_socket; }

private:
    KSocket             m_socket;
};
