#include "LockWrapper.h"
#include <windows.h>

LockWrapper::LockWrapper(BgiLock* p_lock)
	: m_pLock(p_lock)
{
	if (NULL != m_pLock)
		m_pLock->Acquire();
}

LockWrapper::~LockWrapper()
{
	if (NULL != m_pLock)
		m_pLock->Release();
	m_pLock = NULL;
}
