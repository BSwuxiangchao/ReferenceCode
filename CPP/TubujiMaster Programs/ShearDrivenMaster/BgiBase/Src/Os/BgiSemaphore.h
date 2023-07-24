/*
* @Abrief 信号量
* @note 信号量接口实现,Win32实现
* @version 1.0
* @author mosongjin 2021-07-15
*/

#ifndef _BGI_SEMAPHORE_H_
#define _BGI_SEMAPHORE_H_

#ifndef _BGI_EXTERN_DEF_H
#include "ExternDef.h"
#endif

#include<windows.h>



class BGIBASE_CLASS_EXPORT BgiSemaphore
{
public:
	BgiSemaphore(int init_count);
	~BgiSemaphore();
	void Acquire(int count);
	bool TryAcquire(int count);
	void Release(int count);

	HANDLE m_Sema;

};

#endif 


