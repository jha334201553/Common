#pragma once
#ifndef _LGJ_MY_KERNEL32_H_
#define _LGJ_MY_KERNEL32_H_
#include <windows.h>
#pragma comment(lib, "mykernel32.lib")


// 静态变量\宏常量定义
extern "C" const IMAGE_DOS_HEADER __ImageBase; // 指针指向模块的首地址,编译器指定的


// GetModuleHandleEx 参数flags所需值
#define  GET_MODULE_HANDLE_EX_FLAG_PIN                  (0x01)  // 模块一直映射在调用该函数的进程中，直到该进程结束，不管调用多少次FreeLibrary
#define  GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT   (0x02)  // 与 GetModuleHandle相同，不增加引用计数
#define  GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS         (0x04)  // lpModuleName是模块中的一个地址



#ifdef __cplusplus  
extern "C" {  
#endif  

// API 函数定义
BOOL WINAPI DebugSetProcessKillOnExit( BOOL KillOnExit ); // 设置调试器退出时，被调试程序是否退出

BOOL WINAPI DebugActiveProcessStop(DWORD PID); // 解除调试器与被调试程序的关联


// 根据函数名得到函数所在的模块地址
BOOL WINAPI GetModuleHandleExA(DWORD flags, 
							   PCHAR FuncName, 
							   HMODULE* hModule); 

// 根据函数名得到函数所在的模块地址
BOOL WINAPI GetModuleHandleExW(DWORD flags, 
							   PWCHAR FuncName, 
							   HMODULE* hModule); 


// 在宽字节与窄字节中定义统一接口
#ifdef _UNICODE

#define GetModuleHandleEx GetModuleHandleExW

#else

#define GetModuleHandleEx GetModuleHandleExA

#endif















   
#ifdef __cplusplus  
};  
#endif 

#endif // _LGJ_MY_KERNEL32_H_