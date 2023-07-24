#include "BasecallMgrWidget.h"
#include "MasterDefine.h"
#include "OpeStyleSheet.h"
#include "BgiMpsEvent.h"
#include "BgiMasterApp.h"
#include <QAction>
#include <QMessageBox.h>

BasecallMgrWidget::BasecallMgrWidget(QWidget *parent)
	: QMainWindow(parent)
	, m_StatusBasecall(0)
	, m_StatusSbc(0)
    , m_StatusImager(0)
{
	ui.setupUi(this);

	InitControls();
	SetStyleSheet();
}

BasecallMgrWidget::~BasecallMgrWidget()
{}

void BasecallMgrWidget::InitControls()
{
	QObject::connect(ui.ButtonRefresh, SIGNAL(clicked()), this, SLOT(ClickRefresh()));

	//设置表头
	m_MemTable.InitTableWidgetNoTest(ui.TableBasecallMem, gc_lstBasecallMemTableHeader, 2);

	//右键菜单
	ui.TableBasecallMem->setContextMenuPolicy(Qt::CustomContextMenu);
	m_pRightMenu = new QMenu(ui.TableBasecallMem);
	QAction* action = new QAction("释放内存", this);
	m_pRightMenu->addAction(action);
	connect(ui.TableBasecallMem, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowRightMenu(QPoint)));
	connect(action, SIGNAL(triggered(bool)), this, SLOT(SlotReleaseMem(bool)));
}

void BasecallMgrWidget::SetStyleSheet()
{
	OpeStyleSheet styleSheet;
	QString strStyle;
	bool bRet = styleSheet.ReadQPushbuttonStyleSheet(strStyle, 18, 1);
	if (bRet)
	{
		ui.centralWidget->setStyleSheet(strStyle);
	}
}

void BasecallMgrWidget::resizeEvent(QResizeEvent* event)
{
	SetColumnWidth();
}

void BasecallMgrWidget::showEvent(QShowEvent* event)
{
	SetColumnWidth();

	ClickRefresh();
}

void BasecallMgrWidget::SetColumnWidth()
{
	// 表头占比
	QVector<unsigned short> vecColRatio;
	vecColRatio.clear();
	vecColRatio.push_back(2);
	vecColRatio.push_back(1);
	vecColRatio.push_back(1);
	vecColRatio.push_back(1);
	vecColRatio.push_back(1);
	vecColRatio.push_back(1);
	vecColRatio.push_back(1);
	vecColRatio.push_back(1);
	vecColRatio.push_back(1);
	vecColRatio.push_back(1);
	m_MemTable.SetColumnRatio(ui.TableBasecallMem, vecColRatio);
}

void BasecallMgrWidget::ClickRefresh()
{
	if (m_StatusSbc == 3 && m_StatusImager == 1)
	{
		//当SBC状态为ScanRunning，光机状态为Idle时，调用接口会报Basecall is busy
		return;
	}

	BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
	pEvent->SetAction("GetBasecallMems");
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, pEvent);

	BgiMpsEvent* pEvent2 = new BgiMpsEvent("Driver_Event_Imager");
	pEvent2->SetAction("GetVersions");
	BgiMasterApp* app2 = BgiMasterApp::Instance();
	QApplication::postEvent(app2, pEvent2);

	BgiMpsEvent* pEvent3 = new BgiMpsEvent("Driver_Event_Imager");
	pEvent3->SetAction("GetDiskSpace");
	BgiMasterApp* app3 = BgiMasterApp::Instance();
	QApplication::postEvent(app3, pEvent3);
}

void BasecallMgrWidget::ShowRightMenu(QPoint point)
{
	m_pRightMenu->exec(QCursor::pos());
}

void BasecallMgrWidget::SlotReleaseMem(bool checked)
{
	int row = ui.TableBasecallMem->currentRow();
	QTableWidgetItem* item = ui.TableBasecallMem->item(row, 0);
	if (item != NULL)
	{
		QString slideId = item->text();
		BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Imager");
		pEvent->SetAction("ReleaseMem");
		pEvent->AddParams("Param1", slideId);
		BgiMasterApp* app = BgiMasterApp::Instance();
		QApplication::postEvent(app, pEvent);

		QThread::msleep(1000);
		ClickRefresh();
	}
	else
	{
		QMessageBox::critical(this, "错误", "请先选中需要释放内存的行！");
	}
}

void BasecallMgrWidget::SlotBasecallRunInfos(const QList<ImagerParaType::BasecallRunPara>& infos)
{
	QVector<QStringList> vec;
	foreach (auto info, infos)
	{
		QStringList list;
		list.append(info.SlideID);
		list.append(QString::number(info.CurrentCycle));
		list.append(QString::number(info.StarRow));
		list.append(QString::number(info.EndRow));
		list.append(QString::number(info.StartCol));
		list.append(QString::number(info.EndCol));
		list.append(QString::number(info.StartCycle));
		list.append(QString::number(info.Read1Length));
		list.append(QString::number(info.Read2Length));
		list.append(QString::number(info.TotalCycle));
		vec.append(list);
	}

	m_MemTable.UpdateTabelWidget(ui.TableBasecallMem, vec);
}

void BasecallMgrWidget::SlotImagerVersions(const QString& imagerVer, const QString& sbcVer, const QString& bcsVer)
{
	ui.lineImagerVer->setText(imagerVer);
	ui.lineSbcVer->setText(sbcVer);
	ui.lineBcsVer->setText(bcsVer);
}

void BasecallMgrWidget::SlotBasecallDiskSpace(double consumptionValue, double availableValue)
{
	ui.lineConsumptionSpace->setText(QString::number(consumptionValue, 'f', 0) + " G");
	ui.lineFreeSpace->setText(QString::number(availableValue, 'f', 0) + " G");
}

void BasecallMgrWidget::SlotImagerStatus(int basecall, int server, int imager)
{
	m_StatusBasecall = basecall;
	m_StatusSbc = server;
	m_StatusImager = imager;
}
