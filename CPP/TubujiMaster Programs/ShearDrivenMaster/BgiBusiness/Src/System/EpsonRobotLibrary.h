#pragma once

#include "BusinessExternDef.h"
#include "qstring.h"

class EpsonRobotDriver;

class BGIBUSINE_CLASS_EXPORT EpsonRobotLibrary
{
public:
    ~EpsonRobotLibrary();
    static EpsonRobotLibrary* Instance();

#pragma region 测试接口

    void ShowMsgBox();
    int ReturnValue();
#pragma endregion

#pragma region 打印日志
    //level 0:message  1:warning 2:error
    void PrintRecordData(QString strInfo, int level);
#pragma endregion

#pragma region 输出机器人指令接口
    /* --------------------------机械臂指令集-------------------------------------*/
    bool GoHome();
    bool Execute_Go(QString strPosVar);
    bool Execute_Move(QString strPosVar);
    bool OpenGripper();
    bool CloseGripper();
    //输出IO bit
    bool On_OutputIO(int iIObit);
    bool On_OutputIO(QString strIOName);
    bool Off_OutputIO(int iIObit);
    bool Off_OutputIO(QString strIOName);
    //读取指定输出位的状态
    bool Oport_OutputIO(int iIObit, int& iStatus);
    bool Oport_OutputIO(QString strIOName, int& iStatus);
    //读取输入IO bit
    bool Sw_InputIO(int iIObit, int& iStatus);
    bool Sw_InputIO(QString strIOName, int& iStatus);
#pragma endregion
    //判断是否走到位置
    bool IsMoveToPositionSuccess(QString strPosSign);
    // 判断机械臂是否满足移动条件
    bool IsMotorReadyMove();
    // 判断机械臂是否意外停止
    bool IsMoveStopUnexpected();
    // 判断夹手打开是否成功
    bool IsOpenGripperSuccess();
private:
    EpsonRobotLibrary();

private:
    EpsonRobotDriver* m_EpsonRobotDriver;

};

extern "C" _declspec(dllexport) int ReturnValue()
{
    return EpsonRobotLibrary::Instance()->ReturnValue();
}

extern "C" _declspec(dllexport) void ShowMsgBox()
{
    EpsonRobotLibrary::Instance()->ShowMsgBox();
}

extern "C" _declspec(dllexport) void PrintRecordData(char* strInfo, int level)
{
    EpsonRobotLibrary::Instance()->PrintRecordData(strInfo, level);
}

//goHome 
extern "C" _declspec(dllexport) int GoHome()
{
    if (EpsonRobotLibrary::Instance()->GoHome())
        return 1;
    return 0;
}

// go
extern "C" _declspec(dllexport) int Execute_Go(char* strPosVar)
{
    if (EpsonRobotLibrary::Instance()->Execute_Go(strPosVar))
        return 1;
    return 0;
}

// move
extern "C" _declspec(dllexport) int Execute_Move(char* strPosVar)
{
    if (EpsonRobotLibrary::Instance()->Execute_Move(strPosVar))
        return 1;
    return 0;
}

// 开/关夹手
extern "C" _declspec(dllexport) int OpenGripper()
{
    if (EpsonRobotLibrary::Instance()->OpenGripper())
        return 1;
    return 0;
}

extern "C" _declspec(dllexport) int CloseGripper()
{
    if (EpsonRobotLibrary::Instance()->CloseGripper())
        return 1;
    return 0;
}

//输出IO bit
extern "C" _declspec(dllexport) int On_OutputIO(int iIObit)
{
    return EpsonRobotLibrary::Instance()->On_OutputIO(iIObit);
}

extern "C" _declspec(dllexport) int On_OutputIO_Str(char* strIOName)
{
    return EpsonRobotLibrary::Instance()->On_OutputIO(strIOName);
}

extern "C" _declspec(dllexport) int Off_OutputIO(int iIObit)
{
    return EpsonRobotLibrary::Instance()->Off_OutputIO(iIObit);
}

extern "C" _declspec(dllexport) int Off_OutputIO_Str(char* strIOName)
{
    return EpsonRobotLibrary::Instance()->Off_OutputIO(strIOName);
}

//读取指定输出位的状态
extern "C" _declspec(dllexport) int Oport_OutputIO(int iIObit, int& iStatus)
{
    return EpsonRobotLibrary::Instance()->Oport_OutputIO(iIObit, iStatus);
}

extern "C" _declspec(dllexport) int Oport_OutputIO_Str(char* strIOName, int& iStatus)
{
    return EpsonRobotLibrary::Instance()->Oport_OutputIO(strIOName, iStatus);
}

//读取输入IO bit
extern "C" _declspec(dllexport) int Sw_InputIO(int iIObit, int& iStatus)
{
    return EpsonRobotLibrary::Instance()->Oport_OutputIO(iIObit, iStatus);
}

extern "C" _declspec(dllexport) int Sw_InputIO_Str(char* strIOName, int& iStatus)
{
    return EpsonRobotLibrary::Instance()->Oport_OutputIO(strIOName, iStatus);
}

extern "C" _declspec(dllexport) int IsMoveToPositionSuccess(char* strPosSign)
{
    if (EpsonRobotLibrary::Instance()->IsMoveToPositionSuccess(strPosSign))
        return 1;
    return 0;
}

extern "C" _declspec(dllexport) int IsMotorReadyMove()
{
    if (EpsonRobotLibrary::Instance()->IsMotorReadyMove())
        return 1;
    return 0;
}

extern "C" _declspec(dllexport) int IsMoveStopUnexpected()
{
    if (EpsonRobotLibrary::Instance()->IsMoveStopUnexpected())
        return 1;
    return 0;
}

extern "C" _declspec(dllexport) int IsOpenGripperSuccess()
{
    if (EpsonRobotLibrary::Instance()->IsOpenGripperSuccess())
        return 1;
    return 0;
}