#include "stdafx.h"
#include "Logger.h"
#include "FileLogger.h"


static UI_InsertLogFun g_fun = NULL;
void SetUILogFun(UI_InsertLogFun fun)
{
	g_fun = fun;
}

void CreateLogInstance(const TCHAR *pfxfname)
{
	CFileLogger::CreateInstance(pfxfname);
}

void Log(int level, LPCTSTR text)
{
	CFileLogger::Instance()->LogMsgDirect(_T(""), (LOG_OUT_LEVEL)level, text);
}

void Log(int level, LPCTSTR pszFormat, LPCTSTR arg1, LPCTSTR arg2, int arg3)
{
	TCHAR buf[MAX_PATH];
	int len = _stprintf_s(buf, pszFormat, arg1, arg2, arg3);

	Log(level, buf);
}

void Log(int level, LPCTSTR pszFormat, LPCTSTR arg1, LPCTSTR arg2, LPCTSTR arg3)
{
	TCHAR buf[MAX_PATH];
	int len = _stprintf_s(buf, pszFormat, arg1, arg2, arg3);

	Log(level, buf);
}


void Log(int level, LPCTSTR pszFormat, LPCTSTR arg1, LPCTSTR arg2)
{
	TCHAR buf[MAX_PATH];
	int len = _stprintf_s(buf, pszFormat, arg1, arg2);

	Log(level, buf);
}

void Log(int level, LPCTSTR pszFormat, LPCTSTR arg1, int arg2)
{
	TCHAR buf[MAX_PATH];
	int len = _stprintf_s(buf, pszFormat, arg1, arg2);

	Log(level, buf);
}
void Log(int level, LPCTSTR pszFormat, int arg1, int arg2, int arg3, int arg4)
{
	TCHAR buf[MAX_PATH];
	int len = _stprintf_s(buf, pszFormat, arg1, arg2, arg3, arg4);

	Log(level, buf);
}

void Log(int level, LPCTSTR pszFormat, int arg1, int arg2)
{
	TCHAR buf[MAX_PATH];
	int len = _stprintf_s(buf, pszFormat, arg1, arg2);

	Log(level, buf);
}

void Log(int level, LPCTSTR pszFormat, int arg1, int arg2, int arg3)
{
	TCHAR buf[MAX_PATH];
	int len = _stprintf_s(buf, pszFormat, arg1, arg2, arg3);

	Log(level, buf);

}


void Log(int level, LPCTSTR pszFormat, int arg1)
{
	TCHAR buf[MAX_PATH];
	int len = _stprintf_s(buf, pszFormat, arg1);

	Log(level, buf);
}

void Log(int level, LPCTSTR pszFormat, LPCTSTR arg1)
{
	TCHAR buf[512];
	int len = _stprintf_s(buf, pszFormat, arg1);

	Log(level, buf);
}

void AddStringToList(LPCTSTR format, LPCTSTR arg1, int arg2)
{
	TCHAR buf[MAX_PATH];
	int len = _stprintf_s(buf, format, arg1, arg2);

	AddStringToList(buf);
}

void AddStringToList(LPCTSTR format, LPCTSTR arg1)
{
	TCHAR buf[MAX_PATH];
	int len = _stprintf_s(buf, format, arg1);

	AddStringToList(buf);
}

void AddStringToList(LPCTSTR format, float arg1)
{
	TCHAR buf[MAX_PATH];
	int len = _stprintf_s(buf, format, arg1);

	AddStringToList(buf);
}

void AddStringToList(LPCTSTR format, int arg1)
{
	TCHAR buf[MAX_PATH];
	int len = _stprintf_s(buf, format, arg1);

	AddStringToList(buf);
}

void AddStringToList(LPCTSTR text)
{
	if (g_fun)
	{
		g_fun(text);
	}

	Log(3, _T("%s\n"), text);
}