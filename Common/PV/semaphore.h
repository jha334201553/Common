#ifndef _SEMAPHORE_HEADER_FILE_H_
#define _SEMAPHORE_HEADER_FILE_H_
#include <windows.h>
#include <tchar.h>

class CSemaphore
{
public:
    CSemaphore(int TotalVal);
    CSemaphore(int TotalVal, int InitVal); // TotalVal 总容量，InitVal 初始化多少容量

    void P();
    void V();
    ~CSemaphore();
private:
    HANDLE m_semaphore;
};

#ifdef _DEBUG
#pragma comment(lib, "PV_d.lib")
#else
#pragma comment(lib, "PV.lib")
#endif

#endif  // _SEMAPHORE_HEADER_FILE_H_