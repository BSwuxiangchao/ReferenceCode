#include "BgiEnvironmentConfig.h"
#include "BgiLogger.h"

#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <QString> 

using namespace std;

class EnvironmentConfigPrivate
{
	friend class BgiEnvironmentConfig;
public:
	EnvironmentConfigPrivate()
	{
	}

	~EnvironmentConfigPrivate()
	{
	}

	bool LoadFile(const QString& conf_filepath, ConfigParser* conf_parser)
	{
		bool b_ret = false;
		if (conf_parser)
		{
			b_ret = conf_parser->Parser(conf_filepath, m_ConfigMap);
		}
		return b_ret;
	}

public:
	typedef QMap<QString, QVariant> LookupTable;
	typedef LookupTable::const_iterator ConstIter;
	typedef LookupTable::iterator Iter;
	LookupTable m_ConfigMap;
};


//BgiEnvironmentConfig	
BgiEnvironmentConfig::BgiEnvironmentConfig()
	: m_pPrivate(new EnvironmentConfigPrivate())
{
}

BgiEnvironmentConfig::~BgiEnvironmentConfig()
{
}


//单件对象声明为函数内静态成员
BgiEnvironmentConfig* BgiEnvironmentConfig::GetInstance()
{
	static BgiEnvironmentConfig sConfigProfile;
	return &sConfigProfile;
}

void BgiEnvironmentConfig::BindEvnConfig(const QString& config_path, ConfigParser* conf_parser)
{
	m_pPrivate->LoadFile(config_path, conf_parser);

}



//获得键对应的值
QString BgiEnvironmentConfig::GetString(const QString& config_name, bool b_var/*=false*/) const
{
	auto map_iter = m_pPrivate->m_ConfigMap.find(config_name);
	QString val = config_name;
	if (map_iter != m_pPrivate->m_ConfigMap.end())
	{
		if (map_iter->isValid())
		{
			if (map_iter->canConvert(QMetaType::QString))
			{
				val = map_iter->toString();
				int variable_count = val.count('%') / 2;
				if (variable_count > 0)
				{
					QStringList val_list = val.split('%');
					val.clear();
					Q_FOREACH(auto v, val_list)
					{
						if (!v.isEmpty())
							val.append(GetString(v, true));
					}
				}
			}
			else
			{
				char c_error[128] = { 0 };
				sprintf(c_error, "BgiEnvironmentConfig::GetString can't Convert, [%s]", config_name.toStdString().c_str());
				BGI_ASSERT_X(false, c_error);
			}
		}
	}
	else
	{
		if (!b_var)
		{
			char c_error[128] = { 0 };
			sprintf(c_error, "BgiEnvironmentConfig::GetString not find, [%s]", config_name.toStdString().c_str());
			BGI_ASSERT_X(false, c_error);
		}
	}

	return val;
}


float BgiEnvironmentConfig::GetFloat(const QString& config_name) const
{
	EnvironmentConfigPrivate::ConstIter map_iter = m_pPrivate->m_ConfigMap.find(config_name);

	if (map_iter != m_pPrivate->m_ConfigMap.end())
	{
		if (map_iter->canConvert(QMetaType::Double))
		{
			bool b_ret = true;
			double var = map_iter->toDouble(&b_ret);
			if (!b_ret)
			{
				//尝试toString
				var = map_iter->toString().toDouble(&b_ret);
			}

			if (b_ret)
			{
				return var;
			}
			else
			{
				char c_error[128] = { 0 };
				sprintf(c_error, "BgiEnvironmentConfig::GetFloat Convert failed, [%s]", config_name.toStdString().c_str());
				BGI_ASSERT_X(false, c_error);
			}
		}
		else
		{
			char c_error[128] = { 0 };
			sprintf(c_error, "BgiEnvironmentConfig::GetFloat can't Convert, [%s]", config_name.toStdString().c_str());
			BGI_ASSERT_X(false, c_error);
		}
	}
	else
	{
		char c_error[128] = { 0 };
		sprintf(c_error, "BgiEnvironmentConfig::GetFloat not find, [%s]", config_name.toStdString().c_str());
		BGI_ASSERT_X(false, c_error);
	}

	return 0.0;
}

int BgiEnvironmentConfig::GetInt(const QString& config_name) const
{
	EnvironmentConfigPrivate::ConstIter map_iter = m_pPrivate->m_ConfigMap.find(config_name);

	if (map_iter != m_pPrivate->m_ConfigMap.end())
	{
		if (map_iter->canConvert(QMetaType::Int))
		{
			bool b_ret = true;
			int var = map_iter->toInt(&b_ret);
			if (!b_ret)
			{
				//尝试toString
				var = map_iter->toString().toInt(&b_ret);
			}

			if (b_ret)
			{
				return var;
			}
			else
			{
				char c_error[128] = { 0 };
				sprintf(c_error, "BgiEnvironmentConfig::GetInt Convert failed, [%s]", config_name.toStdString().c_str());
				BGI_ASSERT_X(false, c_error);
			}
		}
		else
		{
			char c_error[128] = { 0 };
			sprintf(c_error, "BgiEnvironmentConfig::GetInt can't Convert, [%s]", config_name.toStdString().c_str());
			BGI_ASSERT_X(false, c_error);
		}
	}
	else
	{
		char c_error[128] = { 0 };
		sprintf(c_error, "BgiEnvironmentConfig::GetInt not find, [%s]", config_name.toStdString().c_str());
		BGI_ASSERT_X(false, c_error);
	}

	return -1;
}
