#include "RobotDebugParaControl.h"
#include "ParaControlProvider.h"
#include "BgiLogger.h"
#include "languagemgr.h"

RobotDebugParaControl::RobotDebugParaControl()
	:QObject()
	, m_changeType(toChinese)
	, m_bInitCtrlWidgets(false)
{
	m_RobotParaParaCtrl  = ParaControlProvider::Instance()->GetParameterControl("SystemA", "Robot", "OutputProc", "ParaOpt");
	m_RobotParaParaCtrl->AddObserver(this);
	m_RobotWorkStatusParaCtrl = ParaControlProvider::Instance()->GetParameterControl("SystemA", "Robot", "OutputProc", "WorkStatus");
	m_RobotWorkStatusParaCtrl->AddObserver(this);
	m_RobotPointParaParaCtrl = ParaControlProvider::Instance()->GetParameterControl("SystemA", "Robot", "OutputProc", "RobotPoint");
	m_RobotPointParaParaCtrl->AddObserver(this);
	connect(this, &RobotDebugParaControl::SignalUpdateStatus, this, &RobotDebugParaControl::SlotUpdateStatus);
	InitCtrlWidgets();
}

RobotDebugParaControl::~RobotDebugParaControl()
{
	m_RobotParaParaCtrl->DelObserver(this);
	m_RobotWorkStatusParaCtrl->DelObserver(this);
	m_RobotPointParaParaCtrl->DelObserver(this);
}

void RobotDebugParaControl::InitCtrlWidgets()
{
	m_label_SecuritySwitch1 = NULL;
	m_label_SecuritySwitch2 = NULL;
	m_label_SecuritySwitch3 = NULL;
	m_label_PauseSwitch1 = NULL;
	m_label_PauseSwitch2 = NULL;
	m_label_PauseSwitch3 = NULL;
	m_label_MotorSwitch1 = NULL;
	m_label_MotorSwitch2 = NULL;
	m_label_MotorSwitch3 = NULL;
	m_label_MotorPower1 = NULL;
	m_label_MotorPower2 = NULL;
	m_label_MotorPower3 = NULL;
	m_label_Error1 = NULL;
	m_label_Error2 = NULL;
	m_label_Error3 = NULL;
	m_lineEdit_ErrCode1 = NULL;
	m_lineEdit_ErrCode2 = NULL;
	m_lineEdit_ErrCode3 = NULL;

	m_lineEdit_GetRobotNo = NULL;
	m_lineEdit_GetPTPSpeed = NULL;
	m_lineEdit_GetPTPAccel = NULL;
	m_lineEdit_GetCPSpeed = NULL;
	m_lineEdit_GetCPAccel = NULL;
	m_lineEdit_GetPTPSpeed2 = NULL;
	m_lineEdit_GetPTPAccel2 = NULL;
	m_lineEdit_GetCPSpeed2 = NULL;
	m_lineEdit_GetCPAccel2 = NULL;

	m_lineEdit_PosX = NULL;
	m_lineEdit_PosY = NULL;
	m_lineEdit_PosZ = NULL;
	m_lineEdit_PosU = NULL;
	m_lineEdit_PosV = NULL;
	m_lineEdit_PosW = NULL;

	m_pStatusBar = NULL;
}

void RobotDebugParaControl::InitStatusLabel()
{
	m_statusLabel = new QLabel();
	m_statusLabel->setMinimumSize(300, 30); // 设置标签最小大小
	m_statusLabel->setFrameShape(QFrame::WinPanel); // 设置标签形状
	m_statusLabel->setFrameShadow(QFrame::Sunken); // 设置标签阴影
	m_pStatusBar->addWidget(m_statusLabel);
}

void RobotDebugParaControl::Update()
{
	if (!m_bInitCtrlWidgets)
		return;
	emit SignalUpdateStatus();
}

void RobotDebugParaControl::UpdateParaUnit(RobotParaType::RobotPara* robotPara)
{
	m_lineEdit_GetRobotNo->setText(QString("%1").arg(robotPara->iMotorNo));
	m_lineEdit_GetPTPSpeed->setText(QString("%1").arg(robotPara->iPTPSpeed));
	m_lineEdit_GetPTPAccel->setText(QString("%1").arg(robotPara->iPTPAccel));
	m_lineEdit_GetCPSpeed->setText(QString("%1").arg(robotPara->iCPSpeed));
	m_lineEdit_GetCPAccel->setText(QString("%1").arg(robotPara->iCPAccel));
	m_lineEdit_GetRSpeed->setText(QString("%1").arg(robotPara->iRSpeed));
	m_lineEdit_GetRAccel->setText(QString("%1").arg(robotPara->iRAccel));
	m_lineEdit_GetPTPSpeed2->setText(QString("%1").arg(robotPara->iPTPSpeed));
	m_lineEdit_GetPTPAccel2->setText(QString("%1").arg(robotPara->iPTPAccel));
	m_lineEdit_GetCPSpeed2->setText(QString("%1").arg(robotPara->iCPSpeed));
	m_lineEdit_GetCPAccel2->setText(QString("%1").arg(robotPara->iCPAccel));
}

void RobotDebugParaControl::SlotUpdateStatus()
{
	const void* const_value = m_RobotParaParaCtrl->GetValue();
	RobotParaType::RobotPara* robotPara = (RobotParaType::RobotPara*)const_value;
	UpdateParaUnit(robotPara);
	const_value = m_RobotWorkStatusParaCtrl->GetValue();
	RobotParaType::RobotStatus* robotStatus = (RobotParaType::RobotStatus*)const_value;
	UpdateStatusUnit(robotStatus);
	const_value = m_RobotPointParaParaCtrl->GetValue();
	RobotParaType::RobotPoint* robotPoint = (RobotParaType::RobotPoint*)const_value;
	UpdatePointUnit(robotPoint);
}

void RobotDebugParaControl::UpdateStatusUnit(RobotParaType::RobotStatus* robotStatus)
{
	CLanguageMgr* LanguageMgr = CLanguageMgr::Instance();
	if (robotStatus->iSafetyOn == 0)
	{
		m_label_SecuritySwitch1->setText("关");
		m_label_SecuritySwitch1->setStyleSheet("color: rgb(0, 220, 0);");
		m_label_SecuritySwitch2->setText("关");
		m_label_SecuritySwitch2->setStyleSheet("color: rgb(0, 220, 0);");
		m_label_SecuritySwitch3->setText("关");
		m_label_SecuritySwitch3->setStyleSheet("color: rgb(0, 220, 0);");
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_SecuritySwitch1);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_SecuritySwitch2);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_SecuritySwitch3);
	}
	else
	{
		m_label_SecuritySwitch1->setText("开");
		m_label_SecuritySwitch1->setStyleSheet("color: rgb(220, 0, 0);");
		m_label_SecuritySwitch2->setText("开");
		m_label_SecuritySwitch2->setStyleSheet("color: rgb(220, 0, 0);");
		m_label_SecuritySwitch3->setText("开");
		m_label_SecuritySwitch3->setStyleSheet("color: rgb(220, 0, 0);");
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_SecuritySwitch1);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_SecuritySwitch2);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_SecuritySwitch3);
	}

	if (robotStatus->iEstopOn == 0)
	{
		m_label_PauseSwitch1->setText("关");
		m_label_PauseSwitch1->setStyleSheet("color: rgb(0, 220, 0);");
		m_label_PauseSwitch2->setText("关");
		m_label_PauseSwitch2->setStyleSheet("color: rgb(0, 220, 0);");
		m_label_PauseSwitch3->setText("关");
		m_label_PauseSwitch3->setStyleSheet("color: rgb(0, 220, 0);");
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_PauseSwitch1);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_PauseSwitch2);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_PauseSwitch3);
	}
	else
	{
		m_label_PauseSwitch1->setText("开");
		m_label_PauseSwitch1->setStyleSheet("color: rgb(220, 0, 0);");
		m_label_PauseSwitch2->setText("开");
		m_label_PauseSwitch2->setStyleSheet("color: rgb(220, 0, 0);");
		m_label_PauseSwitch3->setText("开");
		m_label_PauseSwitch3->setStyleSheet("color: rgb(220, 0, 0);");
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_PauseSwitch1);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_PauseSwitch2);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_PauseSwitch3);
	}

	if (robotStatus->iMotorOn == 0)
	{
		m_label_MotorSwitch1->setStyleSheet("color: rgb(0, 220, 0);");
		m_label_MotorSwitch1->setText("关");
		m_label_MotorSwitch2->setStyleSheet("color: rgb(0, 220, 0);");
		m_label_MotorSwitch2->setText("关");
		m_label_MotorSwitch3->setStyleSheet("color: rgb(0, 220, 0);");
		m_label_MotorSwitch3->setText("关");
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_MotorSwitch1);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_MotorSwitch2);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_MotorSwitch3);
	}
	else
	{
		m_label_MotorSwitch1->setText("开");
		m_label_MotorSwitch1->setStyleSheet("color: rgb(0, 0, 220);");
		m_label_MotorSwitch2->setText("开");
		m_label_MotorSwitch2->setStyleSheet("color: rgb(0, 0, 220);");
		m_label_MotorSwitch3->setText("开");
		m_label_MotorSwitch3->setStyleSheet("color: rgb(0, 0, 220);");
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_MotorSwitch1);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_MotorSwitch2);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_MotorSwitch3);
	}

	if (robotStatus->iPower == 0)
	{
		m_label_MotorPower1->setText("低");
		m_label_MotorPower1->setStyleSheet("color: rgb(0, 220, 0);");
		m_label_MotorPower2->setText("低");
		m_label_MotorPower2->setStyleSheet("color: rgb(0, 220, 0);");
		m_label_MotorPower3->setText("低");
		m_label_MotorPower3->setStyleSheet("color: rgb(0, 220, 0);");
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_MotorPower1);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_MotorPower2);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_MotorPower3);
	}
	else
	{
		m_label_MotorPower1->setText("高");
		m_label_MotorPower1->setStyleSheet("color: rgb(0, 0, 220);");
		m_label_MotorPower2->setText("高");
		m_label_MotorPower2->setStyleSheet("color: rgb(0, 0, 220);");
		m_label_MotorPower3->setText("高");
		m_label_MotorPower3->setStyleSheet("color: rgb(0, 0, 220);");
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_MotorPower1);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_MotorPower2);
		LanguageMgr->ChangeUIControlTextUnit(m_changeType, NULL, m_label_MotorPower3);
	}

	m_lineEdit_ErrCode1->setText(QString("%1").arg(robotStatus->iErrorCode));
	m_lineEdit_ErrCode2->setText(QString("%1").arg(robotStatus->iErrorCode));
	m_lineEdit_ErrCode3->setText(QString("%1").arg(robotStatus->iErrorCode));
	m_label_Error1->setText("返回码：");
	m_label_Error2->setText("返回码：");
	m_label_Error3->setText("返回码：");
	if (robotStatus->iErrorCode == 0)
	{
		m_lineEdit_ErrCode1->setText("0000");
		m_lineEdit_ErrCode2->setText("0000");
		m_lineEdit_ErrCode3->setText("0000");
		m_label_Error1->setStyleSheet("background-color: rgb(0, 220, 0);");
		m_label_Error2->setStyleSheet("background-color: rgb(0, 220, 0);");
		m_label_Error3->setStyleSheet("background-color: rgb(0, 220, 0);");
	}
	else if (robotStatus->iErrorCode == 21)
	{
		m_label_Error1->setText("运行中：");
		m_label_Error2->setText("运行中：");
		m_label_Error3->setText("运行中：");
		m_label_Error1->setStyleSheet("QLabel{background:DeepSkyBlue}");
		m_label_Error2->setStyleSheet("QLabel{background:DeepSkyBlue}");
		m_label_Error3->setStyleSheet("QLabel{background:DeepSkyBlue}");
	}
	else
	{
		m_label_Error1->setText("错误码：");
		m_label_Error2->setText("错误码：");
		m_label_Error3->setText("错误码：");
		m_label_Error1->setStyleSheet("background-color: rgb(220, 0, 0);");
		m_label_Error2->setStyleSheet("background-color: rgb(220, 0, 0);");
		m_label_Error3->setStyleSheet("background-color: rgb(220, 0, 0);");
	}
}

void RobotDebugParaControl::UpdatePointUnit(RobotParaType::RobotPoint* robotPoint)
{
	m_lineEdit_PosX->setText(QString::number(robotPoint->f_X, 'f', 3));
	m_lineEdit_PosY->setText(QString::number(robotPoint->f_Y, 'f', 3));
	m_lineEdit_PosZ->setText(QString::number(robotPoint->f_Z, 'f', 3));
	m_lineEdit_PosU->setText(QString::number(robotPoint->f_U, 'f', 3));
	m_lineEdit_PosV->setText(QString::number(robotPoint->f_V, 'f', 3));
	m_lineEdit_PosW->setText(QString::number(robotPoint->f_W, 'f', 3));
	m_statusLabel->setText(QString("当前点位名称：%1").arg(robotPoint->strPointName));
}


void RobotDebugParaControl::SetParentWidget(QGroupBox* RobotPara, QGroupBox* WorkStatus1, QGroupBox* WorkStatus2, QGroupBox* robotPoint,
	QGroupBox* WorkStatus3, QGroupBox* RobotPara2, QStatusBar* qStatusBar)
{
	if (NULL != RobotPara && NULL != WorkStatus1 && NULL != WorkStatus2 && NULL != robotPoint && NULL != WorkStatus3 && NULL != RobotPara2)
	{
		m_bInitCtrlWidgets = true;
	}
	else
		return;
	//m_pComboBoxSystem = m_pGroupBoxFlowmeter->findChild<QComboBox*>("comboBox_flowmeter_systemType");
	m_label_SecuritySwitch1 = WorkStatus1->findChild<QLabel*>("label_SecuritySwitch");
	m_label_SecuritySwitch2 = WorkStatus2->findChild<QLabel*>("label_SecuritySwitch_2");
	m_label_SecuritySwitch3 = WorkStatus3->findChild<QLabel*>("label_SecuritySwitch_3");
	m_label_PauseSwitch1 = WorkStatus1->findChild<QLabel*>("label_PauseSwitch");
	m_label_PauseSwitch2 = WorkStatus2->findChild<QLabel*>("label_PauseSwitch_2");
	m_label_PauseSwitch3 = WorkStatus3->findChild<QLabel*>("label_PauseSwitch_3");
	m_label_MotorSwitch1 = WorkStatus1->findChild<QLabel*>("label_MotorSwitch");
	m_label_MotorSwitch2 = WorkStatus2->findChild<QLabel*>("label_MotorSwitch_2");
	m_label_MotorSwitch3 = WorkStatus3->findChild<QLabel*>("label_MotorSwitch_3");
	m_label_MotorPower1 = WorkStatus1->findChild<QLabel*>("label_MotorPower");
	m_label_MotorPower2 = WorkStatus2->findChild<QLabel*>("label_MotorPower_2");
	m_label_MotorPower3 = WorkStatus3->findChild<QLabel*>("label_MotorPower_3");
	m_label_Error1 = WorkStatus1->findChild<QLabel*>("label_Error");
	m_label_Error2 = WorkStatus2->findChild<QLabel*>("label_Error_2");
	m_label_Error3 = WorkStatus3->findChild<QLabel*>("label_Error_3");
	m_lineEdit_ErrCode1 = WorkStatus1->findChild<QLineEdit*>("lineEdit_ErrCode");
	m_lineEdit_ErrCode2 = WorkStatus2->findChild<QLineEdit*>("lineEdit_ErrCode_2");
	m_lineEdit_ErrCode3 = WorkStatus3->findChild<QLineEdit*>("lineEdit_ErrCode_3");

	m_lineEdit_GetRobotNo = RobotPara->findChild<QLineEdit*>("lineEdit_GetRobotNo");
	m_lineEdit_GetPTPSpeed = RobotPara->findChild<QLineEdit*>("lineEdit_GetPTPSpeed");
	m_lineEdit_GetPTPAccel = RobotPara->findChild<QLineEdit*>("lineEdit_GetPTPAccel");
	m_lineEdit_GetCPSpeed = RobotPara->findChild<QLineEdit*>("lineEdit_GetCPSpeed");
	m_lineEdit_GetCPAccel = RobotPara->findChild<QLineEdit*>("lineEdit_GetCPAccel");
	m_lineEdit_GetRSpeed = RobotPara->findChild<QLineEdit*>("lineEdit_GetRSpeed");
	m_lineEdit_GetRAccel = RobotPara->findChild<QLineEdit*>("lineEdit_GetRAccel");
	m_lineEdit_GetPTPSpeed2 = RobotPara2->findChild<QLineEdit*>("lineEdit_GetPTPSpeed");
	m_lineEdit_GetPTPAccel2 = RobotPara2->findChild<QLineEdit*>("lineEdit_GetPTPAccel");
	m_lineEdit_GetCPSpeed2 = RobotPara2->findChild<QLineEdit*>("lineEdit_GetCPSpeed");
	m_lineEdit_GetCPAccel2 = RobotPara2->findChild<QLineEdit*>("lineEdit_GetCPAccel");

	m_lineEdit_PosX = robotPoint->findChild<QLineEdit*>("lineEdit_PosX");
	m_lineEdit_PosY = robotPoint->findChild<QLineEdit*>("lineEdit_PosY");
	m_lineEdit_PosZ = robotPoint->findChild<QLineEdit*>("lineEdit_PosZ");
	m_lineEdit_PosU = robotPoint->findChild<QLineEdit*>("lineEdit_PosU");
	m_lineEdit_PosV = robotPoint->findChild<QLineEdit*>("lineEdit_PosV");
	m_lineEdit_PosW = robotPoint->findChild<QLineEdit*>("lineEdit_PosW");

	m_pStatusBar = qStatusBar;
	InitStatusLabel();
	Update();
}

void RobotDebugParaControl::SetLanguageChangeType(ChangeType type)
{
	m_changeType = type;
}