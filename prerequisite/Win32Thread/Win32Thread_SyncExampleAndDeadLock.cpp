#include <windows.h>
#include <stdio.h>
#include <iostream>

HANDLE ghEvents[2];
int g_value = 0;

DWORD WINAPI ThreadProc(LPVOID);

int main(void)
{
    HANDLE hThread;
    DWORD i, dwEvent, dwThreadID;

    // Create two event objects

    for (i = 0; i < 2; i++)
    {
        ghEvents[i] = CreateEvent(
            NULL,   // default security attributes
            FALSE,  // auto-reset event object
            FALSE,  // initial state is non-signaled
            NULL);  // unnamed object

        if (ghEvents[i] == NULL)
        {
            printf("CreateEvent error: %d\n", GetLastError());
            ExitProcess(0);
        }
    }

    // Create a thread

    hThread = CreateThread(
        NULL,         // default security attributes
        0,            // default stack size
        (LPTHREAD_START_ROUTINE)ThreadProc,
        NULL,         // no thread function arguments
        0,            // default creation flags
        &dwThreadID); // receive thread identifier

    if (hThread == NULL)
    {
        printf("CreateThread error: %d\n", GetLastError());
        return 1;
    }

    Sleep( 2000 );
    // Wait for the thread to signal one of the event objects
    while (g_value < 10) {
        WaitForSingleObject(
            ghEvents[0],     // array of objects
            INFINITE );       // five-second wait
        std::cout << g_value << std::endl;
        g_value += 1;
        Sleep( 1000 );
        SetEvent( ghEvents[1] );
    }

    // Close event handles

    for (i = 0; i < 2; i++)
        CloseHandle(ghEvents[i]);

    return 0;
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{

    // lpParam not used in this example
    UNREFERENCED_PARAMETER(lpParam);

    // Set one event to the signaled state

    while (g_value < 9) {
        std::cout << g_value << std::endl;
        g_value += 1;
        Sleep( 1000 );
        SetEvent( ghEvents[0] );
        WaitForSingleObject(
            ghEvents[1],     // array of objects
            INFINITE );       // five-second wait
    }
    return 0;
}
