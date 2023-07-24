#include "ImagerMgrWidget.h"
#include "OpeStyleSheet.h"
#include "BgiMpsEvent.h"
#include "BgiMasterApp.h"
#include "MasterDevices.h"
#include "ImagerDriver.h"
#include <QMessageBox.h>
#include "ImagerFovWidget.h"
#include "BasecallMgrWidget.h"
#include <QFileDialog>
#include "BgiLogger.h"

ImagerMgrWidget::ImagerMgrWidget(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	InitControls();
	SetStyleSheet();

	m_ImagerDriver = MasterDevices::Instance()->GetImagerDriver();
	//QObject::connect(m_ImagerDriver, &ImagerDriver::SignalImagerConfigData, this, &ImagerMgrWidget::SlotImagerConfigData);
	QObject::connect(m_ImagerDriver, SIGNAL(SignalImagerConfigData(const ImagerParaType::ImagerConfigPara&)), this, SLOT(SlotImagerConfigData(const ImagerParaType::ImagerConfigPara&)));
	QObject::connect(m_ImagerDriver, SIGNAL(SignalImagerConfigSet(bool)), this, SLOT(SlotImagerConfigSetResult(bool)));

	m_pImagerFovWidget = QSharedPointer<ImagerFovWidget>(new ImagerFovWidget(this));
	m_pBasecallMgrWidget = QSharedPointer<BasecallMgrWidget>(new BasecallMgrWidget(this));
	QObject::connect(m_ImagerDriver, SIGNAL(SignalBasecallRunInfos(const QList<ImagerParaType::BasecallRunPara>&)), m_pBasecallMgrWidget.data(), SLOT(SlotBasecallRunInfos(const QList<ImagerParaType::BasecallRunPara>&)));
	QObject::connect(m_ImagerDriver, SIGNAL(SignalImagerVersions(const QString&, const QString&, const QString&)), m_pBasecallMgrWidget.data(), SLOT(SlotImagerVersions(const QString&, const QString&, const QString&)));
	QObject::connect(m_ImagerDriver, SIGNAL(SignalBasecallDiskSpace(double, double)), m_pBasecallMgrWidget.data(), SLOT(SlotBasecallDiskSpace(double, double)));
	QObject::connect(m_ImagerDriver, SIGNAL(SignalImagerStatus(int, int, int)), m_pBasecallMgrWidget.data(), SLOT(SlotImagerStatus(int, int, int)));

	this->setWindowFlags(this->windowFlags() & ~Qt::WindowMinMaxButtonsHint);
	this->setFixedSize(this->geometry().size());
}

ImagerMgrWidget::~ImagerMgrWidget()
{
}

void ImagerMgrWidget::InitControls()
{
	QObject::connect(ui.ButtonExposureFile, SIGNAL(clicked()), this, SLOT(ClickExposureFile()));
	QObject::connect(ui.ButtonFovSetting, SIGNAL(clicked()), this, SLOT(ClickFovSetting()));
	QObject::connect(ui.ButtonBasecallSetting, SIGNAL(clicked()), this, SLOT(ClickBasecallSetting()));
	QObject::connect(ui.ButtonSave, SIGNAL(clicked()), this, SLOT(ClickSaveConfig()));

	QObject::connect(ui.ButtonLoadingPos, SIGNAL(clicked()), this, SLOT(ClickGoLoadingPos()));
	QObject::connect(ui.ButtonHome, SIGNAL(clicked()), this, SLOT(ClickGoHome()));
	QObject::connect(ui.ButtonOpenVacuum, SIGNAL(clicked()), this, SLOT(ClickOpenVacuum()));
	QObject::connect(ui.ButtonCloseVacuum, SIGNAL(clicked()), this, SLOT(ClickCloseVacuum()));
	QObject::connect(ui.ButtonReset, SIGNAL(clicked()), this, SLOT(ClickReset()));
}

void ImagerMgrWidget::SetStyleSheet()
{
	OpeStyleSheet styleSheet;
	QString strStyle;
	bool bRet = styleSheet.ReadQPushbuttonStyleSheet(strStyle, 18, 1);
	if (bRet)
	{
		ui.centralWidget->setStyleSheet(strStyle);
		//ui.LayoutParams->setStyleSheet(strStyle);
		//ui.LayoutButtons->setStyleSheet(strStyle);
	}
}

void ImagerMgrWidget::showEvent(QShowEvent* event)
{
	//每次打开刷新光机侧的配置
	InitParams();
	GetImagerConfig();
}

void ImagerMgrWidget::InitParams()
{
	ui.comboBasecalling->setCurrentIndex(1);
	ui.checkBoxSaveRawImage->setChecked(false);
	ui.checkBoxImageSimulated->setChecked(false);
	ui.lineSettingTime->setText("");

	ui.lineStartRow->setText("");
	ui.lineEndRow->setText("");
	ui.lineStartCol->setText("");
	ui.lineEndCol->setText("");

	ui.lineSequenceType->setText("查询中...");
}

void ImagerMgrWidget::GetImagerConfig()
{
	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
	pEvent->SetAction("ImagerConfigGet");
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}

void ImagerMgrWidget::SlotImagerConfigData(const ImagerParaType::ImagerConfigPara& data)
{
	BGI_MESSAGE("Recv ImagerConfigData");
	m_ImagerConfigData = data;

	ui.comboBasecalling->setCurrentIndex(data.BasecallMode);
	ui.checkBoxSaveRawImage->setChecked(data.IsSaveRawImage);
	ui.checkBoxImageSimulated->setChecked(data.IsImageSimulated);
	ui.lineSettingTime->setText(QString::number(data.SettingTime));

	ui.lineStartRow->setText(QString::number(data.StartRow));
	ui.lineEndRow->setText(QString::number(data.EndRow));
	ui.lineStartCol->setText(QString::number(data.StartCol));
	ui.lineEndCol->setText(QString::number(data.EndCol));

	ui.lineSequenceType->setText(data.EndCycleProcessMode);

	//发给FOV设置窗口
	int count = m_pImagerFovWidget->SetSaveFovString(data.SaveFovString);
	ui.labelFovCount->setText("存图FOV个数：" + QString::number(count));
}

void ImagerMgrWidget::SetSaveFovString(QString fovString, int count)
{
	m_ImagerConfigData.SaveFovString = fovString;
	ui.labelFovCount->setText("存图FOV个数：" + QString::number(count));
}

void ImagerMgrWidget::ClickExposureFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, "请选择曝光文件", "C:\\BGI\\recipes", "CSV Files(*.csv)");
	if (!fileName.isNull())
	{
		ui.lineExposureFile->setText(fileName);
		ImagerDriver::ReadExposureFile(fileName, m_ImagerConfigData.ExposureTime, m_ImagerConfigData.ExposureFactors);
	}
}

void ImagerMgrWidget::ClickFovSetting()
{
	m_pImagerFovWidget->setWindowState(Qt::WindowActive);
	m_pImagerFovWidget->show();
}

void ImagerMgrWidget::ClickBasecallSetting()
{
	m_pBasecallMgrWidget->setWindowState(Qt::WindowActive);
	m_pBasecallMgrWidget->show();
}

void ImagerMgrWidget::ClickSaveConfig()
{
	BGI_MESSAGE("Save ImagerConfig");
	ImagerParaType::ImagerConfigPara data;

	data.Lane = 1;
	data.Reference = "";

	data.BasecallMode = ui.comboBasecalling->currentIndex();
	data.IsSaveRawImage = ui.checkBoxSaveRawImage->isChecked();
	data.IsImageSimulated = ui.checkBoxImageSimulated->isChecked();
	data.SettingTime = ui.lineSettingTime->text().toInt();

	data.CheckThetaMode = 2;
	data.Direction = 2;

	data.StartRow = ui.lineStartRow->text().toInt();
	data.EndRow = ui.lineEndRow->text().toInt();
	data.StartCol = ui.lineStartCol->text().toInt();
	data.EndCol = ui.lineEndCol->text().toInt();

	data.IsExposureFileClean = false;
	data.ExposureTime = m_ImagerConfigData.ExposureTime;
	data.ExposureFactors = m_ImagerConfigData.ExposureFactors;
	data.SaveFovString = m_ImagerConfigData.SaveFovString;
	data.EndCycleProcessMode = ui.lineSequenceType->text();

	data.VacuumSupplyThreshold = m_ImagerConfigData.VacuumSupplyThreshold;
	data.ExtraSettings = "";

	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
	pEvent->SetAction("ImagerConfigSet");
	QVariant para = QVariant::fromValue(data);
	pEvent->AddParams("Param1", para);
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}

void ImagerMgrWidget::SlotImagerConfigSetResult(bool bSuccess)
{
	if (!this->isActiveWindow())
	{
		return;
	}

	if (bSuccess)
	{
		QMessageBox::information(this, "提示", "光机参数设置成功！");
		//设置成功后再查询一遍光机配置
		GetImagerConfig();
		this->close();
	}
	else
	{
		QMessageBox::critical(this, "错误", "光机参数设置失败！");
	}
}

void ImagerMgrWidget::ClickGoLoadingPos()
{
	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
	pEvent->SetAction("GoLoadingPos");
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}

void ImagerMgrWidget::ClickGoHome()
{
	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
	pEvent->SetAction("GoHome");
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}

void ImagerMgrWidget::ClickOpenVacuum()
{
	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
	pEvent->SetAction("SwitchVacuum");
	pEvent->AddParams("Param1", true);
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}

void ImagerMgrWidget::ClickCloseVacuum()
{
	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
	pEvent->SetAction("SwitchVacuum");
	pEvent->AddParams("Param1", false);
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}

void ImagerMgrWidget::ClickReset()
{
	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
	pEvent->SetAction("Reset");
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);
}
