#pragma once

#include <QMainWindow>
#include "ui_ImagerFovWidget.h"
#include <QMenu>

class ImagerMgrWidget;

class ImagerFovWidget : public QMainWindow
{
	Q_OBJECT

public:
	ImagerFovWidget(ImagerMgrWidget* parent);
	~ImagerFovWidget();

	int SetSaveFovString(QString fovString);

protected:
	virtual void showEvent(QShowEvent* event);

private slots:
	void ClickAddFovs();
	void ClickDelFovs();
	void ClickDelAllFovs();
	void ClickSaveFovs();
	void ShowRightMenu(QPoint point);
	void SlotAddFov(bool checked);
	void SlotDelFov(bool checked);

private:
	void InitControls();
	void SetStyleSheet();

	int ReplaceFovs(const QList<QPoint>& points);
	void AddFovs(const QList<QPoint>& points);
	void DelFovs(const QList<QPoint>& points);
	void ClearFovs();

	QList<QPoint> GetFovPoints();

	void RefreshFovCount();
	void RefreshFovCount(int count);

	void ClearRowColText();

private:
	Ui::ImagerFovWidgetClass ui;
	ImagerMgrWidget* m_pImagerMgrWidget;
	QSharedPointer<QMenu> m_pRightMenu;

	QBrush m_SaveColor;
	QBrush m_NoSaveColor;
};
