#pragma once
#include<vector>
#include<mutex>
#include "MyLog.h"

#define _DESC_LOG_DLL

#ifdef _DESC_LOG_DLL 
#define DLL_EXPORT_API _declspec(dllexport)
#else
#define DLL_EXPORT_API _declspec(dllimport)
#endif

//

//
class SzlLog
{
public:
	SzlLog()
	{
		m_log.clear();
	}
	~SzlLog()
	{

	}
	template<typename _tx, typename... Args>
	void _trace(const _tx&& msg, const Args&...args)
	{
		auto it = m_log.begin(), itend = m_log.end();
		for (; it != itend; ++it)
		{
			it->mTrace(msg, args ...);
		}
	}
	template<typename _tx, typename... Args>
	void _debug(const _tx& msg, const Args&...args)
	{
		auto it = m_log.begin(), itend = m_log.end();
		for (; it != itend; ++it)
		{
			it->mDebug(msg, args...);
		}
	}
	template<typename _tx, typename... Args>
	void _info(const _tx& msg, const Args&...args)
	{
		auto it = m_log.begin(), itend = m_log.end();
		for (; it != itend; ++it)
		{
			it->mInfo(msg, args...);
		}
	}
	template<typename _tx, typename... Args>
	void _warn(const _tx& msg, const Args&...args)
	{
		auto it = m_log.begin(), itend = m_log.end();
		for (; it != itend; ++it)
		{
			it->mWarn(msg, args...);
		}
	}
	template<typename _tx, typename... Args>
	void _error(const _tx& msg, const Args&...args)
	{
		auto it = m_log.begin(), itend = m_log.end();
		for (; it != itend; ++it)
		{
			it->mError(msg, args...);
		}
	}
	template<typename _tx, typename... Args>
	void _fatal(const _tx& msg, const Args&...args)
	{
		auto it = m_log.begin(), itend = m_log.end();
		for (; it != itend; ++it)
		{
			it->mFatal(msg, args...);
		}
	}

	inline void FillLog(MyLog& _log)
	{
		Lock();
		auto it = find(m_log.begin(), m_log.end(), _log);
		if (it == m_log.end())
			m_log.emplace_back(_log);
	}
	inline void Lock()
	{
		std::unique_lock<std::mutex>lock(mut);
	}
private:
	std::vector<MyLog>m_log;
	std::mutex mut;
};
