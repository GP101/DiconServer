#pragma once
#include <vector>
#include <atltime.h>
#include "KCriticalSection.h"
#include "KThread.h"
#include "NetUtil.h"


class KIocpWorkerThread; 
typedef boost::shared_ptr<KIocpWorkerThread> KIocpWorkerThreadPtr;

class KIocpWorkerThread : public KThread
{
public:
    CONSTRUCTOR         KIocpWorkerThread();
    virtual             ~KIocpWorkerThread();

protected:
    virtual void        ThreadRun() override;
    virtual void        ThreadUpdateLoop( DWORD dwElapsedTime_ );
};
