#include "stdafx.h"
#include "../Utility/CppSQLite3.h"
#include "ProductDefines.h"
#include "ProductDatabaseManager.h"

ProductDatabaseManager::ProductDatabaseManager()
{
	m_sqlitePtr = std::make_unique<CppSQLite3DB>();
}

ProductDatabaseManager::~ProductDatabaseManager()
{

}

void ProductDatabaseManager::SetDatabasePath(const std::string& filePath)
{
	m_databasePath = filePath;
}

bool ProductDatabaseManager::Init()
{
	if (m_isOpened)
	{
		return true;
	}

	try
	{
		m_sqlitePtr->open(m_databasePath.c_str());
		m_isOpened = true;

		return true;
	}
	catch (const CppSQLite3Exception& /*e*/)
	{
		m_isOpened = false;

		return false;
	}
}

void ProductDatabaseManager::Uninit()
{
	if (m_isOpened)
	{
		m_sqlitePtr->close();
	}

	m_isOpened = false;
}

bool ProductDatabaseManager::QueryAllProducts(std::vector<ProductInfo>& productInfoList)
{
	if (!m_isOpened)
		return false;

	try
	{
		char sql[] = "SELECT * from GoodsInfoPlc";
		CppSQLite3Query query = m_sqlitePtr->execQuery(sql);

		while (!query.eof())
		{
			ProductInfo info;
			ExtractProductInfo(query, info);

			productInfoList.push_back(info);

			query.nextRow();
		}

		query.finalize();

		return productInfoList.size() > 0;
	}
	catch (CppSQLite3Exception& /*e*/)
	{
		return false;
	}
}

bool ProductDatabaseManager::QueryProductFromDatabase(const std::vector<std::string>& keywords, std::vector<ProductInfo>& productInfoList, bool fromView)
{
	if (!m_isOpened)
		return false;

	try
	{
		KeywordContainer container;
		ParseKeywords(keywords, container);
		OptimizeKeywords(container);
		std::string statement = TransformKeywordsToQueryStatement(container);
		
		CppSQLite3Query query;

		char sql[256] = {0};
		if (fromView)
		{
			sprintf_s(sql, "select * from GoodsInfoPlc_view where %s;", statement.c_str());
		}
		else
		{
			sprintf_s(sql, "drop view if exists GoodsInfoPlc_view;");
			query = m_sqlitePtr->execQuery(sql);
			query.finalize();

			sprintf_s(sql, "create view GoodsInfoPlc_view AS select * from GoodsInfoPlc where %s; ", statement.c_str());
			query = m_sqlitePtr->execQuery(sql);
			query.finalize();

			sprintf_s(sql, "select * from GoodsInfoPlc_view;");
			//sprintf_s(sql, "create view GoodsInfoPlc_view AS select * from GoodsInfoPlc where name like \'%%可乐%%\';");

			/*sprintf_s(sql, "drop view if exists GoodsInfoPlc_view; "
							"create view GoodsInfoPlc_view AS select * from GoodsInfoPlc where %s;", condition.c_str());*/
		}
		
		auto utf8 = nbase::win32::MBCSToUTF8(sql); 

		query = m_sqlitePtr->execQuery(sql);

		while (!query.eof())
		{
			ProductInfo info;
			ExtractProductInfo(query, info);
			
			productInfoList.push_back(info);
			
			query.nextRow();
		}

		query.finalize();

		return productInfoList.size() > 0;
	}
	catch (CppSQLite3Exception& e)
	{
		OutputDebugStringA(nbase::StringPrintf("查询数据错误:  %s\n", e.errorMessage()).c_str());

		return false;
	}
}


bool ProductDatabaseManager::ReplaceProductSearchListDatabaseView(const std::vector<std::string>& keywords)
{
	if (!m_isOpened)
		return false;

	try
	{
		KeywordContainer container;
		
		ParseKeywords(keywords, container);
		
		OptimizeKeywords(container);

		std::string statement = TransformKeywordsToQueryStatement(container);

		CppSQLite3Query query;

		char sql[256] = { 0 };
		sprintf_s(sql, "drop view if exists GoodsInfoPlc_view;");
		query = m_sqlitePtr->execQuery(sql);
		query.finalize();

		sprintf_s(sql, "create view GoodsInfoPlc_view AS select * from GoodsInfoPlc where %s; ", statement.c_str());
		query = m_sqlitePtr->execQuery(sql);
		query.finalize();

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		OutputDebugStringA(nbase::StringPrintf("查询数据错误:  %s\n", e.errorMessage()).c_str());

		return false;
	}
}

void ProductDatabaseManager::ExtractProductInfo(const CppSQLite3Query& query, ProductInfo& info)
{
	info.id = query.getIntField("ID");
	info.barcode = query.getStringField("barcode");
	info.name = nbase::win32::UTF8ToMBCS(query.getStringField("name"));
	//info.name = nbase::win32::UTF8ToMBCS(query.getStringField("name"))+ nbase::win32::UTF8ToMBCS(query.getStringField("specification"));;
	info.mainCategory = nbase::win32::UTF8ToMBCS(query.getStringField("main_class"));
	info.subCategory = nbase::win32::UTF8ToMBCS(query.getStringField("subclass"));
	info.description = nbase::win32::UTF8ToMBCS(query.getStringField("description"));
	info.specification = nbase::win32::UTF8ToMBCS(query.getStringField("specification"));
	info.unit = nbase::win32::UTF8ToMBCS(query.getStringField("unit"));
	info.price = static_cast<float>(query.getFloatField("price"));
	info.amount = 10;
	info.keywords = nbase::win32::UTF8ToMBCS(query.getStringField("keywords"));
	info.image = nbase::win32::UTF8ToMBCS(query.getStringField("image"));
}

bool ProductDatabaseManager::CheckRequeryCondition(const std::vector<std::string>& keywords)
{
	for (auto keyword : keywords)
	{
		auto field = ParseKeywordField(keyword);
		if (field.compare("main_class") == 0)
			return true;
	}
	
	return false;
}

std::string ProductDatabaseManager::MapIndexToField(int index)
{
	switch (index)
	{
	case 1:
		return "ID";
	case 2:
		return "barcode";
	case 3:
		return "name";
	case 4:
		return "main_class";
	case 5:
		return "subclass";
	case 6:
		return "brand";
	case 7:
		return "specification";
	case 8:
		return "unit";
	case 9:
		return "price"; 
	case 10:
		return "description";
	case 11:
		return "keywords";
	case 12:
		return "image";
	default:
		return "";
	}
}

std::string ProductDatabaseManager::ParseKeywordField(const std::string& keyword)
{
	auto prefix = keyword.substr(0, keyword.find_last_of('_'));
	auto postfix = keyword.substr(keyword.find_last_of('_') + 1);
	int index = 0;
	
	nbase::StringToInt(postfix, &index);

	return MapIndexToField(index);
}

void ProductDatabaseManager::ParseKeywords(const std::vector<std::string>& keywords, KeywordContainer &container)
{
	container.clear();

	for (auto keyword : keywords)
	{
		auto prefix = keyword.substr(0, keyword.find_last_of('_'));
		auto postfix = keyword.substr(keyword.find_last_of('_') + 1);
		int index = 0;
		nbase::StringToInt(postfix, &index);
		auto field = MapIndexToField(index);

		container.push_back(std::make_pair(field, prefix));
	}
}

std::string ProductDatabaseManager::TransformKeywordsToQueryStatement(const KeywordContainer& container)
{
	std::string statement;
	for (auto item : container)
	{
		auto buffer = nbase::StringPrintf("%s like \'%%%s%%\'", item.first.c_str(), item.second.c_str());

		// 如果包含 subclass, 使用 or 查询; 否则使用 and
		if (buffer.find("subclass") == std::string::npos) 
		{
			buffer += " and ";
		}
		else
		{
			if (container.size() > 1)
			{
				statement = statement.substr(0, statement.find_last_of(" and ") - 4);
				buffer = " or " + buffer + " and ";
			}
			else
			{
				buffer += " and ";
			}
		}
		statement += buffer;
	}

	// 去掉最后面的 " and "
	statement = statement.substr(0, statement.find_last_of(" and ") - 4);

	return statement;
}

void ProductDatabaseManager::OptimizeKeywords(KeywordContainer& container)
{
	bool shouldAddSubclass = false;
	std::pair<std::string, std::string> mainClassPair;
	for (auto item : container)
	{
		if (item.first.find("main_class") != std::string::npos)
		{
			mainClassPair = item;
			shouldAddSubclass = true;
			break;
		}
	}

	if (shouldAddSubclass)
	{
		for (auto item : container)
		{
			if (item.first.find("subclass") != std::string::npos)
			{
				shouldAddSubclass = false;
				break;
			}
		}
	}

	if (shouldAddSubclass)
	{
		auto subclassPair = std::make_pair("subclass", mainClassPair.second);
		container.push_back(subclassPair);
	}

}
