/*
* @Abrief 线程
* @note 线程内部实现,QT实现
* @version 1.0
* @author mosongjin 2021-07-15
*/

#ifndef _BGI_THREAD_PRIVATE_H
#define _BGI_THREAD_PRIVATE_H

#include <functional>
#include <QObject>
#include "qthread.h"
#include "qsemaphore.h"
#include <string>

using std::string;


//Thread Qt实现，不导出
class BgiThreadPrivate : public QObject
{
	Q_OBJECT
public:
	BgiThreadPrivate();
	~BgiThreadPrivate();

	void SetThreadProcFunc(std::function<void(void)> func);

	void SetPriority(int priority);
	int Priority() const;
	void SetName(const char* p_name);
	string GetName();
	bool IsFinished() const;
	bool IsRunning() const;

	unsigned int GetThreadId();
	static unsigned int GetCurrentThreadId();
	static void Sleep(unsigned long secs);
	static void Msleep(unsigned long msecs);
	static void Usleep(unsigned long usecs);

	void Start(int priority);
	//禁止使用terminate退出线程，terminate()退出线程过于粗暴，造成资源不能释放，甚至互斥锁还处于加锁状态
	//通过在线程类中增加标识变量volatile bool m_stop,通过m_stop变量的值判断run函数是否执行结束返回
	int WaitForEnd();//等待线程退出run结束
	void Run();

	public Q_SLOTS:
	void TryRun();
	//void run();
	void End();

private:
	std::function<void(void)> m_ThreadProcFunc;
	QThread* m_pQThread;
	QSemaphore* m_pQSemaphore;
	int m_ThreadId;
	QString m_strThreadName;
};

//

#endif





