/*
* @Abrief 参数处理规则
* @note 输出参数根据某些参数发生变化的规则
* @version 1.0
* @author wuxiangchao 2021-11-03
*/

#include "ParaProcRuleHelper.h"
#include "InputProcAgentHelper.h"
#include "OutputProcAgentHelper.h"



//轮次变换（大小轮次）
void SortingRoundsRuleDeclare(InputProcAgentHelper& input_helper, OutputProcAgentHelper& output_helper, ParaRule* rule, BgiProc::ESystemId systemId)
{
    //声明需要读的参数
   //input_helper.Transport(systemId).TransportSwitch().AddReader(rule, PM_VALUE);

    //声明需要写的参数
    //output_helper.Sorting(systemId).RoundsDesc().AddWriter(rule, PM_VALUE);
}

void SortingRoundsSwitchRule(InputProcAgentHelper& input_helper, OutputProcAgentHelper& output_helper, BgiProc::ESystemId systemId)
{
    //GeneralParaType::SwitchType::Enum transport_switch = input_helper.Transport(systemId).TransportSwitch().Value();
    //SortingParaType::RoundsDesc rounds_desc = output_helper.Sorting(systemId).RoundsDesc().Value();

    //if (transport_switch == GeneralParaType::SwitchType::Off)
    //{
    //    //本轮转运关闭时
   // }

    
}


//ADD_PARAPROC_RULE(BgiProc::ESystemId::SystemA, SortingRoundsSwitchRuleDeclare, SortingRoundsSwitchRule);



