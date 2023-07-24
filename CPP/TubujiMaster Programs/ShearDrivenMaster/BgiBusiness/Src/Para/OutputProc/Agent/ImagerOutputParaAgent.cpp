#include "ImagerOutputParaAgent.h"
#include "ParaObserver.h"
#include "BgiLogger.h"
#include "ImagerParaType.h"
#include <vector>
using namespace std;

ImagerOutputParaAgent::ImagerOutputParaAgent(BgiProc::ESystemId sys_id)
    : m_SystemId(sys_id)
{
    struct ParaEnumValueIdPair {
        int Enum;
        int ValueId;
    };
    ParaEnumValueIdPair enum_id[] = {
        {OutputProc::Imager::Start,       OutputProc::Imager::Start | BgiProc::ImagerType | BgiProc::OutputProcType},
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

ImagerOutputParaAgent::~ImagerOutputParaAgent()
{
}

void ImagerOutputParaAgent::CreatePara()
{
    int para_enum;
    ParaEnumDesc* desc;
    std::string token;

    para_enum = OutputProc::Imager::Start;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    desc = &m_ParaEnumMap[para_enum];
    BGI_ASSERT(desc->Parameter == NULL);
    desc->Parameter = new Parameter<ImagerParaType::ParaDesc>(para_enum, *this);

}

void ImagerOutputParaAgent::ResetPara()
{
    ImagerParaType::ParaDesc para;
   
    ImagerParaDesc().Value(para);
    ImagerParaDesc().Status(true);
}

BgiProc::ESystemType ImagerOutputParaAgent::GetSystemType()
{
    return BgiProc::ESystemType::ImagerType;
}

BgiProc::EProcType ImagerOutputParaAgent::GetProcType()
{
    return BgiProc::OutputProcType;
}

BgiProc::ESystemId ImagerOutputParaAgent::GetSystemId()
{
    return m_SystemId;
}

Parameter<ImagerParaType::ParaDesc>& ImagerOutputParaAgent::ImagerParaDesc()
{
    int para_enum = OutputProc::Imager::Start;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    ParaEnumDesc& desc = m_ParaEnumMap[para_enum];

    Parameter<ImagerParaType::ParaDesc>* transport_status = dynamic_cast<Parameter<ImagerParaType::ParaDesc> *>(desc.Parameter);
    BGI_ASSERT(transport_status != NULL);
    return *transport_status;
}
