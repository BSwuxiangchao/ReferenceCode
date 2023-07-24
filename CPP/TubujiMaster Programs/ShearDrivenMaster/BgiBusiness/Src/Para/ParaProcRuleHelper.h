/*
* @Abrief 参数处理规则
* @note 参数处理规则帮助者，解决一些因某些参数发生变化会影响到其它参数的情况
* @version 1.0
* @author mosongjin 2021-11-03
*/

#ifndef _PARA_PROC_RULE_HELPER_H_
#define _PARA_PROC_RULE_HELPER_H_

#include "ParaRule.h"
#include <vector>
#include "BgiProcId.h"

class ProcParaManager;
class OutputProcAgentHelper;
class InputProcAgentHelper;

class ParaProcRule : public ParaRule
{
public:
    ParaProcRule(const char* rule_name, BgiProc::ESystemId systemId, void (*rule_func)(InputProcAgentHelper&, OutputProcAgentHelper&, BgiProc::ESystemId systemId), ProcParaManager* manager);
    void Update();

private:
    void (*m_RuleFunc)(InputProcAgentHelper&, OutputProcAgentHelper&, BgiProc::ESystemId systemId);
    ProcParaManager* m_Manager;
    BgiProc::ESystemId m_systemId;
};

class ParaProcRuleHelper
{
public:
    ParaProcRuleHelper(const char* rule_name, BgiProc::ESystemId systemId,
         void (*declare_func)(InputProcAgentHelper&, OutputProcAgentHelper&, ParaRule*, BgiProc::ESystemId systemId)
        , void (*update_func)(InputProcAgentHelper&, OutputProcAgentHelper&, BgiProc::ESystemId systemId));
    ~ParaProcRuleHelper();
    static void AddRuleSet(ProcParaManager *manager);

private:
    struct RuleDesc
    {
        std::string RuleName;
        BgiProc::ESystemId SystemId;
        void (*DeclareFunc)(InputProcAgentHelper&, OutputProcAgentHelper&, ParaRule*, BgiProc::ESystemId systemId);
        void (*RuleFunc)(InputProcAgentHelper&, OutputProcAgentHelper&, BgiProc::ESystemId systemId);

        bool operator==(const RuleDesc& desc)
        {
            return (RuleFunc == desc.RuleFunc);
        }
    };

    static std::vector<RuleDesc>* m_RuleDescVector;
    static int m_RuleDescCounter;
};

#define ADD_PARAPROC_RULE(systemId, declare_func, rule_func)  \
    ParaProcRuleHelper RuleHelper_##systemId_##rule_func_##declare_func(#rule_func, systemId, (declare_func), (rule_func))

#endif
