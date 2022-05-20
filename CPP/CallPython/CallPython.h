// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CALLPYTHON_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CALLPYTHON_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#pragma once

#ifdef CALLPYTHON_EXPORTS
#define CALLPYTHON_API __declspec(dllexport)
#else
#define CALLPYTHON_API __declspec(dllimport)
#endif


CALLPYTHON_API void Call_Pay();
CALLPYTHON_API void Call_Shipment(BOOL succeed);
