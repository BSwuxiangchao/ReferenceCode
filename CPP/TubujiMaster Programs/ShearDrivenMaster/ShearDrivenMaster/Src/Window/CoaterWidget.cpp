#include "CoaterWidget.h"
#include "BgiMpsEvent.h"
#include "BgiMasterApp.h"
#include "MasterDevices.h"
#include "CoaterDriver.h"
#include "languagemgr.h"
#include "OpeStyleSheet.h"
#include "InfoPromptWidget.h"
#include "CommonColors.h"

CoaterWidget::CoaterWidget(QWidget* parent)
	: QWidget(parent)
	, m_strCurLanguage(gc_Chinese)
{
	ui.setupUi(this);

	InitControls();
	SetStyleSheet();

	//涂布机驱动接口
	m_CoaterDriver = MasterDevices::Instance()->GetCoaterDriver();
	QObject::connect(m_CoaterDriver, SIGNAL(SignalCoaterConnectStatus(bool)), this, SLOT(SlotCoaterConnectStatus(bool)));
	QObject::connect(m_CoaterDriver, SIGNAL(SignalDeviceStatus(int, int)), this, SLOT(SlotDeviceStatus(int, int)));
	QObject::connect(m_CoaterDriver, SIGNAL(SignalVacuumValue(int, float)), this, SLOT(SlotVacuumValue(int, float)));
	QObject::connect(m_CoaterDriver, SIGNAL(SignalPumpStatus(int, int)), this, SLOT(SlotPumpStatus(int, int)));
	QObject::connect(m_CoaterDriver, SIGNAL(SignalPumpValue(int, int)), this, SLOT(SlotPumpValue(int, int)));
	QObject::connect(m_CoaterDriver, SIGNAL(SignalPumpSize(int, int)), this, SLOT(SlotPumpSize(int, int)));
	QObject::connect(m_CoaterDriver, SIGNAL(SignalCoaterSafePos(bool)), this, SLOT(SlotUpdateSafePos(bool)));
	QObject::connect(m_CoaterDriver, SIGNAL(SignalNewStep(QString)), this, SLOT(SlotUpdateStepString(QString)));
	QObject::connect(m_CoaterDriver, SIGNAL(SignalCoatingResult(int)), this, SLOT(SlotCoatingResult(int)));
	
	//启动时主动获取一次连接状态
	GetConnStatus();
}

CoaterWidget::~CoaterWidget()
{
}

void CoaterWidget::InitControls()
{
	QObject::connect(ui.ButtonStartup, SIGNAL(clicked()), this, SLOT(ClickStartup()));
	QObject::connect(ui.ButtonPause, SIGNAL(clicked()), this, SLOT(ClickPause()));
	QObject::connect(ui.ButtonReset, SIGNAL(clicked()), this, SLOT(ClickReset()));
	QObject::connect(ui.ButtonStop, SIGNAL(clicked()), this, SLOT(ClickStop()));
	QObject::connect(ui.ButtonClearAlarm, SIGNAL(clicked()), this, SLOT(ClickClearAlarm()));

	ui.LabelSafeSpace->setVisible(false);

	InitStatus();
}

void CoaterWidget::InitStatus()
{
	for (int i = 1; i <= 8; ++i)
	{
		SlotDeviceStatus(i, 0);
	}
	for (int i = 1; i <= 2; ++i)
	{
		SlotVacuumValue(i, 0);
	}
	for (int i = 1; i <= 7; ++i)
	{
		SlotPumpStatus(i, 0);
		SlotPumpValue(i, 0);
	}

	SlotUpdateStepString("");
	SlotCoatingResult(0);
}

void CoaterWidget::SetStyleSheet()
{
	OpeStyleSheet styleSheet;
	QString strStyle;
	bool bRet = styleSheet.ReadQPushbuttonStyleSheet(strStyle, 18, 1);
	if (bRet)
	{
		this->setStyleSheet(strStyle);
	}
}

void CoaterWidget::SlotCoaterConnectStatus(bool bSuccess)
{
	if (bSuccess)
	{
		ui.LabelState->setText("Connected");
		ui.LabelState->setStyleSheet("background-color: rgb(0,220, 0);");
	}
	else
	{
		ui.LabelState->setText("Disconnect");
		ui.LabelState->setStyleSheet("background-color: rgb(150, 150, 150);");
		
		InitStatus();
	}
	
	ui.ButtonPause->setEnabled(bSuccess);
	ui.ButtonStop->setEnabled(bSuccess);
	ui.ButtonClearAlarm->setEnabled(bSuccess);
}

void CoaterWidget::SendCoaterEvent(QString action)
{
	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Coater");
	pEvent->SetAction(action);
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}

void CoaterWidget::GetConnStatus()
{
	SendCoaterEvent("GetConnStatus");
}

void CoaterWidget::ClickStartup()
{
	SendCoaterEvent("Startup");
}

void CoaterWidget::ClickPause()
{
	bool bPause = false;
	if (ui.ButtonPause->text() == "暂停")
	{
		bPause = true;
		ui.ButtonPause->setText("暂停中");
		ui.ButtonPause->setStyleSheet("background-color: rgb(255,241,0);");
	}
	else
	{
		bPause = false;
		ui.ButtonPause->setText("暂停");
		ui.ButtonPause->setStyleSheet("");
	}

	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Coater");
	pEvent->SetAction("Pause");
	pEvent->AddParams("Param1", bPause);
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}

void CoaterWidget::ClickReset()
{
	InfoPromptWidget infoWidget;
	BUTTON_STATE ret = infoWidget.Show("复位确认", "确定要让涂布机复位吗？", true, 3, Qt::red, gc_colorLightBlue);
	if (BUTTON_STATE::SURE != ret)
	{
		return;
	}

	SendCoaterEvent("Reset");
}

void CoaterWidget::ClickStop()
{
	InfoPromptWidget infoWidget;
	BUTTON_STATE ret = infoWidget.Show("停止确认", "确定要让涂布机停止吗？", true, 3, Qt::red, gc_colorLightBlue);
	if (BUTTON_STATE::SURE != ret)
	{
		return;
	}

	SendCoaterEvent("Stop");
}

void CoaterWidget::ClickClearAlarm()
{
	SendCoaterEvent("AlarmReset");
}

void CoaterWidget::ChangeUIControlText(ChangeType type,QString strCurLanguage)
{
	m_strCurLanguage = strCurLanguage;
	//CLanguageMgr* LanguageMgr = CLanguageMgr::Instance();
	//LanguageMgr->ChangeUIControlTextUnit(type, ui.ButtonStartRun);
}

void CoaterWidget::SlotDeviceStatus(int index, int value)
{
	switch (index)
	{
	case 1:
		//设备状态
		switch (value)
		{
		case 1:
			ui.LabelDeviceStatus->setText("手动");
			ui.LabelDeviceStatus->setStyleSheet("QLabel{background:Yellow}");
			break;
		case 2:
			ui.LabelDeviceStatus->setText("自动");
			ui.LabelDeviceStatus->setStyleSheet("QLabel{background:LimeGreen}");
			break;
		case 3:
			ui.LabelDeviceStatus->setText("急停");
			ui.LabelDeviceStatus->setStyleSheet("QLabel{background:Yellow}");
			break;
		case 4:
			ui.LabelDeviceStatus->setText("暂停中");
			ui.LabelDeviceStatus->setStyleSheet("QLabel{background:Yellow}");
			break;
		default:
			ui.LabelDeviceStatus->setText("未知");
			ui.LabelDeviceStatus->setStyleSheet("QLabel{background:DarkGray}");
			break;
		}
		break;
	case 2:
		//上料状态
		switch (value)
		{
		case 1:
			//ui.LabelLoadStatus->setText("可上料");
			ui.LabelLoadStatus->setStyleSheet("QLabel{background:LimeGreen}");
			break;
		default:
			//ui.LabelLoadStatus->setText("");
			ui.LabelLoadStatus->setStyleSheet("QLabel{background:DarkGray}");
			break;
		}
		break;
	case 3:
		//下料状态
		switch (value)
		{
		case 1:
			//ui.LabelUnloadStatus->setText("可下料");
			ui.LabelUnloadStatus->setStyleSheet("QLabel{background:LimeGreen}");
			break;
		default:
			//ui.LabelUnloadStatus->setText("");
			ui.LabelUnloadStatus->setStyleSheet("QLabel{background:DarkGray}");
			break;
		}
		break;
	case 4:
		//设备模式
		switch (value)
		{
		case 1:
			ui.LabelDeviceMode->setText("本地模式");
			ui.LabelDeviceMode->setStyleSheet("QLabel{background:Yellow}");
			break;
		case 2:
			ui.LabelDeviceMode->setText("远程模式");
			ui.LabelDeviceMode->setStyleSheet("QLabel{background:LimeGreen}");
			break;
		default:
			ui.LabelDeviceMode->setText("未知");
			ui.LabelDeviceMode->setStyleSheet("QLabel{background:DarkGray}");
			break;
		}
		break;
	case 5:
		//可启动状态
		switch (value)
		{
		case 1:
			ui.ButtonStartup->setEnabled(true);
			break;
		default:
			ui.ButtonStartup->setEnabled(false);
			break;
		}
		break;
	case 6:
		//可复位状态
		switch (value)
		{
		case 1:
			ui.ButtonReset->setEnabled(true);
			break;
		default:
			ui.ButtonReset->setEnabled(false);
			break;
		}
		break;
	case 7:
		//膜状态
		switch (value)
		{
		case 1:
			ui.LabelPETStatus->setText("需上膜");
			//ui.LabelPETStatus->setStyleSheet("background-color: rgb(255,241, 0);");
			ui.LabelPETStatus->setVisible(true);
			break;
		case 2:
			ui.LabelPETStatus->setText("需下膜");
			//ui.LabelPETStatus->setStyleSheet("background-color: rgb(255,241, 0);");
			ui.LabelPETStatus->setVisible(true);
			break;
		default:
			//ui.LabelPETStatus->setText("");
			ui.LabelPETStatus->setVisible(false);
			break;
		}
		break;
	case 8:
		//报警状态
		switch (value)
		{
		case 1:
			ui.LabelAlarmStatus->setVisible(true);
			break;
		default:
			ui.LabelAlarmStatus->setVisible(false);
			break;
		}
		break;
	default:
		break;
	}
}

void CoaterWidget::SlotVacuumValue(int index, float value)
{
	switch (index)
	{
	case 1:
		ui.LineEditChipVacuum->setText(QString::number(value, 'f', 2));
		break;
	case 2:
		ui.LineEditPETVacuum->setText(QString::number(value, 'f', 2));
		break;
	default:
		break;
	}
}

void CoaterWidget::SlotPumpStatus(int index, int value)
{
	switch (index)
	{
	case 1:
		SetPumpState(ui.LabelPumpState1, ui.Progress1, value);
		break;
	case 2:
		SetPumpState(ui.LabelPumpState2, ui.Progress2, value);
		break;
	case 3:
		SetPumpState(ui.LabelPumpState3, ui.Progress3, value);
		break;
	case 4:
		SetPumpState(ui.LabelPumpState4, ui.Progress4, value);
		break;
	case 5:
		SetPumpState(ui.LabelPumpState5, ui.Progress5, value);
		break;
	case 6:
		SetPumpState(ui.LabelPumpState6, ui.Progress6, value);
		break;
	case 7:
		SetPumpState(ui.LabelPumpState7, ui.Progress7, value);
		break;
	default:
		break;
	}
}

void CoaterWidget::SetPumpState(QLabel* pumpText, QProgressBar* pumpProgress, int value)
{
	switch (value)
	{
	case 1:
		pumpText->setText("闲");
		pumpProgress->setStyleSheet("QProgressBar::chunk{background:DeepSkyBlue}");
		break;
	case 2:
		pumpText->setText("忙");
		pumpProgress->setStyleSheet("QProgressBar::chunk{background:DeepSkyBlue}");
		break;
	//case 3:
	//	pumpText->setText("未初始化");
	//	pumpProgress->setStyleSheet("QProgressBar::chunk{background:DarkGray}");
	//	break;
	default:
		pumpText->setText("未知");
		pumpProgress->setStyleSheet("QProgressBar::chunk{background:DarkGray}");
		break;
	}
}

void CoaterWidget::SlotPumpValue(int index, int value)
{
	float fValue = (float)value / 1000.0;
	if (m_PumpValues.contains(index))
	{
		m_PumpValues[index].first = fValue;
	}
	else
	{
		m_PumpValues.insert(index, QPair<float, float>(fValue, 100));
	}

	SetPumpValue(index, m_PumpValues[index].first, m_PumpValues[index].second);
}

void CoaterWidget::SlotPumpSize(int index, int value)
{
	float fValue = (float)value / 1000.0;
	if (m_PumpValues.contains(index))
	{
		m_PumpValues[index].second = fValue;
	}
	else
	{
		m_PumpValues.insert(index, QPair<float, float>(0, fValue));
	}

	SetPumpValue(index, m_PumpValues[index].first, m_PumpValues[index].second);
}

void CoaterWidget::SetPumpValue(int index, float value, float size)
{
	switch (index)
	{
	case 1:
		SetPumpValue(ui.LabelPumpSize1, ui.Progress1, value, size);
		break;
	case 2:
		SetPumpValue(ui.LabelPumpSize2, ui.Progress2, value, size);
		break;
	case 3:
		SetPumpValue(ui.LabelPumpSize3, ui.Progress3, value, size);
		break;
	case 4:
		SetPumpValue(ui.LabelPumpSize4, ui.Progress4, value, size);
		break;
	case 5:
		SetPumpValue(ui.LabelPumpSize5, ui.Progress5, value, size);
		break;
	case 6:
		SetPumpValue(ui.LabelPumpSize6, ui.Progress6, value, size);
		break;
	case 7:
		SetPumpValue(ui.LabelPumpSize7, ui.Progress7, value, size);
		break;
	default:
		break;
	}
}

void CoaterWidget::SetPumpValue(QLabel* pumpText, QProgressBar* pumpProgress, float value, float size)
{
	pumpText->setText(QString::number(value, 'f', 3));
	pumpProgress->setValue(value / size * 100);
}

void CoaterWidget::SlotUpdateSafePos(bool bSafety)
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

void CoaterWidget::SlotUpdateStepString(QString step)
{
	ui.labeStepString->setText(step);
	ui.labeStepString->setToolTip(step);
}

void CoaterWidget::SlotCoatingResult(int result)
{
	switch (result)
	{
	case 1:
		ui.LabelCoatingResult->setVisible(true);
		break;
	default:
		ui.LabelCoatingResult->setVisible(false);
		break;
	}
}
