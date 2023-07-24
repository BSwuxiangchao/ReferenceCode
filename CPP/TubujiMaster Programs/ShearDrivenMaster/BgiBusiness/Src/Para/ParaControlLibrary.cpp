#include "ParaControlLibrary.h"
#include "InputProcId.h"
#include "OutputProcId.h"
#include "GeneralInputParaControl.h"
#include "RobotInputParaControl.h"
#include "RobotOutputParaControl.h"
#include "CoaterInputParaControl.h"
#include "CoaterOutputParaControl.h"
#include "ImagerInputParaControl.h"
#include "ImagerOutputParaControl.h"
#include "BgiLogger.h"
#include "SortingParaCommonName.h"

ParaControlLibrary::ParaControlLibrary()
{
    m_ParaInfoVec.clear();
    //General input
    ProcParaInfo a_input_general_info[] = {
            {InputProc::Gen::start,                     new GeneralInputParaControl("SystemA", "Start")},
            {InputProc::Gen::SequencingDesc,            new GeneralInputParaControl("SystemA", "SequencingDesc")},
    };
    int a_input_general_info_size = sizeof(a_input_general_info) / sizeof(a_input_general_info[0]);
    CreateProcParaInfo(BgiProc::ESystemId::SystemA,
        BgiProc::ESystemType::GeneralType,
        BgiProc::EProcType::InputProcType, a_input_general_info, a_input_general_info_size);
    //Robot input
    ProcParaInfo a_input_synthesis_info[] = {
            {InputProc::Robot::Start,                   new RobotInputParaControl("SystemA", "Start")},
            {InputProc::Robot::ParaOpt,                 new RobotInputParaControl("SystemA", "ParaOpt")},
    };
    int a_input_synthesis_info_size = sizeof(a_input_synthesis_info) / sizeof(a_input_synthesis_info[0]);
    CreateProcParaInfo(BgiProc::ESystemId::SystemA,
        BgiProc::ESystemType::RobotType,
        BgiProc::EProcType::InputProcType, a_input_synthesis_info, a_input_synthesis_info_size);


    //Robot output
    ProcParaInfo a_output_synthesis_info[] = {
            {OutputProc::Robot::ParaOpt,                    new RobotOutputParaControl("SystemA", "ParaOpt")},
            {OutputProc::Robot::WorkStatus,                 new RobotOutputParaControl("SystemA", "WorkStatus")},
            {OutputProc::Robot::RobotPoint,                 new RobotOutputParaControl("SystemA", "RobotPoint")},
    };
    int a_output_synthesis_info_size = sizeof(a_output_synthesis_info) / sizeof(a_output_synthesis_info[0]);
    CreateProcParaInfo(BgiProc::ESystemId::SystemA,
        BgiProc::ESystemType::RobotType,
        BgiProc::EProcType::OutputProcType, a_output_synthesis_info, a_output_synthesis_info_size);
  
    //Coater input
    ProcParaInfo a_input_Coater_info[] = {
            {InputProc::Coater::Start,                   new CoaterInputParaControl("SystemA", "Start")},
    };
    int a_input_coater_info_size = sizeof(a_input_Coater_info) / sizeof(a_input_Coater_info[0]);
    CreateProcParaInfo(BgiProc::ESystemId::SystemA,
        BgiProc::ESystemType::CoaterType,
        BgiProc::EProcType::InputProcType, a_input_Coater_info, a_input_coater_info_size);


    //coater output
    ProcParaInfo a_output_Coater_info[] = {
            {OutputProc::Coater::ParaOpt,                    new CoaterOutputParaControl("SystemA", "ParaOpt")},
    };
    int a_output_coater_info_size = sizeof(a_output_Coater_info) / sizeof(a_output_Coater_info[0]);
    CreateProcParaInfo(BgiProc::ESystemId::SystemA,
        BgiProc::ESystemType::CoaterType,
        BgiProc::EProcType::OutputProcType, a_output_Coater_info, a_output_coater_info_size);

    //Imager input
    ProcParaInfo a_input_Imager_info[] = {
            {InputProc::Imager::Start,                   new ImagerInputParaControl("SystemA", "Start")},
    };
    int a_input_Imager_info_size = sizeof(a_input_Imager_info) / sizeof(a_input_Imager_info[0]);
    CreateProcParaInfo(BgiProc::ESystemId::SystemA,
        BgiProc::ESystemType::ImagerType,
        BgiProc::EProcType::InputProcType, a_input_Imager_info, a_input_Imager_info_size);


    //Imager output
    ProcParaInfo a_output_Imager_info[] = {
            {OutputProc::Imager::Start,                    new ImagerOutputParaControl("SystemA", "Start")},
    };
    int a_output_imager_info_size = sizeof(a_output_Imager_info) / sizeof(a_output_Imager_info[0]);
    CreateProcParaInfo(BgiProc::ESystemId::SystemA,
        BgiProc::ESystemType::ImagerType,
        BgiProc::EProcType::OutputProcType, a_output_Imager_info, a_output_imager_info_size);
}

ParaControlLibrary::~ParaControlLibrary()
{
    std::map<int, ProcParaInfo>::iterator it = m_ParaInfoVec.begin();
    std::map<int, ProcParaInfo>::iterator it_end = m_ParaInfoVec.end();
    for (; it != it_end; ++it)
    {
        if ((it->second).pParaControl != NULL)
        {
            delete (it->second).pParaControl;
            (it->second).pParaControl = NULL;
        }
    }

    m_ParaInfoVec.clear();
}

bool ParaControlLibrary::CheckParameterExist(QString strSystemId, QString strSystemType, QString strProcType, QString strParaName)
{
    bool bFound = false;
    BgiProc::ESystemId sysytem_id = BgiProc::GetSystemId(strSystemId);
    BgiProc::ESystemType sysytem_type = BgiProc::GetSystemType(strSystemType);
    BgiProc::EProcType proc_type = BgiProc::GetProcType(strProcType);

    int para_enum_id = GetParaEnumId(strSystemId, strSystemType, strProcType, strParaName);

    int id = sysytem_id | sysytem_type | proc_type | para_enum_id;
    std::map<int, ProcParaInfo>::const_iterator iter = m_ParaInfoVec.find(para_enum_id);
    std::map<int, ProcParaInfo>::const_iterator iter_end = m_ParaInfoVec.end();
    if(iter != iter_end)
        bFound = true;
    return bFound;
}

ParaControl* ParaControlLibrary::GetParameterControl(QString strSystemId, QString strSystemType, QString strProcType, QString strParaName)
{
    BgiProc::ESystemId sysytem_id = BgiProc::GetSystemId(strSystemId);
    BgiProc::ESystemType sysytem_type = BgiProc::GetSystemType(strSystemType);
    BgiProc::EProcType proc_type = BgiProc::GetProcType(strProcType);

    int para_enum_id = GetParaEnumId(strSystemId, strSystemType, strProcType, strParaName);

    int id = sysytem_id | sysytem_type | proc_type | para_enum_id;
    std::map<int, ProcParaInfo>::const_iterator iter = m_ParaInfoVec.find(id);
    std::map<int, ProcParaInfo>::const_iterator iter_end = m_ParaInfoVec.end();

    if (iter == iter_end)
    {
        BGI_ASSERT(false);
        return NULL;
    }

    ParaControl* control = iter->second.pParaControl;

    return control;
}

int ParaControlLibrary::GetParaEnumId(QString strSystemId, QString strSystemType, QString strProcType, QString strParaName)
{
    BgiProc::ESystemId sysytem_id = BgiProc::GetSystemId(strSystemId);
    BgiProc::ESystemType sysytem_type = BgiProc::GetSystemType(strSystemType);
    BgiProc::EProcType proc_type = BgiProc::GetProcType(strProcType);

    int para_enum_id = 0;
    if (proc_type == BgiProc::EProcType::InputProcType)
    {
        if (sysytem_type == BgiProc::ESystemType::GeneralType)
        {
            para_enum_id = (int)InputProc::Gen::GetParaId(strParaName);
        }
        else if (sysytem_type == BgiProc::ESystemType::RobotType)
        {
            para_enum_id = (int)InputProc::Robot::GetParaId(strParaName);
        }
        else if (sysytem_type == BgiProc::ESystemType::CoaterType)
        {
            para_enum_id = (int)InputProc::Coater::GetParaId(strParaName);
        }
        else if (sysytem_type == BgiProc::ESystemType::ImagerType)
        {
            para_enum_id = (int)InputProc::Imager::GetParaId(strParaName);
        }
    }
    else
    {
        if (sysytem_type == BgiProc::ESystemType::GeneralType)
        {
            para_enum_id = (int)OutputProc::Gen::GetParaId(strParaName);
        }
        else if (sysytem_type == BgiProc::ESystemType::RobotType)
        {
            para_enum_id = (int)OutputProc::Robot::GetParaId(strParaName);
        }
        else if (sysytem_type == BgiProc::ESystemType::CoaterType)
        {
            para_enum_id = (int)OutputProc::Coater::GetParaId(strParaName);
        }
        else if (sysytem_type == BgiProc::ESystemType::ImagerType)
        {
            para_enum_id = (int)OutputProc::Imager::GetParaId(strParaName);
        }
    }
    return para_enum_id;
}

void ParaControlLibrary::CreateProcParaInfo(BgiProc::ESystemId systemId,BgiProc::ESystemType systemType,
    BgiProc::EProcType procType, ProcParaInfo* para_info, int para_info_size)
{
    for(int i = 0; i < para_info_size; i++)
    {
        int para_id = systemId| systemType| procType|para_info[i].iParaEnumId;
        BGI_ASSERT(m_ParaInfoVec.find(para_id) == m_ParaInfoVec.end());

        ProcParaInfo para;
        para.iParaEnumId = para_info[i].iParaEnumId;
        para.pParaControl = para_info[i].pParaControl;
        m_ParaInfoVec[para_id] = para;
    }
}

