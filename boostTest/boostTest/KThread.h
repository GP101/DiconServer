#pragma once
#include "KGen.h"
#include <string>
#include <map>
#include <process.h>
#include <thread>
#include <utility>
#include <atomic>
#include <condition_variable>

#pragma comment( lib, "Winmm" )


class KThread
{
    typedef unsigned (__stdcall* PTHREAD_START)(void*);

public:
    CONSTRUCTOR         KThread() 
                        {
                        }

    virtual             ~KThread()
                        {
                            assert(m_thread.joinable() == false );
                        }

    virtual bool        BeginThread()
                        {
                            if (m_thread.joinable())
                                return false;
                            m_thread = std::thread(ThreadProc, reinterpret_cast<int>(this));
                            m_threadId = m_thread.get_id();
                            return true;
                        }

    virtual void        EndThread( DWORD dwTimeout_ )
                        {
                            if (m_thread.joinable()) {
                                m_quitFlag = 1;
                                m_cvQuit.notify_all();
                                m_thread.join();
                            }
    }
    
    virtual void        ThreadRun() = 0;
    
    std::thread::id     GetThreadId() const { return m_threadId; }
    bool                IsRunning() const { return m_thread.joinable(); }

protected:
    static void         ThreadProc( int pvParam_ )
                        {
                            KThread* pThread = reinterpret_cast<KThread*>( pvParam_ );
                            assert( pThread != nullptr );
                            pThread->ThreadRun();
                        }
    
protected:
    std::condition_variable m_cvQuit;
    std::mutex          m_cvMutex;
    std::atomic<int>    m_quitFlag{ 0 };
    std::thread::id     m_threadId;
    std::thread         m_thread;
};
