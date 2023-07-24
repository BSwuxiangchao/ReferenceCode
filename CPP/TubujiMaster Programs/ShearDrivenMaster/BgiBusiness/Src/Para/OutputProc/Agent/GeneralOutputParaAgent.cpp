#include "GeneralOutputParaAgent.h"
#include "ParaObserver.h"
#include "BgiLogger.h"

#include <vector>
using namespace std;

GeneralOutputParaAgent::GeneralOutputParaAgent()
{
    struct ParaEnumValueIdPair {
        int Enum;
        int ValueId;
    };
    ParaEnumValueIdPair enum_id[] = {
        {OutputProc::Gen::start,               OutputProc::Gen::start | BgiProc::GeneralType | BgiProc::OutputProcType},
    };
    int enum_id_num = sizeof(enum_id) / sizeof(enum_id[0]);

    std::vector<ParaEnumIdInfo> info_list;
    for (int i = 0; i < enum_id_num; i++)
    {
        ParaEnumIdInfo info;
        info.Enum = enum_id[i].Enum;
        info.ValueId = enum_id[i].ValueId | BgiProc::ParaValue   | BgiProc::RecordType;
        info.StatusId = enum_id[i].ValueId | BgiProc::ParaStatus  | BgiProc::RecordType;
        info_list.push_back(info);
    }
    AddParaEnumDesc(info_list);
}

GeneralOutputParaAgent::~GeneralOutputParaAgent()
{
}

void GeneralOutputParaAgent::CreatePara()
{
    int para_enum;
    ParaEnumDesc* desc;
    std::string token;

    para_enum = OutputProc::Gen::start;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    desc = &m_ParaEnumMap[para_enum];
    BGI_ASSERT(desc->Parameter == NULL);
    desc->Parameter = new Parameter<bool>(para_enum, *this);


}

void GeneralOutputParaAgent::ResetPara()
{
    Start().Value(false);
    Start().Status(true);
}

BgiProc::ESystemType GeneralOutputParaAgent::GetSystemType()
{
    return BgiProc::GeneralType;
}

BgiProc::EProcType GeneralOutputParaAgent::GetProcType()
{
    return BgiProc::OutputProcType;
}

Parameter<bool>& GeneralOutputParaAgent::Start()
{
    int para_enum = OutputProc::Gen::start;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    ParaEnumDesc& desc = m_ParaEnumMap[para_enum];

    Parameter<bool>* start = dynamic_cast<Parameter<bool> *>(desc.Parameter);
    BGI_ASSERT(start != NULL);
    return *start;
}

