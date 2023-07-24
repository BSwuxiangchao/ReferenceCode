#include "EpsonRobotDebug.h"
#include <qevent.h>
#include "CommonColors.h"
#include "OpeStyleSheet.h"
#include "BgiMpsEvent.h"
#include "BgiMasterApp.h"
#include "RobotDebugParaControl.h"
#include "languagemgr.h"
#include <QRegularExpressionValidator>
#include "DeviceDetailCfg.h"
#include "ErrorCodeTranslate.h"
#include "InfoPromptWidget.h"

EpsonRobotDebug::EpsonRobotDebug(QWidget* parent ,QGroupBox* WorkStatus, QGroupBox* GetPara)
	: QMainWindow(parent)
	, m_strCurLanguage(gc_Chinese)
{
	ui.setupUi(this);

	setWindowModality(Qt::ApplicationModal);
	Qt::WindowFlags windowFlag = windowFlags() & ~Qt::WindowMaximizeButtonHint;
	setWindowFlags(windowFlag);
	setFocusPolicy(Qt::StrongFocus); // 窗口将通过鼠标点击、按tab键等方式获取焦点
	//限制输入
	LimitCtrlInput();

	/*****************************************************动作Start*********************/
	connect(ui.pushButton_SetPower, SIGNAL(clicked()), this, SLOT(On_SetPower_Clicked()));
	connect(ui.pushButton_SetPTPSpeed, SIGNAL(clicked()), this, SLOT(On_SetPTPSpeed_Clicked()));
	connect(ui.pushButton_SetPTPAccel, SIGNAL(clicked()), this, SLOT(On_SetPTPAccel_Clicked())); 
	connect(ui.pushButton_SetCPSpeed, SIGNAL(clicked()), this, SLOT(On_SetCPSpeed_Clicked()));
	connect(ui.pushButton_SetCPAccel, SIGNAL(clicked()), this, SLOT(On_SetCPAccel_Clicked()));
	connect(ui.pushButton_SetRSpeed, SIGNAL(clicked()), this, SLOT(On_SetRSpeed_Clicked()));
	connect(ui.pushButton_SetRAccel, SIGNAL(clicked()), this, SLOT(On_SetRAccel_Clicked()));
	connect(ui.pushButton_GoPasePoint, &QPushButton::clicked, this, [=] {PostRobotEvent("GoBasePoint"); });
	connect(ui.pushButton_GoHome, &QPushButton::clicked, this, [=] {PostRobotEvent("GoHome"); });
	connect(ui.pushButton_OpenGripper, &QPushButton::clicked, this, [=] {PostRobotEvent("OpenGripper"); });
	connect(ui.pushButton_CloseGripper, &QPushButton::clicked, this, [=] {PostRobotEvent("CloseGripper"); });
	connect(ui.pushButton_Transfer, SIGNAL(clicked()), this, SLOT(On_TransferPos_Clicked()));
	connect(ui.pushButton_StartOptChip, SIGNAL(clicked()), this, SLOT(On_StartOptChip_Clicked()));
	connect(ui.pushButton_MergencyStop, &QPushButton::clicked, this, [=] {PostRobotEvent("MotorAbort"); });
	connect(ui.pushButton_TurnOnIO, &QPushButton::clicked, this, [=] {On_TurnOnOutIO_Clicked(); });
	connect(ui.pushButton_TurnOffIO, &QPushButton::clicked, this, [=] {On_TurnOffOutIO_Clicked(); });

	/*****************************************************动作End*********************/

	/*****************************************************类RC调试Start*********************/
	connect(ui.pushButton_motorOff, &QPushButton::clicked, this, [=] {PostRobotEvent("MotorOff"); });
	connect(ui.pushButton_motorOn, &QPushButton::clicked, this, [=] {PostRobotEvent("MotorOn"); });
	connect(ui.pushButton_MotorReset, &QPushButton::clicked, this, [=] {PostRobotEvent("MotorReset"); });
	connect(ui.pushButton_Abort, &QPushButton::clicked, this, [=] {PostRobotEvent("MotorAbort"); });
	connect(ui.pushButton_RealoadScript, &QPushButton::clicked, this, [=] {PostRobotEvent("Ctrl_RealoadScript"); });
	connect(ui.pushButton_LowSpeed, &QPushButton::clicked, this, [=] {PostRobotEvent("MotorSpeedSet_Low"); });
	connect(ui.pushButton_HighSpeed, &QPushButton::clicked, this, [=] {PostRobotEvent("MotorSpeedSet_High"); });
	connect(ui.pushButton_GoVar, SIGNAL(clicked()), this, SLOT(On_GoVar_Clicked()));
	connect(ui.pushButton_StepLowX, &QPushButton::clicked, this, [=] {On_StepSport_Clicked(0, 0); });
	connect(ui.pushButton_StepLowY, &QPushButton::clicked, this, [=] {On_StepSport_Clicked(1, 0); });
	connect(ui.pushButton_StepLowZ, &QPushButton::clicked, this, [=] {On_StepSport_Clicked(2, 0); });
	connect(ui.pushButton_StepLowU, &QPushButton::clicked, this, [=] {On_StepSport_Clicked(3, 0); });
	connect(ui.pushButton_StepLowV, &QPushButton::clicked, this, [=] {On_StepSport_Clicked(4, 0); });
	connect(ui.pushButton_StepLowW, &QPushButton::clicked, this, [=] {On_StepSport_Clicked(5, 0); });
	connect(ui.pushButton_StepHighX, &QPushButton::clicked, this, [=] {On_StepSport_Clicked(0, 1); });
	connect(ui.pushButton_StepHighY, &QPushButton::clicked, this, [=] {On_StepSport_Clicked(1, 1); });
	connect(ui.pushButton_StepHighZ, &QPushButton::clicked, this, [=] {On_StepSport_Clicked(2, 1); });
	connect(ui.pushButton_StepHighU, &QPushButton::clicked, this, [=] {On_StepSport_Clicked(3, 1); });
	connect(ui.pushButton_StepHighV, &QPushButton::clicked, this, [=] {On_StepSport_Clicked(4, 1); });
	connect(ui.pushButton_StepHighW, &QPushButton::clicked, this, [=] {On_StepSport_Clicked(5, 1); });
	/*****************************************************类RC调试Start*********************/

	SetStyleSheet();
	SlotUpdatePosCombox();
	ui.statusBar;
	m_pRobotDebugParaControl = new RobotDebugParaControl();
	m_pRobotDebugParaControl->SetParentWidget(ui.groupBox_ParaRead, ui.groupBox_WorkStatus1, ui.groupBox_WorkStatus2, ui.groupBox_RobotPoint, WorkStatus,GetPara,ui.statusBar);
	//先登录
	PostRobotEvent("LogIn");
}

EpsonRobotDebug::~EpsonRobotDebug()
{

}

void EpsonRobotDebug::keyPressEvent(QKeyEvent* event)
{
	// 判断是否同时按下ctrl和c键
	if ((event->modifiers() & Qt::ControlModifier) != 0 && event->key() == Qt::Key_C)
	{
		PostRobotEvent("MotorAbort");
		//ErrorCodeTranslate::Instance()->RecordInfo("KeyPressEvent", "Ctrl C to Abort");
	}
}

void EpsonRobotDebug::ChangeUIControlText(ChangeType type,QString strCurLanguage)
{
	m_strCurLanguage = strCurLanguage;
	if (strCurLanguage == gc_Chinese)
	{
		ui.tabWidget->setTabText(0, "动作");
		ui.tabWidget->setTabText(1, "类RC调试");
	}
	else if (strCurLanguage == gc_English)
	{
		ui.tabWidget->setTabText(0, "Action");
		ui.tabWidget->setTabText(1, "Class RC debugging");
	}
	else if (strCurLanguage == gc_French)
	{
		ui.tabWidget->setTabText(0, "Actions");
		ui.tabWidget->setTabText(1, "Débogage de classe RC");
	}
	m_pRobotDebugParaControl->SetLanguageChangeType(type);
	CLanguageMgr* LanguageMgr = CLanguageMgr::Instance();
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_SetPower);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_SetPTPSpeed);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_SetPTPAccel);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_SetCPSpeed);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_SetCPAccel);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_GoPasePoint);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_OpenGripper);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_CloseGripper);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_Transfer); 
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_MergencyStop);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_StartOptChip);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_TurnOnIO);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_TurnOffIO);

	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_22);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_23);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_24);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_25);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_Error_2);

	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_16);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_18);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_19);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_20);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_21);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_17);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_3);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_30); 
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_31);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_2);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_32);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_33);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_34);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_35);

	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, ui.groupBox_WorkStatus2);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, ui.groupBox_11);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, ui.groupBox_12);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, ui.groupBox_ParaRead);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, ui.groupBox_8); 
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, ui.groupBox_9);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, ui.groupBox);

	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, NULL, ui.comboBox_Power);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, NULL, ui.comboBox_StartPos);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, NULL, ui.comboBox_EndPos);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, NULL, ui.comboBox_ChipPosition);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, NULL, ui.comboBox_OptChips);

	/*****************************************************类RC调试Start*********************/
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_motorOff);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_motorOn);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_MotorReset);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_Abort);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_LowSpeed);
	LanguageMgr->ChangeUIControlTextUnit(type, ui.pushButton_HighSpeed);
	
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_26); 
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_27);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_28);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_29);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, ui.label_Error);

	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, ui.groupBox_WorkStatus1);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, ui.groupBox_2);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, ui.groupBox_6);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, ui.groupBox_3);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, ui.groupBox_RobotPoint);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, ui.groupBox_5);
	LanguageMgr->ChangeUIControlTextUnit(type, NULL, NULL, ui.groupBox_7);
}

void EpsonRobotDebug::LimitInputUnit(QLineEdit* pLineEdit,QString strPalete)
{
	QRegularExpression regExp(strPalete);
	QValidator* validator = new QRegularExpressionValidator(regExp, this);
	pLineEdit->setValidator(validator);
}

void EpsonRobotDebug::LimitCtrlInput()
{
	LimitInputUnit(ui.lineEdit_SetPTPSpeed, "^(?:0|[1-9][0-9]?|100)$");
	LimitInputUnit(ui.lineEdit_SetPTPAccel, "^(?:0|[1-9][0-9]?|120)$");
	LimitInputUnit(ui.lineEdit_SetCPSpeed, "^(0|[1-9]\\d\\d|2000)$");
	LimitInputUnit(ui.lineEdit_SetCPAccel, "^(0|[1-9]\\d\\d|25000)$");
	LimitInputUnit(ui.lineEdit_SetRSpeed, "^(?:0|[1-9][0-9]?|1000)$");
	LimitInputUnit(ui.lineEdit_SetRAccel, "^(0|[1-9]\\d\\d|5000)$");
}

void EpsonRobotDebug::SetStyleSheet()
{
	// 功能选择区样式-120
   // 设置样式
	OpeStyleSheet styleSheet;
	QString strStyle;
	bool bRet = styleSheet.ReadQPushbuttonStyleSheet(strStyle, 18,1);
	if (bRet)
	{
		ui.tabWidget->setStyleSheet(strStyle);
	}
}

void EpsonRobotDebug::PostRobotEvent(QString strActionName)
{
	BgiMpsEvent* p_event = new BgiMpsEvent("Driver_Event_Epson_Robot");
	p_event->SetAction(strActionName);
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, p_event);
}

void EpsonRobotDebug::PostRobotEvent(QString strActionName, QVariant var1)
{
	BgiMpsEvent* p_event = new BgiMpsEvent("Driver_Event_Epson_Robot");
	p_event->SetAction(strActionName);
	p_event->AddParams("Value", var1);

	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, p_event);
}

void EpsonRobotDebug::PostRobotEvent(QString strActionName, QVariant var1, QVariant var2)
{
	BgiMpsEvent* p_event = new BgiMpsEvent("Driver_Event_Epson_Robot");
	p_event->SetAction(strActionName);
	p_event->AddParams("Value1", var1);
	p_event->AddParams("Value2", var2);

	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, p_event);
}

void EpsonRobotDebug::On_SetPower_Clicked()
{
	PostRobotEvent("SetPower", ui.comboBox_Power->currentIndex() == 1);
}

void EpsonRobotDebug::On_SetPTPSpeed_Clicked()
{
	int iValue = ui.lineEdit_SetPTPSpeed->text().toInt();
	PostRobotEvent("SetPTPSpeed", iValue);
	ui.lineEdit_SetPTPSpeed->clear();
}

void EpsonRobotDebug::On_SetPTPAccel_Clicked()
{
	bool Ok = false;
	int iValue = ui.lineEdit_SetPTPAccel->text().toInt(&Ok);
	if(Ok)
		PostRobotEvent("SetPTPAccel", iValue);
	ui.lineEdit_SetPTPAccel->clear();
}

void EpsonRobotDebug::On_SetCPSpeed_Clicked()
{
	int iValue = ui.lineEdit_SetCPSpeed->text().toInt();
	PostRobotEvent("SetCPSpeed", iValue);
	ui.lineEdit_SetCPSpeed->clear();
}

void EpsonRobotDebug::On_SetCPAccel_Clicked()
{
	int iValue = ui.lineEdit_SetCPAccel->text().toInt();
	PostRobotEvent("SetCPAccel", iValue);
	ui.lineEdit_SetCPAccel->clear();
}

void EpsonRobotDebug::On_SetRSpeed_Clicked()
{
	int iValue = ui.lineEdit_SetRSpeed->text().toInt();
	PostRobotEvent("SetRSpeed", iValue);
	ui.lineEdit_SetRSpeed->clear();
}

void EpsonRobotDebug::On_SetRAccel_Clicked()
{
	int iValue = ui.lineEdit_SetRAccel->text().toInt();
	PostRobotEvent("SetRAccel", iValue);
	ui.lineEdit_SetRAccel->clear();
}

void EpsonRobotDebug::On_TransferPos_Clicked()
{
	int iStartIndex = ui.comboBox_StartPos->currentIndex();
	int iEndIndex = ui.comboBox_EndPos->currentIndex();
	PostRobotEvent("Ctrl_TransferPos",iStartIndex, iEndIndex);
}

void EpsonRobotDebug::On_StartOptChip_Clicked()
{
	InfoPromptWidget infoWidget;
	BUTTON_STATE ret = infoWidget.Show("操作确认", "您确定继续当前操作吗？", true, 3, Qt::red, gc_colorLightGreen);

	if (BUTTON_STATE::SURE != ret)
	{
		return;
	}
	int iPosIndex = ui.comboBox_ChipPosition->currentIndex();
	int iOptIndex = ui.comboBox_OptChips->currentIndex();
	PostRobotEvent("Ctrl_OptChips", iPosIndex, iOptIndex);
}

void EpsonRobotDebug::On_TurnOnOutIO_Clicked()
{
	QString strIOSign = ui.comboBox_IOSign->currentText();
	PostRobotEvent("TurnOnOutIO", strIOSign);
}

void EpsonRobotDebug::On_TurnOffOutIO_Clicked()
{
	QString strIOSign = ui.comboBox_IOSign->currentText();
	PostRobotEvent("TurnOffOutIO",strIOSign);
}

void EpsonRobotDebug::On_GoVar_Clicked()
{
	int iIndex = ui.comboBox_CommandClass->currentIndex();
	QString strVar = ui.comboBox_PosVar->currentText();
	PostRobotEvent("MotorGoVar", iIndex,strVar);
}

void EpsonRobotDebug::On_StepSport_Clicked(int iType, int iDircetion)
{
	RobotParaType::RobotSportMode mode;
	if (iType == 0)
		mode.iStep = ui.spinBox_Step_X->text().toInt();
	else if (iType == 1)
		mode.iStep = ui.spinBox_Step_Y->text().toInt();
	else if (iType == 2)
		mode.iStep = ui.spinBox_Step_Z->text().toInt();
	else if (iType == 3)
		mode.iStep = ui.spinBox_Step_U->text().toInt();
	else if (iType == 4)
		mode.iStep = ui.spinBox_Step_V->text().toInt();
	else if (iType == 5)
		mode.iStep = ui.spinBox_Step_W->text().toInt();
	
	mode.iType = iType;
	mode.iDirection = iDircetion;
	QVariant value;
	value.setValue(mode);
	PostRobotEvent("MotorStep", value);
}

void EpsonRobotDebug::SlotUpdatePosCombox()
{
	QStringList list = DeviceDetailCfg::Instance()->GetListPointVar();
	ui.comboBox_PosVar->addItems(list);
	ui.comboBox_PosVar->setCurrentText("");
	list = DeviceDetailCfg::Instance()->GetListOutOptIO();
	ui.comboBox_IOSign->addItems(list);
	ui.comboBox_IOSign->setCurrentText("");
}