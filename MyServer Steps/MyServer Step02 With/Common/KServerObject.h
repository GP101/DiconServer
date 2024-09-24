#pragma once
#include "KGen.h"
#include <vector>
#include <string>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include "NetUtil.h"


/// base class for server object.
class KServerObject
    : public boost::enable_shared_from_this<KServerObject>
    , public boost::noncopyable
{
public:
                        KServerObject() : m_nUid( 0 )
                        {
                            wchar_t buff1[128] = { 0 };
                            wchar_t buff2[128] = { 0 };
                            wchar_t buff3[128] = { 0 };
                            static long long sSeedNum = 0;

							_wstrdate_s(buff1, _countof(buff1));
							_wstrtime_s(buff2, _countof(buff2));
							swprintf_s(buff3, L"UID_%s_%s_%020lld"
								, buff1, buff2, ++sSeedNum);

                            m_wstrName = buff3;
                        }

    virtual             ~KServerObject() {}

    const std::wstring& GetName() const { return m_wstrName; }
    void                SetName(const std::wstring& strName_) { m_wstrName = strName_; }
    void                SetName(const wchar_t* szName_) { m_wstrName = szName_; }

    LONGLONG            GetUid() const { return m_nUid; }
    void                SetUid( LONGLONG nUid ) { m_nUid = nUid; }
    int                 GetRefCount() const { return shared_from_this().use_count() - 1; }

    template<typename T>
    std::shared_ptr<T>
                        GetSharedPtr()
                        {
                            return std::static_pointer_cast<T>( shared_from_this() );
                        }

protected:
    std::wstring        m_wstrName;
    LONGLONG            m_nUid;
};
