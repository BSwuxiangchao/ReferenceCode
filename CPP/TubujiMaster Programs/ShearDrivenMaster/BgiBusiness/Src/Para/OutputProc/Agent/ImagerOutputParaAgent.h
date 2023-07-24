/*
* @Abrief 输出参数的代理类
* @note 访问输出参数的代理类
* @version 1.0
* @author wuxiangchao 2023-02-27
*/

#ifndef _IMAGER_OUTPUT_PARA_CONTROL_H_
#define _IMAGER_OUTPUT_PARA_CONTROL_H_

#include "ParaAgent.h"
#include "Parameter.h"
#include "OutputProcId.h"
#include "BgiProcId.h"
#include "ImagerParaType.h"

class ImagerOutputParaAgent : public ParaAgent
{
public:
    ImagerOutputParaAgent(BgiProc::ESystemId sys_id);
    ~ImagerOutputParaAgent();

    void CreatePara();
    void ResetPara();
    BgiProc::ESystemType GetSystemType();
    BgiProc::EProcType GetProcType();
    BgiProc::ESystemId GetSystemId();

    Parameter<ImagerParaType::ParaDesc>& ImagerParaDesc();
    
private:
    BgiProc::ESystemId m_SystemId;
   
};

#endif

