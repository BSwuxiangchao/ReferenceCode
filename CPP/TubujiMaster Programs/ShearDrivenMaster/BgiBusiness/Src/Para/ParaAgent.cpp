#include "ParaAgent.h"
#include "ParaManager.h"
#include "ParaObserver.h"
#include "BgiLogger.h"
#include "BgiProcId.h"
#include "ParaBase.h"

ParaAgent::ParaAgent()
    : m_ParaManager(NULL)
{
}

ParaAgent::~ParaAgent()
{
    BGI_ASSERT(m_PublishStack.empty());

    if (m_ParaManager != NULL)
    {
        m_ParaManager->DelAgent(this);
        m_ParaManager = NULL;
    }

    //确定所有ParaObserver都已经注销了
    map<int, ParaEnumDesc>::iterator it = m_ParaEnumMap.begin();
    map<int, ParaEnumDesc>::iterator it_end = m_ParaEnumMap.end();
    for (; it != it_end; ++it)
    {
        BGI_ASSERT((it->second).ValueObserver.size() == 0);
        BGI_ASSERT((it->second).StatusObserver.size() == 0);

        delete (it->second).Parameter;
        (it->second).Parameter = NULL;
    }
}

int ParaAgent::GetValueId(int para_enum)
{
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    return m_ParaEnumMap[para_enum].ValueId;
}

int ParaAgent::GetStatusId(int para_enum)
{
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    return m_ParaEnumMap[para_enum].StatusId;
}

//设置新的ParameterManager
void ParaAgent::SetParaManager(ParaManager* manager)
{
    if (m_ParaManager != NULL)
        m_ParaManager->DelAgent(this);

    m_ParaManager = manager;
    if (m_ParaManager != NULL)
    {
        m_ParaManager->AddAgent(this);
        NotifyAllImagingObserver();
    }
}

void ParaAgent::DisableAgent()
{
    vector<int> ids;
    map<int, ParaEnumDesc>::iterator it = m_ParaEnumMap.begin();
    map<int, ParaEnumDesc>::iterator it_end = m_ParaEnumMap.end();
    for (; it != it_end; ++it)
    {
        ids.push_back((it->second).StatusId);
        ids.push_back((it->second).ValueId);
    }

    BGI_ASSERT(m_ParaManager != NULL);
    m_ParaManager->DisableParameter(ids);
}

ParaManager* ParaAgent::GetParaManager()
{
    return m_ParaManager;
}

BgiProc::ESystemId ParaAgent::GetSystemId()
{
    return BgiProc::SystemA;
}

ParaBase* ParaAgent::GetParameter(int para_enum)
{
    map<int, ParaEnumDesc>::iterator desc_it = m_ParaEnumMap.find(para_enum);

    if (desc_it != m_ParaEnumMap.end())
        return (desc_it->second).Parameter;
    else
        return NULL;
}

//changed_id这些参数发生了变化, 通知相关的Observer进行更新, 
//有可能被嵌套调用, 使用m_PublishStack处理这种情况
void ParaAgent::ParaChanged(set<int> changed_id_set)
{
    m_PublishStack.push_back(set<ParaObserver*>());

    set<ParaObserver*>& need_update_observer = m_PublishStack.back();
    set<int>::iterator it = changed_id_set.begin();
    set<int>::iterator it_end = changed_id_set.end();
    for (; it != it_end; ++it)
    {
        int para_enum = BgiProc::GetParaEnum(*it);
        map<int, ParaEnumDesc>::iterator desc_it = m_ParaEnumMap.find(para_enum);
        if (desc_it != m_ParaEnumMap.end())
        {
            if (*it == (desc_it->second).StatusId)
            {
                set<ParaObserver*>& add_set = (desc_it->second).StatusObserver;
                need_update_observer.insert(add_set.begin(), add_set.end());
            }

            if (*it == (desc_it->second).ValueId)
            {
                set<ParaObserver*>& add_set = (desc_it->second).ValueObserver;
                need_update_observer.insert(add_set.begin(), add_set.end());
            }
        }
    }

    while (!need_update_observer.empty())
    {
        BGI_ASSERT(&need_update_observer == &m_PublishStack.back());

        //取出set中一个元素进行更新
        set<ParaObserver*>::iterator set_it = need_update_observer.begin();
        ParaObserver* observer = *set_it;
        need_update_observer.erase(set_it);
        observer->Update();
    }

    m_PublishStack.pop_back();
}

//为参数增加ParaObserver
void ParaAgent::AddParaObserver(int para_enum, int para_mask, ParaObserver* observer)
{
    map<int, ParaEnumDesc>::iterator desc_it = m_ParaEnumMap.find(para_enum);
    BGI_ASSERT(desc_it != m_ParaEnumMap.end());

    if ((para_mask & PM_VALUE) != 0)
    {
        set<ParaObserver*>& observer_set = (desc_it->second).ValueObserver;
        BGI_ASSERT(observer_set.end() == observer_set.find(observer));
        observer_set.insert(observer);
    }

    if ((para_mask & PM_STATUS) != 0)
    {
        set<ParaObserver*>& observer_set = (desc_it->second).StatusObserver;
        BGI_ASSERT(observer_set.end() == observer_set.find(observer));
        observer_set.insert(observer);
    }
}

//将observer从m_ParaEnumMap中删除
void ParaAgent::DelParaObserver(int para_enum, ParaObserver* observer)
{
    map<int, ParaEnumDesc>::iterator desc_it = m_ParaEnumMap.find(para_enum);
    BGI_ASSERT(desc_it != m_ParaEnumMap.end());

    (desc_it->second).ValueObserver.erase(observer);
    (desc_it->second).StatusObserver.erase(observer);

    //将observer从m_PublishStack中删除
    list<set<ParaObserver*> >::iterator it = m_PublishStack.begin();
    list<set<ParaObserver*> >::iterator it_end = m_PublishStack.end();
    for (; it != it_end; ++it)
        (*it).erase(observer);
}

//通知所有注册的ParaObserver
void ParaAgent::NotifyAllImagingObserver()
{
    set<int> all_id_set;

    map<int, ParaEnumDesc>::iterator it = m_ParaEnumMap.begin();
    map<int, ParaEnumDesc>::iterator it_end = m_ParaEnumMap.end();
    for (; it != it_end; ++it)
    {
        int value_id = (it->second).ValueId;
        all_id_set.insert(value_id);

        int status_id = (it->second).StatusId;
        all_id_set.insert(status_id);
    }

    ParaChanged(all_id_set);
}

void ParaAgent::AddReader(int para_enum, int para_mask, ParaRule* rule)
{
    map<int, ParaEnumDesc>::iterator desc_it = m_ParaEnumMap.find(para_enum);
    BGI_ASSERT(desc_it != m_ParaEnumMap.end());
    BGI_ASSERT(m_ParaManager != NULL);

    if ((para_mask & PM_VALUE) != 0)
    {
        int value_id = (desc_it->second).ValueId;
        m_ParaManager->AddReader(value_id, rule);
    }

    if ((para_mask & PM_STATUS) != 0)
    {
        int status_id = (desc_it->second).StatusId;
        m_ParaManager->AddReader(status_id, rule);
    }
}

void ParaAgent::AddWriter(int para_enum, int para_mask, ParaRule* rule)
{
    map<int, ParaEnumDesc>::iterator desc_it = m_ParaEnumMap.find(para_enum);
    BGI_ASSERT(desc_it != m_ParaEnumMap.end());
    BGI_ASSERT(m_ParaManager != NULL);

    if ((para_mask & PM_VALUE) != 0)
    {
        int value_id = (desc_it->second).ValueId;
        m_ParaManager->AddWriter(value_id, rule);
    }

    if ((para_mask & PM_STATUS) != 0)
    {
        int status_id = (desc_it->second).StatusId;
        m_ParaManager->AddWriter(status_id, rule);
    }
}

void ParaAgent::AddParaEnumDesc(const std::vector<ParaEnumIdInfo>& info_list)
{
    std::vector<ParaEnumIdInfo>::const_iterator it = info_list.begin();
    std::vector<ParaEnumIdInfo>::const_iterator it_end = info_list.end();
    for (; it != it_end; ++it)
    {
        ParaEnumDesc desc;
        desc.ValueId = it->ValueId;
        desc.StatusId = it->StatusId;
        desc.Parameter = NULL;

        BGI_ASSERT(m_ParaEnumMap.find(it->Enum) == m_ParaEnumMap.end());
        m_ParaEnumMap[it->Enum] = desc;
    }
}

