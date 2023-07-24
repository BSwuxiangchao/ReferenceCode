#include "CoaterInputParaAgent.h"
#include "ParaObserver.h"
#include "BgiLogger.h"

#include <vector>
using namespace std;

CoaterInputParaAgent::CoaterInputParaAgent(BgiProc::ESystemId sys_id)
    : m_SystemId(sys_id)
{ 
    struct ParaEnumValueIdPair {
        int Enum;
        int ValueId;
    };
    ParaEnumValueIdPair enum_id[] = {
        {InputProc::Coater::Start,               InputProc::Coater::Start             | BgiProc::ESystemType::CoaterType | BgiProc::InputProcType},
    };
    int enum_id_num = sizeof(enum_id) / sizeof(enum_id[0]);

    std::vector<ParaEnumIdInfo> info_list;
    for (int i = 0; i < enum_id_num; i++)
    {
        ParaEnumIdInfo info;
        info.Enum = enum_id[i].Enum;
        info.ValueId = enum_id[i].ValueId | BgiProc::ParaValue   | BgiProc::RecordType | m_SystemId;
        info.StatusId = enum_id[i].ValueId | BgiProc::ParaStatus  | BgiProc::RecordType | m_SystemId;
        info_list.push_back(info);
    }
    AddParaEnumDesc(info_list);
}

CoaterInputParaAgent::~CoaterInputParaAgent()
{
}

void CoaterInputParaAgent::CreatePara()
{
    int para_enum;
    ParaEnumDesc* desc;
    std::string token;

    para_enum = InputProc::Coater::Start;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    desc = &m_ParaEnumMap[para_enum];
    BGI_ASSERT(desc->Parameter == NULL);
    desc->Parameter = new Parameter<bool>(para_enum, *this);

}

void CoaterInputParaAgent::ResetPara()
{
    Start().Value(false);
    Start().Status(true);

   
}

BgiProc::ESystemType CoaterInputParaAgent::GetSystemType()
{
    return BgiProc::CoaterType;
}

BgiProc::EProcType CoaterInputParaAgent::GetProcType()
{
    return BgiProc::InputProcType;
}

BgiProc::ESystemId CoaterInputParaAgent::GetSystemId()
{
    return m_SystemId;
}

Parameter<bool>& CoaterInputParaAgent::Start()
{
    int para_enum = InputProc::Coater::Start;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    ParaEnumDesc& desc = m_ParaEnumMap[para_enum];

    Parameter<bool>* start = dynamic_cast<Parameter<bool> *>(desc.Parameter);
    BGI_ASSERT(start != NULL);
    return *start;
}
