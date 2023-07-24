/*
* @Abrief dump
* @note 软件系统dump时的信息输出,Win32实现
* @version 1.0
* @author mosongjin 2021-07-16
*/

#ifndef _BGI_MINIDUMP_H
#define _BGI_MINIDUMP_H

#include <QString>
#include <QVector>
#include "BgiThread.h"
#include "BgiSemaphore.h"
#include <qobject.h>

const int MAX_ADDRESS_LENGTH = 32;
const int MAX_NAME_LENGTH = 1024;

struct StackTraceInfo
{
	QVector<QString> StackNameList;
	QVector<QString> StackAddrList;
	QVector<QString> StackNums;
	QVector<QString> StackFileList;
	QVector<QString> StackModuleList;
};

class BGIBASE_CLASS_EXPORT BgiThreadWaitOperation : public QObject
{
	Q_OBJECT
public:
	BgiThreadWaitOperation();
	~BgiThreadWaitOperation();

	void SetThreadSync(QThread* thread);
private:
	void OnWait();
	
	private Q_SLOTS:
	void OnWake();
private:
	BgiSemaphore* m_pSema;
};

class ThreadDumpCrashPara
{
public:
	friend class MiniDump;
	ThreadDumpCrashPara(_EXCEPTION_POINTERS* ptr, HANDLE threadHandle);
	~ThreadDumpCrashPara();

private:
	HANDLE m_threadHandle;
	_EXCEPTION_POINTERS* m_ptr;
	BgiThreadWaitOperation* m_pThreadOper;
};


class BGIBASE_CLASS_EXPORT MiniDump
{
public:
    //只有win系统才能用minidump
	static void SaveDump(_EXCEPTION_POINTERS* ePtr, HANDLE currentthreadhandle = GetCurrentThread());

private:
	static void CrashDumpThreadFunc(ThreadDumpCrashPara* crashpara);
	static void RunCrashDumpThreadFunc(ThreadDumpCrashPara* crashpara);
	static void WriteExceptionTxt(const QString& dir, ThreadDumpCrashPara* crashpara);
	static QString GetDumpDir();
	static QString GetDumpTxtPath(const QString& dir);
	static QString GetDumpFilePath(const QString& dir);
	static void WriteDump(const QString& dir, ThreadDumpCrashPara* ePtr);
	static StackTraceInfo* GetStackTraceInfo(CONTEXT* context, HANDLE errthreadHandle); 

	//private:
	//	static EXCEPTION_POINTERS* m_ExceptionPtr;
};

#endif


/*
class MiniDumpThread : public QThread
{
	friend class MiniDumpThread;
public:
	MiniDumpThread() {}
	virtual ~MiniDumpThread() {}

	void SetMinDump(MiniDump* miniDump) {
		m_pMiniDump = miniDump;
	}

private:
	virtual void run() {
		MiniDump::RunCrashDumpThreadFunc(m_pCrashPara);
	}

private:
	MiniDump* m_pMiniDump;
	ThreadDumpCrashPara* m_pCrashPara;
};
*/


