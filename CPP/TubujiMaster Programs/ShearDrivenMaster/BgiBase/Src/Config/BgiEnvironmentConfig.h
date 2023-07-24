/*
* @Abrief 环境配置类
* @note 配置资源的路径，属性值等
* @version 1.0
* @author mosongjin 2021-07-21
*/

#ifndef _BGI_ENVIRONMENT_CONFIG_H
#define _BGI_ENVIRONMENT_CONFIG_H



#include <QString>
#include <QVariant>
#include <QScopedPointer> 
#include "ExternDef.h"
#include "ConfigParser.h"


class EnvironmentConfigPrivate;
class BGIBASE_CLASS_EXPORT BgiEnvironmentConfig
{
public:
    virtual ~BgiEnvironmentConfig();
    static BgiEnvironmentConfig* GetInstance();

    QString GetString(const QString& config_name, bool b_var = false) const;
    float GetFloat(const QString& config_name) const;
    int GetInt(const QString& config_name) const;

    void BindEvnConfig(const QString& config_path, ConfigParser* conf_parser);

private:
    BgiEnvironmentConfig();

    QScopedPointer<EnvironmentConfigPrivate> m_pPrivate;
};


#endif