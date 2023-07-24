#pragma once

#include <QMainWindow>
#include "ui_UserLoginDlg.h"
#include <qtoolbar.h>

class ShearDrivenMaster;
class ModifyUserInfo;
class InfoPromptWidget;

class UserLoginDlg : public QMainWindow
{
	Q_OBJECT

public:
	UserLoginDlg(QWidget *parent = nullptr);
	~UserLoginDlg();

public slots:
	void On_Login_Clicked();
	void On_Update_Clicked();
	void On_Cancel_Clicked();
	void SlotBacktoLoginDlg();
	void SlotCreateMenu();
	void on_func1_TriggerdSlot();
	void on_func2_TriggerdSlot();
private:
	void InitWidget();
	void TipInfo(QString strContent,bool bColorRed = true);
	void TestCode();
private:
	Ui::UserLoginDlgClass ui;

	QSharedPointer<ShearDrivenMaster> m_pShearDrivenMaster;
	QSharedPointer<ModifyUserInfo> m_pModifyUserInfo;
	QToolBar* m_LoginToolBar;

	// 提示框
	InfoPromptWidget* m_pInfoPrompt;
};
