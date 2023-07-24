/*
* @Abrief 全局输入参数的代理类
* @note 访问全局的输入参数的代理类
* @version 1.0
* @author mosongjin 2021-08-02
*/

#ifndef _GENERAL_INPUT_PARA_AGENT_H_
#define _GENERAL_INPUT_PARA_AGENT_H_

#include "ParaAgent.h"
#include "Parameter.h"
#include "InputProcId.h"
#include "BgiProcId.h"
#include "ParaType.h"
#include "qvector.h"

class GeneralInputParaAgent : public ParaAgent
{
public:
    GeneralInputParaAgent();
    ~GeneralInputParaAgent();

    void CreatePara();
    void ResetPara();
    BgiProc::ESystemType GetSystemType();
    BgiProc::EProcType GetProcType();

    Parameter<bool>& Start();
    Parameter<QVector<GeneralParaType::SequencingTableDesc>>& SequencingTableDescVec();


};

#endif
