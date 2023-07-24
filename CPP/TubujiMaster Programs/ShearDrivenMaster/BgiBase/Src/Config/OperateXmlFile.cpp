#include "OperateXmlFile.h"
#include <QDomDocument>                 //文件
#include <QDomProcessingInstruction>    //格式头部
#include <QDomElement>                  //元素
#include <QFile>
#include <QDebug>
#include <QDateTime>

#define cout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

OperateXmlFile::OperateXmlFile()
{

}


//创建xml空文件
void OperateXmlFile::CreateXML(const QString& filePath, const QString& strRootName)
{
    // 关联文件名字
    QFile file(filePath);
    // 判断文件是否存在
    if(file.exists())
    {
        // 存在
        return;
    }
    else
    {
        // 不存在
        // 只写方式打开文件
        bool bIsOpen = file.open(QIODevice::WriteOnly);
        if(bIsOpen)
        {
            // 创建xml文档对象
            QDomDocument doc;
            // 创建xml头部格式 <?xml version='1.0' encoding='UTF-8'?>
            QDomProcessingInstruction ins;
            ins = doc.createProcessingInstruction("xml", "version=\'1.0\' encoding=\'UTF-8\'");
            // 追加元素
            doc.appendChild(ins);

            // 根节点元素
            QDomElement root = doc.createElement(strRootName);
            doc.appendChild(root);

            // 保存
            QTextStream stream(&file);  // 文件流关联
            doc.save(stream, 4);  // 4 表示缩进的字符数
            // 关闭文件
            file.close();
        }
        else
        {
            cout << "WriteOnly error";
            return;
        }
    }
}

//追加节点,strArrName:组名称，element：组下面的节点
void OperateXmlFile::AppendXML(const QString& filePath, const QString& strArrName, const _tagXmlElement& element)
{
    // 追加内容的大体步骤
    // 1. 先读取出xml文件的所有内容
    // 2. 先读取出根节点
    // 3. 判断根节点有无子节点
    // 4. 没有子节点创建，创建子节点
    // 5. 有子节点则判断子节点strArrName是否存在
    // 6. 存在就当前子节点下

    QFile file(filePath);

    // 只读方式打开
    bool bIsOpen = file.open(QIODevice::ReadOnly);

    if(bIsOpen)
    {
        // file和xml文档对象关联
        QDomDocument doc;
        bIsOpen = doc.setContent(&file);
        if(bIsOpen)
        {
            // 关联成功
            file.close();  // 关闭文件
            // 读取根节点元素
            QDomElement root = doc.documentElement();
            // 判断根节点下有没有节点
            if(root.hasChildNodes())
            {
                // 有子节点

                // 遍历判断是否有需要添加标签的组名节点
                root.firstChildElement();
                root.nextSiblingElement();
                QDomNodeList domNodeList = root.childNodes();
                QDomElement domElementFind;
                bool bFind = false;
                for(int i = 0; i < domNodeList.size(); ++i)
                {
                    if(domNodeList.at(i).toElement().tagName() == strArrName)
                    {
                        domElementFind = domNodeList.at(i).toElement();
                        bFind = true;
                        break;
                    }

                }
                if(!bFind)
                {
                    // 未有改组名节点，创建之
                    QDomElement subElement = doc.createElement(strArrName);
                    // 把该节点追加到根节点下
                    root.appendChild(subElement);
                    domElementFind = subElement;
                }

                // 写入有效数据
                WriteXML(doc, domElementFind, element);

            }
            else
            {
                // 无子节点
                // 创建该子节点
                QDomElement subElm = doc.createElement(strArrName);
                // 创建属性
                // 设置属性值
                // 节点和属性关联

                // 把该子节点追加到根节点下
                root.appendChild(subElm);

                // 写有效数据
                WriteXML(doc, subElm, element);
            }

            // 保存文件
            bIsOpen = file.open(QIODevice::WriteOnly);
            if(bIsOpen)
            {
                // 创建文件流和文件关联
                QTextStream stream(&file);
                doc.save(stream, 4);
                file.close();
            }
            else
            {
                cout << "WriteOnly error";
            }
        }
        else
        {
            cout << "setContent error";
            return;
        }
    }
    else
    {
        cout << "ReadOnly error";
        return;
    }
}

// 写入数据-删除掉原有的数据重新写入
bool OperateXmlFile::WriteAllXML(const QString filePath, const QMap<QString, QVector<_tagXmlElement>> mapElemnt)
{
    QFile file(filePath);

    // 只读方式打开
    bool bIsOpen = file.open(QIODevice::ReadOnly);

    if(bIsOpen)
    {
        // file和xml文档对象关联
        QDomDocument doc;
        bIsOpen = doc.setContent(&file);
        if(bIsOpen)
        {
            // 关联成功
            file.close();  // 关闭文件
            // 读取根节点元素
            QDomElement root = doc.documentElement();
            // 删除所有节点
            while (root.hasChildNodes()) {
                root.removeChild(root.firstChildElement());
            }
            // 重新添加新的节点
            QMap<QString, QVector<_tagXmlElement>>::const_iterator it_begin = mapElemnt.begin();
            QMap<QString, QVector<_tagXmlElement>>::const_iterator it_end = mapElemnt.end();
            for(; it_begin != it_end; ++it_begin)
            {
                // 创建该组名节点
                QDomElement subElement = doc.createElement(it_begin.key());
                // 把该节点追加到根节点下
                root.appendChild(subElement);
                QVector<_tagXmlElement> vecElm = it_begin.value();
                for(int i = 0; i < vecElm.size(); ++i)
                {
                    // 写入有效数据
                    WriteXML(doc, subElement, vecElm.at(i));
                }
            }

            // 保存文件
            bIsOpen = file.open(QIODevice::WriteOnly);
            if(bIsOpen)
            {
                // 创建文件流和文件关联
                QTextStream stream(&file);
                doc.save(stream, 4);
                file.close();
            }
            else
            {
                cout << "WriteOnly error";
            }
        }
        else
        {
            cout << "setContent error";
            return false;
        }
    }
    else
    {
        cout << "ReadOnly error";
        return false;
    }

    return true;
}

//读数据
bool OperateXmlFile::ReadXML(const QString filePath, QMap<QString, QVector<_tagXmlElement>>& mapElemnt, QString& strReadResult)
{
    // 只取标签名称和内容；暂时不管属性
    QFile file(filePath);
    bool bIsOk = file.open(QIODevice::ReadOnly);
    if(bIsOk)
    {
        // 打开成功
        // file和xml文档对象关联
        QDomDocument doc;
        bIsOk = doc.setContent(&file);
        if(bIsOk)
        {
            // 如果关联成功
            // 获取根节点
            QDomElement root = doc.documentElement();
            // 文件可以关闭了
            file.close();

            // 获取根节点下的全部子节点
            if(root.hasChildNodes())
            {
                // 有子节点
                QDomNodeList nodeList = root.childNodes();
                QString strArrayName;
                QVector<_tagXmlElement> vecElement;
                _tagXmlElement xmlElement;
                for(int i = 0; i < nodeList.size(); ++i)
                {

                    strArrayName= nodeList.at(i).toElement().tagName();
                    vecElement.clear();
                    QDomNodeList subList = nodeList.at(i).toElement().childNodes();
                    for(int j = 0; j < subList.size(); ++j)
                    {
                        xmlElement.m_strLabelName = subList.at(j).toElement().tagName();
                        xmlElement.m_strLabelContent = subList.at(j).toElement().text();
                        vecElement.push_back(xmlElement);
                    }
                    mapElemnt.insert(strArrayName, vecElement);
                }

                strReadResult = "have node";
            }
            else
            {
                // 无子节点
                strReadResult = "no node";
            }

        }
        else
        {
            // 关联文档失败
            strReadResult = "setContent error";
			// 文件可以关闭了
			file.close();
            return false;
        }
    }
    else
    {
        // 文件打开失败
        strReadResult = "file open faild";
        return false;
    }

    return true;
}

// 删除节点
void OperateXmlFile::DeleteXML(const QString& filePath, const QString& strArrName, const _tagXmlElement& element)
{
    // 删除内容的大体步骤
    // 1. 先读取出xml文件的所有内容
    // 2. 先读取出根节点
    // 3. 判断根节点有无子节点
    // 4. 没有子节点创建，直接返回
    // 5. 有子节点则判断子节点strArrName是否存在
    // 6. 查找是否有该标签
    // 7. 有则删除该标签

    QFile file(filePath);

    // 只读方式打开
    bool bIsOpen = file.open(QIODevice::ReadOnly);

    if(bIsOpen)
    {
        // file和xml文档对象关联
        QDomDocument doc;
        bIsOpen = doc.setContent(&file);
        if(bIsOpen)
        {
            // 关联成功
            file.close();  // 关闭文件
            // 读取根节点元素
            QDomElement root = doc.documentElement();
            // 判断根节点下有没有节点
            if(root.hasChildNodes())
            {
                // 有子节点

                // 遍历判断是否有需要添加标签的组名节点
                root.firstChildElement();
                root.nextSiblingElement();
                QDomNodeList domNodeList = root.childNodes();
                QDomElement domElementFind;
                bool bFind = false;
                for(int i = 0; i < domNodeList.size(); ++i)
                {
                    if(domNodeList.at(i).toElement().tagName() == strArrName)
                    {
                        domElementFind = domNodeList.at(i).toElement();
                        bFind = true;
                    }

                }
                if(bFind)
                {
                    // 找到
                    QDomNodeList nodeList = domElementFind.childNodes();
                    for(int i = 0; i < nodeList.size(); ++i)
                    {
                        if(nodeList.at(i).toElement().tagName() == element.m_strLabelName)
                        {
                            // 删除
                            domElementFind.removeChild(nodeList.at(i));
                            // 保存文件
                            bIsOpen = file.open(QIODevice::WriteOnly);
                            if(bIsOpen)
                            {
                                // 创建文件流和文件关联
                                QTextStream stream(&file);
                                doc.save(stream, 4);
                                file.close();
                            }
                            else
                            {
                                cout << "WriteOnly error";
                            }

                            break;
                        }
                    }
                }
                else
                {
                    return;
                }
            }
        }
        else
        {
            cout << "setContent error";
            return;
        }
    }
    else
    {
        cout << "ReadOnly error";
        return;
    }
}

//写有效数据
void OperateXmlFile::WriteXML(QDomDocument& doc, QDomElement& root, const _tagXmlElement& element)
{
    // 如果存在则覆盖之，不再添加新的标签
    QDomNodeList nodeList = root.childNodes();
    bool bFind = false;
    for(int i = 0; i < nodeList.size(); ++i)
    {
        if(nodeList.at(i).toElement().tagName() == element.m_strLabelName)
        {
            // 有相同，修改原来的内容
            nodeList.at(i).firstChild().setNodeValue(element.m_strLabelContent);
            bFind = true;
            break;
        }
    }

    if(false == bFind)
    {
        // 创建标签
        QDomElement domElement = doc.createElement(element.m_strLabelName);

        // 创建属性
    //    QDomAttr domAttr = doc.createAttribute(element.m_strAttributesName);
    //    // 给属性设置值
    //    domAttr.setNodeValue(element.m_strAttributeContent);
    //    // 标签和属性关联
    //    domElement.setAttributeNode(domAttr);

        // 设置标签内容
        QDomText text = doc.createTextNode(element.m_strLabelContent);
        domElement.appendChild(text);

        // 追加标签到根下
        root.appendChild(domElement);
    }
}

