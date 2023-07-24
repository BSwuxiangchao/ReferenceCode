/*
* @Abrief 输出参数的代理类
* @note 访问输出参数的代理类
* @version 1.0
* @author wuxiangchao 2023-02-27
*/

#ifndef _COATER_OUTPUT_PARA_AGENT_H_
#define _COATER_OUTPUT_PARA_AGENT_H_

#include "ParaAgent.h"
#include "Parameter.h"
#include "OutputProcId.h"
#include "BgiProcId.h"
#include "CoaterParaType.h"

class CoaterOutputParaAgent : public ParaAgent
{
public:
    CoaterOutputParaAgent(BgiProc::ESystemId sys_id);
    ~CoaterOutputParaAgent();

    void CreatePara();
    void ResetPara();
    BgiProc::ESystemType GetSystemType();
    BgiProc::EProcType GetProcType();
    BgiProc::ESystemId GetSystemId();

    Parameter<CoaterParaType::CoaterPara>& CoaterParaDesc();

private:
    BgiProc::ESystemId m_SystemId;
};

#endif
