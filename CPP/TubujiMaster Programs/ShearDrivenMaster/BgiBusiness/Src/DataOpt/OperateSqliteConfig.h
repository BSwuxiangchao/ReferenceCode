/*
* @Abrief QSqlite 操作参数
* @note QSqlite 操作参数，每张芯片对应一套配置
* @version 1.0
* @author wuxiangchao 2023-2-16
*/
#pragma once
#include <QSqlDatabase>
#include <QSqlQuery>
#include "BusinessExternDef.h"
#include "ParaType.h"


class BGIBUSINE_CLASS_EXPORT OperateSqliteConfig
{
public:
	static OperateSqliteConfig* Instance();
	~OperateSqliteConfig();

#pragma region 芯片配置文件

	bool CreateCfgTable();
	bool WriteCfgData(QList<GeneralParaType::SqlCfgData> vecSqlCfgData);
	bool WriteCfgData(GeneralParaType::SqlCfgData strSqlCfgData);
	bool QureyAllCfgData(QList<GeneralParaType::SqlCfgData>& vecSqlCfgData);
	bool QureyCfgDataByKeyWords(QString strBarcode,QString strBatchID,QList<GeneralParaType::SqlCfgData> &vecSqlCfgData);
	bool QureyCfgDataByBarcode(QString strBarcode, GeneralParaType::SqlCfgData& sqlCfgData);
	bool QueryRecentCfgData(GeneralParaType::SqlCfgData& sqlCfgData);
	bool DelteCfgDataBystrBarcode(QString strBarcode);
	bool DelteCfgDataBystrBatchID(QString strBatchID);
	bool ParseChipSetting(QString strCfg, GeneralParaType::SequencingTableDesc& tableDesc);
#pragma endregion

#pragma region 用户信息管理文件
	bool CreateUserTable();
	bool IsUserExit(QString strUserName);
	bool AddUser(const GeneralParaType::UserInfo userInfo);
	bool DelUser(const QString strName);
	bool UpdateUser(const GeneralParaType::UserInfo userInfo);
	bool QueryUserInfoByName(QString strName, GeneralParaType::UserInfo& userINfo,QString& strErrorInfo);
#pragma endregion

#pragma region 错误日志操作
	//***********************************************错误日志操作***************************
	bool CreateErrorLogTable();
	bool RecordErrToDatabase(QString strContent, InfoType iType);
	bool QueryErrInfo(QList<GeneralParaType::sqlErrorData>& vecSqlErrData, QString StartTime = "", QString EndTime = "", InfoType iType = All);
	bool DelErrInfo(QString StartTime = "", QString EndTime = "", InfoType iType = All);
	bool GetRecentRangTime(QString& StartTime, QString& EndTime);
#pragma endregion

private:
	OperateSqliteConfig();

	bool InitDB(QString path);
	bool CheckTabelExist(QString strTableName);
	bool CheckCfgRecordExit(const QString strTableName, const QString field, const QString value);
	void ParserJosn(QJsonObject json_object, QMap<QString, QVariant >& config_map);
	bool JoinJsonToString(GeneralParaType::SequencingTableDesc tableDesc, QString &strCfg);
private:
	QSqlDatabase m_DB;

	//显示到界面的错误信息上限
	int m_ShowErrLimit;
};

