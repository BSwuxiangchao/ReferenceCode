#include "MiniDump.h"
#include <QDir>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <qthread.h>
#include "dbghelp.h"
//#include "LEnvironmentConfig.h"


#pragma comment(lib,"DbgHelp.lib")


ThreadDumpCrashPara::ThreadDumpCrashPara(_EXCEPTION_POINTERS* ptr, HANDLE threadHandle)
	:m_pThreadOper(NULL),
	m_ptr(ptr),
	m_threadHandle(threadHandle)
{
	m_pThreadOper = new BgiThreadWaitOperation();
}
ThreadDumpCrashPara::~ThreadDumpCrashPara()
{
	if (m_pThreadOper != NULL)
	{
		delete m_pThreadOper;
		m_pThreadOper = NULL;
	}
}

BgiThreadWaitOperation::BgiThreadWaitOperation()
	:m_pSema(NULL)
{
	m_pSema = new BgiSemaphore(0);
}

BgiThreadWaitOperation::~BgiThreadWaitOperation()
{
	if (m_pSema != NULL)
	{
		delete m_pSema;
		m_pSema = NULL;
	}
}
void BgiThreadWaitOperation::SetThreadSync(QThread* thread)
{
	/*
	*开始执行线程函数时当前线程等待，线程函数结束时当前线程唤醒
	*/
	if (thread != nullptr)
	{
		QObject::connect(thread, SIGNAL(finished()), this, SLOT(OnWake()));
		QObject::moveToThread(thread);
		OnWait();
	}
}
void BgiThreadWaitOperation::OnWait()
{
	m_pSema->Acquire(1);
}

void BgiThreadWaitOperation::OnWake()
{
	m_pSema->Release(1);
}

/*
*唯一对外开放接口
*/
void MiniDump::SaveDump(_EXCEPTION_POINTERS* ePtr, HANDLE currentThreadHandle)
{
	ThreadDumpCrashPara* crashpara = new ThreadDumpCrashPara(ePtr, currentThreadHandle);

	QThread* MinidumpThread = QThread::create(RunCrashDumpThreadFunc, crashpara);
	MinidumpThread->setPriority(QThread::NormalPriority);
	MinidumpThread->setObjectName("Thread_Dump");

	MinidumpThread->start();
	crashpara->m_pThreadOper->SetThreadSync(MinidumpThread);
	/*
	*释放资源
	*/
	delete crashpara;
	crashpara = NULL;
}
/*
*线程执行函数
*/
void MiniDump::CrashDumpThreadFunc(ThreadDumpCrashPara* crashpara)
{
	int code;
	QThread* CurrentThread = QThread::currentThread();
	__try
	{
		RunCrashDumpThreadFunc(crashpara);
	}
	__except (code = GetExceptionCode(), CurrentThread->quit(), EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

void MiniDump::RunCrashDumpThreadFunc(ThreadDumpCrashPara* crashpara)
{
	if (NULL != crashpara)
	{
		QString dumpDir = GetDumpDir();
		if (NULL != crashpara->m_ptr)
		{
			WriteExceptionTxt(dumpDir, crashpara);
		}

		QString dumpFilePath = GetDumpFilePath(dumpDir);
		/*
		*写dump
		*/
		WriteDump(dumpFilePath, crashpara);
	}
}

/*
*在txt文件中写入一些异常信息
*/
void MiniDump::WriteExceptionTxt(const QString& dir, ThreadDumpCrashPara* crashpara)
{

	QString dumptxtPath = GetDumpTxtPath(dir);

	QFile file(dumptxtPath);
	file.open(QIODevice::ReadWrite | QIODevice::Text);

	QTextStream Stream(&file);
	Stream.seek(file.size());//如果文件存在 文件末接着写

	_EXCEPTION_POINTERS* ErrorPtr = crashpara->m_ptr;
	/*
	*错误码
	*/
	QString ExceptionCode;
	switch (ErrorPtr->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		ExceptionCode.append(QStringLiteral("存储保护异常，错误代码:"));
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		ExceptionCode.append(QStringLiteral("数据类型未对齐异常，错误代码:"));
		break;
	case EXCEPTION_BREAKPOINT:
		ExceptionCode.append(QStringLiteral("中断异常，错误代码:"));
		break;
	case EXCEPTION_SINGLE_STEP:
		ExceptionCode.append(QStringLiteral("单步中断异常，错误代码:"));
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		ExceptionCode.append(QStringLiteral("数组越界异常，错误代码:"));
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
	case EXCEPTION_FLT_INEXACT_RESULT:
	case EXCEPTION_FLT_INVALID_OPERATION:
	case EXCEPTION_FLT_OVERFLOW:
	case EXCEPTION_FLT_STACK_CHECK:
	case EXCEPTION_FLT_UNDERFLOW:
		ExceptionCode.append(QStringLiteral("浮点数计算异常，错误代码:"));
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		ExceptionCode.append(QStringLiteral("被0除异常，错误代码 : "));
		break;
	case EXCEPTION_INT_OVERFLOW:
		ExceptionCode.append(QStringLiteral("数据溢出异常，错误代码:"));
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		ExceptionCode.append(QStringLiteral("页错误异常，错误代码:"));
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		ExceptionCode.append(QStringLiteral("非法指令异常，错误代码:"));
		break;
	case EXCEPTION_STACK_OVERFLOW:
		ExceptionCode.append(QStringLiteral("堆栈溢出异常，错误代码:"));
		break;
	case EXCEPTION_INVALID_HANDLE:
		ExceptionCode.append(QStringLiteral("无效句病异常，错误代码:"));
		break;
	default:
		if (ErrorPtr->ExceptionRecord->ExceptionCode & (1 << 29))
			ExceptionCode.append(QStringLiteral("用户自定义的软件异常，错误代码:"));
		else
			ExceptionCode.append(QStringLiteral("其它异常，错误代码:"));
		break;
	}
	ExceptionCode.append(QString("0X%1").arg(ErrorPtr->ExceptionRecord->ExceptionCode, 8, 16, QLatin1Char('0')));
	/*
	*错误标志
	*/
	QString ExceptionFlags = "ErrorFlags:" + QString::number(ErrorPtr->ExceptionRecord->ExceptionFlags);
	/*
	*错误地址
	*/
	/*char addr[MAX_ADDRESS_LENGTH];
	sprintf_s(addr, "%08X", ePtr->ExceptionRecord->ExceptionAddress);
	QString Qaddr(addr);*/
	QString Qaddr;
	QTextStream ts(&Qaddr);
	ts << ErrorPtr->ExceptionRecord->ExceptionAddress;
	QString ExceptionAddress = "ErrorAddr:" + Qaddr;
	/*
	*错误参数数量
	*/
	QString NumberParameters = "ErrorParaNums:" + QString::number(ErrorPtr->ExceptionRecord->NumberParameters);
	/*
	*堆栈信息等
	*/
	StackTraceInfo* stackInfo = GetStackTraceInfo(ErrorPtr->ContextRecord, crashpara->m_threadHandle);
	QString ExceptionStackInfo;
	ExceptionStackInfo.append("\nStackTrace: \n");
	if (stackInfo != nullptr)
	{
		for (int i = 0; i < stackInfo->StackNameList.count(); ++i)
		{
			ExceptionStackInfo.append("[MethodName]:" + stackInfo->StackNameList[i] + ";\0");
			if (stackInfo->StackModuleList.count() > i)
			{
				ExceptionStackInfo.append("[ModuleName]:" + stackInfo->StackModuleList[i] + ";\0 ");
			}
			if (stackInfo->StackFileList.count() > i)
			{
				ExceptionStackInfo.append("[File]:" + stackInfo->StackFileList[i] + ";\0");
			}
			if (stackInfo->StackNums.count() > i)
			{
				ExceptionStackInfo.append("[LineNum]:" + stackInfo->StackNums[i] + ";\n");
			}
			else
			{
				ExceptionStackInfo.append("\n");
			}
		}
		delete stackInfo;
		stackInfo = NULL;
	}

	Stream << ExceptionCode << "\n" << ExceptionFlags << "\n" << NumberParameters << "\n" << ExceptionAddress << "\n" << ExceptionStackInfo;
	file.close();
}
/*
*获取dump文件所在目录路径
*/
QString MiniDump::GetDumpDir()
{
	//QString dumpDirpath = LEnvironmentConfig::GetInstance()->GetString("DumpPath");
	QString dumpDirpath = "";
	if (dumpDirpath == "LoggerUserPath\\dump")
	{
		dumpDirpath = "D:\\";//表示此时LEnvironmentConfig还没有初始化就出现了异常，则会提供一个默认固定的dump路径来保存此异常信息.
	}
	QDir dir(dumpDirpath);
	dumpDirpath = dir.absolutePath();
	if (!dir.exists(dumpDirpath))
	{
		dir.mkdir(dumpDirpath);
	}
	return dumpDirpath;
}

QString MiniDump::GetDumpTxtPath(const QString& dir)
{
	QString path = dir + "\\dump-siminfo-" + QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss-ms") + ".txt";
	QDir Dir(path);
	return Dir.absolutePath();
}

QString MiniDump::GetDumpFilePath(const QString& dir)
{
	QString path = dir + "\\dump-" + QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss-ms") + ".dump";
	QDir Dir(path);
	return Dir.absolutePath();
}

/*
*获取异常堆栈信息
*/
StackTraceInfo* MiniDump::GetStackTraceInfo(CONTEXT* context, HANDLE hThread)
{
	if (context == nullptr)
	{
		return nullptr;
	}
	StackTraceInfo* errInfo = new StackTraceInfo();

	HANDLE hProcess = GetCurrentProcess();
	SymInitialize(hProcess, NULL, TRUE);

	DWORD dwImageType = IMAGE_FILE_MACHINE_AMD64;
	CONTEXT c = *context;
	STACKFRAME64 sf;
	memset(&sf, 0, sizeof(STACKFRAME64));//qt

#ifdef _M_IX86
	sf.AddrPC.Offset = c.Eip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.Esp;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.Ebp;
	sf.AddrFrame.Mode = AddrModeFlat;
#elif _M_X64
	dwImageType = IMAGE_FILE_MACHINE_AMD64;
	sf.AddrPC.Offset = c.Rip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.Rsp;
	sf.AddrFrame.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.Rsp;
	sf.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
	dwImageType = IMAGE_FILE_MACHINE_IA64;
	sf.AddrPC.Offset = c.StIIP;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.IntSp;
	sf.AddrFrame.Mode = AddrModeFlat;
	sf.AddrBStore.Offset = c.RsBSP;
	sf.AddrBStore.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.IntSp;
	sf.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif
	while (true)
	{
		/*
		*获取堆栈信息
		*/
		bool issuc = StackWalk64(dwImageType, hProcess, hThread, &sf, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL);
		if (!issuc)
		{
			break;
		}
		if (sf.AddrFrame.Offset == 0)
		{
			break;
		}

		BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL64) + MAX_NAME_LENGTH];
		IMAGEHLP_SYMBOL64 *pSymbol = (IMAGEHLP_SYMBOL64*)symbolBuffer;
		memset(pSymbol, 0, sizeof(IMAGEHLP_SYMBOL64) + MAX_NAME_LENGTH);//qt

		pSymbol->SizeOfStruct = sizeof(symbolBuffer);
		pSymbol->MaxNameLength = MAX_NAME_LENGTH;

		DWORD symDisplacement = 0;
		/*
		*得到函数名
		*/
		if (SymGetSymFromAddr64(hProcess, sf.AddrPC.Offset, NULL, pSymbol))
		{
			QString name(pSymbol->Name);
			errInfo->StackNameList.push_back(name);
		}

		IMAGEHLP_LINE64 lineInfo;
		memset(&lineInfo, 0, sizeof(IMAGEHLP_LINE64));//qt

		lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		DWORD dwLineDisplacement;
		/*
		* 得到文件名和所在的代码行
		*/
		if (SymGetLineFromAddr64(hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo))
		{
			QString file(lineInfo.FileName);
			errInfo->StackFileList.push_back(file);
			errInfo->StackNums.push_back(QString::number(lineInfo.LineNumber));
		}

		IMAGEHLP_MODULE64 moduleInfo;
		memset(&moduleInfo, 0, sizeof(IMAGEHLP_MODULE64));

		moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
		/*
		*得到模块名
		*/
		if (SymGetModuleInfo64(hProcess, sf.AddrPC.Offset, &moduleInfo))
		{
			QString module(moduleInfo.ModuleName);
			errInfo->StackModuleList.push_back(module);
		}
	}


	SymCleanup(hProcess);

	return errInfo;
}

/*
*写.dump文件
*/
void MiniDump::WriteDump(const QString& dir, ThreadDumpCrashPara* crashpara)
{

	MINIDUMP_EXCEPTION_INFORMATION eInfo;
	//eInfo.ThreadId = (DWORD)(QThread::currentThreadId());//?
	DWORD threadid = GetThreadId(crashpara->m_threadHandle);
	eInfo.ThreadId = threadid;
	eInfo.ExceptionPointers = crashpara->m_ptr;
	eInfo.ClientPointers = false;

	wchar_t wchardumpPath[256] = { 0 };
	dir.toWCharArray(wchardumpPath);
	HANDLE hFile = CreateFile(wchardumpPath, GENERIC_READ | GENERIC_WRITE, 0,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	/*
	*调用MiniDumpWriteDump API
	*/
	::MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, crashpara->m_ptr ? &eInfo : NULL, NULL, NULL);

}



