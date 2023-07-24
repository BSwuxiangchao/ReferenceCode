/*
* @Abrief 定义指令接口
* @note 描述外界发送给参数系统的命令
* @version 1.0
* @author mosongjin 2021-07-30
*/

#ifndef _PARA_CMD_H_
#define _PARA_CMD_H_

#include "BgiProcId.h"
#include <typeinfo>

struct ParaCmd
{
public:
    enum ECmdType {
        IncLevel,
        DecLevel,
        SetLevel,
        SetValue
    };

    BgiProc::EProcType ProcType;
    BgiProc::ESystemType SystemType;
    BgiProc::ESystemId SystemId;
    int ParaEnum;
    ECmdType Type;
    int Level;
    const void* Value;
    const std::type_info* ValueType;

    ParaCmd() 
    {
        ProcType = BgiProc::EProcType::InputProcType;
        SystemType = BgiProc::ESystemType::GeneralType;
        SystemId = BgiProc::ESystemId::SystemA;
        ParaEnum = 0;
        Type = ParaCmd::IncLevel;
        Level = 0;
        Value = NULL;
        ValueType = NULL;
    }
};

#endif