/*
* @Abrief 涂布机输入参数的代理类
* @note 访问涂布机输入参数的代理类
* @version 1.0
* @author wuxiangchao 2023-02-24
*/

#ifndef _COATER_INPUT_PARA_AGENT_H_
#define _COATER_INPUT_PARA_AGENT_H_

#include "ParaAgent.h"
#include "Parameter.h"
#include "InputProcId.h"
#include "BgiProcId.h"
#include "CoaterParaType.h"
#include "ParaType.h"

class CoaterInputParaAgent : public ParaAgent
{
public:
    CoaterInputParaAgent(BgiProc::ESystemId sys_id);
    ~CoaterInputParaAgent();

    void CreatePara();
    void ResetPara();
    BgiProc::ESystemType GetSystemType();
    BgiProc::EProcType GetProcType();
    BgiProc::ESystemId GetSystemId();

    Parameter<bool>& Start();

private:
    BgiProc::ESystemId m_SystemId;
};

#endif
