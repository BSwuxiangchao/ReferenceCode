#include "ParaProcRuleHelper.h"
#include "OutputProcAgentHelper.h"
#include "InputProcAgentHelper.h"
#include "BgiLogger.h"

ParaProcRule::ParaProcRule(const char* rule_name, BgiProc::ESystemId systemId, void (*rule_func)(InputProcAgentHelper&, OutputProcAgentHelper&, BgiProc::ESystemId systemId), ProcParaManager* manager)
    : ParaRule(rule_name), m_RuleFunc(rule_func), m_Manager(manager), m_systemId(systemId)
{
    BGI_ASSERT(manager != NULL);
}

void ParaProcRule::Update()
{
    InputProcAgentHelper Input_helper(m_Manager);
    OutputProcAgentHelper Output_helper(m_Manager);

    m_RuleFunc(Input_helper, Output_helper, m_systemId);
}

std::vector<ParaProcRuleHelper::RuleDesc>* ParaProcRuleHelper::m_RuleDescVector = NULL;
int ParaProcRuleHelper::m_RuleDescCounter = 0;

ParaProcRuleHelper::ParaProcRuleHelper(const char* rule_name,BgiProc::ESystemId systemId,
    void (*declare_func)(InputProcAgentHelper&, OutputProcAgentHelper&, ParaRule *, BgiProc::ESystemId systemId)
    , void (*rule_func)(InputProcAgentHelper&, OutputProcAgentHelper&, BgiProc::ESystemId systemId))
{
    BGI_ASSERT(rule_name != NULL);
    RuleDesc desc = {rule_name, systemId, declare_func, rule_func};
    if(m_RuleDescCounter == 0)
        m_RuleDescVector = new std::vector<RuleDesc>;
    m_RuleDescCounter++;
    m_RuleDescVector->push_back(desc);
}

ParaProcRuleHelper::~ParaProcRuleHelper()
{
    m_RuleDescCounter--;
    if(m_RuleDescCounter == 0)
    {
        delete m_RuleDescVector;
        m_RuleDescVector = NULL;
    }
}

void ParaProcRuleHelper::AddRuleSet(ProcParaManager *manager)
{
    if(m_RuleDescVector == NULL)
        return;

    std::vector<RuleDesc>::iterator it = m_RuleDescVector->begin();
    std::vector<RuleDesc>::iterator it_end = m_RuleDescVector->end();
    for(; it != it_end; ++it)
    {
        ParaProcRule* rule = new ParaProcRule(it->RuleName.c_str(), it->SystemId, it->RuleFunc, manager);

        //先将rule注册为观察ProbeId, 然后执行注册函数
        InputProcAgentHelper Input_helper(manager);
        OutputProcAgentHelper Output_helper(manager);
        it->DeclareFunc(Input_helper, Output_helper, rule, it->SystemId);
    }
}
