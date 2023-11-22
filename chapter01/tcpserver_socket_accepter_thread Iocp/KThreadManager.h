#pragma once
#include "KThread.h"
#include <atltime.h>
#include "KGen.h"
#include <vector>


class KThreadManager
{
protected:
                        KThreadManager();
public:
    virtual             ~KThreadManager();

public:
    virtual bool        Initialize( DWORD nThreadNum_ );
    virtual void        Finalize();

    void                BeginAllThread();
    void                EndAllThread();

    size_t              GetThreadNum() const { std::lock_guard<std::mutex> lock(m_muVecThread); return m_vecThread.size(); }
    void                SetNumThread( size_t nThreadNum ); // dynamically change number of threads

protected:      
    virtual KThread*    CreateThread() = 0;

    std::vector<std::shared_ptr<KThread>>
                        m_vecThread;
    mutable std::mutex  m_muVecThread;
};
