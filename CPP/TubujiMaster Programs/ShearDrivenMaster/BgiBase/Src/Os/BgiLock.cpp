#include "BgiLock.h"

#include <windows.h>

class LockPrivate
{
public:
	LockPrivate();
	~LockPrivate();

	void Acquire();

	bool TryAcquire();

	void Release();

	bool IsAcquired();

public:
	CRITICAL_SECTION m_CriticalSection;
	DWORD m_AcquiredThreadId;
};

//=======================================

LockPrivate::LockPrivate()
{
	::InitializeCriticalSection(&m_CriticalSection);
}

LockPrivate::~LockPrivate()
{
	::DeleteCriticalSection(&m_CriticalSection);
}


void LockPrivate::Acquire()
{
	::EnterCriticalSection(&m_CriticalSection);
	m_AcquiredThreadId = ::GetCurrentThreadId();
}

bool LockPrivate::TryAcquire()
{
	BOOL result = ::TryEnterCriticalSection(&m_CriticalSection);
	if (result == TRUE)
		m_AcquiredThreadId = ::GetCurrentThreadId();
	return (result == TRUE);
}

void LockPrivate::Release()
{
	::LeaveCriticalSection(&m_CriticalSection);
}

bool LockPrivate::IsAcquired()
{
	DWORD current_thread_id = ::GetCurrentThreadId();
	return (m_AcquiredThreadId == current_thread_id);
}

//=======================================

BgiLock::BgiLock()
	: m_pLockPrivate(NULL)
{
	m_pLockPrivate = new LockPrivate();
}

BgiLock::~BgiLock()
{
	delete m_pLockPrivate;
	m_pLockPrivate = NULL;
}

void BgiLock::Acquire()
{
	m_pLockPrivate->Acquire();
}

bool BgiLock::TryAcquire()
{
	return m_pLockPrivate->TryAcquire();
}

void BgiLock::Release()
{
	m_pLockPrivate->Release();
}

bool BgiLock::IsAcquired()
{
	return m_pLockPrivate->IsAcquired();
}

