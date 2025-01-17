#ifndef _MY_HOOK_API_
#define _MY_HOOK_API_
#define MAX_HOOK_NUM   256

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include "xde32/xde.h"


BOOL HookProcByName(LPCTSTR DllName, LPCSTR ProcName, PVOID MyProcName); // hook指定的dll中的导出函数
BOOL HookProcByAddress(LPVOID ProcAddress, PVOID MyProcAddr); // hook指定的地址

DWORD WINAPI MyHookAll(LPVOID); // DllMain中做HOOK用的例程
DWORD WINAPI MyUnHookAll(LPVOID); // 解除hook函数

BOOL UnHookProcByAddress(LPVOID ProcAddress, PVOID MyProcAddr);
BOOL UnHookProcByName(LPCTSTR DllName, LPCSTR ProcName, PVOID MyProcAddr); // 进行unhook

DWORD WINAPI GetNewAddress(PVOID MyAddr); // 得到原来函数的地址并返回

typedef struct _FunAddress 
{
	DWORD MyFunAddr;   // 处理hook函数地址
	DWORD NewMalloc;   // 复制出来的首地址函数地址
}FunAddress, *PFunAddress;


#endif // _MY_HOOK_API_