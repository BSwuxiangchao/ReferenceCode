#pragma once
/*
* @Abrief 自定义输入框
* @note 自定义输入
* @version 1.0
* @author wuxiangchao 2023-7-17
*/

#include <QDialog>
#include "ui_BGIIputDialog.h"

typedef enum _tagINPUTBTN_STATE
{
	IN_NO_BUTTON = 0,
	IN_SURE = 1,
	IN_CANCEL = 2
}INPUTBTN_STATE;


class BGIIputDialog : public QDialog
{
	Q_OBJECT

public:
	BGIIputDialog(QWidget *parent = nullptr);
	~BGIIputDialog();

	QString getText(const QString& title, const QString& label, bool* ok = nullptr);
/******************************************************************************
 * 功能描述：获取Int数据
 * 时    间：2023/7/17
 * 入    参：title：对话框标题
 * 入    参：label：设置提示内容
 * 入    参：value：输入默认值 
 * 入    参：iButton：设置按钮的显示,1-只显示确定按钮，2-只显示取消，3-显示确定和取消按钮，4-都不显示确定和取消，其他值默认只显示确认按钮
 * 返    回：int: 按钮类型；
 ******************************************************************************/
	int GetInt(const QString& title, const QString& label, bool* ok = nullptr,int value = 0, int minValue = -2147483647, int maxValue = 2147483647);
	double GetDouble(const QString& title, const QString& label, bool* ok = nullptr,double step=1,double value = 0, double minValue = 0, double maxValue =0);

private:
	void SetInputWindowTitle(const QString& title);
	void ShowDlg(const QString& title, const QString& label, int iType ,int value=0, int minValue=0, int maxValue=0);

private slots:
	void on_pushButtonSure_clicked();

	void on_pushButtonCancel_clicked();

	void closeEvent(QCloseEvent* event);

private:
	Ui::BGIIputDialogClass ui;
	// 按钮状态
	INPUTBTN_STATE m_iButtonState;

	QString m_strCurText;;
};
