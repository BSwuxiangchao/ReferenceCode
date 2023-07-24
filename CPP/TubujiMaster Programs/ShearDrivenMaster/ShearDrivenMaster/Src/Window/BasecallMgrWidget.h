#pragma once

#include <QMainWindow>
#include "ui_BasecallMgrWidget.h"
#include "OperateTabelWidget.h"
#include "ImagerParaType.h"
#include <QMenu>

class BasecallMgrWidget : public QMainWindow
{
	Q_OBJECT

public:
	BasecallMgrWidget(QWidget *parent = nullptr);
	~BasecallMgrWidget();

public slots:
	void SlotBasecallRunInfos(const QList<ImagerParaType::BasecallRunPara>& infos);
	void SlotImagerVersions(const QString& imagerVer, const QString& sbcVer, const QString& bcsVer);
	void SlotBasecallDiskSpace(double consumptionValue, double availableValue);
	void SlotImagerStatus(int basecall, int server, int imager);

private slots:
	void ClickRefresh();
	void ShowRightMenu(QPoint point);
	void SlotReleaseMem(bool checked);

protected:
	void resizeEvent(QResizeEvent* event);
	void showEvent(QShowEvent* event);

private:
	void InitControls();
	void SetStyleSheet();

	void SetColumnWidth();

private:
	Ui::BasecallMgrWidgetClass ui;

	QMenu* m_pRightMenu;

	//芯片内存表
	OperateTabelWidget m_MemTable;

	int m_StatusBasecall;
	int m_StatusSbc;
	int m_StatusImager;
};
