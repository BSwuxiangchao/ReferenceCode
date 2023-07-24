/*
* @Abrief 线程类
* @note 提供线程的操作接口,QT实现 C++19
* @version 1.0
* @author mosongjin 2021-07-15
*/

#ifndef _BGI_THREAD_H_
#define _BGI_THREAD_H_

#ifndef _BGI_EXTERN_DEF_H
#include "ExternDef.h"
#endif

#include <string>
using std::string;

class BgiThreadPrivate;
class BGIBASE_CLASS_EXPORT BgiThread
{
	friend class BgiThreadPrivate;

public:
	enum Priority
	{
		IdlePriority,

		LowestPriority,
		LowPriority,
		NormalPriority,
		HighPriority,
		HighestPriority,

		TimeCriticalPriority,

		InheritPriority
	};

public:
	BgiThread();
	~BgiThread();

	void SetPriority(Priority priority);
	Priority GetPriority() const;
	void SetName(const char* p_name);
	string GetName();
	bool IsFinished() const;
	bool IsRunning() const;

	unsigned int GetThreadId();
	static unsigned int GetCurrentThreadId();
	//毫秒（millisecond, ms）--> 微秒（microsecond, μs）-->纳秒（nanosecond, ns）
	static void Sleep(unsigned long secs);//secs seconds
	static void Msleep(unsigned long msecs);//milliseconds
	static void Usleep(unsigned long usecs);//microseconds


	void Start(Priority priority = InheritPriority);
	//禁止使用terminate退出线程，terminate()退出线程过于粗暴，造成资源不能释放，甚至互斥锁还处于加锁状态
	//通过在线程类中增加标识变量volatile bool m_stop,通过m_stop变量的值判断run函数是否执行结束返回
	int WaitForEnd();//等待线程退出run结束

protected:
	virtual void Run(void) = 0;

private:
	BgiThread(const BgiThread&);
	BgiThread& operator=(const BgiThread&);

protected:
	BgiThreadPrivate* m_pThreadData;
};


#endif