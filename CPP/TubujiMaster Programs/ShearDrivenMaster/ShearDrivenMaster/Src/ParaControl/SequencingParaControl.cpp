#include "SequencingParaControl.h"
#include "ParaControlProvider.h"

SequencingParaControl::SequencingParaControl()
	:m_tablewidget(NULL)
{
	m_tableDescList.clear();
	m_SequencingParaCtrl = ParaControlProvider::Instance()->GetParameterControl("SystemA", "General", "InputProc", "SequencingDesc");
	m_SequencingParaCtrl->AddObserver(this);
}

SequencingParaControl::~SequencingParaControl()
{
	ClearSequencingTableWidget();
	m_SequencingParaCtrl->DelObserver(this);
}

void SequencingParaControl::Update()
{
	const void* const_value = m_SequencingParaCtrl->GetValue();
	m_vecSequencingTableDesc = (QVector<GeneralParaType::SequencingTableDesc>*)const_value;
	UpdateParaToUIControl();
}

void SequencingParaControl::UpdateParaToUIControl()
{
	QVector<GeneralParaType::SequencingTableDesc>::iterator iter = m_vecSequencingTableDesc->begin();
	QVector<GeneralParaType::SequencingTableDesc>::iterator iter_end = m_vecSequencingTableDesc->end();
	for (; iter != iter_end; iter++)
	{
		for (int i = 0; i < m_tableDescList.size(); i++)
		{
			if (m_tableDescList[i].strChipNo == (*iter).strChipNo)
			{
				m_tableDescList.replace(i, (*iter));
			}
		}
	}
	UpdateSequencingTableWidget(m_tableDescList);
}

void SequencingParaControl::SetSaveLocalcfg(GeneralParaType::SequencingTableDesc tableDesc)
{
	bool bExist = false;
	for (int i=0;i<m_tableDescList.size();i++)
	{
		if (m_tableDescList[i].strChipNo == tableDesc.strChipNo)
		{
			tableDesc.iSerial = m_tableDescList[i].iSerial;
			m_tableDescList.replace(i, tableDesc);
			bExist = true;
			break;
		}
	}
	if (!bExist)
	{
		if (m_tableDescList.length() >= 1) //如果只需要支持单张芯片
			m_tableDescList.clear();

		tableDesc.iSerial = m_tableDescList.length() + 1;
		m_tableDescList.push_back(tableDesc);
	}
	UpdateSequencingTableWidget(m_tableDescList);

	//设置最新配置参数到参数系统
	QVector<GeneralParaType::SequencingTableDesc> new_desc_valve;
	if (m_vecSequencingTableDesc->size() > 0)
	{
		QVector<GeneralParaType::SequencingTableDesc>::iterator iter = m_vecSequencingTableDesc->begin();
		QVector<GeneralParaType::SequencingTableDesc>::iterator iter_end = m_vecSequencingTableDesc->end();
		for (; iter != iter_end; iter++)
		{
			GeneralParaType::SequencingTableDesc desc;
			desc = (*iter);
			if ((*iter).strChipNo == tableDesc.strChipNo)
			{
				desc = tableDesc;
			}
			new_desc_valve.push_back(desc);
		}
	}
	else
	{
		new_desc_valve.push_back(tableDesc);
	}

	m_SequencingParaCtrl->SetValue(&new_desc_valve);
}

void SequencingParaControl::SetParentWidget(QTableWidget* tablewidget)
{
	m_tablewidget = tablewidget;
	ClearSequencingTableWidget();
	Update();
}

void SequencingParaControl::ClearSequencingTableWidget()
{
	if (m_tablewidget == NULL)
		return;
	QVector<QStringList> vec;
	vec.clear();
	emit SignalUpdateSequencingTableWidget(vec);
}

void SequencingParaControl::UpdateSequencingTableWidget(QList<GeneralParaType::SequencingTableDesc> lstDesc)
{
	ClearSequencingTableWidget();
	QVector<QStringList> vecLst;
	vecLst.clear();
	int count = lstDesc.size();
	QStringList strList;
	for (int i = 0; i < count; ++i)
	{
		strList.clear();
		strList.append(QString::number(lstDesc.at(i).iSerial));
		strList.append(lstDesc.at(i).strChipNo);
		strList.append(lstDesc.at(i).strImagerNo);
		strList.append(lstDesc.at(i).strSequencingMode);
		strList.append(QString::number(lstDesc.at(i).iBiochemistryTimes));
		strList.append(QString::number(lstDesc.at(i).iPhotographTime));
		strList.append(QString::number(lstDesc.at(i).iR1));
		strList.append(QString::number(lstDesc.at(i).iR2));
		strList.append(QString::number(lstDesc.at(i).iB1));
		strList.append(QString::number(lstDesc.at(i).iB2));
		strList.append(QString::number(lstDesc.at(i).iAll));

		vecLst.push_back(strList);
	}
	emit SignalUpdateSequencingTableWidget(vecLst);
}