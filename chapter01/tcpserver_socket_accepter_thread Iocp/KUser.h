#pragma once
#include "KGen.h"
#include "KSocket.h"


class KUser;
typedef std::shared_ptr<KUser> KUserPtr;
class KUser : public noncopyable
{
public:
    KUser() { m_pSocket = new KSocket();  }
    ~KUser()
    {
        delete m_pSocket;
    }
    KSocket*            GetKSocket() { return m_pSocket; }

private:
    KSocket*            m_pSocket;
};
