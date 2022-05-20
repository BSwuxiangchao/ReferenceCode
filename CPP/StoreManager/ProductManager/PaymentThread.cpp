#include "stdafx.h"
#include "PaymentThread.h"


PaymentThread::PaymentThread(enum ThreadId thread_id, const char *name)
	:FrameworkThread(name), m_threadId(thread_id)
{

}

PaymentThread::~PaymentThread()
{
}

void PaymentThread::Init()
{
	nbase::ThreadManager::RegisterThread(m_threadId);
}

void PaymentThread::Cleanup()
{
	nbase::ThreadManager::UnregisterThread();
}

