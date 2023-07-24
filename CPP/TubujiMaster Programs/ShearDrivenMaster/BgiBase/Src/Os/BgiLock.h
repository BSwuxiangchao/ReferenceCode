/*
* @Abrief 加锁
* @note 临界区实现内部使用锁
* @version 1.0
* @author mosongjin 2021-07-20
*/

#ifndef _BGI_LOCK_H_
#define _BGI_LOCK_H_

#include "ExternDef.h"

class BGIBASE_CLASS_EXPORT BgiLock
{
public:
    BgiLock();

    ~BgiLock();

    void Acquire();

    bool TryAcquire();

    void Release();

    bool IsAcquired();

private:
    class LockPrivate* m_pLockPrivate;
};


#endif //BGI_LOCK_H !
