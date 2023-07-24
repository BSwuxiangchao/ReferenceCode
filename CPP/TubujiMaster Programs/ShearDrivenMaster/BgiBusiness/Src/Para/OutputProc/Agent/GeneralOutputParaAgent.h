/*
* @Abrief 全局输出参数的代理类
* @note 访问全局的输出参数的代理类
* @version 1.0
* @author mosongjin 2021-08-02
*/

#ifndef _GENERAL_OUTPUT_PARA_AGENT_H_
#define _GENERAL_OUTPUT_PARA_AGENT_H_

#include "ParaAgent.h"
#include "Parameter.h"
#include "OutputProcId.h"
#include "BgiProcId.h"

class GeneralOutputParaAgent : public ParaAgent
{
public:
    GeneralOutputParaAgent();
    ~GeneralOutputParaAgent();

    void CreatePara();
    void ResetPara();
    BgiProc::ESystemType GetSystemType();
    BgiProc::EProcType GetProcType();

    Parameter<bool>& Start();
};

#endif
