#include "BgiThreadPrivate.h"
#include <functional>

#include "BgiLogger.h"
#include "MiniDump.h"


BgiThreadPrivate::BgiThreadPrivate()
	: m_pQThread(NULL)
	, m_pQSemaphore(NULL)
	, m_ThreadId(0)
{
	m_pQThread = new QThread();
	connect(m_pQThread, SIGNAL(started()), this, SLOT(TryRun()));//run
	connect(m_pQThread, SIGNAL(finished()), this, SLOT(End()));

	moveToThread(m_pQThread);
}

BgiThreadPrivate::~BgiThreadPrivate()
{
	if (m_pQThread)
	{
		delete m_pQThread;
		m_pQThread = NULL;
	}

	if (m_pQSemaphore)
	{
		delete m_pQSemaphore;
		m_pQSemaphore = NULL;
	}
}

void BgiThreadPrivate::SetThreadProcFunc(std::function<void(void)> func)
{
	m_ThreadProcFunc = func;
}

void BgiThreadPrivate::TryRun()
{
	int code;
	m_ThreadId = quintptr(QThread::currentThreadId());
	__try
	{
		Run();
	}
	__except (code = GetExceptionCode(), MiniDump::SaveDump(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER)
	{}
}

void BgiThreadPrivate::Run()
{
	m_pQSemaphore = new QSemaphore(0);
	if (m_ThreadProcFunc)
	{
		BGI_MESSAGE(true, "LThread[name:%s, id:0x%08x] run.", GetName().c_str(), GetThreadId());
		m_ThreadProcFunc();
	}
	else
	{
		BGI_MESSAGE(true, "LThread[name:%s, id:0x%08x] run, but ProcFunc null.", GetName().c_str(), GetThreadId());
	}
}

void BgiThreadPrivate::End()
{
	if (m_pQSemaphore)
	{
		m_pQSemaphore->release();
	}
}

void BgiThreadPrivate::SetPriority(int priority)
{
	m_pQThread->setPriority((QThread::Priority)priority);
}

int BgiThreadPrivate::Priority() const
{
	return (int)m_pQThread->priority();
}

void BgiThreadPrivate::SetName(const char* p_name)
{
	m_pQThread->setObjectName(p_name);
}

string BgiThreadPrivate::GetName()
{
	if (NULL != m_pQThread)
	{
		m_strThreadName = m_pQThread->objectName();
	}
	return m_strThreadName.toStdString();
}

bool BgiThreadPrivate::IsFinished() const
{
	return m_pQThread->isFinished();
}

bool BgiThreadPrivate::IsRunning() const
{
	return m_pQThread->isRunning();
}


unsigned int BgiThreadPrivate::GetThreadId()
{
	return m_ThreadId;
}

unsigned int BgiThreadPrivate::GetCurrentThreadId()
{
	return quintptr(QThread::currentThreadId());
}

void BgiThreadPrivate::Sleep(unsigned long secs)
{
	return QThread::sleep(secs);
}

void BgiThreadPrivate::Msleep(unsigned long msecs)
{
	return QThread::msleep(msecs);
}

void BgiThreadPrivate::Usleep(unsigned long usecs)
{
	return QThread::usleep(usecs);
}



void BgiThreadPrivate::Start(int priority)
{
	if (NULL != m_pQThread)
	{
		m_pQThread->start((QThread::Priority)priority);
	}
}

int BgiThreadPrivate::WaitForEnd()
{
	m_pQThread->quit();
	m_pQThread->wait();
	if (NULL != m_pQSemaphore)
	{
		//m_pQSemaphore->acquire();
	}
	return 0;
}