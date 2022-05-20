// Copyright 2020 s_dragon.

// 日志组件，支持多端打印，各种日志目标：可对日志文件进行循环输出；可每日生成日志文件；支持控制台日志输出(支持颜色)；
// 系统日志Windows debugger；较容易扩展自定义日志目标；

/* $Header: /MyLog/MyLog.h      6/15/2021 10:18a s_dragon $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  --- S_DRAGON                                      ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : MyLog                                                        *
 *                                                                                             *
 *                    File Name : MyLog.h                                                      *
 *                                                                                             *
 *                   Programmer : s_dragon                                                     *
 *                                                                                             *
 *                   Start Date : June 25, 2020                                                *
 *                                                                                             *
 *                  Last Update : June 25, 2020 [s_dragon]                                     *
 *                                                                                             *
 *-------------------------------------------------------------------------------------------- */
 /*  Functions:                                                                                *
*           MyLog::MyLog                --构造函数                                             *
*           MyLog::~MyLog               --析构函数                                             *
*           MyLog::InitFileMyLog        --初始化，写入文件                                     *
*           MyLog::InitTerminalMyLog    --初始化，写入终端                                     *
*           MyLog::InitDailyLog         --初始化，写入定时日志文件                             *
*           MyLog::InitRotatingLog      --初始化，大小限制，循环日志文件                       *
*           MyLog::EndLog               --结束日志                                             *
*           MyLog::SetLevel             --设置全局日志级别                                     *
*           MyLog::SetFLevel            --设置文件端日志级别                                   *
*           MyLog::SetTLevel            --设置终端日志级别                                     *
*           MyLog::SetDLevel            --设置定时日志级别                                     *
*           MyLog::SetRLevel            --设置循环日志级别                                     *
*           MyLog::ApplyAll             --应用到全局                                           *
*           MyLog::GetId                --获取线程id                                           *
*
* --------------------------------------------------------------------------------------------*/

#pragma once
#ifndef _MY_LOG_DRAGON_
#define _MY_LOG_DRAGON_
#include <stdarg.h>
#include<sstream>
#include<thread>
#include<iomanip>
#include<mutex>
#include "spdlog.h"
#include "stdout_color_sinks.h"
//#include "MyLog_export.h"

#define _DESC_LOG_DLL

#ifdef _DESC_LOG_DLL 
#define DLL_EXPORT_API _declspec(dllexport)
#else
#define DLL_EXPORT_API _declspec(dllimport)
#endif

/// 日志级别
enum LOG_LEVEL
{
	LEVEL_TRACE = 0,  /**< TRACE级别，最低*/
	LEVEL_DEBUG,    /**< Debug级别*/
	LEVEL_INFO,     /**< Info级别*/
	LEVEL_WARN,     /**< Warn级别*/
	LEVEL_ERROR,    /**< Error级别*/
	LEVEL_FATAL,    /**< Fatal级别*/
	LEVEL_OFF,
	LEVEL_NOLEVEL
};

///终端颜色支持
enum TERMINAL_COLOR
{
	COLOR_ALWAYS = spdlog::color_mode::always,
	COLOR_AUTOMATIC = spdlog::color_mode::automatic,
	COLOR_NEVER = spdlog::color_mode::never,
	COLOR_END
};

#ifdef _WIN32
//strrchr:查找字符在指定字符串从右面开始的第一次出现的位置，如果成功，返回该字符以及后面的字符，如果失败，返回NULL
//strcgr:查找字符在指定字符串首次出现的位置
#define __FILENAME__ (strrchr(__FILE__,'\\')?(strrchr(__FILE__,'\\')+1):__FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__,'/')?(strrchr(__FILE__,'/')+1):__FILE__)
#endif //_WIN32

#ifndef SUFFIX
//在日志后面追加线程号，文件名，函数名，行号
#define SUFFIX(msg) std::string(msg)\
                    .append(",  [file:")\
                    .append(__FILENAME__).append("(")\
                    .append(std::to_string(__LINE__)).append(")],  [function:")\
                    .append(__FUNCTION__)\
                    .append("]").c_str()
#endif //suffix

/*
日志等级：LEVEL_TRACE，LEVEL_DEBUG, LEVEL_INFO, LEVEL_WARN,LEVEL_ERROR, LEVEL_FATAL, LEVEL_OFF,LEVEL_NOLEVEL
使用方法：包含MyLog.h头文件,调用初始化函数,使用LDebug等打印日志信息
例：
MyLog::Instance().InitFileMyLog("logger_name", "testlog.txt");
int i = 10;
double d_number = 10.01;
SZL_TRACE("MyLog::Async message");
SZL_TRACE("MyLog::Async message #{0},d_number:{1}", i,d_number);
注：使用{}格式化字符串，里面的数字为占位符
*/

/* 根据初始化日志类型选择日志打印方式，实现多端打印*/

#define SZL_TRACE(msg,...)         MyLog::Instance()->mTrace(SUFFIX(msg), __VA_ARGS__)
													
#define SZL_DEBUG(msg,...)         MyLog::Instance()->mDebug(SUFFIX(msg),__VA_ARGS__)
													
#define SZL_INFO(msg,...)          MyLog::Instance()->mInfo(SUFFIX(msg),__VA_ARGS__)   
													
#define SZL_WARN(msg,...)          MyLog::Instance()->mWarn(SUFFIX(msg),__VA_ARGS__)
													
#define SZL_ERROR(msg,...)         MyLog::Instance()->mError(SUFFIX(msg),__VA_ARGS__)
													
#define SZL_FATAL(msg,...)         MyLog::Instance()->mFatal(SUFFIX(msg),__VA_ARGS__)


class DLL_EXPORT_API MyLog
{
public:
	static MyLog* Instance();
	void InitFileMyLog(std::string file_name, int log_level = LEVEL_TRACE);  //默认为最低级
	void InitTerminalMyLog(TERMINAL_COLOR color = COLOR_AUTOMATIC);           //默认终端颜色为自动
	void InitDailyLog(std::string file_name, int hour = 0, int minute = 0);        //默认每天00:00创建日志
	void InitRotatingLog(std::string filename, size_t logsize = 20, size_t lognum = 5); //默认大小20M,5个循环文件
	void EndLog();
	template<typename _tx, typename... Args>
	void mTrace(const _tx& msg, const Args&...args)const
	{
		if (fileLog)
		{
			my_logger_->trace(msg, args...);
		}
		if (terminalLog)
		{
			my_terminallog->trace(msg, args...);
		}
		if (dailyLog)
		{
			my_dailylog->trace(msg, args...);
		}
		if (rotatingLog)
		{
			my_rotatingLog->trace(msg, args...);
		}
	}
	template<typename _tx, typename... Args>
	void mDebug(const _tx& msg, const Args&...args)const
	{
		if (fileLog)
		{
			my_logger_->debug(msg, args...);
		}
		if (terminalLog)
		{
			my_terminallog->debug(msg, args...);
		}
		if (dailyLog)
		{
			my_dailylog->debug(msg, args...);
		}
		if (rotatingLog)
		{
			my_rotatingLog->debug(msg, args...);
		}
	}
	template<typename _tx, typename... Args>
	void mInfo(const _tx& msg, const Args&...args)const
	{
		if (fileLog)
		{
			my_logger_->info(msg, args...);
		}
		if (terminalLog)
		{
			my_terminallog->info(msg, args...);
		}
		if (dailyLog)
		{
			my_dailylog->info(msg, args...);
		}
		if (rotatingLog)
		{
			my_rotatingLog->info(msg, args...);
		}
	}
	template<typename _tx, typename... Args>
	void mWarn(const _tx& msg, const Args&...args)const
	{
		if (fileLog)
		{
			my_logger_->warn(msg, args...);
		}
		if (terminalLog)
		{
			my_terminallog->warn(msg, args...);
		}
		if (dailyLog)
		{
			my_dailylog->warn(msg, args...);
		}
		if (rotatingLog)
		{
			my_rotatingLog->warn(msg, args...);
		}
	}
	template<typename _tx, typename... Args>
	void mError(const _tx& msg, const Args&...args)const
	{
		if (fileLog)
		{
			my_logger_->error(msg, args...);
		}
		if (terminalLog)
		{
			my_terminallog->error(msg, args...);
		}
		if (dailyLog)
		{
			my_dailylog->error(msg, args...);
		}
		if (rotatingLog)
		{
			my_rotatingLog->error(msg, args...);
		}
	}
	template<typename _tx, typename... Args>
	void mFatal(const _tx& msg, const Args&...args)const
	{
		if (fileLog)
		{
			my_logger_->critical(msg, args...);
		}
		if (terminalLog)
		{
			my_terminallog->critical(msg, args...);
		}
		if (dailyLog)
		{
			my_dailylog->critical(msg, args...);
		}
		if (rotatingLog)
		{
			my_rotatingLog->critical(msg, args...);
		}
	}
	void SetLevel(int level = LEVEL_TRACE);   //默认日志等级为最低级，打印所有级别日志.
	void SetFLevel(int level = LEVEL_TRACE);  //默认日志等级为最低级，打印所有级别日志.
	void SetTLevel(int level = LEVEL_TRACE);  //默认日志等级为最低级，打印所有级别日志.
	void SetDLevel(int level = LEVEL_TRACE);  //默认日志等级为最低级，打印所有级别日志.
	void SetRLevel(int level = LEVEL_TRACE);  //默认日志等级为最低级，打印所有级别日志.
	void ApplyAll();
	std::string GetId();
	inline bool IsFileLog()
	{
		return fileLog;
	}
	inline bool IsTerminalLog()
	{
		return terminalLog;
	}
	inline bool IsDailyLog()
	{
		return dailyLog;
	}
	inline bool IsRotaingLog()
	{
		return rotatingLog;
	}
	inline auto GetLogger()
	{
		return my_logger_;
	}
	inline auto GetTlogger()
	{
		return my_terminallog;
	}
	inline auto GetDlogger()
	{
		return my_dailylog;
	}
	inline auto GetRlogger()
	{
		return my_rotatingLog;
	}
	inline void Lock()
	{
		std::unique_lock<std::mutex>lock(mut);
	}
public:
	static MyLog* m_log;
private:
	//私有构造函数，拷贝构造函数和拷贝赋值函数，禁止在类外声明实例
	MyLog();
	~MyLog();
	MyLog(const MyLog& _e) = delete;
	//MyLog(const MyLog& _e) //拷贝构造函数
	//{
	//	fileLog = _e.fileLog;
	//	terminalLog = _e.terminalLog;
	//	dailyLog = _e.dailyLog;
	//	rotatingLog = _e.rotatingLog;
	//	my_logger_ = _e.my_logger_;
	//	my_terminallog = _e.my_terminallog;
	//	my_dailylog = _e.my_dailylog;
	//	my_rotatingLog = _e.my_rotatingLog;
	//}
	//MyLog(MyLog&& _e)noexcept         //移动构造函数
	//{
	//	fileLog = _e.fileLog;
	//	terminalLog = _e.terminalLog;
	//	dailyLog = _e.dailyLog;
	//	rotatingLog = _e.rotatingLog;
	//	my_logger_ = std::move(_e.my_logger_);
	//	my_terminallog = std::move(_e.my_terminallog);
	//	my_dailylog = std::move(_e.my_dailylog);
	//	my_rotatingLog = std::move(_e.my_rotatingLog);
	//}
	//MyLog& operator=(const MyLog& _e) //
	//{
	//	fileLog = _e.fileLog;
	//	terminalLog = _e.terminalLog;
	//	dailyLog = _e.dailyLog;
	//	rotatingLog = _e.rotatingLog;
	//	my_logger_ = _e.my_logger_;
	//	my_terminallog = _e.my_terminallog;
	//	my_dailylog = _e.my_dailylog;
	//	my_rotatingLog = _e.my_rotatingLog;
	//	return *this;
	//}
	//bool operator==(const MyLog& _e)const
	//{
	//	return (fileLog == _e.fileLog
	//		&& terminalLog == _e.terminalLog
	//		&& dailyLog == _e.dailyLog
	//		&& rotatingLog == _e.rotatingLog
	//		&& my_logger_ == _e.my_logger_
	//		&& my_terminallog == _e.my_terminallog
	//		&& my_dailylog == _e.my_dailylog
	//		&& my_rotatingLog == _e.my_rotatingLog);
	//}
private:
	bool fileLog;       //文件日志开关
	bool terminalLog;   //终端日志开关
	bool dailyLog;      //定时文件日志开关
	bool rotatingLog;   //循环日志开关
	std::shared_ptr<spdlog::logger> my_logger_;              //日志输出到文件
	std::shared_ptr<spdlog::logger> my_terminallog;          //日志输出到终端
	std::shared_ptr<spdlog::logger> my_dailylog;             //每天定时创建新日志文件
	std::shared_ptr<spdlog::logger> my_rotatingLog;          //日志输出到循环文件
	std::mutex mut;
};
#endif//_MY_LOG_DRAGON_
