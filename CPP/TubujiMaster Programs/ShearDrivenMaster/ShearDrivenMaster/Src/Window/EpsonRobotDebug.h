/*
* @Abrief Eposn机械臂
* @note Eposn机械臂调试界面
* @version 1.0
* @author wuxiangchao 2023-2-8
*/
#pragma once

#include <QMainWindow>
#include "ui_EpsonRobotDebug.h"
#include "RobotParaType.h"
#include "MasterDefine.h"

class RobotDebugParaControl;

class EpsonRobotDebug : public QMainWindow
{
	Q_OBJECT

public:
	EpsonRobotDebug(QWidget *parent, QGroupBox* WorkStatus, QGroupBox* GetPara);
	~EpsonRobotDebug();

	virtual void keyPressEvent(QKeyEvent* event);
	void ChangeUIControlText(ChangeType type,QString strCurLanguage);

public slots:
	/*****************************************************动作Start*********************/
	void On_SetPower_Clicked();
	void On_SetPTPSpeed_Clicked();
	void On_SetPTPAccel_Clicked();
	void On_SetCPSpeed_Clicked();
	void On_SetCPAccel_Clicked();
	void On_SetRSpeed_Clicked();
	void On_SetRAccel_Clicked();
	void On_TransferPos_Clicked();
	void On_StartOptChip_Clicked();
	void On_TurnOnOutIO_Clicked();
	void On_TurnOffOutIO_Clicked();
	/*****************************************************动作End*********************/

	/*****************************************************类RC调试Start*********************/
	void On_GoVar_Clicked();
	void On_StepSport_Clicked(int iType, int iDircetion);
	/*****************************************************类RC调试End*********************/
	void SlotUpdatePosCombox();
private:
	void LimitInputUnit(QLineEdit* pLineEdit,QString strPalete);
	void LimitCtrlInput();
	// 设置样式
	void SetStyleSheet();
	void PostRobotEvent(QString strActionName);
	void PostRobotEvent(QString strActionName, QVariant var1);
	void PostRobotEvent(QString strActionName, QVariant var1, QVariant var2);
private:
	Ui::EpsonRobotDebugClass ui;

	RobotDebugParaControl* m_pRobotDebugParaControl;
	QString m_strCurLanguage;
};
