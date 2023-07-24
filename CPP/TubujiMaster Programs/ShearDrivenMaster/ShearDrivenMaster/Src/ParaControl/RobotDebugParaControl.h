/*
* @Abrief 机器手参数
* @note 通过参数系统监听机器人参数变化
* @version 1.0
* @author wuxiangchao 2023-2-7
*/


#pragma once

#include <qlineedit.h>
#include <qstring.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qstatusbar.h>
#include "ParaControl.h"
#include "ParaObserver.h"
#include "RobotParaType.h"
#include "MasterDefine.h"

class RobotDebugParaControl : public QObject, public ParaObserver
{
	Q_OBJECT
public:
	RobotDebugParaControl();
	~RobotDebugParaControl();

	virtual void Update();
	void SetParentWidget(QGroupBox* RobotPara, QGroupBox* WorkStatus1, QGroupBox* WorkStatus2, QGroupBox* robotPoint, 
		QGroupBox* WorkStatus3,QGroupBox* RobotPara2, QStatusBar* qStatusBar);
	void SetLanguageChangeType(ChangeType type);

signals:
	void SignalUpdateStatus();

public slots:
	void SlotUpdateStatus();

private:
	void InitCtrlWidgets();
	void InitStatusLabel();
	void UpdateParaUnit(RobotParaType::RobotPara* robotStatus);
	void UpdateStatusUnit(RobotParaType::RobotStatus* robotStatus);
	void UpdatePointUnit(RobotParaType::RobotPoint* robotPoint);
private:
	ParaControl* m_RobotParaParaCtrl;
	ParaControl* m_RobotWorkStatusParaCtrl;
	ParaControl* m_RobotPointParaParaCtrl;

	QLabel* m_label_SecuritySwitch1, *m_label_SecuritySwitch2,* m_label_SecuritySwitch3;
	QLabel* m_label_PauseSwitch1, *m_label_PauseSwitch2, * m_label_PauseSwitch3;
	QLabel* m_label_MotorSwitch1, *m_label_MotorSwitch2, * m_label_MotorSwitch3;
	QLabel* m_label_MotorPower1, * m_label_MotorPower2, * m_label_MotorPower3;
	QLabel* m_label_Error1, * m_label_Error2, * m_label_Error3;
	QLineEdit* m_lineEdit_ErrCode1, *m_lineEdit_ErrCode2, * m_lineEdit_ErrCode3;

	QLineEdit* m_lineEdit_GetRobotNo;
	QLineEdit* m_lineEdit_GetPTPSpeed, * m_lineEdit_GetPTPSpeed2;
	QLineEdit* m_lineEdit_GetPTPAccel, * m_lineEdit_GetPTPAccel2;
	QLineEdit* m_lineEdit_GetCPSpeed, * m_lineEdit_GetCPSpeed2;
	QLineEdit* m_lineEdit_GetCPAccel, * m_lineEdit_GetCPAccel2;
	QLineEdit* m_lineEdit_GetRSpeed;
	QLineEdit* m_lineEdit_GetRAccel;

	QLineEdit* m_lineEdit_PosX;
	QLineEdit* m_lineEdit_PosY;
	QLineEdit* m_lineEdit_PosZ;
	QLineEdit* m_lineEdit_PosU;
	QLineEdit* m_lineEdit_PosV;
	QLineEdit* m_lineEdit_PosW;

	QStatusBar* m_pStatusBar;
	QLabel* m_statusLabel;

	ChangeType m_changeType;

	// 是否已经初始化过控件
	bool m_bInitCtrlWidgets;
};

