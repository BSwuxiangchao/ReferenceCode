#include "EpsonRobotEventHandler.h"
#include "EpsonRobotDriver.h"
#include "DevHXS1606SelectValve.h"
#include "BgiLogger.h"
#include "OperatePromptInfo.h"
#include "EpsonRobotDriver.h"
#include "EpsonRobotFlowCtrl.h"
#include "MasterDevices.h"
#include "BgiTcpSocket.h"
#include "ErrorCodeTranslate.h"



EpsonRobotEventHandler::EpsonRobotEventHandler()
	:m_bOptRobotSocket(false)
{
	m_EpsonRobotDriver = MasterDevices::Instance()->GetEpsonRobotDriver();
	m_EpsonRobotDriver->StartRead();

}

EpsonRobotEventHandler::~EpsonRobotEventHandler()
{
	m_EpsonRobotDriver->EndRead();
}

bool EpsonRobotEventHandler::HandleEvent(BgiMpsEvent* pEvent)
{
	m_curEvent = pEvent;
	QString event_action = pEvent->Action();
	if (event_action == "OptRobotSocket")
	{
		OptRobotSocket();
		m_bOptRobotSocket = true;
	}
	if (!m_EpsonRobotDriver->GetConnectStatus() && !m_bOptRobotSocket)
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotEventHandler", "机械手网络未连接，请检查网络状态！", gc_colorYellow);
		return false;
	}
	else
		m_bOptRobotSocket = false;

	if (event_action == "LogIn")
	{
		m_EpsonRobotDriver->Login();
	}
	else if (event_action == "MotorOff")
	{
		m_EpsonRobotDriver->SetMotorsOff();
	}
	else if (event_action == "MotorOn")
	{
		m_EpsonRobotDriver->SetMotorsOn();
	}
	else if (event_action == "MotorReset")
	{
		m_EpsonRobotDriver->Reset();
	}
	else if (event_action == "MotorPause")
	{
		m_EpsonRobotDriver->Pause();
	}
	else if (event_action == "MotorContinue")
	{
		m_EpsonRobotDriver->Continue();
	}
	else if (event_action == "MotorAbort")
	{
		m_EpsonRobotDriver->Abort();
	}
	else if (event_action == "SetPTPSpeed")
	{
		SetPTPSpeed();
	}
	else if (event_action == "SetPTPAccel")
	{
		SetPTPAccel();
	}
	else if (event_action == "SetCPSpeed")
	{
		SetCPSpeed();
	}
	else if (event_action == "SetCPAccel")
	{
		SetCPAccel();
	}
	else if (event_action == "SetRSpeed")
	{
		SetRSpeed();
	}
	else if (event_action == "SetRAccel")
	{
		SetRAccel();
	}
	else if (event_action == "GoBasePoint")
	{
		m_EpsonRobotDriver->GoBasePoint();
	}
	else if (event_action == "GoHome")
	{
		m_EpsonRobotDriver->GoHome();
		//EpsonRobotFlowCtrl::Instance()->TestMsgBox();
	}
	else if (event_action == "MotorGoVar")
	{
		MotorGoVar();
	}
	else if (event_action == "SetPower")
	{
		SetPower();
	}
	else if (event_action == "MotorDoCommand")
	{
		MotorDoCommand();
	}
	else if (event_action == "MotorStep")
	{
		MotorStep();
	}
	else if (event_action == "MotorInit")
	{
		m_EpsonRobotDriver->MotorInit();
	}
	else if (event_action == "Ctrl_TransferPos")
	{
		TransferPos();
	}
	else if (event_action == "Ctrl_OptChips")
	{
		OptChips();
	}
	else if (event_action == "OpenGripper")
	{
		m_EpsonRobotDriver->OpenGripper();
		
	}
	else if (event_action == "CloseGripper")
	{
		m_EpsonRobotDriver->CloseGripper();
	}
	else if (event_action == "Ctrl_RealoadScript")
	{
		EpsonRobotFlowCtrl::Instance()->Ctrl_RealoadScript();
	}
	else if (event_action == "MotorSpeedSet_Low")
	{
		m_EpsonRobotDriver->MotorSetLowSpeed();
	}
	else if (event_action == "MotorSpeedSet_High")
	{
		m_EpsonRobotDriver->MotorSetHighSpeed();
	}
	else if (event_action == "TurnOnOutIO")
	{
		TurnOnOutIO();
	}
	else if (event_action == "TurnOffOutIO")
	{
		TurnOffOutIO();
	}
	return true;
}

void EpsonRobotEventHandler::SetPower()
{
	const QVariantHash& param_hash = m_curEvent->Params();

	QVariant Value = param_hash["Value"];
	m_EpsonRobotDriver->SetPower(Value.toBool());
}

void EpsonRobotEventHandler::SetPTPSpeed()
{
	const QVariantHash& param_hash = m_curEvent->Params();

	QVariant Value = param_hash["Value"];
	m_EpsonRobotDriver->SetPTP_Speed(Value.toInt());
}

void EpsonRobotEventHandler::SetPTPAccel()
{
	const QVariantHash& param_hash = m_curEvent->Params();

	QVariant Value = param_hash["Value"];
	m_EpsonRobotDriver->SetPTP_Accel(Value.toInt());
}

void EpsonRobotEventHandler::SetCPSpeed()
{
	const QVariantHash& param_hash = m_curEvent->Params();

	QVariant Value = param_hash["Value"];
	m_EpsonRobotDriver->SetCP_Speeds(Value.toInt());
}

void EpsonRobotEventHandler::SetCPAccel()
{
	const QVariantHash& param_hash = m_curEvent->Params();

	QVariant Value = param_hash["Value"];
	m_EpsonRobotDriver->SetCP_Accels(Value.toInt());
}

void EpsonRobotEventHandler::SetRSpeed()
{
	const QVariantHash& param_hash = m_curEvent->Params();

	QVariant Value = param_hash["Value"];
	m_EpsonRobotDriver->SetR_Speedr(Value.toInt());
}

void EpsonRobotEventHandler::SetRAccel()
{
	const QVariantHash& param_hash = m_curEvent->Params();

	QVariant Value = param_hash["Value"];
	m_EpsonRobotDriver->SetR_Accelr(Value.toInt());
}

void EpsonRobotEventHandler::TransferPos()
{
	const QVariantHash& param_hash = m_curEvent->Params();

	QVariant Value1 = param_hash["Value1"];
	QVariant Value2 = param_hash["Value2"];
	EpsonRobotFlowCtrl::Instance()->TransferPos(Value1.toInt(), Value2.toInt());
}

void EpsonRobotEventHandler::OptChips()
{
	const QVariantHash& param_hash = m_curEvent->Params();

	QVariant Value1 = param_hash["Value1"];
	QVariant Value2 = param_hash["Value2"];
	EpsonRobotFlowCtrl::Instance()->OptChips(Value1.toInt(), Value2.toInt());
}

void EpsonRobotEventHandler::MotorDoCommand()
{
	const QVariantHash& param_hash = m_curEvent->Params();

	QVariant Value = param_hash["Value"];
	m_EpsonRobotDriver->SendCommand(Value.toString());
}

void EpsonRobotEventHandler::MotorGoVar()
{
	const QVariantHash& param_hash = m_curEvent->Params();
	QVariant Value1 = param_hash["Value1"];
	QVariant Value2 = param_hash["Value2"];
	if (0 == Value1.toInt())
		m_EpsonRobotDriver->Execute_Go(Value2.toString());
	else
		m_EpsonRobotDriver->Execute_Move(Value2.toString());
}

void EpsonRobotEventHandler::MotorStep()
{
	const QVariantHash& param_hash = m_curEvent->Params();

	QVariant Value = param_hash["Value"];
	m_EpsonRobotDriver->Execute_ConsecutiveSport(Value.value<RobotParaType::RobotSportMode>());
}

void EpsonRobotEventHandler::OptRobotSocket()
{
	const QVariantHash& param_hash = m_curEvent->Params();

	QVariant Value = param_hash["Value"];
	if (Value.toBool())
	{
		BgiTcpSocket* pBgiTcpSocket = NULL;
		pBgiTcpSocket = MasterDevices::Instance()->GetRobotSocket();
		pBgiTcpSocket->Open();
	}
	else
	{
		m_EpsonRobotDriver->CloseRobotSocket();
	}
}

void EpsonRobotEventHandler::TurnOnOutIO()
{
	const QVariantHash& param_hash = m_curEvent->Params();

	QVariant Value = param_hash["Value"];
	bool bOk = false;
	int iIoNum = Value.toInt(&bOk);
	if (bOk)
		m_EpsonRobotDriver->On_OutputIO(iIoNum);
	else
		m_EpsonRobotDriver->On_OutputIO(Value.toString());
}

void EpsonRobotEventHandler::TurnOffOutIO()
{
	const QVariantHash& param_hash = m_curEvent->Params();

	QVariant Value = param_hash["Value"];
	bool bOk = false;
	int iIoNum = Value.toInt(&bOk);
	if (bOk)
		m_EpsonRobotDriver->Off_OutputIO(iIoNum);
	else
		m_EpsonRobotDriver->Off_OutputIO(Value.toString());
}