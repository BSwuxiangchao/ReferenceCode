#pragma once

#include <QMainWindow>
#include "ui_LocalDataWidgeet.h"
#include "ParaType.h"
#include "QToolBar.h"

class LocalDataWidgeet : public QMainWindow
{
	Q_OBJECT

public:
	LocalDataWidgeet(QWidget *parent = nullptr);
	~LocalDataWidgeet();
	void SetSaveBtnState(bool bShow);

signals:
	void SignalSaveLocalcfg(GeneralParaType::SequencingTableDesc strTableDesc);
	void SignalUpdatePosCombox();

public slots:
	void SlotUpdateCycle(GeneralParaType::SequencingTableDesc tableDesc);

protected slots:
	void On_Create_Clicked();
	void On_Delete_Clicked();
	void On_SearchFor_Clicked();
	void On_Barcode_Changed();
	void On_BatchID_Changed();
	void On_SaveCfg_Clicked();
	void On_ScanCode_Clicked();
private:
	void LimitInputUnit(QString strPatern,QLineEdit *lineEdit= NULL,QComboBox *comboBox= NULL);
	void LimitLineEditInput();
	void MsgBoxTool(QString strContext, int iType);
	void ChangeTotalLength();
	void ReloadBarcodeCtrDetail();
	void LoadSqlCfg();
	void on_LocalDataWidgeet_customContextMenuRequested();
	void CreateToolBar();
	void SetStyleSheet();
	void InitWidgets();
	void ClearUIControlText();
	void SetUIControlText(GeneralParaType::SqlCfgData sqlCfgData);
	void GetUIControlText(GeneralParaType::SqlCfgData & data);
	//判断芯片编号在数据库中是否已经存在
	bool IsBarcodeCfgExist();
	// 判断条码和batchID 是否合法
	bool IsBarcodeAndBatchIDIllegal();
	void TestCode();
private:
	Ui::LocalDataWidgeetClass ui;
	QList<GeneralParaType::SqlCfgData> m_vecSqlCfgData;
	bool m_bNewCreate;
	// On_Barcode_Changed变更batchID后，下一次点击batchID Combox控件，即使不切换batchID也会触发一次Combox控件事件
	QString m_strLastBatchID;
	bool m_bAutoChangeBatchID;
	bool m_bShowAllBatchID;
	bool m_bAlreadyLoadAllCfg;
	QToolBar* m_mainToolBar;
	bool m_bScanCode;
};
