#pragma once
#ifndef LOG_H__
#define LOG_H__

#include "easylogging++.h"

/*
* 文件说明：统一的日志输出模块，封装了easylogging++接口
* 注意：AddLogMsg函数实现因为关联了各个模块的日志显示窗口，需要自己实现
*/

// 输出日志到各个模块的日志显示窗口，需要各个模块自己实现
void AddLogToListView(const char* strMsg, ...);

void LogDebug(LPCSTR category, LPCSTR pstrFormat, ...);
void LogInfo(LPCSTR category, LPCSTR pstrFormat, ...);
void LogTrace(LPCSTR category, LPCSTR pstrFormat, ...);
void LogWarning(LPCSTR category, LPCSTR pstrFormat, ...);
void LogError(LPCSTR category, LPCSTR pstrFormat, ...);


class Clog
{
public:
	Clog(const char *configFile);
	~Clog();

private:
	void Init(const char *configFileName);
	void SetWorkDirectory();
};




#endif // !LOG_H_

