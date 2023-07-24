#include "RobotConnectParaControl.h"
#include "ParaControlProvider.h"
#include "BgiLogger.h"
#include "RobotParaType.h"
#include "RobotWidget.h"
#include "EpsonRobotDebug.h"

RobotConnectParaControl::RobotConnectParaControl()
	:QObject()
	, m_robotWidget(NULL)
	, m_iConnectCount(0)
{
	m_RobotParaCtrl = ParaControlProvider::Instance()->GetParameterControl("SystemA", "Robot", "OutputProc", "WorkStatus");
	m_RobotParaCtrl->AddObserver(this);
}

RobotConnectParaControl::~RobotConnectParaControl()
{
	m_RobotParaCtrl->DelObserver(this);
}

void RobotConnectParaControl::Update()
{
	const void* const_value = m_RobotParaCtrl->GetValue();
	RobotParaType::RobotStatus* robotStatus = (RobotParaType::RobotStatus*)const_value;
	if (robotStatus->iConnect)
	{
		if (0 == m_iConnectCount)
		{
			emit SignalUpdateStatus(true);
			emit SignalSetRobotConnectStatus(true);
		}
		m_iConnectCount++;
	}
	else
	{
		m_iConnectCount = 0;
		//if (m_userInfo.userType != GeneralParaType::UserType::Operator)
		//	m_SettingPushButton->setEnabled(false);
		//m_robotWidget->SetRobotConnectStatus(false);
		emit SignalUpdateStatus(false);
		emit SignalSetRobotConnectStatus(false);
		
	}
	if (robotStatus->iSafetySpace)
		emit SignalSetRobotSafetyStatus(1);
	else
		emit SignalSetRobotSafetyStatus(0);
}

void RobotConnectParaControl::SlotUpdateStatus(bool bConnect)
{
	if (m_userInfo.userType != GeneralParaType::UserType::Operator)
		m_SettingPushButton->setEnabled(bConnect);
	//m_robotWidget->SetRobotConnectStatus(bConnect);
	if(bConnect == false)
		m_debuDlg->close();
}

void RobotConnectParaControl::SetParentWidget(QPushButton* pushButton, RobotWidget* robotWidget, EpsonRobotDebug* debuDlg)
{
	m_SettingPushButton = pushButton;
	m_robotWidget = robotWidget;
	m_debuDlg = debuDlg;
	connect(this, &RobotConnectParaControl::SignalSetRobotConnectStatus, m_robotWidget, &RobotWidget::SetRobotConnectStatus, Qt::QueuedConnection);
	connect(this, &RobotConnectParaControl::SignalSetRobotSafetyStatus, m_robotWidget, &RobotWidget::SetRobotSafetyStatus, Qt::QueuedConnection);
	connect(this, &RobotConnectParaControl::SignalUpdateStatus, this,&RobotConnectParaControl::SlotUpdateStatus,Qt::QueuedConnection);
}

void RobotConnectParaControl::SetUserInfo(const GeneralParaType::UserInfo userInfo)
{
	m_userInfo = userInfo;
	Update();
}