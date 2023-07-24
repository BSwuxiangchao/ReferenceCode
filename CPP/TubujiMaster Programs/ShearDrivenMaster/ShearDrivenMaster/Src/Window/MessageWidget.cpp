#include "MessageWidget.h"
#include <qscrollbar.h>
#include "ProcessOperateRecordInfoThread.h"

MessageWidget::MessageWidget(QWidget *parent)
	: QWidget(parent)
	, m_pProcessOpeRecordThread(NULL)
{
	ui.setupUi(this);

	//this->show();

	// 记录数据线程
	m_pProcessOpeRecordThread = new ProcessOperateRecordInfoThread();
	connect(m_pProcessOpeRecordThread, &ProcessOperateRecordInfoThread::SendRecordData, this, &MessageWidget::SlotOpeRecordInfo);
	m_pProcessOpeRecordThread->StartThread();
}

MessageWidget::~MessageWidget()
{
	if (NULL != m_pProcessOpeRecordThread)
	{
		m_pProcessOpeRecordThread->StopThread();
		delete m_pProcessOpeRecordThread;
		m_pProcessOpeRecordThread = NULL;
	}
}

// 设置插入字体颜色
void MessageWidget::SetInsertTextColor(const QColor& col, QPlainTextEdit& plainTextEdit)
{
	QTextCharFormat fmt;
	fmt.setForeground(col);
	QTextCursor cursor = plainTextEdit.textCursor();
	cursor.mergeCharFormat(fmt);
	plainTextEdit.mergeCurrentCharFormat(fmt);
}

void MessageWidget::AddRecordToPlainTextEditInfo(QPlainTextEdit& plainTextEdit, const QString& strInfo, const QColor& col)
{
	// 设置文本颜色
	SetInsertTextColor(col, plainTextEdit);
	plainTextEdit.appendPlainText(strInfo);
	// 移动滚动条到底部
	QScrollBar* scrollbar = plainTextEdit.verticalScrollBar();
	if (scrollbar)
	{
		scrollbar->setSliderPosition(scrollbar->maximum());
	}
}

// 接收操作记录信息
void MessageWidget::SlotOpeRecordInfo(QString str, QColor col)
{
	AddRecordToPlainTextEditInfo(*ui.plainTextEdit_DisplayOpe, str, col);
}