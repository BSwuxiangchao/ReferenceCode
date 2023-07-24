#pragma once

#include <QDialog>
#include "ui_ModifyUserInfo.h"
#include"ParaType.h"

class InfoPromptWidget;

class ModifyUserInfo : public QDialog
{
	Q_OBJECT

public:
	ModifyUserInfo(QWidget *parent = nullptr);
	~ModifyUserInfo();

	//iOperaType: 1=》删除用户  2=》更新用户
	void SetUserInfo(const GeneralParaType::UserInfo userInfo,int iOperaType);

public slots:
	void On_ModifyOk_Clicked();
	void On_ModifyCancel_Clicked();
private:
	void TipInfo(QString strContent, bool bColorRed = true);

private:
	Ui::ModifyUserInfoClass ui;

	GeneralParaType::UserInfo m_userInfo;
	// 提示框
	InfoPromptWidget* m_pInfoPrompt;
	int m_iOperaType;
};
