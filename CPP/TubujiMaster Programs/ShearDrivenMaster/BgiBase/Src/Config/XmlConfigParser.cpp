#include "XmlConfigParser.h"
#include "BgiLogger.h"

#include <QFile>
#include <QString> 
#include <QXmlStreamReader>
#include <QCoreApplication>
#include <QDir>

bool XmlConfigParser::Parser(const QString& conf_filepath, QMap<QString, QVariant >& config_map)
{
	bool b_ret = false;
	QFile* file = new QFile(conf_filepath);
	if (file->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QXmlStreamReader reader(file);
		reader.readNext();

		while (!reader.atEnd())
		{
			if (reader.isStartElement())
			{
				QString key = reader.name().toString();
				QString strElementValue;
				if (key == "Bgi-Roson")
				{
					QString appdir_name = QCoreApplication::applicationDirPath();
					QDir application_dir(appdir_name);
					reader.readNext();
					while (!reader.atEnd())
					{
						if (reader.isStartElement())
						{
							key = reader.name().toString();
							strElementValue = reader.readElementText();
							BGI_ASSERT(!config_map.contains(key));
							if (strElementValue.contains("..\\"))
							{
								strElementValue = QDir::cleanPath(application_dir.absoluteFilePath(strElementValue));
							}
							config_map.insert(key, QVariant(strElementValue));
						}
						//else
						{
							reader.readNext();
						}
					}
				}

			}
			else
			{
				reader.readNext();
			}
		}
		file->close();

		if (reader.hasError())
		{
			// do error handling
			int m_error = reader.error();
			b_ret = false;
		}
	}
	delete file;
	file = NULL;

	return b_ret;
}


//读数据
bool XmlConfigParser::ReadErrorCodeXML(const QString filePath, QMap<QString, QVector<ErrDesc>>& mapElemnt, QString& strReadResult)
{
    mapElemnt.clear();
    // 只取标签名称和内容；暂时不管属性
    QFile file(filePath);
    bool bIsOk = file.open(QIODevice::ReadOnly);
    if (bIsOk)
    {
        // 打开成功
        // file和xml文档对象关联
        QDomDocument doc;
        bIsOk = doc.setContent(&file);
        if (bIsOk)
        {
            // 如果关联成功
            // 获取根节点
            QDomElement root = doc.documentElement();
            // 文件可以关闭了
            file.close();

            // 获取根节点下的全部子节点
            if (root.hasChildNodes())
            {
                // 有子节点
                QDomNodeList nodeList = root.childNodes();
                QString strErrType = "";
                for (int i = 0; i < nodeList.size(); ++i)
                {

                    strErrType = nodeList.at(i).toElement().tagName();
                    QDomNodeList subList = nodeList.at(i).toElement().childNodes();
                    QVector<ErrDesc> vecXmlElement;
                    for (int j = 0; j < subList.size(); ++j)
                    {
                        ErrDesc xmlElement;
                        QString strErrCode = subList.at(j).toElement().tagName();
                        QDomNodeList ssubList = subList.at(j).toElement().childNodes();
                        xmlElement.ErrCode = strErrCode;
                        xmlElement.Description = ssubList.at(0).toElement().text();
                        xmlElement.Solution = ssubList.at(1).toElement().text();
                        vecXmlElement.push_back(xmlElement);

                    }
                    mapElemnt.insert(strErrType, vecXmlElement);
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