#pragma once

#include <QWidget>
#include "ui_ImagerWidget.h"
#include "MasterDefine.h"
#include "ImagerParaType.h"

class ImagerDriver;

class ImagerWidget : public QWidget
{
	Q_OBJECT

public:
	ImagerWidget(QWidget *parent = nullptr);
	~ImagerWidget();

	void ChangeUIControlText(ChangeType type,QString strCurLanguage);

signals:
	void SignalBasecallSetting();

public slots:
	void SlotImagerConnectStatus(bool bSuccess);
	void SlotImagerRunInfo(const ImagerParaType::ImagerRunPara& info);
	void SlotImagerVacuumValue(float value);
	void SlotImagerStatus(int basecall, int server, int imager);
	void SlotImagerData(const QString& key, const QString& value);
	void SlotImagerConfigData(const ImagerParaType::ImagerConfigPara& data);
	void InitRunStatus();
	void SlotUpdateSafePos(bool bSafety);

private slots:
	void ClickGoLoadingPos();
	void ClickOpenVacuum();
	void ClickCloseVacuum();
	void ClickReset();
	void ClickSetBasecall();

private:
	void InitControls();
	void InitStatus();
	void SetStyleSheet();

	void GetConnStatus();

private:
	Ui::ImagerWidgetClass ui;
	QString m_strCurLanguage;

	ImagerDriver* m_ImagerDriver;
};
