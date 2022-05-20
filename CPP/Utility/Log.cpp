#include <tchar.h>
#include <io.h>
#include "Log.h"


void LogDebug(LPCSTR category, LPCSTR pstrFormat, ...)
{
#if defined(DEBUG) || defined(_DEBUG)
	char buffer[1024];
	va_list args;
	va_start(args, pstrFormat);
	_vsnprintf_s(buffer, 511, 512, pstrFormat, args);
	va_end(args);

	LOG(DEBUG) << ("[") << category << ("]") << buffer;

	AddLogToListView(("[DEBUG][%s]%s"), category, buffer);

#endif

}

void LogTrace(LPCSTR category, LPCSTR pstrFormat, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, pstrFormat);
	_vsnprintf_s(buffer, 511, 512, pstrFormat, args);
	va_end(args);

	LOG(TRACE) << ("[") << category << ("]") << buffer;

	AddLogToListView(("[TRACE][%s]%s"), category, buffer);

}


void LogInfo(LPCSTR category, LPCSTR pstrFormat, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, pstrFormat);
	_vsnprintf_s(buffer, 511, 512, pstrFormat, args);
	va_end(args);

	//LOG(INFO) << _T("[") << category << _T("]") << buffer;
	LOG(INFO) << ("[") << category << ("]") << buffer;

	AddLogToListView(("[INFO][%s]%s"), category, buffer);
}

void LogWarning(LPCSTR category, LPCSTR pstrFormat, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, pstrFormat);
	_vsnprintf_s(buffer, 511, 512, pstrFormat, args);
	va_end(args);

	LOG(WARNING) << ("[") << category << ("]---") << buffer;

	AddLogToListView(("[WARN][%s]---%s"), category, buffer);
}

void LogError(LPCSTR category, LPCSTR pstrFormat, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, pstrFormat);
	_vsnprintf_s(buffer, 511, 512, pstrFormat, args);
	va_end(args);

	LOG(ERROR) << ("[") << category << ("]------") << buffer;

	AddLogToListView(("[ERROR][%s]------%s"), category, buffer);
}


Clog::Clog(const char *configFile)
{
	Init(configFile);
}

Clog::~Clog()
{
	LOG(INFO) << _T("*************** log end ***************") << std::endl << std::endl;
}

void Clog::Init(const char *configFileName)
{
	SetWorkDirectory();

	//启动日志
	if (-1 == (_access(configFileName, 0)))
	{
		return;
	}

	el::Configurations conf(configFileName);
	el::Loggers::reconfigureLogger("default", conf);
	el::Loggers::reconfigureAllLoggers(conf);
	LOG(INFO) << _T("*************** log start ***************");
}

void Clog::SetWorkDirectory()
{
	TCHAR filePath[MAX_PATH];
	GetModuleFileName(NULL, filePath, sizeof(filePath));
	TCHAR* p = _tcsrchr(filePath, _T('\\'));
	*p = '\0';

	// 设置当前工作目录，避免程序的相对路径改变时找不到配置文件
	SetCurrentDirectory(filePath);
}

//*/