#include "RobotInputParaAgent.h"
#include "ParaObserver.h"
#include "BgiLogger.h"

#include <vector>
using namespace std;

RobotInputParaAgent::RobotInputParaAgent(BgiProc::ESystemId sys_id)
    : m_SystemId(sys_id)
{
    struct ParaEnumValueIdPair {
        int Enum;
        int ValueId;
    };
    ParaEnumValueIdPair enum_id[] = {
        {InputProc::Robot::Start,               InputProc::Robot::Start | BgiProc::ESystemType::RobotType | BgiProc::InputProcType},
        {InputProc::Robot::ParaOpt,               InputProc::Robot::ParaOpt | BgiProc::ESystemType::RobotType | BgiProc::InputProcType},
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

RobotInputParaAgent::~RobotInputParaAgent()
{
}

void RobotInputParaAgent::CreatePara()
{
    int para_enum;
    ParaEnumDesc* desc;
    std::string token;

    para_enum = InputProc::Robot::Start;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    desc = &m_ParaEnumMap[para_enum];
    BGI_ASSERT(desc->Parameter == NULL);
    desc->Parameter = new Parameter<bool>(para_enum, *this);

    para_enum = InputProc::Robot::ParaOpt;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    desc = &m_ParaEnumMap[para_enum];
    BGI_ASSERT(desc->Parameter == NULL);
    desc->Parameter = new Parameter<RobotParaType::RobotPara>(para_enum, *this);

}

void RobotInputParaAgent::ResetPara()
{
    Start().Value(false);
    Start().Status(true);

    RobotParaType::RobotPara para_valve_vec;

    RobotPara().Value(para_valve_vec);
    RobotPara().Status(true);
}

BgiProc::ESystemType RobotInputParaAgent::GetSystemType()
{
    return BgiProc::RobotType;
}

BgiProc::EProcType RobotInputParaAgent::GetProcType()
{
    return BgiProc::InputProcType;
}

BgiProc::ESystemId RobotInputParaAgent::GetSystemId()
{
    return m_SystemId;
}

Parameter<bool>& RobotInputParaAgent::Start()
{
    int para_enum = InputProc::Robot::Start;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    ParaEnumDesc& desc = m_ParaEnumMap[para_enum];

    Parameter<bool>* start = dynamic_cast<Parameter<bool> *>(desc.Parameter);
    BGI_ASSERT(start != NULL);
    return *start;
}

Parameter<RobotParaType::RobotPara>& RobotInputParaAgent::RobotPara()
{
    int para_enum = InputProc::Robot::ParaOpt;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    ParaEnumDesc& desc = m_ParaEnumMap[para_enum];

    Parameter<RobotParaType::RobotPara>* value = dynamic_cast<Parameter<RobotParaType::RobotPara> *>(desc.Parameter);
    BGI_ASSERT(value != NULL);
    return *value;
}
