#pragma once

#include <QMainWindow>
#include "ui_CoaterDataWidget.h"
#include "CoaterDriver.h"

class CoaterDataWidget : public QMainWindow
{
	Q_OBJECT

public:
	CoaterDataWidget(QWidget *parent = nullptr);
	~CoaterDataWidget();

public slots:
	//涂布机的连接状态
	void SlotNewData(PLCDataType type, const QVector<int>& data);

protected:
	virtual void showEvent(QShowEvent* event);
	virtual void closeEvent(QCloseEvent* event);

private:
	void InitControls();

private:
	Ui::CoaterDataWidgetClass ui;
	
};
