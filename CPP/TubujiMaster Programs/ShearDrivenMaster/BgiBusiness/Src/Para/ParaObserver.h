/*
* @Abrief 参数观察者类
* @note 定义了ParaObserver接口, 通过实现该接口可以用于观察成像参数等的变化
* @version 1.0
* @author mosongjin 2021-07-27
*/

#ifndef _PARA_OBSERVER_H_
#define _PARA_OBSERVER_H_

#include "BusinessExternDef.h"

class BGIBUSINE_CLASS_EXPORT ParaObserver
{
public:
    virtual ~ParaObserver() {}
    virtual void Update() = 0;
};

#endif