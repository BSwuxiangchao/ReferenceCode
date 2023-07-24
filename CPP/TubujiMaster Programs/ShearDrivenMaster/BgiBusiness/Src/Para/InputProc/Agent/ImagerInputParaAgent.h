/*
* @Abrief 光机输入参数的代理类
* @note 光机输入参数的代理类
* @version 1.0
* @author wuxiangchao 2023-02-24
*/

#ifndef _IMAGER_INPUT_PARA_AGENT_H_
#define _IMAGER_INPUT_PARA_AGENT_H_

#include "ParaAgent.h"
#include "Parameter.h"
#include "InputProcId.h"
#include "BgiProcId.h"
#include <qvector.h>

class ImagerInputParaAgent : public ParaAgent
{
public:
    ImagerInputParaAgent(BgiProc::ESystemId sys_id);
    ~ImagerInputParaAgent();

    void CreatePara();
    void ResetPara();
    BgiProc::ESystemType GetSystemType();
    BgiProc::EProcType GetProcType();
    BgiProc::ESystemId GetSystemId();

    Parameter<GeneralParaType::WorkStatus>& WorkStatus();

private:
    BgiProc::ESystemId m_SystemId;
};

#endif
