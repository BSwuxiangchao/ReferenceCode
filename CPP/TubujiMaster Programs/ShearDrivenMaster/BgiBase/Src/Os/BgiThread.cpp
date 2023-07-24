#include "BgiThread.h"
#include "BgiThreadPrivate.h"
#include <functional>
#include "BgiLogger.h"
using namespace std;



BgiThread::BgiThread()
	:m_pThreadData(NULL)
{
	m_pThreadData = new BgiThreadPrivate();
	m_pThreadData->SetThreadProcFunc(
		std::bind(&BgiThread::Run, this)
	);

	BGI_MESSAGE("Thread[address:0x%08x, name:%s,id:0x%08x] construct.", this, GetName().c_str(), GetThreadId());
}

BgiThread::~BgiThread()
{
	BGI_MESSAGE("Thread[address:0x%08x, name:%s,id:0x%08x] destruct.", this, GetName().c_str(), GetThreadId());
	if (m_pThreadData)
	{
		delete m_pThreadData;
		m_pThreadData = NULL;
	}
}

void BgiThread::SetPriority(Priority priority)
{
	if (NULL != m_pThreadData)
	{
		m_pThreadData->SetPriority((int)priority);
	}

}

BgiThread::Priority BgiThread::GetPriority() const
{
	if (NULL != m_pThreadData)
	{
		return (Priority)m_pThreadData->Priority();
	}
	return Priority::InheritPriority;
}

void BgiThread::SetName(const char* p_name)
{
	if (NULL != m_pThreadData)
	{
		m_pThreadData->SetName(p_name);
	}

}

string BgiThread::GetName()
{
	if (NULL != m_pThreadData)
	{
		return m_pThreadData->GetName();
	}
	BGI_ASSERT(false);
	return string();
}


bool BgiThread::IsFinished() const
{
	if (NULL != m_pThreadData)
	{
		return m_pThreadData->IsFinished();
	}
	BGI_ASSERT(false);
	return true;

}

bool BgiThread::IsRunning() const
{
	if (NULL != m_pThreadData)
	{
		return m_pThreadData->IsRunning();
	}
	BGI_ASSERT(false);
	return false;
}


unsigned int BgiThread::GetThreadId()
{
	if (NULL != m_pThreadData)
	{
		return m_pThreadData->GetThreadId();
	}
	BGI_ASSERT(false);
	return 0;
}

unsigned int BgiThread::GetCurrentThreadId()
{
	return BgiThreadPrivate::GetCurrentThreadId();
}


void BgiThread::Sleep(unsigned long secs)
{
	return BgiThreadPrivate::Sleep(secs);
}

void BgiThread::Msleep(unsigned long msecs)
{
	return BgiThreadPrivate::Msleep(msecs);
}

void BgiThread::Usleep(unsigned long usecs)
{
	return BgiThreadPrivate::Usleep(usecs);
}


void BgiThread::Start(Priority priority)
{
	if (NULL != m_pThreadData)
	{
		m_pThreadData->Start((int)priority);
	}
}

int BgiThread::WaitForEnd()
{
	if (NULL != m_pThreadData)
	{
		return m_pThreadData->WaitForEnd();
	}
	BGI_ASSERT(false);
	return -1;

}


