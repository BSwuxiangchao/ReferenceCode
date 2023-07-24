/*
* @Abrief 数据监控
* @note 监控生化信息及参数配置信息
* @version 1.0
* @author wuxiangchao 2023-2-8
*/
#pragma once
#include "ParaType.h"
#include <qlist.h>
#include <qtablewidget.h>
#include "ParaControl.h"
#include "ParaObserver.h"

class SequencingParaControl : public QObject, public ParaObserver
{
	Q_OBJECT
public:
	SequencingParaControl();
	~SequencingParaControl();
	virtual void Update();

	void SetSaveLocalcfg(GeneralParaType::SequencingTableDesc tableDesc);
	void SetParentWidget(QTableWidget* tablewidget);

signals:
	void SignalUpdateSequencingTableWidget(QVector<QStringList> vec);
private:
	void UpdateParaToUIControl();
	void ClearSequencingTableWidget();
	void UpdateSequencingTableWidget(QList<GeneralParaType::SequencingTableDesc> lstDesc);

private:
	QList<GeneralParaType::SequencingTableDesc> m_tableDescList;
	QTableWidget* m_tablewidget;
	ParaControl* m_SequencingParaCtrl;
	QVector<GeneralParaType::SequencingTableDesc>* m_vecSequencingTableDesc;
};

