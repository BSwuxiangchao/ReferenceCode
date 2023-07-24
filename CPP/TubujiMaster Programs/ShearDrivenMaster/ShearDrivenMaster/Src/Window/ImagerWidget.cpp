#include "ImagerWidget.h"
#include "languagemgr.h"
#include "MasterDevices.h"
#include "ImagerDriver.h"
#include "BgiMpsEvent.h"
#include "BgiMasterApp.h"
#include "OpeStyleSheet.h"

ImagerWidget::ImagerWidget(QWidget *parent)
	: QWidget(parent)
	, m_strCurLanguage(gc_Chinese)
{
	ui.setupUi(this);

	InitControls();
	SetStyleSheet();

	//光机驱动接口
	m_ImagerDriver = MasterDevices::Instance()->GetImagerDriver();
	QObject::connect(m_ImagerDriver, SIGNAL(SignalImagerConnectStatus(bool)), this, SLOT(SlotImagerConnectStatus(bool)));
	QObject::connect(m_ImagerDriver, SIGNAL(SignalImagerRunInfo(const ImagerParaType::ImagerRunPara&)), this, SLOT(SlotImagerRunInfo(const ImagerParaType::ImagerRunPara&)));
	QObject::connect(m_ImagerDriver, SIGNAL(SignalImagerVacuumValue(float)), this, SLOT(SlotImagerVacuumValue(float)));
	QObject::connect(m_ImagerDriver, SIGNAL(SignalImagerStatus(int, int, int)), this, SLOT(SlotImagerStatus(int, int, int)));
	QObject::connect(m_ImagerDriver, SIGNAL(SignalImagerData(const QString&, const QString&)), this, SLOT(SlotImagerData(const QString&, const QString&)));
	QObject::connect(m_ImagerDriver, SIGNAL(SignalImagerConfigData(const ImagerParaType::ImagerConfigPara&)), this, SLOT(SlotImagerConfigData(const ImagerParaType::ImagerConfigPara&)));
	QObject::connect(m_ImagerDriver, SIGNAL(SignalImagerSafePos(bool)), this, SLOT(SlotUpdateSafePos(bool)));

	//启动时主动获取一次连接状态
	GetConnStatus();
}

ImagerWidget::~ImagerWidget()
{}

void ImagerWidget::ChangeUIControlText(ChangeType type, QString strCurLanguage)
{
	m_strCurLanguage = strCurLanguage;
	//CLanguageMgr* LanguageMgr = CLanguageMgr::Instance();
	//LanguageMgr->ChangeUIControlTextUnit(type, ui.ButtonStartRun);
}

void ImagerWidget::InitControls()
{
	QObject::connect(ui.ButtonGoLoadingPos, SIGNAL(clicked()), this, SLOT(ClickGoLoadingPos()));
	QObject::connect(ui.ButtonOpenVacuum, SIGNAL(clicked()), this, SLOT(ClickOpenVacuum()));
	QObject::connect(ui.ButtonCloseVacuum, SIGNAL(clicked()), this, SLOT(ClickCloseVacuum()));
	QObject::connect(ui.ButtonReset, SIGNAL(clicked()), this, SLOT(ClickReset()));
	QObject::connect(ui.ButtonBasecall, SIGNAL(clicked()), this, SLOT(ClickSetBasecall()));

	ui.LabelSafeSpace->setVisible(false);

	InitStatus();
}

void ImagerWidget::InitStatus()
{
	SlotImagerStatus(-1, -1, -1);

	ui.lineRowRange->setText("");
	ui.lineColRange->setText("");
	ui.lineFovNum->setText("");

	ui.lineVacuum->setText("");

	InitRunStatus();
}

void ImagerWidget::InitRunStatus()
{
	ui.lineCycle->setText("");
	ui.lineRow->setText("");
	ui.ProgressScan->setValue(0);
	ui.labeRecvData->setText("");
}

void ImagerWidget::SetStyleSheet()
{
   // 设置样式
	OpeStyleSheet styleSheet;
	QString strStyle;
	bool bRet = styleSheet.ReadQPushbuttonStyleSheet(strStyle, 18, 1);
	if (bRet)
	{
		this->setStyleSheet(strStyle);
	}
}

void ImagerWidget::SlotImagerConnectStatus(bool bSuccess)
{
	if (bSuccess)
	{
		ui.LabelState->setText("Connected");
		ui.LabelState->setStyleSheet("background-color: rgb(0,220, 0);");

		//查询光机配置
		BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
		pEvent->SetAction("ImagerConfigGet");
		BgiMasterApp* app = BgiMasterApp::Instance();
		QApplication::postEvent(app, pEvent);
	}
	else
	{
		ui.LabelState->setText("Disconnect");
		ui.LabelState->setStyleSheet("background-color: rgb(150, 150, 150);");

		InitStatus();
	}
}

void ImagerWidget::GetConnStatus()
{
	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
	pEvent->SetAction("GetConnStatus");
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}

void ImagerWidget::ClickGoLoadingPos()
{
	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
	pEvent->SetAction("GoLoadingPos");
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}

void ImagerWidget::ClickOpenVacuum()
{
	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
	pEvent->SetAction("SwitchVacuum");
	pEvent->AddParams("Param1", true);
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}

void ImagerWidget::ClickCloseVacuum()
{
	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
	pEvent->SetAction("SwitchVacuum");
	pEvent->AddParams("Param1", false);
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}

void ImagerWidget::ClickReset()
{
	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
	pEvent->SetAction("Reset");
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}

void ImagerWidget::ClickSetBasecall()
{
	emit SignalBasecallSetting();
}

void ImagerWidget::SlotImagerRunInfo(const ImagerParaType::ImagerRunPara& info)
{
	ui.lineCycle->setText(QString::number(info.CurrentCycle));
	ui.lineRow->setText(QString::number(info.CurrentRow));
	//ui.lineColumn->setText(QString::number(info.CurrentCol));

	int totalCount = info.EndRow - info.StartRow + 1;
	if (totalCount > 0)
	{
		int finishCount = 0;
		if (info.CurrentLane > 0 && info.CurrentRow > 0 && info.CurrentCol >= 0)
		{
			if (info.ScanDirect == 0 || info.ScanDirect == 2)
			{
				//从上到下
				finishCount = info.CurrentRow;
			}
			else if (info.ScanDirect == 1 || info.ScanDirect == 3)
			{
				//从下到上
				finishCount = totalCount - info.CurrentRow;
			}
		}
		int value = (int)((float)finishCount / (float)totalCount * 100 + 0.5);
		ui.ProgressScan->setValue(value);
	}
}

void ImagerWidget::SlotImagerVacuumValue(float value)
{
	ui.lineVacuum->setText(QString::number(value, 'f', 2));
}

void ImagerWidget::SlotImagerStatus(int basecall, int server, int imager)
{
	switch (basecall)
	{
	case 0:
		ui.labelStateBasecall->setText("Connect");
		ui.labelStateBasecall->setStyleSheet("QLabel{background:LimeGreen}");
		break;
	case 1:
		ui.labelStateBasecall->setText("Disconnect");
		ui.labelStateBasecall->setStyleSheet("QLabel{background:DarkGray}");
		break;
	default:
		ui.labelStateBasecall->setText("未知");
		ui.labelStateBasecall->setStyleSheet("QLabel{background:DarkGray}");
		break;
	}

	switch (server)
	{
	case 0:
		ui.labelStateServer->setText("Idle");
		ui.labelStateServer->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 1:
		ui.labelStateServer->setText("Ready");
		ui.labelStateServer->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 2:
		ui.labelStateServer->setText("ImagerRun");
		ui.labelStateServer->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 3:
		ui.labelStateServer->setText("ScanRunning");
		ui.labelStateServer->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 4:
		ui.labelStateServer->setText("SetupRun");
		ui.labelStateServer->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 5:
		ui.labelStateServer->setText("WriteFq");
		ui.labelStateServer->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 6:
		ui.labelStateServer->setText("RunTask");
		ui.labelStateServer->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	default:
		ui.labelStateServer->setText("未知");
		ui.labelStateServer->setStyleSheet("QLabel{background:DarkGray}");
	}

	switch (imager)
	{
	case 0:
		ui.labelStateImager->setText("Disconnect");
		ui.labelStateImager->setStyleSheet("QLabel{background:DarkGray}");
		break;
	case 1:
		ui.labelStateImager->setText("Idle");
		ui.labelStateImager->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 2:
		ui.labelStateImager->setText("Reset");
		ui.labelStateImager->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 3:
		ui.labelStateImager->setText("GoHome");
		ui.labelStateImager->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 4:
		ui.labelStateImager->setText("GoLoadingPos");
		ui.labelStateImager->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 5:
		ui.labelStateImager->setText("GoWashPos");
		ui.labelStateImager->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 6:
		ui.labelStateImager->setText("SwitchVacuum");
		ui.labelStateImager->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 7:
		ui.labelStateImager->setText("PreInjectSRE");
		ui.labelStateImager->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 8:
		ui.labelStateImager->setText("PreProcessing");
		ui.labelStateImager->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 9:
		ui.labelStateImager->setText("SlideLeavedImager");
		ui.labelStateImager->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 10:
		ui.labelStateImager->setText("StartRun");
		ui.labelStateImager->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 11:
		ui.labelStateImager->setText("Ready");
		ui.labelStateImager->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 12:
		ui.labelStateImager->setText("Scanning");
		ui.labelStateImager->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	case 13:
		ui.labelStateImager->setText("SelfCheck");
		ui.labelStateImager->setStyleSheet("QLabel{background:DeepSkyBlue}");
		break;
	default:
		ui.labelStateImager->setText("未知");
		ui.labelStateImager->setStyleSheet("QLabel{background:DarkGray}");
	}
}

void ImagerWidget::SlotImagerData(const QString& key, const QString& value)
{
	ui.labeRecvData->setText(key + "=" + value);
}

void ImagerWidget::SlotImagerConfigData(const ImagerParaType::ImagerConfigPara& data)
{
	ui.lineRowRange->setText(QString::number(data.StartRow) + " - " + QString::number(data.EndRow));
	ui.lineColRange->setText(QString::number(data.StartCol) + " - " + QString::number(data.EndCol));
	ui.lineFovNum->setText(QString::number(data.SaveFovString.split(';', Qt::SkipEmptyParts).size()));
}

void ImagerWidget::SlotUpdateSafePos(bool bSafety)
{
	if (bSafety)
	{
		//ui.LabelSafeSpace->setText("机械臂未进入");
		//ui.LabelSafeSpace->setStyleSheet("");
		ui.LabelSafeSpace->setVisible(false);
	}
	else
	{
		//ui.LabelSafeSpace->setText("机械臂进入");
		//ui.LabelSafeSpace->setStyleSheet("background-color: rgb(255,241, 0);");
		ui.LabelSafeSpace->setVisible(true);
	}
}
