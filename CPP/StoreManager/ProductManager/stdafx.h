// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include "afxwin.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include "httplib.h"

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
// base header
#include "base/base.h"

// duilib
#include "duilib/UIlib.h"

// ui components
#include "ui_components/ui_components.h"


//#define ELPP_THREAD_SAFE

#include "Threads.h"
#include "utility/SimpleIni.h"
#include "Utility/Log.h"

static LPCTSTR IniConfigFile = _T(".\\config.ini");


#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "Snmpapi.lib")
#pragma comment(lib, "IPHLPAPI.lib")
