// Copyright 2020 s_dragon.

// 日志组件，各种日志目标：可对日志文件进行循环输出；可每日生成日志文件；支持控制台日志输出(支持颜色)；系统日志
// Windows debugger；较容易扩展自定义日志目标；

/* $Header: /MyLog/MyLog.cpp      6/25/2020 10:23a s_dragon $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  --- S_DRAGON                                      ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : MyLog                                                        *
 *                                                                                             *
 *                    File Name : MyLog.cpp                                                    *
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

#include "MyLog.h"
#include "basic_file_sink.h"
#include "daily_file_sink.h"
#include "rotating_file_sink.h"
#include "async.h"


MyLog* MyLog::m_log = nullptr;

MyLog* MyLog::Instance()
{
	if (m_log == nullptr)
	{
		m_log = new MyLog;
	}
	return m_log;
}
MyLog::MyLog()
{
	fileLog = false;       //文件日志开关
	terminalLog = false;   //终端日志开关
	dailyLog = false;      //定时文件日志开关
	rotatingLog = false;   //循环日志开关
}
MyLog::~MyLog()
{
	EndLog();
}
void MyLog::InitFileMyLog(std::string file_name, int log_level)
{
	//设置日志等级
	spdlog::set_level(static_cast<spdlog::level::level_enum>(log_level));
	//获取线程号作为logger_name
	std::thread::id _gid = std::this_thread::get_id();
	std::stringstream logname;
	logname.clear();
	//设置输出流格式
	logname << "f" << std::setw(10) << std::setiosflags(std::ios::right) << std::setfill('*') << _gid;
	//设置日志为异步日志，不带滚动，日志文件会一直写入
	//spdlog::init_thread_pool(32768, 1);  //改变默认线程池
	my_logger_ = spdlog::basic_logger_mt<spdlog::async_factory >(logname.str(), file_name);
	fileLog = true;
	//当遇到错误级别以上的立刻刷新到日志
	my_logger_->flush_on(spdlog::level::err);
	//每三秒刷新一次
	spdlog::flush_every(std::chrono::seconds(3));

	//测试
	/*for (int i = 0; i < 101; i++)
	{
		my_logger_->info("MyLog::Async message #{}", i);
	}*/
}
void MyLog::InitTerminalMyLog(TERMINAL_COLOR color)
{
	//获取线程号作为logger_name
	std::thread::id _gid = std::this_thread::get_id();
	std::stringstream logname;
	logname.clear();
	//设置输出流格式
	logname << "t" << std::setw(10) << std::setiosflags(std::ios::right) << std::setfill('*') << _gid;
	spdlog::color_mode _color = (spdlog::color_mode)color;
	my_terminallog = spdlog::stdout_color_mt(logname.str(), _color);
	terminalLog = true;
	//当遇到错误级别以上的立刻刷新到日志
	my_terminallog->flush_on(spdlog::level::err);
	//每三秒刷新一次
	spdlog::flush_every(std::chrono::seconds(3));
}
void MyLog::InitDailyLog(std::string file_name, int hour, int minute)
{
	//获取线程号作为logger_name
	std::thread::id _gid = std::this_thread::get_id();
	std::stringstream logname;
	logname.clear();
	//设置输出流格式
	logname << "d" << std::setw(10) << std::setiosflags(std::ios::right) << std::setfill('*') << _gid;
	my_dailylog = spdlog::daily_logger_mt(logname.str(), file_name, hour, minute);
	dailyLog = true;
	//当遇到错误级别以上的立刻刷新到日志
	my_dailylog->flush_on(spdlog::level::err);
	//每三秒刷新一次
	spdlog::flush_every(std::chrono::seconds(3));
}
void MyLog::InitRotatingLog(std::string filename, size_t logsize, size_t lognum)
{
	//获取线程号作为logger_name
	std::thread::id _gid = std::this_thread::get_id();
	std::stringstream logname;
	logname.clear();
	//设置输出流格式
	logname << "r" << std::setw(10) << std::setiosflags(std::ios::right) << std::setfill('*') << _gid;
	my_rotatingLog = spdlog::rotating_logger_mt(logname.str(), filename, 1048576 * logsize, lognum);
	rotatingLog = true;
	//当遇到错误级别以上的立刻刷新到日志
	my_rotatingLog->flush_on(spdlog::level::err);
	//每三秒刷新一次
	spdlog::flush_every(std::chrono::seconds(3));
}
void MyLog::EndLog()
{
	spdlog::shutdown();
}
void MyLog::SetLevel(int level)
{
	spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
}
void MyLog::SetFLevel(int level)
{
	my_logger_->set_level(static_cast<spdlog::level::level_enum>(level));
}
void MyLog::SetTLevel(int level)
{
	my_terminallog->set_level(static_cast<spdlog::level::level_enum>(level));
}
void MyLog::SetDLevel(int level)
{
	my_dailylog->set_level(static_cast<spdlog::level::level_enum>(level));
}
void MyLog::SetRLevel(int level)
{
	my_rotatingLog->set_level(static_cast<spdlog::level::level_enum>(level));
}
void MyLog::ApplyAll()
{
	spdlog::apply_all([&](std::shared_ptr<spdlog::logger> lf) { lf->info("End of example."); });
}
std::string MyLog::GetId()
{
	//std::mutex mt;
	//std::unique_lock<std::mutex>lock(mt);
	std::thread::id _gid = std::this_thread::get_id();
	std::stringstream s_pid;
	s_pid.clear();
	//设置输出流格式
	s_pid << std::setw(10) << std::setiosflags(std::ios::right) << std::setfill('*') << _gid;
	return s_pid.str();
}
