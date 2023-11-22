#pragma once
#include <vector>
#include <atltime.h>
#include <mutex>
#include "KThread.h"


class KIocpWorkerThread; 
typedef std::shared_ptr<KIocpWorkerThread> KIocpWorkerThreadPtr;

class KIocpWorkerThread : public KThread
{
public:
    CONSTRUCTOR         KIocpWorkerThread();
    virtual             ~KIocpWorkerThread();

protected:
    virtual void        ThreadRun() override;
    virtual void        ThreadUpdateLoop( DWORD dwElapsedTime_ );
};
