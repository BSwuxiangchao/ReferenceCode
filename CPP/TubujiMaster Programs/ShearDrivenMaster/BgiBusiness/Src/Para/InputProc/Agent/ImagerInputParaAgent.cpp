#include "ImagerInputParaAgent.h"
#include "ParaObserver.h"
#include "BgiLogger.h"

#include <vector>
using namespace std;

ImagerInputParaAgent::ImagerInputParaAgent(BgiProc::ESystemId sys_id)
    : m_SystemId(sys_id)
{
    struct ParaEnumValueIdPair {
        int Enum;
        int ValueId;
    };
    ParaEnumValueIdPair enum_id[] = {
        {InputProc::Imager::Start,                InputProc::Imager::Start | BgiProc::ESystemType::ImagerType | BgiProc::InputProcType},
       
    };
    int enum_id_num = sizeof(enum_id) / sizeof(enum_id[0]);

    std::vector<ParaEnumIdInfo> info_list;
    for (int i = 0; i < enum_id_num; i++)
    {
        ParaEnumIdInfo info;
        info.Enum = enum_id[i].Enum;
        info.ValueId = enum_id[i].ValueId  | BgiProc::ParaValue   | BgiProc::RecordType | m_SystemId;
        info.StatusId = enum_id[i].ValueId | BgiProc::ParaStatus  | BgiProc::RecordType | m_SystemId;
        info_list.push_back(info);
    }
    AddParaEnumDesc(info_list);
}

ImagerInputParaAgent::~ImagerInputParaAgent()
{
}

void ImagerInputParaAgent::CreatePara()
{
    int para_enum;
    ParaEnumDesc* desc;
    std::string token;
    para_enum = InputProc::Imager::Start;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    desc = &m_ParaEnumMap[para_enum];
    BGI_ASSERT(desc->Parameter == NULL);
    desc->Parameter = new Parameter<GeneralParaType::WorkStatus>(para_enum, *this);

}

void ImagerInputParaAgent::ResetPara()
{
    GeneralParaType::WorkStatus transport_status;
    transport_status.bPause = false;
    transport_status.bEnd = false;
    transport_status.bAuto = false;
    WorkStatus().Value(transport_status);
    WorkStatus().Status(true);
}

BgiProc::ESystemType ImagerInputParaAgent::GetSystemType()
{
    return BgiProc::ImagerType;
}

BgiProc::EProcType ImagerInputParaAgent::GetProcType()
{
    return BgiProc::InputProcType;
}

BgiProc::ESystemId ImagerInputParaAgent::GetSystemId()
{
    return m_SystemId;
}

Parameter<GeneralParaType::WorkStatus>& ImagerInputParaAgent::WorkStatus()
{
    int para_enum = InputProc::Imager::Start;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    ParaEnumDesc& desc = m_ParaEnumMap[para_enum];

    Parameter<GeneralParaType::WorkStatus>* para = dynamic_cast<Parameter<GeneralParaType::WorkStatus> *>(desc.Parameter);
    BGI_ASSERT(para != NULL);
    return *para;
}


