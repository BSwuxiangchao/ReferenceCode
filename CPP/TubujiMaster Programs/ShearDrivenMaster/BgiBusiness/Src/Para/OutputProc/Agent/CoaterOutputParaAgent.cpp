#include "CoaterOutputParaAgent.h"
#include "ParaObserver.h"
#include "BgiLogger.h"

#include <vector>
using namespace std;

CoaterOutputParaAgent::CoaterOutputParaAgent(BgiProc::ESystemId sys_id)
    : m_SystemId(sys_id)
{
    struct ParaEnumValueIdPair {
        int Enum;
        int ValueId;
    };
    ParaEnumValueIdPair enum_id[] = {
        {OutputProc::Coater::ParaOpt,               OutputProc::Coater::ParaOpt | BgiProc::CoaterType | BgiProc::OutputProcType},
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

CoaterOutputParaAgent::~CoaterOutputParaAgent()
{
}

void CoaterOutputParaAgent::CreatePara()
{
    int para_enum;
    ParaEnumDesc* desc;
    std::string token;

    para_enum = OutputProc::Coater::ParaOpt;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    desc = &m_ParaEnumMap[para_enum];
    BGI_ASSERT(desc->Parameter == NULL);
    desc->Parameter = new Parameter<CoaterParaType::CoaterPara>(para_enum, *this);
}

void CoaterOutputParaAgent::ResetPara()
{
    CoaterParaType::CoaterPara para;
    CoaterParaDesc().Value(para);
    CoaterParaDesc().Status(true);
}

BgiProc::ESystemType CoaterOutputParaAgent::GetSystemType()
{
    return BgiProc::ESystemType::CoaterType;
}

BgiProc::EProcType CoaterOutputParaAgent::GetProcType()
{
    return BgiProc::OutputProcType;
}

BgiProc::ESystemId CoaterOutputParaAgent::GetSystemId()
{
    return m_SystemId;
}

Parameter<CoaterParaType::CoaterPara>& CoaterOutputParaAgent::CoaterParaDesc()
{
    int para_enum = OutputProc::Coater::ParaOpt;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    ParaEnumDesc& desc = m_ParaEnumMap[para_enum];

    Parameter<CoaterParaType::CoaterPara>* value = dynamic_cast<Parameter<CoaterParaType::CoaterPara> *>(desc.Parameter);
    BGI_ASSERT(value != NULL);
    return *value;
}
