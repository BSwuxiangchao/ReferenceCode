#pragma once

#include <map>
#include <list>
#include <vector>
#include <string>
#include <tchar.h>
#include<Windows.h>

using String= std::basic_string<TCHAR>;

void Log(int level, LPCTSTR text);
void Log(int level, LPCTSTR pszFormat, LPCTSTR arg1, LPCTSTR arg2, int arg3);
void Log(int level, LPCTSTR pszFormat, LPCTSTR arg1, LPCTSTR arg2, LPCTSTR arg3);
void Log(int level, LPCTSTR pszFormat, LPCTSTR arg1, LPCTSTR arg2);
void Log(int level, LPCTSTR pszFormat, LPCTSTR arg1, int arg2);
void Log(int level, LPCTSTR pszFormat, int arg1);
void Log(int level, LPCTSTR pszFormat, int arg1, int arg2);
void Log(int level, LPCTSTR pszFormat, int arg1, int arg2, int arg3);
void Log(int level, LPCTSTR pszFormat, int arg1, int arg2, int arg3, int arg4);
void Log(int level, LPCTSTR pszFormat, LPCTSTR arg1);
void AddStringToList(LPCTSTR text);
void AddStringToList(LPCTSTR format, LPCTSTR arg1);
void AddStringToList(LPCTSTR format, LPCTSTR arg1, int arg2);
void AddStringToList(LPCTSTR format, int arg1);
void AddStringToList(LPCTSTR format, float arg1);

void CreateLogInstance(const TCHAR *pfxfname);
typedef void (*UI_InsertLogFun)(LPCTSTR msg);
void SetUILogFun(UI_InsertLogFun fun);
