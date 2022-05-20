/********************************************************************
created:	2017/01/20
created:	20:1:2017   14:28
filename: 	.\ProgramAgent.h
file path:	.\
file base:	Utility
file ext:	h
author:

purpose:	�ṩ�رճ����������򡢹ػ����������ԵȲ���
			���磺m_pProgramAgent = new ProgramAgent("CarClient.exe");

*********************************************************************/

#ifndef ProgramAgent_h__
#define ProgramAgent_h__

#include <windows.h>
//#include <timeapi.h>
#include <mmsystem.h>
#include <string>
//#include <afx.h>
#include <tlhelp32.h>
//#include <afxstat_.h>

#pragma comment(lib, "winmm.lib")

class ProgramAgent
{
public:
	ProgramAgent(std::string strProgram) { m_strProgram = strProgram; }
	virtual ~ProgramAgent() { ; }

public:
	/*
	* �رյ���
	*/
	virtual void PowerOff()
	{
		//AFX_MANAGE_STATE(AfxGetStaticModuleState());
		ChangeTokenPrivilege();
		ExitWindowsEx(EWX_POWEROFF, EWX_FORCE);
	}

	/*
	* ��������
	*/
	virtual void Reboot()
	{
		//AFX_MANAGE_STATE(AfxGetStaticModuleState());
		ChangeTokenPrivilege();
		ExitWindowsEx(EWX_REBOOT, EWX_FORCE);
	}

	//��������
	virtual void Launch(void)
	{
		//�ж�ָ���������Ľ����Ƿ���� 0==������ ��0�����
		if (0 != GetProcessidFromName(m_strProgram.c_str()))
		{
			KillProcessByName(m_strProgram.c_str());
		}

		//�����½���
		CreateNewProcess(m_strProgram.c_str());
	}

	//�رճ���
	virtual void Shutdown()
	{
		//�ж�ָ���������Ľ����Ƿ���� 0==������
		if (0 != GetProcessidFromName(m_strProgram.c_str()))
		{
			KillProcessByName(m_strProgram.c_str());
		}
	}

private:
	//Ȩ��
	virtual BOOL ChangeTokenPrivilege()
	{
		LUID	 luID;
		HANDLE hProcess;
		HANDLE hAccessToken;
		TOKEN_PRIVILEGES tpTokenPrivilege;

		hProcess = GetCurrentProcess();
		DWORD processID = GetCurrentProcessId();
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, processID);
		DWORD dwError = GetLastError();
		if (OpenProcessToken(hProcess,
			TOKEN_ADJUST_PRIVILEGES,
			&hAccessToken))
		{
			if (LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &luID))
			{
				tpTokenPrivilege.PrivilegeCount = 1;
				tpTokenPrivilege.Privileges[0].Luid = luID;
				tpTokenPrivilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

				BOOL bRet = AdjustTokenPrivileges(hAccessToken,
					FALSE,
					&tpTokenPrivilege,
					sizeof(TOKEN_PRIVILEGES),
					NULL,
					NULL);
				if (!bRet) {
					//TRACE("%s\nLINE:%d\nCode Meaning:%s",__FILE__,__LINE__,GetLastErrorInfo()); 
					return false;
				}
			}
			else
			{
				return false;
				//TRACE("%s\nLINE:%d\nCode Meaning:%s",__FILE__,__LINE__,GetLastErrorInfo()); 
			}
		}
		else
		{
			//TRACE("%s\nLINE:%d\nCode Meaning:%s",__FILE__,__LINE__,GetLastErrorInfo()); 
			return false;
		}
		//LookupPrivilegeName(NULL,&luID,SE_SHUTDOWN_NAME,(unsigned long*)&cbName);
		return true;
	}

	//�ж�ָ���������Ľ����Ƿ���� 0==������
	virtual DWORD GetProcessidFromName(LPCTSTR name)
		{
			PROCESSENTRY32 pe;
			DWORD id = 0;
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			pe.dwSize = sizeof(PROCESSENTRY32);
			if (!Process32First(hSnapshot, &pe))
				return 0;
			while (1)
			{
				pe.dwSize = sizeof(PROCESSENTRY32);
				if (Process32Next(hSnapshot, &pe) == FALSE)
					break;
				if (strcmp(pe.szExeFile, name) == 0)
				{
					id = pe.th32ProcessID;

					break;
				}


			}
			CloseHandle(hSnapshot);
			return id;
		}

	//��������
	virtual	BOOL KillProcess(DWORD dwPid)
		{
			HANDLE hPrc;

			if (0 == dwPid) return FALSE;

			hPrc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);  // Opens handle to the process.

			if (!TerminateProcess(hPrc, 0)) // Terminates a process.
			{
				CloseHandle(hPrc);
				return FALSE;
			}
			else
				WaitForSingleObject(hPrc, 100); // At most ,waite 2000  millisecond.

			CloseHandle(hPrc);
			return TRUE;
		}

	virtual	BOOL KillProcessByName(const TCHAR *lpszProcessName) {
			unsigned int   pid = -1;
			BOOL    retval = TRUE;

			if (lpszProcessName == NULL)
				return -1;

			DWORD dwRet = 0;
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			PROCESSENTRY32 processInfo;
			processInfo.dwSize = sizeof(PROCESSENTRY32);
			int flag = Process32First(hSnapshot, &processInfo);

			// Find the process with name as same as lpszProcessName  
			while (flag != 0)
			{
				if (_tcscmp(processInfo.szExeFile, lpszProcessName) == 0) {
					// Terminate the process.  
					pid = processInfo.th32ProcessID;
					HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);

					if (TerminateProcess(hProcess, 0) != TRUE) { // Failed to terminate it.  
						retval = FALSE;
						break;
					}
				}

				flag = Process32Next(hSnapshot, &processInfo);
			} // while (flag != 0)  

			CloseHandle(hSnapshot);

			if (pid == -1)
				return FALSE;

			return retval;
		}

	//�����½���
	virtual	int CreateNewProcess(LPCSTR pszExeName)
	{
		//�򿪳������õ��ı���
		PROCESS_INFORMATION piProcInfoGPS;
		STARTUPINFO siStartupInfo;

		SECURITY_ATTRIBUTES saProcess, saThread;
		ZeroMemory(&siStartupInfo, sizeof(siStartupInfo));
		siStartupInfo.cb = sizeof(siStartupInfo);
		saProcess.nLength = sizeof(saProcess);
		saProcess.lpSecurityDescriptor = NULL;
		saProcess.bInheritHandle = true;
		saThread.nLength = sizeof(saThread);
		saThread.lpSecurityDescriptor = NULL;
		saThread.bInheritHandle = true;
		return ::CreateProcess(NULL, (LPTSTR)pszExeName, &saProcess, &saThread, false,
			CREATE_NEW_CONSOLE, NULL, NULL, &siStartupInfo, &piProcInfoGPS);
	}

private:
	//���򿪵ĳ�������
	std::string m_strProgram;
};


#endif	//ProgramAgent