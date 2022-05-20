#include "stdafx.h"
#include "MiscThread.h"


MiscThread::MiscThread(enum ThreadId thread_id, const char *name)
	:FrameworkThread(name), m_threadId(thread_id)
{

}

MiscThread::~MiscThread()
{
}

void MiscThread::Init()
{
	nbase::ThreadManager::RegisterThread(m_threadId);
}

void MiscThread::Cleanup()
{
	nbase::ThreadManager::UnregisterThread();
}

