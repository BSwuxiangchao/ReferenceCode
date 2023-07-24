#include "BgiSemaphore.h"


//#include "Bgilogger.h"




BgiSemaphore::BgiSemaphore(int init_count)
	:m_Sema(NULL)
{
	if (init_count < 0)
	{
		//IM_ASSERT_X(false, "Semaphore init count < 0");
		init_count = 0;
	}
	m_Sema = CreateSemaphore(NULL, init_count, LONG_MAX, NULL);
}

BgiSemaphore::~BgiSemaphore()
{
	if (m_Sema != NULL)
		CloseHandle(m_Sema);
}

void BgiSemaphore::Release(int count)
{
	if (m_Sema != NULL)
		ReleaseSemaphore(m_Sema, count, NULL);
}

void BgiSemaphore::Acquire(int count)
{
	for (int i = 0; i < count; i++)
	{
		if (m_Sema != NULL)
			WaitForSingleObject(m_Sema, INFINITE);
	}
}

bool BgiSemaphore::TryAcquire(int count)
{
	//暂时没有实现
	//IM_ASSERT(false);
	return false;
}

