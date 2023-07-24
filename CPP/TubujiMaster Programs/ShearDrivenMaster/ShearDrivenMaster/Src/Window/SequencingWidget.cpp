#include "SequencingWidget.h"
#include "MasterDefine.h"
#include "SequencingParaControl.h"
#include "OperateSqliteConfig.h"

SequencingWidget::SequencingWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	// 更新表格表头
	m_opeTable.InitTableWidgetNoTest(ui.tableWidget_sequencingInfo, gc_lstSequencingTableHeader, 2);

	//TestCode();
	m_timer.setInterval(1 * 1000);
	connect(&m_timer, &QTimer::timeout, this, &SequencingWidget::TimeoutDeal);

	m_SequencingParaControl = new SequencingParaControl();
	connect(m_SequencingParaControl, &SequencingParaControl::SignalUpdateSequencingTableWidget, this, &SequencingWidget::SlotUpdateSequencingTableWidget);
	m_SequencingParaControl->SetParentWidget(ui.tableWidget_sequencingInfo);

	//查询一条最近时间保存的记录，显示到主界面测序信息
	GeneralParaType::SqlCfgData sqlCfgData;
	OperateSqliteConfig::Instance()->QueryRecentCfgData(sqlCfgData);
	GeneralParaType::SequencingTableDesc tableDesc = sqlCfgData.strTableDesc;
	SlotSaveLocalcfg(sqlCfgData.strTableDesc);
}

SequencingWidget::~SequencingWidget()
{
}

void SequencingWidget::SetStatisticalTableRatio()
{
	QVector<unsigned short> vecColRatio;
	vecColRatio.clear();
	// 表头占空比
	vecColRatio.push_back(1);
	vecColRatio.push_back(4);
	vecColRatio.push_back(2);
	vecColRatio.push_back(2);
	vecColRatio.push_back(2);
	vecColRatio.push_back(2);
	vecColRatio.push_back(1);
	vecColRatio.push_back(1);
	vecColRatio.push_back(1);
	vecColRatio.push_back(1);
	vecColRatio.push_back(1);
	m_opeTable.SetColumnRatio(ui.tableWidget_sequencingInfo, vecColRatio);
	int i = 0;
	int iCount = 1000;
	while (i++ < iCount)
	{
		QCoreApplication::processEvents();
	}
}

void SequencingWidget::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event);
	// 启动定时器，重新设置表格的列宽占比
	m_timer.start();
}

void SequencingWidget::TimeoutDeal()
{
	// 设置统计表格列宽占空比
	SetStatisticalTableRatio();

	static int iCount = 0;
	if (1 == iCount++)
	{
		// 关闭定时器
		m_timer.stop();
	}
}

QString SequencingWidget::GetCurBarcode()
{
	return m_CurSequencingInfo.strChipNo;
}

void SequencingWidget::SlotSaveLocalcfg(GeneralParaType::SequencingTableDesc tableDesc)
{
	m_CurSequencingInfo = tableDesc;
	m_SequencingParaControl->SetSaveLocalcfg(tableDesc);
}

void SequencingWidget::SlotUpdateSequencingTableWidget(QVector<QStringList> vec)
{
	m_opeTable.UpdateTabelWidget(ui.tableWidget_sequencingInfo, vec);
}

void SequencingWidget::TestCode()
{
	
}