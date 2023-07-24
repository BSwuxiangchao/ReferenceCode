/*
* @Abrief 错误码转换文字描述
* @note 从配置文件获取对应错误码所代表的含义
* @version 1.0
* @author wuxiangchao 2023-03-15
*/

#ifndef _ERRORCODE_TRANSLATE_DOC_H
#define _ERRORCODE_TRANSLATE_DOC_H

#include <qstring.h>
#include <qmap.h>
#include "BusinessExternDef.h"
#include "XmlConfigParser.h"
#include "CommonColors.h"
#include "ParaType.h"

const QString gc_str_General_groupName = "General";
const QString gc_str_Robot_groupName = "Robot";
const QString gc_str_Coater_groupName = "Coater";
const QString gc_str_Imager_groupName = "Imager";

class BGIBUSINE_CLASS_EXPORT ErrorCodeTranslate
{
public:	
	static ErrorCodeTranslate* Instance();
	~ErrorCodeTranslate();

	//iType: 0=>general  1=>robot  2=>coater 3=>imager
	QString GetErrDescription(QString strErrCode,int iType);
	
	bool GetInfoFromErrCode(QString strErrCode, InfoType iType, ErrDesc& strErrDesc);
	void ShowErrinfoDetail(QString strErrCode, InfoType iType);
	void RecordInfo(QString strInfoType, QString strContent, QColor color = gc_colorBlack,bool bWrite = true);
	bool FalseWithTip(QString strFuncName);
	void PromptInfo(QString strInfoType, QString strContent, QColor color = gc_colorBlack);


private:
	ErrorCodeTranslate();

	void LoadErrorMapUnit(QString strPath,QString strGroupName, QMap<QString, QString>& map);
	void LoadAllErrMap(QString strPath);

private:
	QMap<QString, QString> m_GeneralErrMap;
	QMap<QString, QString> m_RobotErrMap;
	QMap<QString, QString> m_CoaterErrMap;
	QMap<QString, QString> m_ImagerErrMap;
	bool m_bReadSuc;

    QMap <QString, QVector<ErrDesc>> m_xmlErrorMap;

};




#endif



