/*
* @Abrief 配置文件XML
* @note 解释XML文件
* @version 1.1
* @author wuxiangchao 2023-03-15
*/

#ifndef _BGI_XML_CONFIG_PARSER_H
#define _BGI_XML_CONFIG_PARSERF_H

#include "ConfigParser.h"
#include "ExternDef.h"
#include <QDomDocument>

//错误码
struct BGIBASE_CLASS_EXPORT ErrDesc
{
    QString ErrCode;
    QString Description;
    QString Solution;
    ErrDesc()
    {
        ErrCode = "";
        Description = "";
        Solution = "";
    }

};

class BGIBASE_CLASS_EXPORT XmlConfigParser : public ConfigParser
{
public:
	virtual bool Parser(const QString& conf_filepath, QMap<QString, QVariant >& config_map);
    bool ReadErrorCodeXML(const QString filePath, QMap<QString, QVector<ErrDesc>>& mapElemnt, QString& strReadResult);
};


#endif