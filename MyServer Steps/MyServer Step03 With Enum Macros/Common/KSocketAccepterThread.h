#pragma once
#include <atltime.h>
#include <NetUtil.h>
#include "KThread.h"
#include "KUser.h"


class KSocketAccepterThread;
typedef std::shared_ptr<KSocketAccepterThread> KSocketAccepterPtr;
class KSocket;

class KSocketAccepterThread : public KThread
{
    typedef KUserPtr (*CreateUserCallback)();

public:
    struct KSocketAccepterInitParam
    {
        u_short         m_usPort;
        CreateUserCallback
                        m_pfnCreateUser;
    };

    CONSTRUCTOR         KSocketAccepterThread();
    virtual             ~KSocketAccepterThread() {}

    void                BindAndListen( u_short usPort_, CreateUserCallback pfnCreateUser_, bool bNagleAlgOn_ = true );

    virtual bool        BeginThread() override;
    virtual void        EndThread( DWORD dwTimeOut_ ) override;

private:
    virtual void        ThreadRun() override;
    virtual void        Update( DWORD dwElapsedTime_ );
    void                _CloseSocket();

private:
    SOCKET              m_sock;
    CreateUserCallback  m_pfnCreateUser;
};
