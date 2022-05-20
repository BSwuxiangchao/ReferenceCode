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

	// ����Ƿ��������²�ѯ���ݿ��������ؼ����������main_class(��Ʒ����)
	bool CheckRequeryCondition(const std::vector<std::string>& keywords);
private:
	// ����http�����е��β��±꣬��Ӧ���ݿ��еĲ�ѯ�ֶ�
	std::string MapIndexToField(int index);

	std::string ParseKeywordField(const std::string& keyword);

	void ParseKeywords(const std::vector<std::string>& keywords, KeywordContainer &container);

	/*
	** �Ż���ѯ���
	** 1.main_class ��ѯ��Ҫ���� sub_class���
	**/
	void OptimizeKeywords(KeywordContainer& container);

	std::string TransformKeywordsToQueryStatement(const KeywordContainer& container);

private:
	std::unique_ptr<CppSQLite3DB> m_sqlitePtr;
	bool m_isOpened = false;
	std::string m_databasePath;
	
	KeywordContainer m_keywordPairs;
};
