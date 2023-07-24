#include "ErrorLogWidget.h"
#include "OperateSqliteConfig.h"
#include "MasterDefine.h"
#include "OpeStyleSheet.h"
#include "InfoPromptWidget.h"
#include "CommonColors.h"

ErrorLogWidget::ErrorLogWidget(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setWindowModality(Qt::ApplicationModal);
	Qt::WindowFlags windowFlag = windowFlags() & ~Qt::WindowMinimizeButtonHint;
	setWindowFlags(windowFlag);
	//TestCode();
	// 更新表格表头
	
	InitWidget();
	m_timer.setInterval(1 * 1000);
	connect(&m_timer, &QTimer::timeout, this, &ErrorLogWidget::TimeoutDeal);
}

ErrorLogWidget::~ErrorLogWidget()
{}


void ErrorLogWidget::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event);
	// 启动定时器，重新设置表格的列宽占比
	m_timer.start();
}

void ErrorLogWidget::TimeoutDeal()
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

QString ErrorLogWidget::GetDateEditTotalText(QDateEdit* DateEdit)
{
	QString strYear = DateEdit->sectionText(QDateEdit::YearSection);
	QString strMonth = DateEdit->sectionText(QDateEdit::MonthSection);
	QString strDay = DateEdit->sectionText(QDateEdit::DaySection);
	//2023-04-14 16:25:37
	if (strMonth.toInt() < 10)
		strMonth = "0" + strMonth;
	if (strDay.toInt() < 10)
		strDay = "0" + strDay;
	QString strTotalDate = strYear + "-" + strMonth + "-" + strDay;
	return strTotalDate;
}

void ErrorLogWidget::SetStyleSheet()
{
   // 设置样式
	OpeStyleSheet styleSheet;
	QString strStyle;
	bool bRet = styleSheet.ReadUICtrlsStyleSheetByFileName(strStyle, "QPushbutton-NofontNoRadius");
	if (bRet)
	{
		ui.pushButton_ScreenErr->setStyleSheet(strStyle);
		ui.pushButton_DeleteErr->setStyleSheet(strStyle);
	}
}

void ErrorLogWidget::ResetTimeRange()
{
	QString StartTime;
	QString EndTime;
	OperateSqliteConfig::Instance()->GetRecentRangTime(StartTime, EndTime);
	QDateTime StartDateTime = QDateTime::fromString(StartTime, "yyyy-MM-dd HH:mm:ss");
	QDateTime EndDateTime = QDateTime::fromString(EndTime, "yyyy-MM-dd HH:mm:ss").addDays(1);
	ui.dateEdit_StartTime->setDateTime(StartDateTime);
	ui.dateEdit_StartTime->setMinimumDateTime(StartDateTime);
	ui.dateEdit_StartTime->setMaximumDateTime(EndDateTime);

	ui.dateEdit_EndTime->setDateTime(EndDateTime);
	ui.dateEdit_EndTime->setMinimumDateTime(StartDateTime);
	ui.dateEdit_EndTime->setMaximumDateTime(EndDateTime);
}

void ErrorLogWidget::InitWidget()
{
	m_opeTable.InitTableWidgetOneSelect(ui.tableWidget_ErrInfo, gc_lstErrLogTableHeader, 2);
	SetStatisticalTableRatio();
	ui.dateEdit_StartTime->setCalendarPopup(true);
	ui.dateEdit_StartTime->setDate(QDate::currentDate());
	ui.dateEdit_EndTime->setCalendarPopup(true);
	ui.dateEdit_EndTime->setDate(QDate::currentDate());
	SetStyleSheet();
	connect(ui.pushButton_ScreenErr, SIGNAL(clicked()), this, SLOT(On_ScreenErr_Clicked()));
	connect(ui.pushButton_DeleteErr, SIGNAL(clicked()), this, SLOT(On_DeleteErr_Clicked()));

	QList<GeneralParaType::sqlErrorData> vecSqlErrData;
	OperateSqliteConfig::Instance()->QueryErrInfo(vecSqlErrData);
	UpdateErrInfoTableWidget(vecSqlErrData);
	ResetTimeRange();
}

void ErrorLogWidget::SetStatisticalTableRatio()
{
	QVector<unsigned short> vecColRatio;
	vecColRatio.clear();
	// 表头占空比
	vecColRatio.push_back(2);
	vecColRatio.push_back(2);
	vecColRatio.push_back(9);
	m_opeTable.SetColumnRatio(ui.tableWidget_ErrInfo, vecColRatio);
	int i = 0;
	int iCount = 1000;
	while (i++ < iCount)
	{
		QCoreApplication::processEvents();
	}
}

void ErrorLogWidget::UpdateErrInfoTableWidget(QList<GeneralParaType::sqlErrorData> vecSqlErrData)
{
	QVector<QStringList> vecLst;
	vecLst.clear();
	m_opeTable.UpdateTabelWidget(ui.tableWidget_ErrInfo, vecLst);
	int count = vecSqlErrData.size();
	QStringList strList;
	for (int i = 0; i < count; ++i)
	{
		strList.clear();
		strList.append(vecSqlErrData.at(i).ErrTime);
		strList.append(vecSqlErrData.at(i).Errtype);
		strList.append(vecSqlErrData.at(i).ErrContent);
		
		vecLst.push_back(strList);
	}
	m_opeTable.UpdateTabelWidget(ui.tableWidget_ErrInfo, vecLst);
}

void ErrorLogWidget::On_ScreenErr_Clicked()
{
	QString strStart = GetDateEditTotalText(ui.dateEdit_StartTime);
	QString strEnd = GetDateEditTotalText(ui.dateEdit_EndTime);
	QList<GeneralParaType::sqlErrorData> vecSqlErrData;
	OperateSqliteConfig::Instance()->QueryErrInfo(vecSqlErrData, strStart, strEnd, InfoType(ui.comboBox_ErrType->currentIndex()));
	UpdateErrInfoTableWidget(vecSqlErrData);
}

void ErrorLogWidget::On_DeleteErr_Clicked()
{
	for (int i = 0; i < 2; i++)
	{
		_sleep(1000);
		QString test = QString("这是一些错位测试%1").arg(i);
		OperateSqliteConfig::Instance()->RecordErrToDatabase(test, GeneralType);

	}
	return;
	

	InfoPromptWidget infoWidget;
	BUTTON_STATE ret = infoWidget.Show("删除确认", "您确定要删除这些错误日志吗？", true, 3, Qt::red, gc_colorLightGreen);

	if (BUTTON_STATE::SURE != ret)
	{
		return ;
	}
	//删除
	QString strStart = GetDateEditTotalText(ui.dateEdit_StartTime);
	QString strEnd = GetDateEditTotalText(ui.dateEdit_EndTime);
	OperateSqliteConfig::Instance()->DelErrInfo(strStart, strEnd, InfoType(ui.comboBox_ErrType->currentIndex()));
	//重载数据
	On_ScreenErr_Clicked();
	ResetTimeRange();
}

void ErrorLogWidget::TestCode()
{
	/*for (int i = 0; i < 2; i++)
	{
		_sleep(1000);
		QString test = QString("这是一些错位测试%1").arg(i);
		ErrorCodeTranslate::Instance()->RecordErrToDatabase(test, GeneralType);

	}*/
	QList<GeneralParaType::sqlErrorData> vecSqlErrData;
	OperateSqliteConfig::Instance()->QueryErrInfo(vecSqlErrData);

}