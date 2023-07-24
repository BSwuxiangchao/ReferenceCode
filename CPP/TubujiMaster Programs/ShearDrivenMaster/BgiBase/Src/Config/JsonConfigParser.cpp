#include "JsonConfigParser.h"
#include "BgiLogger.h"

#include <QFile>
#include <QString> 
#include <QtGlobal>
#include "qjsondocument.h"
#include "qdir.h"
#include "qsettings.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qjsonvalue.h"
#include "qbytearray.h"


bool JsonConfigParser::Parser(const QString& conf_filepath, QMap<QString, QVariant >& config_map)
{
	bool b_ret = false;
	//.json
	QFile* file = new QFile(conf_filepath);
	if (file->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QString str_json_context = file->readAll();
		file->close();

		QJsonParseError jsonError;
		QJsonDocument jsonDom = QJsonDocument::fromJson(str_json_context.toUtf8(), &jsonError);
		// 解析未发生错误
		if (!jsonDom.isNull() && (jsonError.error == QJsonParseError::NoError))
		{
			QJsonObject jsonObject = jsonDom.object();
			ParserJosn(jsonObject, config_map);
			b_ret = true;
		}
		else
		{
			BGI_ASSERT_X(false, "JsonConfigParser::Parser failed, Json format error, please check file env_config.json");
			qFatal("JsonConfigParser::Parser failed, Json format error, please check file env_config.json");
		}
	}
	delete file;
	file = NULL;

	return b_ret;
}


void JsonConfigParser::ParserJosn(QJsonObject json_object, QMap<QString, QVariant >& config_map)
{
	QStringList keys_list = json_object.keys();

	for (auto key : keys_list)
	{
		QJsonValue json_value = json_object[key];
		if (json_value.isObject())
		{
			ParserJosn(json_value.toObject(), config_map);
		}
		else
		{
			BGI_ASSERT(!config_map.contains(key));
			config_map.insert(key, QVariant(json_value));
		}
	}
}
