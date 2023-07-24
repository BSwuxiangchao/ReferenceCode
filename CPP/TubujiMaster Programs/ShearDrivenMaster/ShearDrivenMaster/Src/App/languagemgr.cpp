#include "languagemgr.h"
#include <QFile>
#include <QCoreApplication>
#include <QSettings>
#include <QtCore5Compat/qtextcodec.h>
#include "BgiLogger.h"
#include "BgiEnvironmentConfig.h"
#include "MasterDefine.h"
#include "BgiFileTools.h"

CLanguageMgr::CLanguageMgr()
    :m_LastTranslateLanguage(gc_Chinese)
    ,m_CurTranslateLanguage(gc_Chinese)
{
    m_mapEnglish.clear();
    LoadLanguageUnit(gc_English);
    LoadLanguageUnit(gc_French);
}

CLanguageMgr* CLanguageMgr::Instance()
{
    static CLanguageMgr LanguageMgr;
    return &LanguageMgr;
}

void CLanguageMgr::SetLastLanguage(QString language)
{
    m_LastTranslateLanguage = language;
}

QString CLanguageMgr::GetLastLanguage()
{
    return m_LastTranslateLanguage;
}

void CLanguageMgr::SetCurLanguage(QString language)
{
    m_CurTranslateLanguage = language;
}

QString CLanguageMgr::GetCurLanguage()
{
    return m_CurTranslateLanguage;
}

void CLanguageMgr::LoadLanguageUnit(QString strLanguage)
{
    QString path = "";
    BgiFileTools::GetDataDirPath(path);
    QString strLangFile = path+ "//Data//Input//language.ini";
    QFile file(strLangFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString strlog = QString("CLanguageMgr::Init(), open File %1 Failed, %2").arg(strLangFile, file.errorString());
        BGI_MESSAGE(strlog.toStdString().c_str());
        return;
    }

    QSettings settings(strLangFile, QSettings::IniFormat);
    //settings.setIniCodec(QTextCodec::codecForName("UTF_8"));

    int nReadLines = 0;
    QString strValue;

    // 语言的关键字是否已经被读取到
    bool  bLanguageReaded = false;

    bool bLoadEnglish = false;
    bool bLoadFrench = false;
    // 根据语言分组计算组内Key的数目
    if (0 == strLanguage.compare(gc_English, Qt::CaseInsensitive))
    {
        settings.beginGroup(gc_English);
        bLoadEnglish = true;
    }
    else if (0 == strLanguage.compare(gc_French, Qt::CaseInsensitive))
    {
        settings.beginGroup(gc_French);
        bLoadFrench = true;
    }

    QStringList keys = settings.childKeys();
    int nChineseKeys = keys.size();
    /*BGI_MESSAGE(QString("language %1 keysize:%2 keys:").arg(strLanguage).arg(nChineseKeys).toStdString().c_str());
    for (QString key : keys)
        BGI_MESSAGE(QString("%1").arg(key).toLatin1().toStdString().c_str());*/
    settings.endGroup();

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        // 中英
        if (0 == line.compare(QString("[%1]").arg(strLanguage), Qt::CaseInsensitive))
        {
            bLanguageReaded = true;
            continue;
        }
        if (line.indexOf("//") == 0)
            continue;
        if (line != "" && nReadLines <= nChineseKeys && bLanguageReaded)
        {
            // 获取"="所在的位置
            int nFlag = line.lastIndexOf("=");
            // 获取"="左边的关键字
            strValue = line.left(nFlag).trimmed();
            // 获取"="右边的键值
            line = line.mid(nFlag + 1).trimmed();
            if(bLoadEnglish)
                m_mapEnglish[strValue] = line;
            else if(bLoadFrench)
                m_mapFrench[strValue] = line;

            if (nReadLines == nChineseKeys)
            {
                bLanguageReaded = false;
                nReadLines = 0;
                continue;
            }
            else
            {
                nReadLines++;
            }
        }

    }

    file.close();
}

QString CLanguageMgr::Translate(const QString& strKey)
{
    QString strLang = GetCurLanguage();
    if (0 == strLang.compare(gc_English, Qt::CaseInsensitive))
    {
        if (m_mapEnglish.contains(strKey))
            return m_mapEnglish[strKey];
        else
            return strKey;
    }
    else if (0 == strLang.compare(gc_French, Qt::CaseInsensitive))
    {
        if (m_mapFrench.contains(strKey))
            return m_mapFrench[strKey];
        else
            return strKey;
    }
    return strKey;
}

QString CLanguageMgr::IndirectTranslate(const QString& strValue)
{
    QString strLastLang = GetLastLanguage();
    QString strCurLang = GetCurLanguage();
    QString tempKey = "";
    QString tempValue = "";
    if (0 == strLastLang.compare(gc_English, Qt::CaseInsensitive))
    {
        QList<QString> listValue = m_mapEnglish.values();
        if (listValue.contains(strValue))
            tempKey =  m_mapEnglish.key(strValue);
    }
    else if (0 == strLastLang.compare(gc_French, Qt::CaseInsensitive)) 
    {
        QList<QString> listValue = m_mapFrench.values();
        if (listValue.contains(strValue))
            tempKey = m_mapFrench.key(strValue);
    }
    if (tempKey == "")
        return strValue;
    if (0 == strCurLang.compare(gc_English, Qt::CaseInsensitive))
    {
        if (m_mapEnglish.contains(tempKey))
            return m_mapEnglish[tempKey];
    }
    if (0 == strCurLang.compare(gc_French, Qt::CaseInsensitive))
    {
        if (m_mapFrench.contains(tempKey))
            return m_mapFrench[tempKey];
    }
    return strValue;
}

QString CLanguageMgr::ITranslate(const QString& strValue)
{
    QString strLang = GetCurLanguage();
    if (0 == strLang.compare(gc_English, Qt::CaseInsensitive))
    {
        QList<QString> listValue = m_mapEnglish.values();
        if (listValue.contains(strValue))
            return m_mapEnglish.key(strValue);
        else
            return strValue;
    }
    else if (0 == strLang.compare(gc_French, Qt::CaseInsensitive))
    {
        QList<QString> listValue = m_mapFrench.values();
        if (listValue.contains(strValue))
            return m_mapFrench.key(strValue);
        else
            return strValue;
    }
    return strValue;
}

void CLanguageMgr::ChangeUIControlTextUnit(ChangeType changeType, QPushButton* button, QLabel* Label, QGroupBox* groupBox,
    QComboBox* comboBox)
{
    QString strText = "";
    if (button != NULL)
    {
        if (changeType == toChinese)
            strText = ITranslate(button->text());
        else if (changeType == DtoForeign)
            strText = Translate(button->text());
        else
            strText = IndirectTranslate(button->text());
        button->setText(strText);
    }
    else if(Label != NULL)
    {
        if (changeType == toChinese)
            strText = ITranslate(Label->text());
        else if (changeType == DtoForeign)
            strText = Translate(Label->text());
        else
            strText = IndirectTranslate(Label->text());
        Label->setText(strText);
    }
    else if(groupBox != NULL)
    {
        if (changeType == toChinese)
            strText = ITranslate(groupBox->title());
        else if (changeType == DtoForeign)
            strText = Translate(groupBox->title());
        else
            strText = IndirectTranslate(groupBox->title());
        groupBox->setTitle(strText);
    }
    else if (comboBox != NULL)
    {
        for (int idx = 0; idx < comboBox->count(); idx++)
        {
            if (changeType == toChinese)
                strText = ITranslate(comboBox->itemText(idx));
            else if (changeType == DtoForeign)
                strText = Translate(comboBox->itemText(idx));
            else
                strText = IndirectTranslate(comboBox->itemText(idx));
            comboBox->setItemText(idx,strText);
        }
    }
}