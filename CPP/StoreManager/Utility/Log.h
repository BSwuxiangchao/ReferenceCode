#pragma once
#ifndef LOG_H__
#define LOG_H__

#include "easylogging++.h"

/*
* �ļ�˵����ͳһ����־���ģ�飬��װ��easylogging++�ӿ�
* ע�⣺AddLogMsg����ʵ����Ϊ�����˸���ģ�����־��ʾ���ڣ���Ҫ�Լ�ʵ��
*/

// �����־������ģ�����־��ʾ���ڣ���Ҫ����ģ���Լ�ʵ��
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

