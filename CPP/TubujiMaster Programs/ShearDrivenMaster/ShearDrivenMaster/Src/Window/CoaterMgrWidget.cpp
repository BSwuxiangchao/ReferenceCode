#include "CoaterMgrWidget.h"
#include "OpeStyleSheet.h"
#include "MasterDevices.h"
#include "CoaterDriver.h"
#include "BgiMpsEvent.h"
#include "BgiMasterApp.h"
#include "CoaterDataWidget.h"
#include "InfoPromptWidget.h"
#include "CommonColors.h"

CoaterMgrWidget::CoaterMgrWidget(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	InitControls();
	SetStyleSheet();

	//涂布机驱动接口
	m_CoaterDriver = MasterDevices::Instance()->GetCoaterDriver();
	QObject::connect(m_CoaterDriver, SIGNAL(SignalAllAlarm(const QMap<int, QString>&)), this, SLOT(SlotAllAlarm(const QMap<int, QString>&)));
	QObject::connect(m_CoaterDriver, SIGNAL(SignalDeviceStatus(int, int)), this, SLOT(SlotDeviceStatus(int, int)));
	QObject::connect(m_CoaterDriver, SIGNAL(SignalCoaterConnectStatus(bool)), this, SLOT(SlotCoaterConnectStatus(bool)));

	//启动时主动获取一次连接状态
	GetConnStatus();

	//PLC原始数据
	m_pDataWidget = QSharedPointer<CoaterDataWidget>(new CoaterDataWidget(this));
	connect(m_CoaterDriver, SIGNAL(SignalNewData(PLCDataType, const QVector<int>&)), m_pDataWidget.data(), SLOT(SlotNewData(PLCDataType, const QVector<int>&)));

	this->setWindowFlags(this->windowFlags() & ~Qt::WindowMinimizeButtonHint);
	//this->setFixedSize(this->geometry().size());

	//暂停涉及到状态切换，需要与主界面同步，本界面的暂不显示
	ui.ButtonStartup->setVisible(false);
	ui.ButtonPause->setVisible(false);
	ui.ButtonReset->setVisible(false);
	ui.ButtonStop->setVisible(false);
	ui.ButtonClearAlarm->setVisible(false);
}

CoaterMgrWidget::~CoaterMgrWidget()
{}

void CoaterMgrWidget::InitControls()
{
	//设置表头
	QStringList colNames = {"报警地址", "报警信息" };
	m_MemTable.InitTableWidgetNoTest(ui.TableAlarm, colNames, 2);

	//QObject::connect(ui.ButtonStartup, SIGNAL(clicked()), this, SLOT(ClickStartup()));
	
	//初始化泵按钮
	//foreach (auto obj, ui.GBPump->children())
	//{
	//	QString name = obj->objectName();
	//	//ButtonPump_Stop_7
	//	if (name.startsWith("ButtonPump"))
	//	{
	//		QPushButton* btn = qobject_cast<QPushButton*>(obj);
	//		QObject::connect(btn, SIGNAL(clicked()), this, SLOT(ButtonPumpClicked()));
	//	}
	//}
}

void CoaterMgrWidget::SetStyleSheet()
{
	OpeStyleSheet styleSheet;
	QString strStyle;
	bool bRet = styleSheet.ReadQPushbuttonStyleSheet(strStyle, 18, 1);
	if (bRet)
	{
		ui.centralWidget->setStyleSheet(strStyle);
	}
}

void CoaterMgrWidget::showEvent(QShowEvent* event)
{
	SetColumnWidth();
}

void CoaterMgrWidget::resizeEvent(QResizeEvent* event)
{
	SetColumnWidth();
}

void CoaterMgrWidget::SetColumnWidth()
{
	//表头占比
	QVector<unsigned short> vecColRatio;
	vecColRatio.push_back(2);
	vecColRatio.push_back(8);
	m_MemTable.SetColumnRatio(ui.TableAlarm, vecColRatio);
}

void CoaterMgrWidget::SlotAllAlarm(const QMap<int, QString>& alarms)
{
	QVector<QStringList> vec;
	for (auto it = alarms.begin(); it != alarms.end(); ++it)
	{
		QStringList list;
		list.append("DM" + QString::number(it.key()));
		list.append(it.value());
		vec.append(list);
	}

	m_MemTable.UpdateTabelWidget(ui.TableAlarm, vec);
}

void CoaterMgrWidget::SlotDeviceStatus(int index, int value)
{
	switch (index)
	{
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
	default:
		break;
	}
}

void CoaterMgrWidget::SlotCoaterConnectStatus(bool bSuccess)
{
	ui.ButtonPause->setEnabled(bSuccess);
	ui.ButtonStop->setEnabled(bSuccess);
	ui.ButtonHalt->setEnabled(bSuccess);
	ui.ButtonOpenVacuum->setEnabled(bSuccess);
	ui.ButtonCloseVacuum->setEnabled(bSuccess);
	ui.ButtonClearAlarm->setEnabled(bSuccess);

	if (!bSuccess)
	{
		ui.ButtonStartup->setEnabled(false);
		ui.ButtonReset->setEnabled(false);
	}
}

void CoaterMgrWidget::SendCoaterEvent(QString action, bool bParam)
{
	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Coater");
	pEvent->SetAction(action);
	pEvent->AddParams("Param1", bParam);
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}

void CoaterMgrWidget::GetConnStatus()
{
	SendCoaterEvent("GetConnStatus");
}

void CoaterMgrWidget::on_ButtonStartup_clicked()
{
	SendCoaterEvent("Startup");
}

void CoaterMgrWidget::on_ButtonPause_clicked()
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

	SendCoaterEvent("Pause", bPause);
}

void CoaterMgrWidget::on_ButtonReset_clicked()
{
	SendCoaterEvent("Reset");
}

void CoaterMgrWidget::on_ButtonStop_clicked()
{
	SendCoaterEvent("Stop");
}

void CoaterMgrWidget::on_ButtonHalt_clicked()
{
	InfoPromptWidget infoWidget;
	BUTTON_STATE ret = infoWidget.Show("急停确认", "确定要让涂布机急停吗？", true, 3, Qt::red, gc_colorLightBlue);
	if (BUTTON_STATE::SURE != ret)
	{
		return;
	}

	SendCoaterEvent("Halt");
}

void CoaterMgrWidget::on_ButtonOpenVacuum_clicked()
{
	SendCoaterEvent("SwitchVacuum", true);
}

void CoaterMgrWidget::on_ButtonCloseVacuum_clicked()
{
	SendCoaterEvent("SwitchVacuum", false);
}

void CoaterMgrWidget::on_ButtonClearAlarm_clicked()
{
	SendCoaterEvent("AlarmReset");
}

void CoaterMgrWidget::on_ButtonPLCData_clicked()
{
	m_pDataWidget->show();
}

//void CoaterMgrWidget::ButtonPumpClicked()
//{
//	QPushButton* btn = qobject_cast<QPushButton*>(sender());
//	QString name = btn->objectName();
//	//ButtonPump_Stop_7
//	auto strs = name.split('_');
//	if (strs.count() == 3)
//	{
//
//	}
//}
