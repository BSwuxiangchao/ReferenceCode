#include "RobotOutputParaAgent.h"
#include "ParaObserver.h"
#include "BgiLogger.h"

#include <vector>
using namespace std;

RobotOutputParaAgent::RobotOutputParaAgent(BgiProc::ESystemId sys_id)
    : m_SystemId(sys_id)
{
    struct ParaEnumValueIdPair {
        int Enum;
        int ValueId;
    };
    ParaEnumValueIdPair enum_id[] = {
        {OutputProc::Robot::ParaOpt,                OutputProc::Robot::ParaOpt | BgiProc::RobotType | BgiProc::OutputProcType},
        {OutputProc::Robot::WorkStatus,             OutputProc::Robot::WorkStatus | BgiProc::RobotType | BgiProc::OutputProcType},
        {OutputProc::Robot::RobotPoint,             OutputProc::Robot::RobotPoint | BgiProc::RobotType | BgiProc::OutputProcType},
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

RobotOutputParaAgent::~RobotOutputParaAgent()
{
}

void RobotOutputParaAgent::CreatePara()
{
    int para_enum;
    ParaEnumDesc* desc;
    std::string token;

    para_enum = OutputProc::Robot::ParaOpt;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    desc = &m_ParaEnumMap[para_enum];
    BGI_ASSERT(desc->Parameter == NULL);
    desc->Parameter = new Parameter<RobotParaType::RobotPara>(para_enum, *this);

    para_enum = OutputProc::Robot::WorkStatus;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    desc = &m_ParaEnumMap[para_enum];
    BGI_ASSERT(desc->Parameter == NULL);
    desc->Parameter = new Parameter<RobotParaType::RobotStatus>(para_enum, *this);

    para_enum = OutputProc::Robot::RobotPoint;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    desc = &m_ParaEnumMap[para_enum];
    BGI_ASSERT(desc->Parameter == NULL);
    desc->Parameter = new Parameter<RobotParaType::RobotPoint>(para_enum, *this);
}

void RobotOutputParaAgent::ResetPara()
{
    RobotParaType::RobotPara papa_robot;
  
    RobotPara().Value(papa_robot);
    RobotPara().Status(true);

    RobotParaType::RobotStatus status_robot;
    RobotStatus().Value(status_robot);
    RobotStatus().Status(true);

    RobotParaType::RobotPoint point_robot;
    RobotPoint().Value(point_robot);
    RobotPoint().Status(true);

}

BgiProc::ESystemType RobotOutputParaAgent::GetSystemType()
{
    return BgiProc::ESystemType::RobotType;
}

BgiProc::EProcType RobotOutputParaAgent::GetProcType()
{
    return BgiProc::OutputProcType;
}

BgiProc::ESystemId RobotOutputParaAgent::GetSystemId()
{
    return m_SystemId;
}

Parameter<RobotParaType::RobotPara>& RobotOutputParaAgent::RobotPara()
{
    int para_enum = OutputProc::Robot::ParaOpt;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    ParaEnumDesc& desc = m_ParaEnumMap[para_enum];

    Parameter<RobotParaType::RobotPara>* chip_cnt = dynamic_cast<Parameter<RobotParaType::RobotPara> *>(desc.Parameter);
    BGI_ASSERT(chip_cnt != NULL);
    return *chip_cnt;
}

Parameter<RobotParaType::RobotStatus>& RobotOutputParaAgent::RobotStatus()
{
    int para_enum = OutputProc::Robot::WorkStatus;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    ParaEnumDesc& desc = m_ParaEnumMap[para_enum];

    Parameter<RobotParaType::RobotStatus>* chip_cnt = dynamic_cast<Parameter<RobotParaType::RobotStatus> *>(desc.Parameter);
    BGI_ASSERT(chip_cnt != NULL);
    return *chip_cnt;
}

Parameter<RobotParaType::RobotPoint>& RobotOutputParaAgent::RobotPoint()
{
    int para_enum = OutputProc::Robot::RobotPoint;
    BGI_ASSERT(m_ParaEnumMap.find(para_enum) != m_ParaEnumMap.end());
    ParaEnumDesc& desc = m_ParaEnumMap[para_enum];

    Parameter<RobotParaType::RobotPoint>* chip_cnt = dynamic_cast<Parameter<RobotParaType::RobotPoint> *>(desc.Parameter);
    BGI_ASSERT(chip_cnt != NULL);
    return *chip_cnt;
}
