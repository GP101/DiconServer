#pragma once
#include <iostream>
#include "KUser.h"
#include "KThreadManager.h"
#include "KBaseSingleton.h"


class KUserLayer
    : public KThread
    , public KBaseSingleton<KUserLayer>
{
    friend class KBaseSingleton<KUserLayer>;

    using               KThread::BeginThread;
    using               KThread::EndThread;

protected:
                        KUserLayer();
public:
    virtual             ~KUserLayer();

    virtual bool        Initialize( DWORD dwParam_ );
    virtual void        Finalize();

    virtual void        Update( DWORD dwElapsedTime_ );

protected:
    virtual void        ThreadRun() override;

    // @todo    you need to maintain server authentification related stuff
};

#define _KUserLayer     KUserLayer::Instance()
