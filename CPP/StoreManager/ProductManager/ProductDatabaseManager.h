#pragma once

class CppSQLite3DB;
class CppSQLite3Query;
struct ProductInfo;
class ProductDatabaseManager
{
	using KeywordContainer = std::vector<std::pair<std::string, std::string>>;

public:
	SINGLETON_DEFINE(ProductDatabaseManager);

private:
	ProductDatabaseManager();
	~ProductDatabaseManager();

public:
	void SetDatabasePath(const std::string &filePath);
	bool Init();
	void Uninit();
	
	bool QueryAllProducts(std::vector<ProductInfo>& productInfoList);

	bool QueryProductFromDatabase(const std::vector<std::string>& keywords, std::vector<ProductInfo>& productInfoList, bool fromView = true);

	bool ReplaceProductSearchListDatabaseView(const std::vector<std::string>& keywords);

	void ExtractProductInfo(const CppSQLite3Query& query, ProductInfo& info);

	bool IsDatabaseOpened() { return m_isOpened; }

	// 检查是否满足重新查询数据库条件：关键字里面包含main_class(商品大类)
	bool CheckRequeryCondition(const std::vector<std::string>& keywords);
private:
	// 解析http请求中的形参下标，对应数据库中的查询字段
	std::string MapIndexToField(int index);

	std::string ParseKeywordField(const std::string& keyword);

	void ParseKeywords(const std::vector<std::string>& keywords, KeywordContainer &container);

	/*
	** 优化查询语句
	** 1.main_class 查询需要包含 sub_class结果
	**/
	void OptimizeKeywords(KeywordContainer& container);

	std::string TransformKeywordsToQueryStatement(const KeywordContainer& container);

private:
	std::unique_ptr<CppSQLite3DB> m_sqlitePtr;
	bool m_isOpened = false;
	std::string m_databasePath;
	
	KeywordContainer m_keywordPairs;
};
