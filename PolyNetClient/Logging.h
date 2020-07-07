#pragma once

#define WIDE2(x) L##x
#define WIDECHAR(x) WIDE2(x)
#define WIDE_FUNCTION WIDECHAR(__FUNCTION__)

#ifdef __cplusplus
#ifdef ENABLE_LOG
#include <iostream>
#endif

#ifdef ENABLE_LOG
#define LOG(x) std::cout << "[" << __FUNCTION__ << "] " << x << std::endl
#define LOGW(x) std::wcout << L"[" << WIDE_FUNCTION << L"] " << x << std::endl
#define DBG_WAIT() WinapiSleep(100)
#else
#define LOG(x) 
#define LOGW(x) 
#define DBG_WAIT() 
#endif
#else

#ifdef ENABLE_LOG
#include <stdio.h>
#endif

#ifdef ENABLE_LOG
#define LOG(f_, ...) printf(("[" __FUNCTION__ "] " f_ "\n"), __VA_ARGS__)
#define LOGW(x) 
#else
#define LOG(x) 
#define LOGW(x) 
#endif

#endif