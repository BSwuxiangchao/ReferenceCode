#pragma once
#include "./CrashRpt/CrashRpt.h"
#include <stdio.h>
#include <tchar.h>
#include <CTime>

#ifndef _DEBUG
#ifdef _WIN64
#pragma comment(lib, "../Utility/CrashRpt/CrashRpt1403_x64.lib")
#else
#pragma comment(lib, "../Utility/CrashRpt/CrashRpt1402.lib")
#endif
#endif

class CRegisterCrashRpt
{
public:
	~CRegisterCrashRpt() {
#ifndef _DEBUG
		crUninstall(); // Uninstall crash reporting
#endif
	}

	static CRegisterCrashRpt& getInstance() {
		static CRegisterCrashRpt _instance;
		return _instance;
	}

	BOOL registerCrashRpt() {
		return InstallCrashRpt();
	}

protected:
	CRegisterCrashRpt() {}
	BOOL InstallCrashRpt(const char* versionStr = nullptr); // Install crash reporting

private:

};

BOOL CRegisterCrashRpt::InstallCrashRpt(const char* versionStr)
{
#ifndef _DEBUG
	TCHAR strIniFile[MAX_PATH];
	TCHAR strAppName[MAX_PATH];
	GetModuleFileName(NULL, strIniFile, sizeof(strIniFile));
	TCHAR* p = strrchr(strIniFile, '\\');
	strcpy_s(strAppName, p+2);
	*p = '\0';
	//_stprintf_s(strIniFile, _T("%s\\crashrpt_lang_ZH-CN.ini"), strIniFile);
#ifdef _WIN64
	_stprintf_s(strIniFile, _T("%s\\crashrpt_lang_ZH-CN.ini"), strIniFile);
#else
	_stprintf_s(strIniFile, _T("%s\\crashrpt_lang.ini"), strIniFile);
#endif

	CR_INSTALL_INFO info;
	memset(&info, 0, sizeof(CR_INSTALL_INFO));
	info.cb = sizeof(CR_INSTALL_INFO);

	/*char rptName[MAX_PATH];
	SYSTEMTIME systm;
	GetLocalTime(&systm);
	sprintf(rptName, "%s_%.4d%.2d%.2d%.2d%.2d%.2d", strAppName,
		systm.wYear, systm.wMonth, systm.wDay,
		systm.wHour, systm.wMinute, systm.wSecond);*/
	info.pszAppName = strAppName;
	info.pszAppVersion = (nullptr == versionStr) ? _T("1.0.1") : versionStr;// Define application version.
	// URL for sending error reports over HTTP.
	info.pszUrl = _T("http://www.octvision.com/crashrpt.php");
	// Install all available exception handlers.
	info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS | CR_INST_DONT_SEND_REPORT | CR_INST_STORE_ZIP_ARCHIVES;
	info.pszErrorReportSaveDir = _T(".\\CrashRpt");
	info.pszLangFilePath = strIniFile;
	// Provide privacy policy URL
	info.pszPrivacyPolicyURL = _T("http://www.octvision.com/privacy.html");
	int nResult = crInstall(&info);
	if (nResult != 0)
	{
		TCHAR buff[256];
		crGetLastErrorMsg(buff, 256);
		MessageBox(NULL, buff, _T("crInstall error"), MB_OK);
		return FALSE;
	}
	// Take screenshot of the app window at the moment of crash
	crAddScreenshot2(CR_AS_MAIN_WINDOW | CR_AS_USE_JPEG_FORMAT, 95);
#endif
	return TRUE;
}