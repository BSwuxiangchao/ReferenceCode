#pragma once

#include <QWidget>
#include "ui_MessageWidget.h"
#include "OperateRecordInfo.h"

class ProcessOperateRecordInfoThread;

class MessageWidget : public QWidget
{
	Q_OBJECT

public:
	MessageWidget(QWidget *parent = nullptr);
	~MessageWidget();

public slots:
	void SlotOpeRecordInfo(QString str, QColor col);

private:
	// 设置插入字体颜色
	void SetInsertTextColor(const QColor& col, QPlainTextEdit& plainTextEdit);
	void AddRecordToPlainTextEditInfo(QPlainTextEdit& plainTextEdit, const QString& strInfo, const QColor& col);


private:
	Ui::MessageWidgetClass ui;

	// 记录数据线程
	ProcessOperateRecordInfoThread* m_pProcessOpeRecordThread;
};
