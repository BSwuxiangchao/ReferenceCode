#include "EpsonRobotLibrary.h"
#include "MasterDevices.h"
#include "EpsonRobotDriver.h"
#include "BgiLogger.h"
#include "OperateRecordInfo.h"
#include "CommonColors.h"

EpsonRobotLibrary::EpsonRobotLibrary()
	:m_EpsonRobotDriver(NULL)
{
	m_EpsonRobotDriver = MasterDevices::Instance()->GetEpsonRobotDriver();
}

EpsonRobotLibrary::~EpsonRobotLibrary()
{
}

EpsonRobotLibrary* EpsonRobotLibrary::Instance()
{
	static EpsonRobotLibrary instance;
	return &instance;
}

#pragma region 测试接口
void EpsonRobotLibrary::ShowMsgBox()
{
	m_EpsonRobotDriver->TestShowMsgBox();
}

int EpsonRobotLibrary::ReturnValue()
{
	return false;
}

#pragma endregion



//level 0:message  1:warning 2:error
void EpsonRobotLibrary::PrintRecordData(QString strInfo, int level)
{
	RecordData data;
	data.m_strInfoType = "Python调度";
	data.m_strContent = strInfo;

	if (level == 0)
		BGI_MESSAGE(strInfo.toStdString().c_str());
	else if (level == 1)
	{
		BGI_WARNING(strInfo.toStdString().c_str());
		data.m_color = gc_colorYellow;
	}
	else
	{
		BGI_ERROR(strInfo.toStdString().c_str());
		data.m_color = gc_colorRed;
	}
	OperateRecordInfo::AddRecordData(data);
}

bool EpsonRobotLibrary::GoHome()
{
	return m_EpsonRobotDriver->GoHome();
}

bool EpsonRobotLibrary::Execute_Go(QString strPosVar)
{
	return m_EpsonRobotDriver->Execute_Go(strPosVar);
}

bool EpsonRobotLibrary::Execute_Move(QString strPosVar)
{
	return m_EpsonRobotDriver->Execute_Move(strPosVar);
}

bool EpsonRobotLibrary::OpenGripper()
{
	return m_EpsonRobotDriver->OpenGripper();
}

bool EpsonRobotLibrary::CloseGripper()
{
	return m_EpsonRobotDriver->CloseGripper();
}

//输出IO bit
bool EpsonRobotLibrary::On_OutputIO(int iIObit)
{
	return m_EpsonRobotDriver->On_OutputIO(iIObit);
}

bool EpsonRobotLibrary::On_OutputIO(QString strIOName)
{
	return m_EpsonRobotDriver->On_OutputIO(strIOName);
}

bool EpsonRobotLibrary::Off_OutputIO(int iIObit)
{
	return m_EpsonRobotDriver->Off_OutputIO(iIObit);
}

bool EpsonRobotLibrary::Off_OutputIO(QString strIOName)
{
	return m_EpsonRobotDriver->Off_OutputIO(strIOName);
}

//读取指定输出位的状态
bool EpsonRobotLibrary::Oport_OutputIO(int iIObit, int& iStatus)
{
	return m_EpsonRobotDriver->Oport_OutputIO(iIObit, iStatus);
}

bool EpsonRobotLibrary::Oport_OutputIO(QString strIOName, int& iStatus)
{
	return m_EpsonRobotDriver->Oport_OutputIO(strIOName, iStatus);
}

//读取输入IO bit
bool EpsonRobotLibrary::Sw_InputIO(int iIObit, int& iStatus)
{
	return m_EpsonRobotDriver->Sw_InputIO(iIObit, iStatus);
}

bool EpsonRobotLibrary::Sw_InputIO(QString strIOName, int& iStatus)
{
	return m_EpsonRobotDriver->Sw_InputIO(strIOName, iStatus);
}

bool EpsonRobotLibrary::IsMoveToPositionSuccess(QString strPosSign)
{
	return m_EpsonRobotDriver->IsMoveToPositionSuccess(strPosSign);
}

// 判断机械臂是否满足移动条件
bool EpsonRobotLibrary::IsMotorReadyMove()
{
	return m_EpsonRobotDriver->IsMotorReadyMove();
}

bool EpsonRobotLibrary::IsMoveStopUnexpected()
{
	return m_EpsonRobotDriver->IsMoveStopUnexpected();
}

// 判断夹手打开是否成功
bool EpsonRobotLibrary::IsOpenGripperSuccess()
{
	return m_EpsonRobotDriver->IsOpenGripperSuccess();
}