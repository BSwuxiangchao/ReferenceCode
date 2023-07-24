#pragma once

#include <QWidget>
#include "ui_CoaterWidget.h"
#include "MasterDefine.h"

class CoaterDriver;

class CoaterWidget : public QWidget
{
	Q_OBJECT

public:
	CoaterWidget(QWidget* parent = nullptr);
	~CoaterWidget();

	void ChangeUIControlText(ChangeType type,QString strCurLanguage);

public slots:
	void SlotCoaterConnectStatus(bool bSuccess);
	void SlotDeviceStatus(int index, int value);
	void SlotVacuumValue(int index, float value);
	void SlotPumpStatus(int index, int value);
	void SlotPumpValue(int index, int value);
	void SlotPumpSize(int index, int value);
	void SlotUpdateSafePos(bool bSafety);
	void SlotUpdateStepString(QString step);
	void SlotCoatingResult(int result);

private slots:
	void ClickStartup();
	void ClickPause();
	void ClickReset();
	void ClickStop();
	void ClickClearAlarm();

private:
	void InitControls();
	void InitStatus();
	void SetStyleSheet();

	void GetConnStatus();

	void SendCoaterEvent(QString action);

	void SetPumpState(QLabel* pumpText, QProgressBar* pumpProgress, int value);
	void SetPumpValue(int index, float value, float size);
	void SetPumpValue(QLabel* pumpText, QProgressBar* pumpProgress, float value, float size);

private:
	Ui::CoaterWidgetClass ui;

	CoaterDriver* m_CoaterDriver;
	QString m_strCurLanguage;

	QMap<int, QPair<float, float>> m_PumpValues;
};
