#pragma once
#include "KThreadManager.h"
#include "KBaseSingleton.h"
#include "LayerUtil.h"


class KDbLayer
    : public KThreadManager
    , public KBaseSingleton<KDbLayer>
{
    friend class KBaseSingleton<KDbLayer>;

    using               KThreadManager::BeginAllThread;
    using               KThreadManager::EndAllThread;

protected:
                        KDbLayer();
public:
    virtual             ~KDbLayer();

    virtual bool        Initialize( DWORD dwParam_ ) override;
    virtual void        Finalize() override;
    void                Dump();

protected: 
    virtual KThread*    CreateThread() override;

    std::wstring        m_strFileDSN;
    CreateDbThreadCallback
                        m_pfnCreateThread;
};

#define _KDbLayer       KDbLayer::Instance()
