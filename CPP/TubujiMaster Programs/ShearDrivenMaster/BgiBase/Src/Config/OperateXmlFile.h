/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 创建时间：2022/6/8 16:25
 * 功能描述：操作xml文件的读写；
 ************************************* end-XCW *****************************************/

#ifndef OPERATEXMLFILE_H
#define OPERATEXMLFILE_H
#include <QString>
#include <QDomDocument>
#include <QMap>
#include <QVector>
#include "ExternDef.h"

// 标签结构体
typedef struct BGIBASE_CLASS_EXPORT _tagXmlElement
{
    _tagXmlElement()
    {
        m_strLabelName = "";
        m_strLabelContent = "";
        m_strAttributesName = "";
        m_strAttributeContent = "";
    }

    _tagXmlElement& operator=(const _tagXmlElement& obj)
    {
        if(this != &obj)
        {
            m_strLabelName = obj.m_strLabelName;
            m_strLabelContent = obj.m_strLabelContent;
            m_strAttributesName = obj.m_strAttributesName;
            m_strAttributeContent =obj.m_strAttributeContent ;
        }
        return *this;
    }

    bool operator==(const _tagXmlElement& obj)
    {
        if(m_strLabelName == obj.m_strLabelName && m_strLabelContent == obj.m_strLabelContent &&
                m_strAttributesName == obj.m_strAttributesName && m_strAttributeContent == obj.m_strAttributeContent)
        {
            return true;
        }
        return false;
    }

    // 标签名称
    QString m_strLabelName;
    // 标签内容
    QString m_strLabelContent;
    // 属性名称
    QString m_strAttributesName;
    // 属性内容
    QString m_strAttributeContent;
}_tagXmlElement;


class BGIBASE_CLASS_EXPORT OperateXmlFile
{
public:
    OperateXmlFile();

    // 维持一个数组记录该xml文件所有内容，增删改查都操作这个数据，然后重新写入文件即可实现

    //创建xml空文件
    static void CreateXML(const QString& filePath, const QString& strRootName);

    //追加节点,strArrName:组名称，element：组下面的节点
    static void AppendXML(const QString& filePath, const QString& strArrName, const _tagXmlElement& element);

    // 写入数据-删除掉原有的数据重新写入
    static bool WriteAllXML(const QString filePath, const QMap<QString, QVector<_tagXmlElement>> mapElemnt);

    //读数据
    static bool ReadXML(const QString filePath, QMap<QString, QVector<_tagXmlElement>>& mapElemnt, QString& strReadResult);

    // 删除节点
    static void DeleteXML(const QString& filePath, const QString& strArrName, const _tagXmlElement& element);

private:
    //写有效数据
    static void WriteXML(QDomDocument& doc, QDomElement& root, const _tagXmlElement& element);

};

#endif // OPERATEXMLFILE_H
