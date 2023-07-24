/*
* @Abrief 比较值
* @note 定义比较变量值是否相等的接口
* @version 1.0
* @author mosongjin 2021-07-27
*/


#ifndef _EQUAL_PRED_H_
#define _EQUAL_PRED_H_

#include <typeinfo>


class EqualPred
{
public:
    virtual ~EqualPred() {}
    virtual bool IsEqual(const void* p1, const void* p2, const std::type_info& info) const = 0;
};



#endif
