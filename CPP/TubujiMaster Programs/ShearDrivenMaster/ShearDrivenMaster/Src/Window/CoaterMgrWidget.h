#pragma once

#include <QMainWindow>
#include "ui_CoaterMgrWidget.h"
#include "OperateTabelWidget.h"

class CoaterDriver;
class CoaterDataWidget;

class CoaterMgrWidget : public QMainWindow
{
	Q_OBJECT

public:
	CoaterMgrWidget(QWidget *parent = nullptr);
	~CoaterMgrWidget();

protected:
	virtual void showEvent(QShowEvent* event);
	virtual void resizeEvent(QResizeEvent* event);

private slots:
	void SlotAllAlarm(const QMap<int, QString>& alarms);
	void SlotDeviceStatus(int index, int value);
	void SlotCoaterConnectStatus(bool bSuccess);

	void on_ButtonStartup_clicked();
	void on_ButtonPause_clicked();
	void on_ButtonReset_clicked();
	void on_ButtonStop_clicked();
	void on_ButtonHalt_clicked();
	void on_ButtonOpenVacuum_clicked();
	void on_ButtonCloseVacuum_clicked();
	void on_ButtonClearAlarm_clicked();
	void on_ButtonPLCData_clicked();

private:
	void InitControls();
	void SetStyleSheet();
	void SetColumnWidth();

	void GetConnStatus();

	void SendCoaterEvent(QString action, bool bParam = false);

private:
	Ui::CoaterMgrWidgetClass ui;

	//报警列表
	OperateTabelWidget m_MemTable;

	CoaterDriver* m_CoaterDriver;
	QSharedPointer<CoaterDataWidget> m_pDataWidget;
};
