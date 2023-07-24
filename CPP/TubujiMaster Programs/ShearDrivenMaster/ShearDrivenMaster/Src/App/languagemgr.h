/*
* @Abrief
* @note 多语言控制类
* @version 1.0
* @author wuxiangchao 2023-03-06
*/

#ifndef CLANGUAGEMGR_H
#define CLANGUAGEMGR_H

#include <QString>
#include <QMap>
#include "qpushbutton.h"
#include <qlabel.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include "MasterDefine.h"

class CLanguageMgr
{
public:
    static CLanguageMgr* Instance();
    void SetLastLanguage(QString language);
    QString GetLastLanguage();
    void SetCurLanguage(QString language);
    QString GetCurLanguage();
    
    void ChangeUIControlTextUnit(ChangeType changeType,QPushButton* button, QLabel* Label=NULL,QGroupBox *groupBox = NULL,
        QComboBox *comboBox=NULL);

private:
    CLanguageMgr();
    // 初始化，加载配置文件
    void LoadLanguageUnit(QString strLanguage);
    QString Translate(const QString& strKey);

    QString IndirectTranslate(const QString& strKey);

    // 逆翻译，由value找key
    QString ITranslate(const QString& strValue);

private:
    QString m_LastTranslateLanguage;   //上一次语言
    QString m_CurTranslateLanguage;     //当前要设置的语言

    QMap<QString, QString> m_mapEnglish;    //中英对照表
    QMap<QString, QString> m_mapFrench;    //中法对照表

};

#endif // CLANGUAGEMGR_H
