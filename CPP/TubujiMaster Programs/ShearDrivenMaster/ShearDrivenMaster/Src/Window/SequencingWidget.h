#pragma once

#include <QWidget>
#include "ui_SequencingWidget.h"
#include "qtimer.h"
#include "OperateTabelWidget.h"
#include "ParaType.h"

class SequencingParaControl;

class SequencingWidget : public QWidget
{
	Q_OBJECT

public:
	SequencingWidget(QWidget *parent = nullptr);
	~SequencingWidget();

	QString GetCurBarcode();

public slots:
	void SlotSaveLocalcfg(GeneralParaType::SequencingTableDesc tableDesc);
	void SlotUpdateSequencingTableWidget(QVector<QStringList> vec);
private:
	void SetStatisticalTableRatio();
	
	void TestCode();

protected slots:
	void resizeEvent(QResizeEvent* event);
	void TimeoutDeal();

private:
	Ui::SequencingWidgetClass ui;

	// 表格操作
	OperateTabelWidget m_opeTable;

	// 定时器
	QTimer m_timer;

	SequencingParaControl *m_SequencingParaControl;

	GeneralParaType::SequencingTableDesc m_CurSequencingInfo;
};
