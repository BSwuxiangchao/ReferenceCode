/*
* @Abrief 参数管理类
* @note 使用一致的方式保存参数, 可以对参数的值进行设置和获取, 在设置的时候值状态发生变化会通知注册的Observer,
*      在获取值的时候, 根据"需要使用, 必定依赖"的原则检查对参数的值的使用和依赖是否合法
* @version 1.0
* @author mosongjin 2021-07-27
*/

#ifndef _PARA_MANAGER_H_
#define _PARA_MANAGER_H_


#include <vector>
#include <map>
#include <list>
#include <set>
#include <utility>

#include "BgiLogger.h"
#include "Archive.h"
#include "EqualPred.h"
#include "ParaValue.h"
#include <typeinfo>

class ParaRule;
class ParaAgent;
class ParaObserver;

class ParaManager
{
public:
    ParaManager(int id);
    virtual ~ParaManager();

    void Add(int id, Archive* archive, EqualPred* pred);
    bool IsValueValid(int id) const;
    bool IsExist(int id) const;
    void Trigger(int id);
    int GetVersion(int id) const;

    template<class T> void GetValue(int id, const T*& value_ptr, bool check = true) {
        std::map<int, Parameter*>::iterator it = m_ParameterMap.find(id);
        BGI_ASSERT(it != m_ParameterMap.end());
        BGI_ASSERT(it->second != NULL);

        Parameter& parameter = *(it->second);
        ParaValueBase* para_value = parameter.m_Value;
        BGI_ASSERT(para_value != NULL);
        BGI_ASSERT(para_value->GetType() == typeid(T));
        value_ptr = (const T*)para_value->GetValue();

        if ((m_UpdatingRule != NULL) && check)
        {
            bool is_ok = IsAllowedReader(parameter, m_UpdatingRule);
            BGI_ASSERT(is_ok);
        }
    }
    template<class T> bool SetValue(int id, const T& value) {
        //限制为只有在BeginChange()和EndChange()之间才能改变参数值
        BGI_ASSERT((m_ChangeCount > 0) || (m_UpdatingRule != NULL));

        std::map<int, Parameter*>::iterator it = m_ParameterMap.find(id);
        BGI_ASSERT(it != m_ParameterMap.end());
        BGI_ASSERT(it->second != NULL);

        bool new_value = false;
        Parameter& parameter = *(it->second);
        if (parameter.m_Value == NULL)
        {
            BGI_ASSERT(parameter.m_Arch != NULL);
            BGI_ASSERT(parameter.m_Pred != NULL);

            new_value = true;
            parameter.m_Value = new ParaValue<T>(id, value);
            parameter.m_Version = 0;
        }
        ParaValueBase* para_value = parameter.m_Value;

        if (m_UpdatingRule != NULL)
        {
            //检查是否为合法的rule
            bool is_ok = IsAllowedWriter(parameter, m_UpdatingRule);
            BGI_ASSERT(is_ok);
        }

        const void* current_value = parameter.m_Value->GetValue();
        bool is_equal = parameter.m_Pred->IsEqual(current_value, &value, typeid(T));
        if (!is_equal)
        {
            //不相等, 设置新值并更新版本号
            para_value->SetValue(&value, typeid(T));
            parameter.m_Version++;
        }
        return (!is_equal || new_value);
    }
    void AddReader(int id, ParaRule* rule);
    void AddWriter(int id, ParaRule* rule);

    void AddAgent(ParaAgent* agent);
    void DelAgent(ParaAgent* agent);

    void DisableParameter(const std::vector<int>& ids);
    void EvalRuleStatus();

private:
    int m_Id;
    std::list<ParaRule*> m_UpdatingRuleList;
    int m_ChangeCount;
    ParaRule* m_UpdatingRule;

    std::set<ParaAgent*> m_ParameterAgentSet;
    std::set<int> m_ChangedIdSet;

protected:
    struct Rule
    {
        Rule();
        ~Rule();

        bool m_Enable;
        ParaRule* m_ParameterRule;

        std::set<int> m_NeedReadSet;
        std::set<int> m_NeedWriteSet;
    };
    std::map<ParaRule*, Rule*> m_RuleMap;

    struct Parameter
    {
        Parameter();
        ~Parameter();

        int m_Id;
        bool m_Enable;
        Archive* m_Arch;
        EqualPred* m_Pred;
        ParaValueBase* m_Value;
        int m_Version;

        std::vector<Rule*> m_ReaderList;
        std::vector<Rule*> m_WriterList;
    };
    std::map<int, Parameter*> m_ParameterMap;

protected:
    void BeginChange();
    void EndChange();

    std::pair<const unsigned char*, int> LoadValue(int id, const unsigned char* mem_ptr, int total_len);
    std::pair<const unsigned char*, int> SaveValue(Parameter& parameter);

    bool IsAllowedReader(const Parameter& parameter, ParaRule* rule);
    bool IsAllowedWriter(const Parameter& parameter, ParaRule* rule);

    void Execute();
};


#endif
