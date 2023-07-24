/*
* @Abrief 定义参数代理基类
* @note 定义了访问保存在PararManager中参数的接口, ImagingObserver通过该接口观察参数变化
* @version 1.0
* @author mosongjin 2021-07-27
*/

#ifndef _PARA_AGENT_H_
#define _PARA_AGENT_H_

#include <set>
#include <map>
#include <vector>
#include <list>

#include "BgiProcId.h"

//参数mask, 用于AddParaObserver, AddRule
const int PM_VALUE = 0x01;
const int PM_STATUS = 0x02;

class ParaObserver;
class ParaRule;
class ParaManager;
class ParaBase;

class ParaAgent
{
public:
    ParaAgent();
    virtual ~ParaAgent();
    virtual void CreatePara() = 0;
    virtual void ResetPara() = 0;
    virtual BgiProc::ESystemType GetSystemType() = 0;
    virtual BgiProc::EProcType GetProcType() = 0;
    virtual BgiProc::ESystemId GetSystemId();

    virtual int GetValueId(int para_enum);
    virtual int GetStatusId(int para_enum);

    void ParaChanged(std::set<int> changed_id_set);
    void SetParaManager(ParaManager* manager);
    void DisableAgent();

    ParaManager* GetParaManager();
    ParaBase* GetParameter(int para_enum);

    //用于为每个参数增加对其感兴趣的View
    void AddParaObserver(int para_enum, int para_mask, ParaObserver* observer);
    void DelParaObserver(int para_enum, ParaObserver* observer);

    //用于为某个参数增加规则
    void AddReader(int para_enum, int para_mask, ParaRule* rule);
    void AddWriter(int para_enum, int para_mask, ParaRule* rule);

protected:
    void NotifyAllImagingObserver();

    ParaManager* m_ParaManager;

    struct ParaEnumDesc
    {
        int ValueId;
        int StatusId;
        ParaBase* Parameter;
        std::set<ParaObserver*> ValueObserver;
        std::set<ParaObserver*> StatusObserver;
    };
    std::map<int, ParaEnumDesc> m_ParaEnumMap;

    struct ParaEnumIdInfo
    {
        int Enum;
        int ValueId;
        int StatusId;
    };
    void AddParaEnumDesc(const std::vector<ParaEnumIdInfo>& info_list);

    std::list<std::set<ParaObserver*> > m_PublishStack;
};

#endif
