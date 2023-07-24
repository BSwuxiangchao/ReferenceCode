/*
* @Abrief 机器手连接
* @note 通过参数系统监听机械手连接状态
* @version 1.0
* @author wuxiangchao 2023-2-28
*/


#pragma once

#include <qlineedit.h>
#include <qstring.h>
#include <qgroupbox.h>
#include "ParaControl.h"
#include "ParaObserver.h"
#include "QPushButton.h"
#include "ParaType.h"

class RobotWidget;
class EpsonRobotDebug;

class RobotConnectParaControl : public QObject, public ParaObserver
{
	Q_OBJECT
public:
	RobotConnectParaControl();
	~RobotConnectParaControl();

	virtual void Update();
	void SetParentWidget(QPushButton* pushButton, RobotWidget* robotWidget, EpsonRobotDebug* debuDlg);
	void SetUserInfo(const GeneralParaType::UserInfo userInfo);

public slots:
	void SlotUpdateStatus(bool bConnect);

signals:
	void SignalSetRobotConnectStatus(bool bConnect);
	void SignalSetRobotSafetyStatus(bool bSafety);
	void SignalUpdateStatus(bool bConnect);
private:
	ParaControl* m_RobotParaCtrl;
	QPushButton* m_SettingPushButton;
	RobotWidget* m_robotWidget;
	EpsonRobotDebug* m_debuDlg;
	GeneralParaType::UserInfo m_userInfo;
	int m_iConnectCount;
};

