#pragma once
#include "KThreadManager.h"
#include "KBaseSingleton.h"
#include "LayerUtil.h"
#define OTL_ODBC // Compile OTL 4.0/ODBC
#include "otlv4.h" // include the OTL 4.0 header file


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
    virtual             ~KDbLayer() noexcept(true);

    virtual bool        Initialize( DWORD dwParam_ ) override;
    virtual void        Finalize() override;
    void                Dump();

protected: 
    virtual KThread*    CreateThread() override;

    std::wstring        m_strFileDSN;
    CreateDbThreadCallback
                        m_pfnCreateThread;
	otl_connect			m_odbc; // connect database object
};

#define _KDbLayer       KDbLayer::Instance()
