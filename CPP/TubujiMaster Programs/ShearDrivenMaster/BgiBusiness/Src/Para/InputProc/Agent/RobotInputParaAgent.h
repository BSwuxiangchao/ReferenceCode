/*
* @Abrief 输入参数的代理类
* @note 分拣输入参数的代理类
* @version 1.0
* @author wuxiangchao 2023-02-24
*/

#ifndef _ROBOT_INPUT_PARA_AGENT_H_
#define _ROBOT_INPUT_PARA_AGENT_H_

#include "ParaAgent.h"
#include "Parameter.h"
#include "InputProcId.h"
#include "BgiProcId.h"
#include "RobotParaType.h"
#include <qvector.h>

class RobotInputParaAgent : public ParaAgent
{
public:
    RobotInputParaAgent(BgiProc::ESystemId sys_id);
    ~RobotInputParaAgent();

    void CreatePara();
    void ResetPara();
    BgiProc::ESystemType GetSystemType();
    BgiProc::EProcType GetProcType();
    BgiProc::ESystemId GetSystemId();

    Parameter<bool>& Start();
	Parameter<RobotParaType::RobotPara>& RobotPara();

private:
    BgiProc::ESystemId m_SystemId;
};

#endif
