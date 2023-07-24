#pragma once

#include <QMainWindow>
#include "ui_ImagerMgrWidget.h"
#include "ImagerParaType.h"

class ImagerDriver;
class ImagerFovWidget;
class BasecallMgrWidget;

class ImagerMgrWidget : public QMainWindow
{
	Q_OBJECT

public:
	ImagerMgrWidget(QWidget *parent = nullptr);
	~ImagerMgrWidget();

	void SetSaveFovString(QString fovString, int count);

public slots:
	void SlotImagerConfigData(const ImagerParaType::ImagerConfigPara& data);
	void SlotImagerConfigSetResult(bool bSuccess);
	void ClickBasecallSetting();

protected:
	virtual void showEvent(QShowEvent* event);

private slots:
	void ClickExposureFile();
	void ClickFovSetting();
	void ClickSaveConfig();

	void ClickGoLoadingPos();
	void ClickGoHome();
	void ClickOpenVacuum();
	void ClickCloseVacuum();
	void ClickReset();

private:
	void InitControls();
	void SetStyleSheet();
	void InitParams();
	void GetImagerConfig();

private:
	Ui::ImagerMgrWidgetClass ui;

	ImagerDriver* m_ImagerDriver;
	QSharedPointer<ImagerFovWidget> m_pImagerFovWidget;
	QSharedPointer<BasecallMgrWidget> m_pBasecallMgrWidget;

	ImagerParaType::ImagerConfigPara m_ImagerConfigData;
};
