#pragma once
#include "KUserLayer.h"


class KMyUserLayer : public KUserLayer
{
public:
                        KMyUserLayer();
    virtual             ~KMyUserLayer();

    virtual bool        Initialize( DWORD dwParam_ ) override;
    virtual void        Finalize() override;

    void                Dump() {}

    // derived from KUserLayer
    virtual void        Update(DWORD dwElapsedTime_) override;
};

#define _KMyUserLayer   std::static_pointer_cast<KMyUserLayer,KUserLayer>(KUserLayer::Instance())
