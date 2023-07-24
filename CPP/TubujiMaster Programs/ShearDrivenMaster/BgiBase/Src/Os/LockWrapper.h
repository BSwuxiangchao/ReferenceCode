/*
* @Abrief 加锁
* @note 临界区实现内部使用锁
* @version 1.0
* @author mosongjin 2021-07-20
*/

#ifndef _BGI_LOCK_WRAPPER_H_
#define _BGI_LOCK_WRAPPER_H_

#include "ExternDef.h"
#include "BgiLock.h"


class BGIBASE_CLASS_EXPORT LockWrapper
{
public:
    LockWrapper(BgiLock* p_lock);
    ~LockWrapper();
private:
    BgiLock* m_pLock;
};

#ifndef GETPOSTLOCK
#define GETPOSTLOCK() LockWrapper wrapper(m_Lock)
#endif



#endif //TOOL_LOCK_H !
