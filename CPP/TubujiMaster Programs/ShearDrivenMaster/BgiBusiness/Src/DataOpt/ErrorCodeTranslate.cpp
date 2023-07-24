#include "ErrorCodeTranslate.h"
#include <qfile.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qdebug.h>
#include <qsettings.h>
#include <qfileinfo.h>
#include "BgiLogger.h"
#include <qcoreapplication.h>
#include "BgiFileTools.h"
#include "OperatePromptInfo.h"
#include "OperateRecordInfo.h"
#include "TransformCoding.h"


ErrorCodeTranslate* ErrorCodeTranslate::Instance()
{
	static ErrorCodeTranslate instance;
	return &instance;
}

ErrorCodeTranslate::ErrorCodeTranslate()
	:m_bReadSuc(false)
{
	QString path;
	BgiFileTools::GetDataDirPath(path);

	path += "//Data//Input//ErrorCode.xml";
	//LoadAllErrMap(path);
    QString strReadResult;
	XmlConfigParser xmlConfigParser;

	xmlConfigParser.ReadErrorCodeXML(path, m_xmlErrorMap, strReadResult);
}

void ErrorCodeTranslate::LoadErrorMapUnit(QString strPath, QString strGroupName, QMap<QString, QString>& map)
{
	QFileInfo fileinfo;
	if (!fileinfo.exists(strPath))
	{
		BGI_WARNING("Load Errorcode failed !!!");
		return;
	}
	QSettings qSettings(strPath,QSettings::IniFormat);
	qSettings.beginGroup(strGroupName);
	QStringList keys = qSettings.childKeys();
	for (QString key : keys)
	{
		QString value = qSettings.value(key).toString();
		map.insert(key, value);
	}
	qSettings.endGroup();
}

ErrorCodeTranslate::~ErrorCodeTranslate()
{
}

void ErrorCodeTranslate::LoadAllErrMap(QString strPath)
{
	LoadErrorMapUnit(strPath, gc_str_General_groupName, m_GeneralErrMap);
	LoadErrorMapUnit(strPath, gc_str_Robot_groupName, m_RobotErrMap);
	LoadErrorMapUnit(strPath, gc_str_Coater_groupName, m_CoaterErrMap);
	LoadErrorMapUnit(strPath, gc_str_Imager_groupName, m_ImagerErrMap);
}

//iType: 0=>general  1=>robot  2=>coater 3=>imager
QString ErrorCodeTranslate::GetErrDescription(QString strErrCode, int iType)
{
	if (iType == 0)
	{
		QMap<QString, QString>::iterator iter = m_GeneralErrMap.find(strErrCode);
		if (iter != m_GeneralErrMap.end())
			return iter.value();
	}
	else if (iType == 1)
	{
		QMap<QString, QString>::iterator iter = m_RobotErrMap.find(strErrCode);
		if (iter != m_RobotErrMap.end())
			return iter.value();
	}
	else if (iType == 2)
	{
		QMap<QString, QString>::iterator iter = m_CoaterErrMap.find(strErrCode);
		if (iter != m_CoaterErrMap.end())
			return iter.value();
	}
	else if (iType == 3)
	{
		QMap<QString, QString>::iterator iter = m_ImagerErrMap.find(strErrCode);
		if (iter != m_ImagerErrMap.end())
			return iter.value();
	}
	return strErrCode;
}

bool ErrorCodeTranslate::GetInfoFromErrCode(QString strErrCode, InfoType iType, ErrDesc& strErrDesc)
{
	bool bFind = false;
	QString strErrType = "";
	if (GeneralType == iType)
		strErrType = gc_str_General_groupName;
	else if (RobotType == iType)
		strErrType = gc_str_Robot_groupName;
	else if (CoaterType == iType)
		strErrType = gc_str_Coater_groupName;
	else if (ImagerType == iType)
		strErrType = gc_str_Imager_groupName;
	QMap <QString, QVector<ErrDesc>>::iterator iter = m_xmlErrorMap.find(strErrType);
	if (iter != m_xmlErrorMap.end())
	{
		QVector<ErrDesc>::iterator sub_iter = iter.value().begin();
		for (; sub_iter != iter.value().end(); sub_iter++)
		{
			if (0 == sub_iter->ErrCode.compare(strErrCode, Qt::CaseInsensitive))
			{
				strErrDesc = (*sub_iter);
				bFind = true;
			}
		}
	}
	return bFind;
}

void ErrorCodeTranslate::ShowErrinfoDetail(QString strErrCode, InfoType iType)
{
	ErrDesc strErrDesc;
	_tagPromptData promptData;
	promptData.m_bModel = true;
	promptData.m_colForeground = gc_colorRed;
	promptData.m_strTitle = "异常提示";

	QString strContent = QString("错误码：%1").arg(strErrCode);
	if (GetInfoFromErrCode(strErrCode, iType, strErrDesc))
	{
		strContent.append(QString("\n问题描述：%1").arg(strErrDesc.Description));
		strContent.append(QString("\n解决方案：%1").arg(strErrDesc.Solution));
	}
	
	promptData.m_strContent = strContent;
	promptData.m_iBtnType = 1; // 按钮
	OperatePromptInfo::AddPromptInfo(promptData);
}

void ErrorCodeTranslate::RecordInfo(QString strInfoType, QString strContent, QColor color, bool bWrite)
{
	RecordData strData;
	strData.m_strInfoType = strInfoType;
	strData.m_strContent = strContent;
	strData.m_color = color;
	strData.m_bWrite = bWrite;
	OperateRecordInfo::AddRecordData(strData);
}

bool ErrorCodeTranslate::FalseWithTip(QString strFuncName)
{
	RecordInfo("函数调用", QString("%1 调用失败").arg(strFuncName), gc_colorYellow,true);
	return false;
}

void ErrorCodeTranslate::PromptInfo(QString strInfoType, QString strContent, QColor color)
{
	_tagPromptData promptData;
	promptData.m_bModel = true;
	promptData.m_colForeground = gc_colorRed;
	promptData.m_strTitle = strInfoType;
	promptData.m_strContent = strContent;
	promptData.m_iBtnType = 1; // 按钮
	OperatePromptInfo::AddPromptInfo(promptData);
}

