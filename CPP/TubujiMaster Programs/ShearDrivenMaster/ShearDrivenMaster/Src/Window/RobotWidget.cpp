#include "RobotWidget.h"
#include <qthread.h>
#include "BgiMpsEvent.h"
#include "BgiMasterApp.h"
#include "OpeStyleSheet.h"
#include "languagemgr.h"
#include "ShearDrivenMaster.h"

RobotWidget::RobotWidget(QWidget *parent)
	: QWidget(parent)
	, m_bRorotConnectSuccess(false)
	, m_changeType(toChinese)
	, m_strCurLanguage(gc_Chinese)
	, m_isRunIng(false)
{
	ui.setupUi(this);
	connect(ui.pushButton_BackHome, SIGNAL(clicked()), this, SLOT(On_BackHome_Clicked()));
	connect(ui.pushButton_StartOrStopTransfer, SIGNAL(clicked()), this, SLOT(On_StartOrStopTransfer_Clicked()));
	connect(ui.pushButton_EmergencyStopR, &QPushButton::clicked, this, [=] {PostRobotEvent("MotorAbort", 0); });
	connect(ui.pushButton_RReset, &QPushButton::clicked, this, [=] {PostRobotEvent("MotorReset", 0); });
	connect(ui.pushButton_MotorOnR, &QPushButton::clicked, this, [=] {PostRobotEvent("MotorOn", 0); });
	connect(ui.pushButton_MotorOffR, &QPushButton::clicked, this, [=] {PostRobotEvent("MotorOff", 0); });
	SetRobotConnectStatus(false);
	//this->show();
	SetStyleSheet();
	ShearDrivenMaster* p_ShearDrivenMaster = (ShearDrivenMaster*)parent;
	p_ShearDrivenMaster->SetRobotWidgetCtrl(ui.groupBox_WorkStatus3,ui.groupBox_GetPara);
}

RobotWidget::~RobotWidget()
{}

void RobotWidget::SetStyleSheet()
{
	// 功能选择区样式-120
   // 设置样式
	OpeStyleSheet styleSheet;
	QString strStyle;
	bool bRet = styleSheet.ReadQPushbuttonStyleSheet(strStyle, 18, 1);
	if (bRet)
	{
		ui.pushButton_BackHome->setStyleSheet(strStyle);
		ui.pushButton_StartOrStopTransfer->setStyleSheet(strStyle);
		ui.pushButton_EmergencyStopR->setStyleSheet(strStyle);
		ui.pushButton_RReset->setStyleSheet(strStyle);
		ui.pushButton_MotorOnR->setStyleSheet(strStyle);
		ui.pushButton_MotorOffR->setStyleSheet(strStyle);
	}
}

void RobotWidget::AddRecordData(QString strContext, QColor color)
{
	RecordData recordData;
	recordData.m_strModelName = "机械手";
	recordData.m_strInfoType = "RobotWidget";
	recordData.m_color = color;
	recordData.m_strContent = strContext;
	OperateRecordInfo::AddRecordData(recordData);
}

void RobotWidget::ApplicationSleep(int mSec)
{
	while (mSec)
	{
		QCoreApplication::processEvents();
		QThread::msleep(1);
		mSec--;
	}
}

void RobotWidget::SetRobotConnectStatus(bool bConnect)
{
	if (bConnect)
	{
		ui.label_RobotConnect->setText("Connected");
		ui.label_RobotConnect->setStyleSheet("background-color: rgb(0,220, 0);");
		m_bRorotConnectSuccess = true;
		ui.pushButton_StartOrStopTransfer->setText("禁止转移");
		AddRecordData("机械臂网络已连接！", gc_colorGreen);
	}
	else
	{
		ui.label_RobotConnect->setText("Disconnect");
		ui.label_RobotConnect->setStyleSheet("background-color: rgb(150, 150, 150);");
		m_bRorotConnectSuccess = false;
		ui.pushButton_StartOrStopTransfer->setText("启动转移");
		AddRecordData("机械臂网络已断开！", gc_colorYellow);
	}
	CLanguageMgr* LanguageMgr = CLanguageMgr::Instance();
	LanguageMgr->ChangeUIControlTextUnit(m_changeType, ui.pushButton_StartOrStopTransfer);
}

void RobotWidget::SetRobotSafetyStatus(bool bSafety)
{
	if (bSafety)
	{
		ui.label_WorkSpace->setText("机械臂在安全工作空间");
		ui.label_WorkSpace->setStyleSheet("background-color: rgb(0,220, 0);");
	}
	else
	{
		ui.label_WorkSpace->setText("机械臂不在安全工作空间");
		ui.label_WorkSpace->setStyleSheet("background-color: rgb(255,241, 0);");
	}
}

void RobotWidget::ChangeUIControlText(ChangeType type, QString strCurLanguage)
{
	m_strCurLanguage = strCurLanguage;
	m_changeType = type;
	CLanguageMgr* LanguageMgr = CLanguageMgr::Instance();
	LanguageMgr->ChangeUIControlTextUnit(type,NULL, ui.label_2);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_3);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_4);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_6);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_8);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_Error_3); 

	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_BackHome);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_StartOrStopTransfer);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_EmergencyStopR);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_RReset);
}

void RobotWidget::SetRunState(bool isRunIng)
{
	m_isRunIng = isRunIng;
	if (isRunIng)
	{
		ui.pushButton_BackHome->setEnabled(false);
		ui.pushButton_RReset->setEnabled(false);
	}
	else
	{
		ui.pushButton_BackHome->setEnabled(true);
		ui.pushButton_RReset->setEnabled(true);
	}
}

void RobotWidget::PostRobotEvent(QString strActionName, QVariant var1)
{
	BgiMpsEvent* p_event = new BgiMpsEvent("Driver_Event_Epson_Robot");
	p_event->SetAction(strActionName);
	p_event->AddParams("Value", var1);

	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, p_event);
}

void RobotWidget::On_BackHome_Clicked()
{
	//EpsonRobotFlowCtrl::Instance()->TestMsgBox();
	PostRobotEvent("GoHome",0);
}

void RobotWidget::On_StartOrStopTransfer_Clicked()
{
	/*if (m_isRunIng)
	{
		if (m_bRorotConnectSuccess)
		{
			PostRobotEvent("MotorOff", 0);
			ApplicationSleep(1000);
		}
	}*/
	if (m_bRorotConnectSuccess)
		AddRecordData("点击禁止转移！", gc_colorBlack);
	else
		AddRecordData("点击启动转移！", gc_colorBlack);
	PostRobotEvent("OptRobotSocket", !m_bRorotConnectSuccess);
	/*if (m_isRunIng)
	{
		ApplicationSleep(500);	//等待状态从false 再变为true
		if (m_bRorotConnectSuccess)
		{
			PostRobotEvent("MotorOn", 0);
		}
	}*/
}