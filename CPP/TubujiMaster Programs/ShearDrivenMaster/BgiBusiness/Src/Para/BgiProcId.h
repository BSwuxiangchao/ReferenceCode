/*
* @Abrief 定义功能业务的ID
* @note 定义用于功能业务处理参数的Id, 以及Id bit的含义
* @version 1.0
* @author wuxiangchao 2023-02-24
*/

#ifndef _BGI_PROC_ID_H_
#define _BGI_PROC_ID_H_

#include <string>
#include <qstring.h>


class BgiProc
{
    //功能业务参数的Id为32bit:
    //bit22~31  保留位, 0
    //bit20~21  为系统的ID，如：默认只有一个系统A，后续视情况添加
    //bit18~19  为是否被记录的类型
    //bit16~17  为参数被处理类型
    //bit12~15  为设备模块功能, 例如机器人、光机、涂布机
    //bit8~11   为参数类型: 参数值, 参数使能状态
    //bit0~7    为具体参数, 例如A轮序列数量

public:
    enum EFieldMask
    {
        SystemIdMask    = 0x03 << 20,
        RecordTypeMask  = 0x03 << 18,
        ProcTypeMask    = 0x0f << 16,
        SystemTypeMask  = 0x0f << 12,
        ParaTypeMask    = 0x0f << 8,
        ParaEnumMask    = 0x0ff
    };
   
    enum ESystemId
    {
        SystemA         = 0x1 << 20,
    };

    enum ERecordType
    {
        RecordType      = 0x1 << 18,
        NoRecordType    = 0x2 << 18,
    };

    enum EProcType
    {
        InputProcType   = 0x1 << 16,
        OutputProcType  = 0x2 << 16,
    };

    enum ESystemType
    {
        GeneralType   = 0x1 << 12,
        RobotType   = 0x2 << 12,
        CoaterType = 0x3 << 12,
        ImagerType = 0x4 << 12,

    };

    enum EParaType
    {
        ParaValue       = 0x1 << 8,
        ParaStatus      = 0x2 << 8
    };

    static int GetParaEnum(int id);
    static ESystemId GetSystemId(QString strSystemId);
    static ESystemType GetSystemType(QString systemType);
    static EProcType GetProcType(QString procType);

    static std::string GetToken(int id);
};

#endif