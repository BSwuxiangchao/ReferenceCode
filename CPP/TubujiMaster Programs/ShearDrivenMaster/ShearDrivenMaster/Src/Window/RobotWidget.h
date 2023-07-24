#pragma once

#include <QWidget>
#include "ui_RobotWidget.h"
#include "OperateRecordInfo.h"
#include "CommonColors.h"
#include "MasterDefine.h"


class RobotWidget : public QWidget
{
	Q_OBJECT

public:
	RobotWidget(QWidget *parent = nullptr);
	~RobotWidget();

	void ChangeUIControlText(ChangeType type,QString strCurLanguage);
	void SetRunState(bool isRunIng);
public slots:
	void SetRobotConnectStatus(bool bConnect);
	void SetRobotSafetyStatus(bool bSafety);
	void On_BackHome_Clicked();
	void On_StartOrStopTransfer_Clicked();

private:
	void PostRobotEvent(QString strActionName, QVariant var1);
	void SetStyleSheet();
	void AddRecordData(QString strContext, QColor color);
	void ApplicationSleep(int mSec);
private:
	Ui::RobotWidgetClass ui;
	bool m_bRorotConnectSuccess;
	ChangeType m_changeType;
	QString m_strCurLanguage;
	bool m_isRunIng;
};
