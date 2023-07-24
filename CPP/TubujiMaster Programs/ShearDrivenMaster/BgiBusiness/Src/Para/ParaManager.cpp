#include <memory.h>
#include <algorithm>

#include "ParaManager.h"
#include "ParaAgent.h"
#include "ParaRule.h"
#include "ParaObserver.h"

using namespace std;

ParaManager::Rule::Rule()
{
    m_Enable = false;
    m_ParameterRule = NULL;
}

ParaManager::Rule::~Rule()
{
    if (m_ParameterRule != NULL)
    {
        delete m_ParameterRule;
        m_ParameterRule = NULL;
    }
}

ParaManager::Parameter::Parameter()
{
    m_Enable = false;
    m_Arch = NULL;
    m_Pred = NULL;
    m_Value = NULL;
    m_Version = -1;
}

ParaManager::Parameter::~Parameter()
{
    if (m_Value != NULL)
    {
        delete m_Value;
        m_Value = NULL;
    }

    if (m_Pred != NULL)
    {
        delete m_Pred;
        m_Pred = NULL;
    }

    if (m_Arch != NULL)
    {
        delete m_Arch;
        m_Arch = NULL;
    }

    m_ReaderList.clear();
    m_WriterList.clear();
}

ParaManager::ParaManager(int id)
    : m_Id(id)
{
    m_ChangeCount = 0;
    m_UpdatingRule = NULL;
}

ParaManager::~ParaManager()
{
    BGI_ASSERT(m_ParameterAgentSet.size() == 0);

    map<int, Parameter*>::iterator it = m_ParameterMap.begin();
    map<int, Parameter*>::iterator it_end = m_ParameterMap.end();
    for (; it != it_end; ++it)
    {
        delete it->second;
        it->second = NULL;
    }

    //释放Rule的空间
    map<ParaRule*, Rule*>::iterator rule_it = m_RuleMap.begin();
    map<ParaRule*, Rule*>::iterator rule_it_end = m_RuleMap.end();
    for (; rule_it != rule_it_end; ++rule_it)
    {
        delete (rule_it->second);
        rule_it->second = NULL;
    }
}

//注意pred和archive由ParameterManager负责删除
void ParaManager::Add(int id, Archive* archive, EqualPred* pred)
{
    BGI_ASSERT(archive != NULL);
    BGI_ASSERT(pred != NULL);

    map<int, Parameter*>::iterator it = m_ParameterMap.find(id);
    if (it == m_ParameterMap.end())
    {
        Parameter* para = new Parameter;
        para->m_Id = id;
        para->m_Enable = true;
        para->m_Arch = archive;
        para->m_Pred = pred;
        para->m_Value = NULL;
        para->m_Version = -1;
        m_ParameterMap[id] = para;
    }
    else
    {
        BGI_ASSERT(false);
        delete pred;
        delete archive;
    }
}

bool ParaManager::IsExist(int id) const
{
    map<int, Parameter*>::const_iterator it = m_ParameterMap.find(id);
    return (it != m_ParameterMap.end());
}

//记录参数变化, 并触发更新Observer
void ParaManager::Trigger(int id)
{
    map<int, Parameter*>::iterator para_it = m_ParameterMap.find(id);
    BGI_ASSERT(para_it != m_ParameterMap.end());
    BGI_ASSERT(para_it->second != NULL);

    Parameter& parameter = *para_it->second;
    const vector<Rule*>& readers = parameter.m_ReaderList;
    vector<Rule*>::const_iterator it = readers.begin();
    vector<Rule*>::const_iterator it_end = readers.end();
    for (; it != it_end; ++it)
    {
        if (!(*it)->m_Enable)
            continue;

        list<ParaRule*>::iterator list_it_end = m_UpdatingRuleList.end();
        list<ParaRule*>::iterator list_it_begin = m_UpdatingRuleList.begin();
        list<ParaRule*>::iterator list_it = find(list_it_begin, list_it_end, (*it)->m_ParameterRule);
        if (list_it != list_it_end)
            m_UpdatingRuleList.erase(list_it);
        m_UpdatingRuleList.push_back((*it)->m_ParameterRule);
    }

    //保存发生了变化的参数
    m_ChangedIdSet.insert(id);
}

//获取id参数值的版本号, 低24位为参数版本序列号, 高8位为ParameterManager的Id
int ParaManager::GetVersion(int id) const
{
    map<int, Parameter*>::const_iterator para_it = m_ParameterMap.find(id);
    BGI_ASSERT(para_it != m_ParameterMap.end());
    BGI_ASSERT(para_it->second != NULL);

    return ((para_it->second)->m_Version & 0xffffff) | ((m_Id & 0xff) << 24);
}

pair<const unsigned char*, int> ParaManager::LoadValue(int id, const unsigned char* mem_ptr, int total_len)
{
    //限制为只有在BeginChange()和EndChange()之间才能改变参数值
    BGI_ASSERT(m_ChangeCount > 0);
    BGI_ASSERT(m_UpdatingRule == NULL);

    std::map<int, Parameter*>::iterator it = m_ParameterMap.find(id);
    if (it == m_ParameterMap.end())
        return pair<const unsigned char*, int>(mem_ptr, 0);

    BGI_ASSERT((it->second) != NULL);
    Parameter& parameter = *(it->second);
    BGI_ASSERT(parameter.m_Value != NULL);

    //恢复参数
    Archive::VarDesc var_desc = parameter.m_Arch->Load(mem_ptr, total_len, BgiProc::GetToken(id).c_str());
    if (var_desc.VarPtr == NULL)
        return pair<const unsigned char*, int>(mem_ptr, 0);

    BGI_ASSERT(parameter.m_Value->GetType() == *var_desc.VarType);
    const void* current_value = parameter.m_Value->GetValue();

    bool is_equal = parameter.m_Pred->IsEqual(current_value, var_desc.VarPtr, *var_desc.VarType);
    if (!is_equal)
    {
        //不相等, 设置新值更新版本号, Trigger更新
        parameter.m_Value->SetValue(var_desc.VarPtr, *var_desc.VarType);
        parameter.m_Version++;
        Trigger(id);
    }

    parameter.m_Arch->FreeVarPtr(var_desc.VarPtr);
    var_desc.VarPtr = NULL;
    return pair<const unsigned char*, int>(var_desc.MemPtr, var_desc.MemSize);
}

pair<const unsigned char*, int> ParaManager::SaveValue(Parameter& parameter)
{
    ParaValueBase* para_value = parameter.m_Value;
    BGI_ASSERT(para_value != NULL);

    const type_info& type = para_value->GetType();
    const void* var_ptr = para_value->GetValue();

    int id = parameter.m_Id;
    Archive::VarDesc var_desc = parameter.m_Arch->Save(var_ptr, type, BgiProc::GetToken(id).c_str());
    return pair<const unsigned char*, int>(var_desc.MemPtr, var_desc.MemSize);
}

//为id参数增加读rule, 当参数变化时通知rule重新执行
void ParaManager::AddReader(int id, ParaRule* rule)
{
    //添加到ParameterMap
    map<int, Parameter*>::iterator it = m_ParameterMap.find(id);
    BGI_ASSERT(it != m_ParameterMap.end());
    BGI_ASSERT(it->second != NULL);

    map<ParaRule*, Rule*>::iterator rule_it = m_RuleMap.find(rule);
    if (rule_it == m_RuleMap.end())
    {
        Rule* new_rule = new Rule;
        new_rule->m_Enable = true;
        new_rule->m_ParameterRule = rule;
        new_rule->m_NeedReadSet.insert(id);
        m_RuleMap[rule] = new_rule;

        vector<Rule*>& list = (it->second)->m_ReaderList;
        list.push_back(new_rule);
    }
    else
    {
        BGI_ASSERT(rule_it->second != NULL);
        BGI_ASSERT((rule_it->second)->m_ParameterRule == rule);

        vector<Rule*>& list = (it->second)->m_ReaderList;
        vector<Rule*>::iterator reader_it = find(list.begin(), list.end(), rule_it->second);
        if (reader_it == list.end())
            list.push_back(rule_it->second);

        rule_it->second->m_NeedReadSet.insert(id);
    }
}

//为id参数增加写rule
void ParaManager::AddWriter(int id, ParaRule* rule)
{
    //添加到ParameterMap
    map<int, Parameter*>::iterator it = m_ParameterMap.find(id);
    BGI_ASSERT(it != m_ParameterMap.end());
    BGI_ASSERT((it->second) != NULL);

    map<ParaRule*, Rule*>::iterator rule_it = m_RuleMap.find(rule);
    if (rule_it == m_RuleMap.end())
    {
        Rule* new_rule = new Rule;
        new_rule->m_Enable = true;
        new_rule->m_ParameterRule = rule;
        new_rule->m_NeedWriteSet.insert(id);
        m_RuleMap[rule] = new_rule;

        vector<Rule*>& list = (it->second)->m_WriterList;
        list.push_back(new_rule);
    }
    else
    {
        BGI_ASSERT(rule_it->second != NULL);
        BGI_ASSERT((rule_it->second)->m_ParameterRule == rule);

        vector<Rule*>& list = (it->second)->m_WriterList;
        vector<Rule*>::iterator writer_it = find(list.begin(), list.end(), rule_it->second);
        if (writer_it == list.end())
            list.push_back(rule_it->second);

        rule_it->second->m_NeedWriteSet.insert(id);
    }
}

//注册ParameterAgent; 参数调节完成后, 会将变化了的参数列表发送给注册的Agent
void ParaManager::AddAgent(ParaAgent* agent)
{
    //不允许在更新过程中修改m_ParameterAgentSet
    BGI_ASSERT(m_UpdatingRule == NULL);
    BGI_ASSERT(m_ChangedIdSet.empty());

    pair<set<ParaAgent*>::iterator, bool> result = m_ParameterAgentSet.insert(agent);
    BGI_ASSERT(result.second);
}

//注销ParameterAgent
void ParaManager::DelAgent(ParaAgent* agent)
{
    //不允许在更新过程中修改m_ParameterAgentSet
    BGI_ASSERT(m_UpdatingRule == NULL);
    BGI_ASSERT(m_ChangedIdSet.empty());

    int result = m_ParameterAgentSet.erase(agent);
    BGI_ASSERT(result == 1);
}

void ParaManager::BeginChange()
{
    if (m_ChangeCount == 0)
    {
        //不允许重入
        BGI_ASSERT(m_UpdatingRule == NULL);
        BGI_ASSERT(m_ChangedIdSet.empty());
    }

    m_ChangeCount++;
}

void ParaManager::EndChange()
{
    m_ChangeCount--;
    if (m_ChangeCount == 0)
    {
        Execute();

        //通知注册的Agent, 为了处理发布过程中引发嵌套的参数改变, 先保存m_ChangedIdSet
        std::set<int> changed_id_set = m_ChangedIdSet;
        m_ChangedIdSet.clear();

        set<ParaAgent*>::iterator it = m_ParameterAgentSet.begin();
        set<ParaAgent*>::iterator it_end = m_ParameterAgentSet.end();
        for (; it != it_end; ++it)
            (*it)->ParaChanged(changed_id_set);
    }
}

bool ParaManager::IsAllowedReader(const Parameter& parameter, ParaRule* rule)
{
    bool is_allowed = false;
    const vector<Rule*>& list = parameter.m_ReaderList;
    vector<Rule*>::const_iterator it = list.begin();
    vector<Rule*>::const_iterator it_end = list.end();
    for (; it != it_end; ++it)
    {
        if ((*it)->m_ParameterRule == rule)
        {
            is_allowed = true;
            break;
        }
    }

    return is_allowed;
}

bool ParaManager::IsAllowedWriter(const Parameter& parameter, ParaRule* rule)
{
    bool is_allowed = false;
    const vector<Rule*>& list = parameter.m_WriterList;
    vector<Rule*>::const_iterator it = list.begin();
    vector<Rule*>::const_iterator it_end = list.end();
    for (; it != it_end; ++it)
    {
        if ((*it)->m_ParameterRule == rule)
        {
            is_allowed = true;
            break;
        }
    }

    return is_allowed;
}

void ParaManager::Execute()
{
    set<ParaObserver*> exec_rule_set;
    size_t prev_exec_rule_count = 0;
    size_t conti_equal_count = 0;

    while (!m_UpdatingRuleList.empty())
    {
        ParaRule* rule = m_UpdatingRuleList.front();
        m_UpdatingRuleList.pop_front();
        m_UpdatingRule = rule;
        m_UpdatingRule->Update();

        //检测是否由于循环依赖造成死循环
        exec_rule_set.insert(m_UpdatingRule);
        if (exec_rule_set.size() != prev_exec_rule_count)
        {
            prev_exec_rule_count = exec_rule_set.size();
            conti_equal_count = 0;
        }
        conti_equal_count++;
        if (conti_equal_count >= 10 * exec_rule_set.size())
        {
            //执行规则的次数大于10所执行的不同规则数目, 判定为进入循环依赖造成的死循环, 强制退出
            BGI_WARNING("ParaManager::Execute(): Can't exit loop, Exit abnormally.");
            m_UpdatingRuleList.clear();
            break;
        }
    }
    m_UpdatingRule = NULL;
}

bool ParaManager::IsValueValid(int id) const
{
    map<int, Parameter*>::const_iterator it = m_ParameterMap.find(id);
    BGI_ASSERT(it != m_ParameterMap.end());
    return ((it->second)->m_Value != NULL);
}

void ParaManager::DisableParameter(const vector<int>& ids)
{
    vector<int>::const_iterator it = ids.begin();
    vector<int>::const_iterator it_end = ids.end();
    for (; it != it_end; ++it)
    {
        map<int, Parameter*>::iterator map_it = m_ParameterMap.find(*it);
        BGI_ASSERT(map_it != m_ParameterMap.end());
        (map_it->second)->m_Enable = false;
    }
}

void ParaManager::EvalRuleStatus()
{
    map<ParaRule*, Rule*>::iterator it = m_RuleMap.begin();
    map<ParaRule*, Rule*>::iterator it_end = m_RuleMap.end();
    for (; it != it_end; ++it)
    {
        BGI_ASSERT(it->second != NULL);
        const set<int>& need_write_set = (it->second)->m_NeedWriteSet;
        set<int>::const_iterator set_it = need_write_set.begin();
        set<int>::const_iterator set_it_end = need_write_set.end();

        //评估Rule是否需要使能, 如果需要改写的参数都没有使能就禁用该规则
        bool enable = false;
        for (; set_it != set_it_end; ++set_it)
        {
            map<int, Parameter*>::iterator para_it = m_ParameterMap.find(*set_it);
            BGI_ASSERT(para_it != m_ParameterMap.end());
            BGI_ASSERT(para_it->second != NULL);

            if ((para_it->second)->m_Enable)
            {
                enable = true;
                break;
            }
        }

        (it->second)->m_Enable = enable;
    }
}