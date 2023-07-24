/*
* @Abrief 参数处理规则
* @note 输入参数根据某些参数发生变化的规则
* @version 1.0
* @author mosongjin 2021-11-04
*/

//#include "ParaProcRuleHelper.h"
//#include "InputProcAgentHelper.h"
//#include "OutputProcAgentHelper.h"
//#include "SortingOutputParaAgent.h"
//#include "SortingInputParaAgent.h"
//
//
////轮次变换（大小轮次）
//void SortingAutoEndRuleDeclare(InputProcAgentHelper& input_helper, OutputProcAgentHelper& output_helper, ParaRule* rule, BgiProc::ESystemId systemId)
//{
//    //声明需要读的参数
//    output_helper.Sorting(systemId).BlowingCntDone().AddReader(rule, PM_VALUE);
//    output_helper.Sorting(systemId).CurrentRound().AddReader(rule, PM_VALUE);
//    input_helper.Sorting(systemId).SubRoundsDesc().AddReader(rule, PM_VALUE);
//    input_helper.Sorting(systemId).SortEnd().AddReader(rule, PM_VALUE);
//
//    //声明需要写的参数
//    input_helper.Sorting(systemId).SortingSwitch().AddWriter(rule, PM_VALUE);
//}
//
//void SortingAutoEndRule(InputProcAgentHelper& input_helper, OutputProcAgentHelper& output_helper, BgiProc::ESystemId systemId)
//{
//    SortingParaType::BlowingCntDesc blowing_cnt_desc = output_helper.Sorting(systemId).BlowingCntDone().Value();
//    SortingParaType::SortEndDesc sort_end_desc = input_helper.Sorting(systemId).SortEnd().Value();
//    SortingParaType::RoundsDesc rounds_desc = output_helper.Sorting(systemId).CurrentRound().Value();
//    SortingParaType::SubRoundsDesc sub_rounds_desc = input_helper.Sorting(systemId).SubRoundsDesc().Value();
//
//    if (sort_end_desc.bSwitch)
//    {
//        //开启分拣超时
//        //通过分拣超时开关，在外部启动分拣超时功能
//    }
//    else
//    {
//        int sum = blowing_cnt_desc.iBlow1 + blowing_cnt_desc.iBlow2 + blowing_cnt_desc.iBlow3 
//            + blowing_cnt_desc.iBlow4 + blowing_cnt_desc.iBlow5 + blowing_cnt_desc.iBlow6;
//        if (rounds_desc.subRounds == SortingParaType::ESubRound::SubA)
//        {
//            //A轮分拣达到完成数量
//            if (sum > sub_rounds_desc.subACnt)
//            {
//                input_helper.Sorting(systemId).SortingSwitch().Value(GeneralParaType::SwitchType::Off);
//            }
//        }
//        else
//        {
//            //B轮分拣达到完成数量
//            if (sum > sub_rounds_desc.subBCnt)
//            {
//                input_helper.Sorting(systemId).SortingSwitch().Value(GeneralParaType::SwitchType::Off);
//            }
//        }
//    }
//}
//
//
//ParaProcRuleHelper AddSortingAutoEndRuleSystemA("AddSortingAutoEndRuleSystemA", BgiProc::ESystemId::SystemA,
//    SortingAutoEndRuleDeclare, SortingAutoEndRule);
//ParaProcRuleHelper AddSortingAutoEndRuleSystemB("SortingAutoEndRuleSystemB", BgiProc::ESystemId::SystemB,
//    SortingAutoEndRuleDeclare, SortingAutoEndRule);
//
////ADD_PARAPROC_RULE(BgiProc::ESystemId::SystemA, SortingRoundsSwitchRuleDeclare, SortingRoundsSwitchRule);
//


