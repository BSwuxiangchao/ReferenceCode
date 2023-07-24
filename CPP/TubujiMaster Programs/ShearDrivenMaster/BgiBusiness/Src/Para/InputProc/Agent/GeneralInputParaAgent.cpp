#include "GeneralInputParaAgent.h"
#include "ParaObserver.h"
#include "BgiLogger.h"
#include "ParaPred.h"
#include "ParaArchive.h"

using namespace std;

GeneralInputParaAgent::GeneralInputParaAgent()
{
    struct ParaEnumValueIdPair {
        int Enum;
        int ValueId;
    };
    ParaEnumValueIdPair enum_id[] = {
        {InputProc::Gen::start,               InputProc::Gen::start | BgiProc::GeneralType | BgiProc::InputProcType},
        {InputProc::Gen::SequencingDesc,      InputProc::Gen::SequencingDesc | BgiProc::GeneralType | BgiProc::InputProcType},
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

GeneralInputParaAgent::~GeneralInputParaAgent()
{
}

void GeneralInputParaAgent::CreatePara()
{
    int para_enum;
    ParaEnumDesc* desc;
    std::string token;

    para_enum = InputProc::Gen::start;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    desc = &m_ParaEnumMap[para_enum];
    BGI_ASSERT(desc->Parameter == NULL);
    desc->Parameter = new Parameter<bool>(para_enum, *this);

    para_enum = InputProc::Gen::SequencingDesc;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    desc = &m_ParaEnumMap[para_enum];
    BGI_ASSERT(desc->Parameter == NULL);
    desc->Parameter = new Parameter<QVector<GeneralParaType::SequencingTableDesc>>(para_enum, *this);
}

void GeneralInputParaAgent::ResetPara()
{
    Start().Value(false);
    Start().Status(true);

    QVector<GeneralParaType::SequencingTableDesc> TableDesc_vec;
    SequencingTableDescVec().Value(TableDesc_vec);
    SequencingTableDescVec().Status(true);
}

BgiProc::ESystemType GeneralInputParaAgent::GetSystemType()
{
    return BgiProc::GeneralType;
}

BgiProc::EProcType GeneralInputParaAgent::GetProcType()
{
    return BgiProc::InputProcType;
}

Parameter<bool>& GeneralInputParaAgent::Start()
{
    int para_enum = InputProc::Gen::start;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    ParaEnumDesc& desc = m_ParaEnumMap[para_enum];

    Parameter<bool>* start = dynamic_cast<Parameter<bool> *>(desc.Parameter);
    BGI_ASSERT(start != NULL);
    return *start;
}

Parameter<QVector<GeneralParaType::SequencingTableDesc>>& GeneralInputParaAgent::SequencingTableDescVec()
{
    int para_enum = InputProc::Gen::SequencingDesc;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    ParaEnumDesc& desc = m_ParaEnumMap[para_enum];

    Parameter<QVector<GeneralParaType::SequencingTableDesc>>* value = dynamic_cast<Parameter<QVector<GeneralParaType::SequencingTableDesc>> *>(desc.Parameter);
    BGI_ASSERT(value != NULL);
    return *value;

}