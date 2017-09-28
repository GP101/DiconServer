#pragma once
#define MAX_PACKET_SIZE 16384	// 1024 * 16 (16Kbyte)

class KOverlapped : public OVERLAPPED
{
public:
    enum EIoMode
    {
        IO_RECEIVE,
        IO_SEND
    };

                        KOverlapped()
                        {
                            hEvent              = NULL;

                            Clear();
                            m_wsaBuffer.buf       = m_pBuffer;
                            m_wsaBuffer.len       = 0;
                            m_dwRemained              = 0;
                            m_bSending            = false;
                        }

    void                Clear() 
                        {
                            Internal            = 0;
                            InternalHigh        = 0;
                            Offset              = 0;
                            OffsetHigh          = 0;
                        } 

public:
    EIoMode             m_eIoMode;
    WSABUF              m_wsaBuffer;
    char                m_pBuffer[MAX_PACKET_SIZE];
    DWORD               m_dwRemained;
    bool                m_bSending;
};