#pragma once

#include <QMainWindow>
#include "ui_ErrorLogWidget.h"
#include "OperateTabelWidget.h"
#include "qtimer.h"
#include "ParaType.h"

class ErrorLogWidget : public QMainWindow
{
	Q_OBJECT

public:
	ErrorLogWidget(QWidget *parent = nullptr);
	~ErrorLogWidget();

protected slots:
	void resizeEvent(QResizeEvent* event);
	void TimeoutDeal();
	void On_ScreenErr_Clicked();
	void On_DeleteErr_Clicked();
private:
	QString GetDateEditTotalText(QDateEdit* DateEdit);
	void SetStyleSheet();
	void ResetTimeRange();
	void CreateToolBar();
	void InitWidget();
	void SetStatisticalTableRatio();
	void UpdateErrInfoTableWidget(QList<GeneralParaType::sqlErrorData> vecSqlErrData);
	void TestCode();


private:
	Ui::ErrorLogWidgetClass ui;

	// 表格操作
	OperateTabelWidget m_opeTable;

	// 定时器
	QTimer m_timer;

};
