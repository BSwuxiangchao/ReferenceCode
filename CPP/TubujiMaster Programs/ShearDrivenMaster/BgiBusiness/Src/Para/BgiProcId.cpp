#include "BgiProcId.h"
#include "InputProcId.h"
#include "OutputProcId.h"
#include "BgiLogger.h"

int BgiProc::GetParaEnum(int id)
{
    return id & EFieldMask::ParaEnumMask;
}

BgiProc::ESystemId BgiProc::GetSystemId(QString strsystemId)
{
    struct SystemIdTokenTable
    {
        BgiProc::ESystemId SystemId;
        QString Token;
    };

    SystemIdTokenTable system_id_token_table[] = {
        {SystemA,   "SystemA"},
    };
    int sys_id_token_table_len = int(sizeof(system_id_token_table) / sizeof(system_id_token_table[0]));
    for (int i = 0; i < sys_id_token_table_len; i++)
    {
        if (system_id_token_table[i].Token == strsystemId)
        {
            return system_id_token_table[i].SystemId;
        }
    }

    BGI_ASSERT(false);

    return BgiProc::ESystemId::SystemA;
}


BgiProc::ESystemType BgiProc::GetSystemType(QString systemType)
{
    struct SystemTypeTokenTable
    {
        ESystemType SystemType;
        QString Token;
    };
    SystemTypeTokenTable system_type_token_table[] = {
        {ESystemType::GeneralType,          "General"},
        {ESystemType::RobotType,            "Robot"},
        {ESystemType::CoaterType,           "Coater"},
        {ESystemType::ImagerType,           "Imager"}
    };
    int system_type_token_table_len = int(sizeof(system_type_token_table) / sizeof(system_type_token_table[0]));
    for (int i = 0; i < system_type_token_table_len; i++)
    {
        if (system_type_token_table[i].Token == systemType)
        {
            return system_type_token_table[i].SystemType;
        }
    }
    
    BGI_ASSERT(false);
    return BgiProc::ESystemType::GeneralType;
}


BgiProc::EProcType BgiProc::GetProcType(QString procType)
{
    struct ProcTypeTokenTable
    {
        BgiProc::EProcType ProcType;
        QString Token;
    };
    ProcTypeTokenTable proc_token_table[] = {
        {EProcType::InputProcType,   "InputProc"},
        {EProcType::OutputProcType,  "OutputProc"},
    };
    int proc_token_table_len = int(sizeof(proc_token_table) / sizeof(proc_token_table[0]));

    bool proc_type_ok = false;
    for (int i = 0; i < proc_token_table_len; i++)
    {
        if (proc_token_table[i].Token == procType)
        {
            return proc_token_table[i].ProcType;
        }
    }

    BGI_ASSERT(false);
    return BgiProc::EProcType::InputProcType;
}


std::string BgiProc::GetToken(int id)
{
    BgiProc::ERecordType record_type = (BgiProc::ERecordType)(id & BgiProc::EFieldMask::RecordTypeMask);
    BgiProc::EProcType proc_type = (BgiProc::EProcType)(id & BgiProc::EFieldMask::ProcTypeMask);
    BgiProc::ESystemId system_id = (BgiProc::ESystemId)(id & BgiProc::EFieldMask::SystemIdMask);
    BgiProc::ESystemType system_type = (BgiProc::ESystemType)(id & BgiProc::EFieldMask::SystemTypeMask);
    BgiProc::EParaType para_type = (BgiProc::EParaType)(id & BgiProc::EFieldMask::ParaTypeMask);
    int para_enum = (id & EFieldMask::ParaEnumMask);

    std::string token;
    if (record_type == ERecordType::NoRecordType)
        return token;

    BGI_ASSERT(record_type == ERecordType::RecordType);
    token = "Record";

    struct ProcTypeTokenTable
    {
        BgiProc::EProcType ProcType;
        const char* Token;
    };
    ProcTypeTokenTable proc_token_table[] = {
        {EProcType::InputProcType,   "InputProc"},
        {EProcType::OutputProcType,  "OutputProc"},
    };
    int proc_token_table_len = int(sizeof(proc_token_table) / sizeof(proc_token_table[0]));

    bool proc_type_ok = false;
    for (int i = 0; i < proc_token_table_len; i++)
    {
        if (proc_token_table[i].ProcType == proc_type)
        {
            token += ".";
            token += proc_token_table[i].Token;
            proc_type_ok = true;
        }
    }
    BGI_ASSERT(proc_type_ok);

    //
    struct SystemIdTokenTable
    {
        BgiProc::ESystemId SystemId;
        const char* Token;
    };
    SystemIdTokenTable system_id_token_table[] = {
        {SystemA,   "SystemA"},
    };
    int sys_id_token_table_len = int(sizeof(system_id_token_table) / sizeof(system_id_token_table[0]));

    //获取SystemId的Token
    bool sys_id_token_ok = false;
    for (int i = 0; i < sys_id_token_table_len; i++)
    {
        if (system_id_token_table[i].SystemId == system_id)
        {
            token += ".";
            token += system_id_token_table[i].Token;
            sys_id_token_ok = true;
        }
    }
    BGI_ASSERT(sys_id_token_ok);

    //
    struct SystemTypeTokenTable
    {
        ESystemType SystemType;
        const char* Token;
    };
    SystemTypeTokenTable system_type_token_table[] = {
        {ESystemType::GeneralType,          "General"},
        {ESystemType::RobotType,            "Robot"},
        {ESystemType::CoaterType,           "Coater"},
        {ESystemType::ImagerType,           "Imager"}
    };
    int system_type_token_table_len = int(sizeof(system_type_token_table) / sizeof(system_type_token_table[0]));

    bool system_type_token_ok = false;
    for (int i = 0; i < system_type_token_table_len; i++)
    {
        if (system_type_token_table[i].SystemType == system_type)
        {
            token += ".";
            token += system_type_token_table[i].Token;
            system_type_token_ok = true;
        }
    }
    BGI_ASSERT(system_type_token_ok);



    struct ParaTypeTokenTable
    {
        EParaType ParaType;
        const char* Token;
    };
    ParaTypeTokenTable para_type_token_table[] = {
        {EParaType::ParaValue,     "Value"},
        {EParaType::ParaStatus,    "Status"}
    };
    int para_type_token_table_len = int(sizeof(para_type_token_table) / sizeof(para_type_token_table[0]));

    bool para_type_token_ok = false;
    for (int i = 0; i < para_type_token_table_len; i++)
    {
        if (para_type_token_table[i].ParaType == para_type)
        {
            token += ".";
            token += para_type_token_table[i].Token;
            para_type_token_ok = true;
        }
    }
    BGI_ASSERT(para_type_token_ok);

    struct TokenFuncTable
    {
        ESystemType SystemType;
        std::string(*TokenFunc)(int id);
    };
    TokenFuncTable input_token_func_table[] = {
        {ESystemType::GeneralType,          InputProc::Gen::GetToken},
        {ESystemType::RobotType,            InputProc::Robot::GetToken},
        {ESystemType::CoaterType,           InputProc::Coater::GetToken},
        {ESystemType::ImagerType,           InputProc::Imager::GetToken},
    };
    int input_token_func_table_len = int(sizeof(input_token_func_table) / sizeof(input_token_func_table[0]));

    TokenFuncTable output_token_func_table[] = {
        {ESystemType::GeneralType,          OutputProc::Gen::GetToken},
        {ESystemType::RobotType,            OutputProc::Robot::GetToken},
        {ESystemType::CoaterType,           OutputProc::Coater::GetToken},
        {ESystemType::ImagerType,           OutputProc::Imager::GetToken},
    };
    int output_token_func_table_len = int(sizeof(output_token_func_table) / sizeof(output_token_func_table[0]));

    bool para_token_ok = false;
    TokenFuncTable* token_func_table = NULL;
    int token_func_table_len = 0;
    switch (proc_type)
    {
    case EProcType::InputProcType:
        token_func_table = input_token_func_table;
        token_func_table_len = input_token_func_table_len;
        break;
    case EProcType::OutputProcType:
        token_func_table = output_token_func_table;
        token_func_table_len = output_token_func_table_len;
        break;
    default:
        BGI_ASSERT(false);
        break;
    }
    for (int i = 0; i < token_func_table_len; i++)
    {
        if (token_func_table[i].SystemType == (BgiProc::ESystemType)(id & EFieldMask::SystemTypeMask))
        {
            token += ".";
            token += token_func_table[i].TokenFunc(para_enum);
            para_token_ok = true;
        }
    }
    BGI_ASSERT(para_token_ok);

    return token;
}