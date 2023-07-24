#include "OperateSqliteConfig.h"
#include "BgiFileTools.h"
#include "BgiLogger.h"
#include <QSqlError>
#include <QDebug>
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "DeviceDetailCfg.h"


const QString gc_strCfgTableName = "ChipSetting";
const QString gc_ErrorLogTableName = "ErrorInfoLog";

const QString gc_strUserInfoTableName = "UsersInfo";
const QString gc_strOperator = "Operator";
const QString gc_strMaintainer = "Maintainer";
const QString gc_strAdmin = "Admin";

OperateSqliteConfig::OperateSqliteConfig()
	:m_ShowErrLimit(100)
{
	QString path;
	BgiFileTools::GetDataDirPath(path);
	path += "//Data//Input//MasterDBConfig.db";

	InitDB(path);
	CreateCfgTable();
	CreateUserTable();
	CreateErrorLogTable();
}

OperateSqliteConfig::~OperateSqliteConfig()
{
	m_DB.close();
}

OperateSqliteConfig* OperateSqliteConfig::Instance()
{
	static OperateSqliteConfig instance;
	return &instance;
}


bool OperateSqliteConfig::InitDB(QString path)
{
	//检测已连接的方式
	if (QSqlDatabase::contains("qt_sql_default_connection"))
		m_DB = QSqlDatabase::database("qt_sql_default_connection");
	else
		//添加数据库驱动
		m_DB = QSqlDatabase::addDatabase("QSQLITE");
	//设置数据库名称
	m_DB.setDatabaseName(path);
	//打开数据库
	if (m_DB.open())
		return true;
	BGI_WARNING("Init Database error!");
	return false;
}

bool OperateSqliteConfig::CheckTabelExist(QString strTableName)
{
	const QString query_table = QString("select name from sqlite_master where name='%1'").arg(strTableName);
	QSqlQuery sql_Query(query_table,m_DB);
	return sql_Query.next();
}

bool OperateSqliteConfig::CheckCfgRecordExit(const QString strTableName, const QString field, const QString value)
{
	const QString query_table = QString("select * from %1 where %2='%3'").arg(strTableName).arg(field).arg(value);
	QSqlQuery sql_Query(query_table, m_DB);
	return sql_Query.next();
}

bool OperateSqliteConfig::CreateCfgTable()
{
	QSqlQuery sql_Query(m_DB);
	//先查询表是否存在
	if (CheckTabelExist(gc_strCfgTableName))
	{
		return true;
	}
	const QString creat_sql = QString("create table %1 (Barcode varchar(30) primary key, BatchID varchar(30), Config TEXT, SetTime varchar(30))").arg(gc_strCfgTableName);
	sql_Query.prepare(creat_sql);
	if (!sql_Query.exec())
	{
		qDebug() << "Err info " << sql_Query.lastError();
		BGI_WARNING("Create cfgtable error!");
		return false;
	}
	return true;
}

bool OperateSqliteConfig::WriteCfgData(QList<GeneralParaType::SqlCfgData> vecSqlCfgData)
{
	QSqlQuery sql_Query(m_DB);

	sql_Query.prepare(QString("insert into %1(Barcode,BatchID,Config,SetTime)"\
		"VALUES (:Barcode,:BatchID,:Config,:SetTime)").arg(gc_strCfgTableName));
	for (int i = 0; i < vecSqlCfgData.size(); i++)
	{
		QString strCfg;
		JoinJsonToString(vecSqlCfgData.at(i).strTableDesc, strCfg);

		sql_Query.bindValue(":Barcode", vecSqlCfgData.at(i).strBarCode);
		sql_Query.bindValue(":BatchID", vecSqlCfgData.at(i).strBatchID);
		sql_Query.bindValue(":Config", strCfg);
		sql_Query.bindValue(":SetTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
		if (!sql_Query.exec())
			return false;
	}
	return true;
}

bool OperateSqliteConfig::WriteCfgData(GeneralParaType::SqlCfgData strSqlCfgData)
{
	QString dbname = m_DB.databaseName();
	QSqlQuery sql_Query(m_DB);
	QString sql = "";
	QString strCfg;
	JoinJsonToString(strSqlCfgData.strTableDesc, strCfg);
	//查询记录是否存在，存在则更新，不存在则插入
	if (CheckCfgRecordExit(gc_strCfgTableName, "Barcode", strSqlCfgData.strBarCode))
	{
		sql = QString("update %1 set Config='%2',SetTime='%3' where Barcode='%4'").arg(gc_strCfgTableName).arg(strCfg)
			.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(strSqlCfgData.strBarCode);
	}
	else
	{
		sql = QString("insert into %1(Barcode,BatchID,Config,SetTime) values('%2','%3','%4','%5')").arg(gc_strCfgTableName).arg(strSqlCfgData.strBarCode)
			.arg(strSqlCfgData.strBatchID).arg(strCfg).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

	}
	return sql_Query.exec(sql);
}

bool OperateSqliteConfig::QureyAllCfgData(QList<GeneralParaType::SqlCfgData>& vecSqlCfgData)
{
	vecSqlCfgData.clear();
	QSqlQuery sql_Query(m_DB);
	QString sql = QString("SELECT * FROM %1").arg(gc_strCfgTableName);
	if (sql_Query.exec(sql))
	{
		GeneralParaType::SqlCfgData data;
		while (sql_Query.next())
		{
			data.strBarCode = sql_Query.value(0).toString();
			data.strBatchID = sql_Query.value(1).toString();
			GeneralParaType::SequencingTableDesc strTableDesc;
			ParseChipSetting(sql_Query.value(2).toString(), strTableDesc);
			data.strTableDesc = strTableDesc;
			vecSqlCfgData.append(data);
		}
		return true;
	}
	return false;

}

bool OperateSqliteConfig::QureyCfgDataByKeyWords(QString strBarcode, QString strBatchID, QList<GeneralParaType::SqlCfgData>& vecSqlCfgData)
{
	//不在这里检查最小长度，后面的sql语句为like，不是精确匹配，且有对strBarcode、strBatchID为空的处理
	//if (strBarcode.size() < 4 && strBatchID.size() < 4)
	//	return false;

	vecSqlCfgData.clear();
	QSqlQuery sql_Query(m_DB);
	QString sql = QString("SELECT * FROM %1 WHERE Barcode LIKE '%%2' OR BatchID LIKE '%%3'").arg(gc_strCfgTableName)
		.arg(strBarcode).arg(strBatchID);
	if(strBarcode == "")
		sql = QString("SELECT * FROM %1 WHERE BatchID LIKE '%%2'").arg(gc_strCfgTableName).arg(strBatchID);
	if(strBatchID == "")
		sql = QString("SELECT * FROM %1 WHERE Barcode LIKE '%%2'").arg(gc_strCfgTableName).arg(strBarcode);

	if (sql_Query.exec(sql))
	{
		GeneralParaType::SqlCfgData data;
		while (sql_Query.next())
		{
			data.strBarCode = sql_Query.value(0).toString();
			data.strBatchID = sql_Query.value(1).toString();
			GeneralParaType::SequencingTableDesc strTableDesc;
			ParseChipSetting(sql_Query.value(2).toString(), strTableDesc);
			data.strTableDesc = strTableDesc;
			vecSqlCfgData.append(data);
		}
		return true;
	}
	return false;

}

bool OperateSqliteConfig::QureyCfgDataByBarcode(QString strBarcode, GeneralParaType::SqlCfgData& sqlCfgData)
{
	if (strBarcode.size() < 4)
		return false;

	QSqlQuery sql_Query(m_DB);
	QString sql = QString("SELECT * FROM %1 WHERE Barcode LIKE '%%2'").arg(gc_strCfgTableName).arg(strBarcode);

	if (sql_Query.exec(sql))
	{
		if (sql_Query.next())
		{
			sqlCfgData.strBarCode = sql_Query.value(0).toString();
			sqlCfgData.strBatchID = sql_Query.value(1).toString();
			GeneralParaType::SequencingTableDesc strTableDesc;
			ParseChipSetting(sql_Query.value(2).toString(), strTableDesc);
			sqlCfgData.strTableDesc = strTableDesc;
			return true;
		}
	}
	return false;
}

bool OperateSqliteConfig::QueryRecentCfgData(GeneralParaType::SqlCfgData& sqlCfgData)
{
	QSqlQuery sql_Query(m_DB);
	QString sql = QString("SELECT * FROM %1 Order BY SetTime DESC LIMIT 1").arg(gc_strCfgTableName);

	if (sql_Query.exec(sql))
	{
		if (sql_Query.next())
		{
			sqlCfgData.strBarCode = sql_Query.value(0).toString();
			sqlCfgData.strBatchID = sql_Query.value(1).toString();
			GeneralParaType::SequencingTableDesc strTableDesc;
			ParseChipSetting(sql_Query.value(2).toString(), strTableDesc);
			sqlCfgData.strTableDesc = strTableDesc;
		}
		return true;
	}
	return false;
}

bool OperateSqliteConfig::DelteCfgDataBystrBarcode(QString strBarcode)
{
	if (strBarcode.size() <= 0)
		return false;
	QSqlQuery sql_Query(m_DB);
	QString sql = QString("DELETE FROM %1 WHERE Barcode='%2'").arg(gc_strCfgTableName).arg(strBarcode);
	if (sql_Query.exec(sql))
		return true;
	return false;
}

bool OperateSqliteConfig::DelteCfgDataBystrBatchID(QString strBatchID)
{
	if (strBatchID.size() <= 0)
		return false;
	QSqlQuery sql_Query(m_DB);
	QString sql = QString("DELETE FROM %1 WHERE BatchID='%2'").arg(gc_strCfgTableName).arg(strBatchID);
	if (sql_Query.exec(sql))
		return true;
	return false;
}

void OperateSqliteConfig::ParserJosn(QJsonObject json_object, QMap<QString, QVariant >& config_map)
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

bool OperateSqliteConfig::JoinJsonToString(GeneralParaType::SequencingTableDesc tableDesc, QString &strCfg)
{
	QVariantMap config_map;
	config_map.insert("BiochemistryTimes", tableDesc.iBiochemistryTimes);
	config_map.insert("PhotographTime", tableDesc.iPhotographTime);
	config_map.insert("R1", tableDesc.iR1);
	config_map.insert("R2", tableDesc.iR2);
	config_map.insert("B1", tableDesc.iB1);
	config_map.insert("B2", tableDesc.iB2);
	config_map.insert("Total", tableDesc.iAll);
	config_map.insert("ChipNo", tableDesc.strChipNo);
	config_map.insert("ImagerNo", tableDesc.strImagerNo);
	config_map.insert("SequencingMode", tableDesc.strSequencingMode);
	config_map.insert("ReagentKit", tableDesc.strReagentKit);
	config_map.insert("Reference", tableDesc.strReference);
	config_map.insert("BarCodePah", tableDesc.strBarCodePah);
	QJsonObject qjsonobj = QJsonObject::fromVariantMap(config_map);
	strCfg = QString(QJsonDocument(qjsonobj).toJson());
	return true;
}

bool OperateSqliteConfig::ParseChipSetting(QString strCfg, GeneralParaType::SequencingTableDesc &tableDesc)
{
	if (strCfg == "")
		return false;
	bool b_ret = false;
	QMap<QString, QVariant > config_map;
	QJsonParseError jsonError;
	QJsonDocument jsonDom = QJsonDocument::fromJson(strCfg.toUtf8(), &jsonError);
	// 解析未发生错误
	if (!jsonDom.isNull() && (jsonError.error == QJsonParseError::NoError))
	{
		QJsonObject jsonObject = jsonDom.object();
		ParserJosn(jsonObject, config_map);
		b_ret = true;
	}
	else
	{
		return false;
	}
	
	tableDesc.iBiochemistryTimes = config_map.value("BiochemistryTimes").toInt();
	tableDesc.iPhotographTime = config_map.value("PhotographTime").toInt();
	tableDesc.iR1 = config_map.value("R1").toInt();
	tableDesc.iR2 = config_map.value("R2").toInt();
	tableDesc.iB1 = config_map.value("B1").toInt();
	tableDesc.iB2 = config_map.value("B2").toInt();
	tableDesc.iAll = config_map.value("Total").toInt();
	tableDesc.strChipNo = config_map.value("ChipNo").toString();
	tableDesc.strImagerNo = config_map.value("ImagerNo").toString();
	tableDesc.strSequencingMode = config_map.value("SequencingMode").toString();
	tableDesc.strReagentKit = config_map.value("ReagentKit").toString();
	tableDesc.strReference = config_map.value("Reference").toString();
	tableDesc.strBarCodePah = config_map.value("BarCodePah").toString();

	return b_ret;
}


bool OperateSqliteConfig::CreateUserTable()
{
	QSqlQuery sql_Query(m_DB);
	//先查询表是否存在
	if (CheckTabelExist(gc_strUserInfoTableName))
	{
		return true;
	}
	const QString creat_sql = QString("create table %1 (UserName varchar(30) primary key, Password varchar(30), UserType varchar(20), SetTime varchar(30))").arg(gc_strUserInfoTableName);
	sql_Query.prepare(creat_sql);
	if (!sql_Query.exec())
	{
		qDebug() << "Err info " << sql_Query.lastError();
		BGI_WARNING("Create userinfo table error!");
		return false;
	}
	return true;
}

bool OperateSqliteConfig::IsUserExit(QString strUserName)
{
	QSqlQuery sql_Query(m_DB);
	QString sql = QString("SELECT UserName FROM %1").arg(gc_strUserInfoTableName);
	if (sql_Query.exec(sql))
	{
		while (sql_Query.next())
		{
			if (sql_Query.value(0).toString() == strUserName)
				return true;
		}
	}
	return false;
}

bool OperateSqliteConfig::AddUser(const GeneralParaType::UserInfo userInfo)
{
	QSqlQuery sql_Query(m_DB);
	//查询记录是否存在
	if (CheckCfgRecordExit(gc_strUserInfoTableName, "UserName", userInfo.strName))
	{
		return false;
	}
	else
	{
		QString strUserType = "";
		if (userInfo.userType == GeneralParaType::UserType::Operator)
			strUserType = gc_strOperator;
		else if (userInfo.userType == GeneralParaType::UserType::Maintainer)
			strUserType = gc_strMaintainer;
		else
			strUserType = gc_strAdmin;
		QString sql = QString("insert into %1(UserName,Password,UserType,SetTime) values('%2','%3','%4','%5')").arg(gc_strUserInfoTableName).arg(userInfo.strName)
			.arg(userInfo.password).arg(strUserType).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
		return sql_Query.exec(sql);
	}
}

bool OperateSqliteConfig::DelUser(const QString strName)
{
	if (strName.size() <= 0)
		return false;
	QSqlQuery sql_Query(m_DB);
	QString sql = QString("DELETE FROM %1 WHERE UserName='%2'").arg(gc_strUserInfoTableName).arg(strName);
	if (sql_Query.exec(sql))
		return true;
	return false;
}

bool OperateSqliteConfig::UpdateUser(const GeneralParaType::UserInfo userInfo)
{
	QSqlQuery sql_Query(m_DB);
	//查询记录是否存在
	if (!CheckCfgRecordExit(gc_strUserInfoTableName, "UserName", userInfo.strName))
	{
		return false;
	}
	else
	{
		QString strUserType = "";
		if (userInfo.userType == GeneralParaType::UserType::Operator)
			strUserType = gc_strOperator;
		else if (userInfo.userType == GeneralParaType::UserType::Maintainer)
			strUserType = gc_strMaintainer;
		else
			strUserType = gc_strAdmin;
		QString sql = QString("update %1 set Password='%2',SetTime='%3',UserType='%4' where UserName='%5'").arg(gc_strUserInfoTableName)
			.arg(userInfo.password).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(strUserType).arg(userInfo.strName);
		return sql_Query.exec(sql);
	}
}

bool OperateSqliteConfig::QueryUserInfoByName(QString strName, GeneralParaType::UserInfo& userINfo,QString& strErrorInfo)
{
	QSqlQuery sql_Query(m_DB);
	QString sql = QString("SELECT * FROM %1 WHERE UserName='%2'").arg(gc_strUserInfoTableName).arg(strName);
	if (sql_Query.exec(sql))
	{
		while (sql_Query.next())
		{
			userINfo.strName = sql_Query.value(0).toString();
			userINfo.password = sql_Query.value(1).toString();
			if (sql_Query.value(2).toString() == gc_strOperator)
				userINfo.userType = GeneralParaType::UserType::Operator;
			else if(sql_Query.value(2).toString() == gc_strMaintainer)
				userINfo.userType = GeneralParaType::UserType::Maintainer;
			else
				userINfo.userType = GeneralParaType::UserType::Admin;
			return true;
		}
		strErrorInfo = "未找到用户信息";
		return false;
	}
	strErrorInfo = "查找语句执行异常";
	return false;
}

bool OperateSqliteConfig::CreateErrorLogTable()
{
	QSqlQuery sql_Query(m_DB);
	//先查询表是否存在
	if (CheckTabelExist(gc_ErrorLogTableName))
	{
		return true;
	}
	const QString creat_sql = QString("create table %1 (ErrTime varchar(30) primary key, ErrType varchar(20), ErrInfo varchar(100))").arg(gc_ErrorLogTableName);
	sql_Query.prepare(creat_sql);
	if (!sql_Query.exec())
	{
		BGI_WARNING("Create ErrorLogTable error!");
		return false;
	}
	return true;
}

bool OperateSqliteConfig::RecordErrToDatabase(QString strContent, InfoType iType)
{
	QString strErrType = "";
	if (GeneralType == iType)
		strErrType = "General";
	else if (RobotType == iType)
		strErrType = "Robot";
	else if (CoaterType == iType)
		strErrType = "Coater";
	else if (ImagerType == iType)
		strErrType = "Imager";
	if (strErrType == "")
		return false;

	QSqlQuery sql_Query(m_DB);
	QString sql = QString("insert into %1(ErrTime,ErrType,ErrInfo) values('%2','%3','%4')").arg(gc_ErrorLogTableName)
		.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(strErrType).arg(strContent);
	return sql_Query.exec(sql);
}

bool OperateSqliteConfig::QueryErrInfo(QList<GeneralParaType::sqlErrorData>& vecSqlErrData, QString StartTime, QString EndTime, InfoType iType)
{
	QString dbname = m_DB.databaseName();
	QString Value;
	DeviceDetailCfg::Instance()->GetCfgValue("GeneralCfg/ShowErrLimit", Value);
	m_ShowErrLimit = Value.toInt();

	vecSqlErrData.clear();
	QString strErrType = "";
	if (GeneralType == iType)
		strErrType = "General";
	else if (RobotType == iType)
		strErrType = "Robot";
	else if (CoaterType == iType)
		strErrType = "Coater";
	else if (ImagerType == iType)
		strErrType = "Imager";
	else if (All == iType)
		strErrType = "";

	QSqlQuery sql_Query(m_DB);
	QString sql = "";
	if (StartTime == "" && EndTime == "")
	{
		if (strErrType == "")
			sql = QString("select * from %1 limit %2").arg(gc_ErrorLogTableName).arg(m_ShowErrLimit);
		else
			sql = QString("select * from %1 where ErrType='%2' limit %3").arg(gc_ErrorLogTableName).arg(strErrType).arg(m_ShowErrLimit);
	}
	else if (StartTime != "" && EndTime == "")
	{
		if (strErrType == "")
			sql = QString("select * from %1 where ErrTime>='%2' limit %3").arg(gc_ErrorLogTableName).arg(StartTime).arg(m_ShowErrLimit);
		else
			sql = QString("select * from %1 where ErrType='%2' and ErrTime>='%3' limit %4").arg(gc_ErrorLogTableName).arg(strErrType).arg(StartTime).arg(m_ShowErrLimit);
	}
	else if (StartTime == "" && EndTime != "")
	{
		if (strErrType == "")
			sql = QString("select * from %1 where ErrTime<='%2' limit %3").arg(gc_ErrorLogTableName).arg(EndTime).arg(m_ShowErrLimit);
		else
			sql = QString("select * from %1 where ErrType='%2' and ErrTime<='%3' limit %4").arg(gc_ErrorLogTableName).arg(strErrType).arg(EndTime).arg(m_ShowErrLimit);
	}
	else if (StartTime != "" && EndTime != "")
	{
		if (strErrType == "")
			sql = QString("select * from %1 where ErrTime>='%2' and ErrTime<='%3' limit %4").arg(gc_ErrorLogTableName).arg(StartTime).arg(EndTime).arg(m_ShowErrLimit);
		else
			sql = QString("select * from %1 where ErrType='%2' and ErrTime>='%3' and ErrTime<='%4'limit %5").arg(gc_ErrorLogTableName).arg(strErrType).arg(StartTime).arg(EndTime).arg(m_ShowErrLimit);
	}
	if (sql == "")
		return false;
	if (sql_Query.exec(sql))
	{
		GeneralParaType::sqlErrorData data;
		while (sql_Query.next())
		{
			data.ErrTime = sql_Query.value(0).toString();
			data.Errtype = sql_Query.value(1).toString();
			data.ErrContent = sql_Query.value(2).toString();
			vecSqlErrData.append(data);
		}
		return true;
	}
	return false;
}

bool OperateSqliteConfig::DelErrInfo(QString StartTime, QString EndTime, InfoType iType)
{
	QSqlQuery sql_Query(m_DB);
	QString strErrType = "";
	if (GeneralType == iType)
		strErrType = "General";
	else if (RobotType == iType)
		strErrType = "Robot";
	else if (CoaterType == iType)
		strErrType = "Coater";
	else if (ImagerType == iType)
		strErrType = "Imager";
	else if (All == iType)
		strErrType = "";

	QString sql = "";
	if (StartTime != "" && EndTime != "")
	{
		if (strErrType == "")
			sql = QString("delete from %1 where ErrTime>='%2' and ErrTime<='%3'").arg(gc_ErrorLogTableName).arg(StartTime).arg(EndTime);
		else
			sql = QString("delete from %1 where ErrType='%2' and ErrTime>='%3' and ErrTime<='%4'").arg(gc_ErrorLogTableName).arg(strErrType).arg(StartTime).arg(EndTime);
	}
	if (sql == "")
		return false;
	return sql_Query.exec(sql);
}

bool OperateSqliteConfig::GetRecentRangTime(QString& StartTime, QString& EndTime)
{
	QSqlQuery sql_Query(m_DB);
	bool bQueryed = false;
	QString sql = QString("select ErrTime from %1 order by ErrTime asc limit 1").arg(gc_ErrorLogTableName);
	if (sql_Query.exec(sql))
	{
		while (sql_Query.next())
		{
			StartTime = sql_Query.value(0).toString();
			bQueryed = true;
		}
	}
	if (!bQueryed)
		return false;
	sql = QString("select ErrTime from %1 order by ErrTime desc limit 1").arg(gc_ErrorLogTableName);
	if (sql_Query.exec(sql))
	{
		while (sql_Query.next())
		{
			EndTime = sql_Query.value(0).toString();
			return true;
		}
	}
	return false;
}