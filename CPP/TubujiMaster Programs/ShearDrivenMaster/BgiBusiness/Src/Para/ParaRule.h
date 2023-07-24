/*
* @Abrief 参数变化规则
* @note 定义处理函数的基类, 用于获取规则的名称
* @version 1.0
* @author mosongjin 2021-07-27
*/

#ifndef _PARA_RULE_H_
#define _PARA_RULE_H_


#include "ParaObserver.h"
#include <string>

class ParaRule : public ParaObserver
{
public:
    ParaRule(const char* name);

    const char* GetName();

private:
    std::string m_Name;
};


#endif
