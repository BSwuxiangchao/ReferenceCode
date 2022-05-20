/*
 * ProductForm类包含了太多逻辑，把部分与AI端之间的交互逻辑拆分到这个文件
*/
#include "stdafx.h"
#include <fstream>
#include <string>
#include <set>
#include "Resource.h"
#include "ProductHttpService.h"
#include "ProductItem.h"
#include "SystemSettingsForm.h"
#include "LogForm.h"
#include "../Utility/ArduinoJson.h"
#include "AISpeakButtonConfiguration.h"
#include "ProductDatabaseManager.h"
#include "ProductForm.h"


void ProductForm::AppendShoppingEventListener()
{
	// 处理显示AI对话内容
	ProductHttpService::GetInstance()->AppendShoppingEventListener("aiSpeak", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIMachineSpeak(request, response);
	});

	// 处理显示AI对话内容
	ProductHttpService::GetInstance()->AppendShoppingEventListener("customerSpeak", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAICustomerSpeak(request, response);
	});

	// 处理查询商品(从商品数据库里面查找)
	ProductHttpService::GetInstance()->AppendShoppingEventListener("queryProduct", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIQueryProduct(request, response);
	});

	// 处理按多个关键字查询商品(从商品数据库里面查找)
	ProductHttpService::GetInstance()->AppendShoppingEventListener("keyword1", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIQueryProductByKeywords(request, response);
	});

	// 处理添加商品(从商品数据库里面查找)
	ProductHttpService::GetInstance()->AppendShoppingEventListener("addProduct", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIAddProduct(request, response);
	});

	// 处理减少商品数量(从购物车里面查找)
	ProductHttpService::GetInstance()->AppendShoppingEventListener("reduceProduct", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIReduceProduct(request, response);
	});

	// 处理删除商品(从购物车里面查找)
	ProductHttpService::GetInstance()->AppendShoppingEventListener("deleteProduct", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIDeleteProduct(request, response);
	});

	// 处理商品结算请求
	ProductHttpService::GetInstance()->AppendShoppingEventListener("purchaseProduct", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIPurchaseProduct(request, response);
	});

	// 处理取消结算请求
	ProductHttpService::GetInstance()->AppendShoppingEventListener("cancelPurchase", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAICancelPurchase(request, response);
	});

	// 处理取消结算请求
	ProductHttpService::GetInstance()->AppendShoppingEventListener("animation", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIAnimation(request, response);
	});

	// 显示商品介绍
	ProductHttpService::GetInstance()->AppendShoppingEventListener("introduceProduct", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIIntroduceProduct(request, response);
	});

	// 接收顾客信息
	ProductHttpService::GetInstance()->AppendShoppingEventListener("customerInfo", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessCustomerInfo(request, response);
	});

	// 接收熟客顾客信息
	ProductHttpService::GetInstance()->AppendShoppingEventListener("regularCustomer", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessRegularCustomerInfo(request, response);
	});

	// 接收开始录音信息
	ProductHttpService::GetInstance()->AppendShoppingEventListener("startRecord", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIStartVoiceRecord(request, response);
	});

	// 接收停止录音信息
	ProductHttpService::GetInstance()->AppendShoppingEventListener("stopRecord", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIStopVoiceRecord(request, response);
	});

	ProductHttpService::GetInstance()->AppendShoppingEventListener("holidayMode", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAISetHolidayMode(request, response);
	});

	ProductHttpService::GetInstance()->AppendShoppingEventListener("selectSalesman", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAISelectSalesman(request, response);
	});

	ProductHttpService::GetInstance()->AppendShoppingEventListener("sayHello", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAISayHello(request, response);
	});

	ProductHttpService::GetInstance()->AppendShoppingEventListener("selectProduct", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAISelectProduct(request, response);
	});

	ProductHttpService::GetInstance()->AppendShoppingEventListener("setNumber", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAISetProductNumber(request, response);
	});

	ProductHttpService::GetInstance()->AppendShoppingEventListener("setProductPref", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAISetProductPref(request, response);
	});

	ProductHttpService::GetInstance()->AppendShoppingEventListener("showPrompt", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIShowPrompt(request, response);
	});

	ProductHttpService::GetInstance()->AppendShoppingEventListener("luckyDraw", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAILuckyDraw(request, response);
	});

	ProductHttpService::GetInstance()->AppendShoppingEventListener("luckyDrawStop", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAILuckyDrawStop(request, response);
	});

	ProductHttpService::GetInstance()->AppendShoppingEventListener("inquiryProductCategory", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIInquiryProductCategory(request, response);
	});

	ProductHttpService::GetInstance()->AppendShoppingEventListener("productDetail", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIShowProductDetail(request, response);
	});

	ProductHttpService::GetInstance()->AppendShoppingEventListener("replaceProduct", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIReplaceProduct(request, response);
	});

	ProductHttpService::GetInstance()->AppendShoppingEventListener("wakeup", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIWakeup(request, response);
	});

	ProductHttpService::GetInstance()->AppendShoppingEventListener("enterStorageMode", [this](const httplib::Request& request, httplib::Response& response) {
		ProcessAIEnterStorageMode(request, response);
	});

	/*ProductHttpService::GetInstance()->AppendShoppingEventListener("exitStorageMode", [this](const httplib::Request& request, httplib::Response& response) {
	ProcessAIExitStorageMode(request, response);
	});*/

	ProductHttpService::GetInstance()->AppendShoppingEventListener("launchStorageDevice", [this](const httplib::Request& request, httplib::Response& response) {
		ProcessAILaunchStorageDevice(request, response);
	});
}

void ProductForm::ProcessAIMachineSpeak(const httplib::Request &request, httplib::Response &response)
{
	// http协议采用UTF-8编码
	std::string contentUTF8 = request.get_param_value("aiSpeak");
	if (contentUTF8.length() == 0)
		return;

	std::wstring content = nbase::UTF8ToUTF16(contentUTF8);

	std::string contentANSI = nbase::win32::UnicodeToMBCS(content);

	/*nbase::ThreadManager::PostTask(kThreadUI, [content, contentANSI]() {
	LogInfo("Http", "AI说话: %s", contentANSI.c_str());
	});*/
	
	SetJsonFormatHttpResponse(response, true, "请求成功");
	if (contentANSI.find("现在买单吗")!=std::string::npos)
	{
		ProcessAITriggerIsPurchase(request, response);
		nbase::ThreadManager::PostTask(kThreadUI, [content, contentANSI]() {
			LogInfo("Http", "AI说话: %s", contentANSI.c_str());
		});
	}
	RakNet::BitStream bsOut;
	RakNet::RakWString speakContent(content.c_str());
	bsOut.WriteCompressed(speakContent);

	// 向游戏客户端发送显示对话内容消息
	SignalMessage(IDShowAISpeakContent, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED);
}

void ProductForm::ProcessAICustomerSpeak(const httplib::Request &request, httplib::Response &response)
{
	// http协议采用UTF-8编码
	std::string contentUTF8 = request.get_param_value("customerSpeak");
	if (contentUTF8.length() == 0)
		return;

	std::wstring content = nbase::UTF8ToUTF16(contentUTF8);

	std::string contentANSI = nbase::win32::UnicodeToMBCS(content);

	/*nbase::ThreadManager::PostTask(kThreadUI, [content, contentANSI]() {
	LogInfo("Http", "顾客说话: %s", contentANSI.c_str());
	});*/

	SetJsonFormatHttpResponse(response, true, "请求成功");

	RakNet::BitStream bsOut;
	RakNet::RakWString speakContent(content.c_str());
	bsOut.WriteCompressed(speakContent);

	// 向游戏客户端发送显示对话内容消息
	SignalMessage(IDShowCustomerSpeakContent, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED);
}

void ProductForm::ProcessAIQueryProduct(const httplib::Request &request, httplib::Response &response)
{
	std::string dataUTF8 = request.get_param_value("queryProduct");
	if (dataUTF8.length() == 0)
		return;

	std::string productMBCS = nbase::win32::UTF8ToMBCS(dataUTF8);

	if (m_shoppingStage == ShoppingStage::PayProduct)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "接收到[查询商品 %s] 请求.查询失败, 请完成商品支付后再进行操作", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "请完成商品支付后再进行操作");

		return;
	}

	std::vector<ProductInfo> productList;
	QueryProductInDatabase(productMBCS, productList);
	if (productList.size() == 0)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "接收到 [查询商品 %s] 请求. 查询失败, 商品未上架", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "商品未上架");

		SignalStockOutEvent();

		return;
	}
	else
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogInfo("Http", "接收到 [查询商品 %s] 请求. 查询操作成功", productMBCS.c_str());
		});
	}

	SetJsonFormatHttpResponse(response, true, "查询操作成功", productList);

	std::vector<ProductInfo>::iterator it = productList.begin();
	RakNet::BitStream bsOut;
	//std::wstring dataUTF16 = nbase::UTF8ToUTF16(dataUTF8);
	std::wstring dataUTF16 = nbase::UTF8ToUTF16(dataUTF8+it->specification);
	RakNet::RakWString productOut(dataUTF16.c_str());
	bsOut.WriteCompressed(productOut);

	SignalMessage(IDQueryProduct, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED);
}

void ProductForm::ProcessAIQueryProductByKeywords(const httplib::Request &request, httplib::Response &response)
{
	std::vector<std::string> keywordsList;
	for (int i = 1; ; ++i)
	{
		auto param = nbase::StringPrintf("keyword%d", i);
		auto keyword = request.get_param_value(param.c_str());
		if (!keyword.empty())
		{
			keywordsList.push_back(keyword);
		}
		else
			break;
	}

	if (keywordsList.size() == 0)
		return;

	auto keywordsString = ArrayToString(keywordsList);
	if (m_shoppingStage == ShoppingStage::PayProduct)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [keywordsString]() {
			LogWarning("Http", "接收到[按关键字 %s 查询商品] 请求.查询失败, 请完成商品支付后再进行操作", keywordsString.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "请完成商品支付后再进行操作");

		return;
	}

	for (auto keyword : keywordsList)
	{
		auto prefix = keyword.substr(0, keyword.find_last_of('_'));
		std::string prefixMBCS = nbase::win32::UTF8ToMBCS(prefix);
		switch (CheckKeywordCategory(keyword))
		{
		case InquiryCategory::All:
			//case InquiryCategory::MainCategory:
		{
			std::set<std::string> categoryList;
			GetSubCategoryList(prefixMBCS, categoryList);

			RakNet::BitStream bs;
			RakNet::RakWString category(TEXT("商品"));
			//category = nbase::win32::MBCSToUnicode(prefixMBCS).c_str();
			bs.WriteCompressed(category);

			bs.Write(categoryList.size());
			for (auto item : categoryList)
			{
				RakNet::RakWString subCategory = nbase::win32::MBCSToUnicode(item).c_str();
				bs.WriteCompressed(subCategory);
			}

			SignalMessage(IDInquiryProductCategory, &bs, PacketPriority::HIGH_PRIORITY, RELIABLE);

			SetJsonFormatHttpResponse(response, true, "操作成功");

			return;
		}
		}
	}

	std::vector<ProductInfo> list;

	if (ProductDatabaseManager::GetInstance()->CheckRequeryCondition(keywordsList))
	{
		// 直接从数据库中查询
		ProductDatabaseManager::GetInstance()->QueryProductFromDatabase(keywordsList, list, false);
	}
	else
	{
		// 如果之前的查询列表非空
		if (m_productSearchResultList.size() > 0)
		{
			// 从数据库View里面查询商品
			ProductDatabaseManager::GetInstance()->QueryProductFromDatabase(keywordsList, list, true);

			//View中找不到商品
			if (list.size() == 0)
			{
				// 从数据库中查找
				ProductDatabaseManager::GetInstance()->QueryProductFromDatabase(keywordsList, list, false);
			}
		}
		else // 如果之前的查询列表为空，从数据库里面查询商品
		{
			ProductDatabaseManager::GetInstance()->QueryProductFromDatabase(keywordsList, list, false);
		}
	}

	auto count = list.size();
	if (count == 0) // 商品未上架
	{
		nbase::ThreadManager::PostTask(kThreadUI, [keywordsString]() {
			LogWarning("Http", "接收到 [按关键字 %s 查询商品] 请求. 查询失败, 商品未上架", keywordsString.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "商品未上架");

		SignalStockOutEvent();

		return;
	}

	nbase::ThreadManager::PostTask(kThreadUI, [keywordsString, count]() {
		LogInfo("Http", "接收到 [按关键字 %s 查询商品] 请求. 查询操作成功, 共查询到 %d 种商品", keywordsString.c_str(), count);
	});

	// 检查是否需要更新Search List
	bool containsInSearchList = true;
	for (auto& item1 : list)
	{
		containsInSearchList = false;

		for (auto& item2 : m_productSearchResultList)
		{
			if (item2.id == item1.id)
			{
				containsInSearchList = true;
				break;
			}
		}

		if (!containsInSearchList)
			break;
	}

	if (!containsInSearchList || count > 1)
	{
		// 更新商品查询结果列表
		m_productSearchResultList.clear();
		m_productSearchResultList.assign(list.begin(), list.end());

		// 替换商品Database View
		ProductDatabaseManager::GetInstance()->ReplaceProductSearchListDatabaseView(keywordsList);

		SignalRefreshSearchListEvent(list);
	}

	SetJsonFormatHttpResponse(response, true, "查询操作成功", list);
}

void ProductForm::ProcessAIAddProduct(const httplib::Request &request, httplib::Response &response)
{
	// 解决AI同时添加多种商品时，互动游戏端处理不过来的问题
	double elapsedTime = (std::clock() - m_lastAIAddProductTime) / (double)CLOCKS_PER_SEC;
	if (elapsedTime <= 0.15)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
	}
	m_lastAIAddProductTime = std::clock();

	std::string productUTF8 = request.get_param_value("addProduct");
	if (productUTF8.length() == 0)
		return;

	std::string productMBCS = nbase::win32::UTF8ToMBCS(productUTF8); // product字符串 可以是商品名称、商品种类、商品描述

	if (m_shoppingStage == ShoppingStage::PayProduct)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "接收到[添加商品 %s] 请求.商品添加失败, 请完成商品支付后再进行操作", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "请完成商品支付后再进行操作");

		return;
	}

	PRODUCT_ARRAY list;
	GetProductInfoListByNameInSearchResult(productMBCS, list);
	if (list.size() == 0)
	{
		GetProductInfoListByDescriptionInSearchResult(productMBCS, list);
	}

	if (list.size() == 0)
	{
		GetProductInfoListByNameInDatabse(productMBCS, list);
		if (list.size() == 0)
			GetProductInfoListByDescriptionInDatabse(productMBCS, list);
	}

	auto count = list.size();
	if (count == 0)
	{
		std::string message = nbase::StringPrintf("接收到[添加商品 %s] 请求.商品添加失败, 商品未上架", productMBCS.c_str());
		nbase::ThreadManager::PostTask(kThreadUI, [message]() {
			LogWarning("Http", message.c_str());
		});

		SignalStockOutEvent();

		SetJsonFormatHttpResponse(response, false, "商品未上架");

		return;
	}
	else if (count > 1) // 找到了多种商品
	{

		std::string message = nbase::StringPrintf("接收到[添加商品 %s] 请求. 共找到 %d 种商品", productMBCS.c_str(), count);
		nbase::ThreadManager::PostTask(kThreadUI, [message]() {
			LogWarning("Http", message.c_str());
		});

		SetJsonFormatHttpResponse(response, true, "找到多种商品", list);

		return;
	}
	else // 找到指定的一种商品
	{
		unsigned int id = list[0].id;

		int number = 0;
		if (request.has_param("number"))
		{
			auto value = request.get_param_value("number");
			nbase::StringToInt(value, &number);
		}

		if (number == 0)
		{
			number = 1;
			nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
				LogWarning("Http", "接收到 [添加商品 %s, 数量 0] 请求; 数量默认改为1", productMBCS.c_str());
			});
		}

		if (!CheckProductStock(id, number))
		{
			nbase::ThreadManager::PostTask(kThreadUI, [productMBCS, number]() {
				LogError("Http", "接收到 [添加商品 %s, 数量 %d] 请求. 商品添加失败, 商品数量已经超过库存数量", productMBCS.c_str(), number);
			});

			SignalStockOutEvent();

			SetJsonFormatHttpResponse(response, false, "商品购买数量已经超过库存数量");

			return;
		}

		auto result = AddOrIncreaseProductNumInShoppingList(id, number);
		SetJsonFormatHttpResponse(response, true, "商品已经成功添加到购物车", { *result });

		nbase::ThreadManager::PostTask(kThreadUI, [id, number, this]() {

			RefreshShoppingListBox();

			SignalRefreshShoppingListEvent(m_shoppingList);

			DumpProductAmountMessage(id);
		});
	}


}

void ProductForm::ProcessAIReduceProduct(const httplib::Request &request, httplib::Response &response)
{
	std::string dataUTF8 = request.get_param_value("reduceProduct");
	if (dataUTF8.length() == 0)
		return;

	std::string productMBCS = nbase::win32::UTF8ToMBCS(dataUTF8);

	if (m_shoppingStage == ShoppingStage::PayProduct)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "接收到[修改商品 %s] 请求. 商品修改失败, 请完成商品支付后再进行操作", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "请完成商品支付后再进行操作");
		return;
	}

	PRODUCT_ARRAY list;
	GetProductInfoListByNameInShoppingList(productMBCS, list);
	if (list.size() == 0)
	{
		GetProductInfoListByDescriptionInShoppingList(productMBCS, list);
	}

	auto count = list.size();
	if (count == 0)
	{
		std::string message = nbase::StringPrintf("接收到[修改商品 %s] 请求. 购物车中找不到商品", productMBCS.c_str());
		nbase::ThreadManager::PostTask(kThreadUI, [message]() {
			LogWarning("Http", message.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "购物车中找不到商品");

		return;
	}
	else if (count > 1)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [=]() {
			LogInfo("Http", "接收到[修改商品 %s] 请求. 共找到 %d 种商品", productMBCS.c_str(), count);
		});

		SetJsonFormatHttpResponse(response, true, "找到多种商品", list);
		return;
	}
	else
	{
		unsigned int productID = list[0].id;

		int number = 1; // 获取到的number有正有负：正表示增加商品数量；负表示减少商品数量
		if (request.has_param("number"))
		{
			auto value = request.get_param_value("number");
			nbase::StringToInt(value, &number);
		}

		if (number == 0)
		{
			SetJsonFormatHttpResponse(response, false, "操作失败", list);
			nbase::ThreadManager::PostTask(kThreadUI, [productMBCS, number, productID]() {
				LogWarning("Http", "接收到 [修改商品 %s, 数量 %d] 请求. 修改商品数量0失败", productMBCS.c_str(), number);
			});
			return;
		}

		// number > 0 增加商品数量
		if (number > 0)
		{
			auto result = IncreaseProductNumInShoppingList(productID, number);
			if (result != nullptr)
			{
				ProductInfo info = *result;
				PRODUCT_ARRAY array;
				array.push_back(info);

				SetJsonFormatHttpResponse(response, true, "操作成功", array);
			}
			else
			{
				SetJsonFormatHttpResponse(response, true, "操作成功", list);
			}
		}
		else
		{
			number = -number;

			auto result = ReduceProductNumInShoppingList(productID, number);
			SetJsonFormatHttpResponse(response, true, "操作成功", { *result });

		}

		// 刷新客户端购物车
		SignalRefreshShoppingListEvent(m_shoppingList);

		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS, number, productID]() {
			LogInfo("Http", "接收到 [修改商品 %s, 数量 %d] 请求. 修改商品数量成功", productMBCS.c_str(), number);

			ProductForm::GetInstance()->RefreshShoppingListBox();

			ProductForm::GetInstance()->DumpProductAmountMessage(productID);
		});
	}

}

void ProductForm::ProcessAIDeleteProduct(const httplib::Request &request, httplib::Response &response)
{
	std::string dataUTF8 = request.get_param_value("deleteProduct");
	if (dataUTF8.length() == 0)
		return;

	std::string productMBCS = nbase::win32::UTF8ToMBCS(dataUTF8);
	if (m_shoppingStage == ShoppingStage::PayProduct)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "接收到[移除商品 %s] 请求. 商品移除失败, 请完成商品支付后再进行操作", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "请完成商品支付后再进行操作");
		return;
	}

	PRODUCT_ARRAY list;
	GetProductInfoListByNameInShoppingList(productMBCS, list);
	if (list.size() == 0)
	{
		GetProductInfoListByDescriptionInShoppingList(productMBCS, list);
	}

	auto count = list.size();
	if (count == 0)
	{
		std::string message = nbase::StringPrintf("接收到 [移除商品 %s] 请求. 商品移除失败, 购物车中找不到商品", productMBCS.c_str());
		nbase::ThreadManager::PostTask(kThreadUI, [message]() {
			LogWarning("Http", message.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "购物车中找不到商品");

		return;
	}
	else if (count > 1)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [=]() {
			LogInfo("Http", "接收到[移除商品 %s] 请求. 共找到 %d 种商品", productMBCS.c_str(), count);
		});

		SetJsonFormatHttpResponse(response, true, "找到多种商品", list);
		return;
	}
	else
	{
		SetJsonFormatHttpResponse(response, true, "操作成功", list);

		nbase::ThreadManager::PostTask(kThreadUI, [=]() {
			LogInfo("Http", "接收到 [移除商品 %s] 请求. 操作成功", productMBCS.c_str());

			unsigned int productID = list[0].id;
			RemoveProductFromShoppingList(productID);

			// 刷新客户端购物车
			SignalRefreshShoppingListEvent(m_shoppingList);

			RefreshShoppingListBox();

			ProductForm::GetInstance()->DumpProductAmountMessage(productID);
		});
	}
}


void ProductForm::ProcessAIIntroduceProduct(const httplib::Request &request, httplib::Response &response)
{
	std::string dataUTF8 = request.get_param_value("introduceProduct");
	if (dataUTF8.length() == 0)
		return;

	std::wstring productUTF16 = nbase::UTF8ToUTF16(dataUTF8);

	std::string productMBCS = nbase::win32::UnicodeToMBCS(productUTF16);

	std::string idUTF8 = request.get_param_value("id");
	int id = 0;
	nbase::StringToInt(idUTF8, &id);

	if (m_shoppingStage == ShoppingStage::PayProduct)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "接收到[显示商品 %s 介绍] 请求. 请求失败, 请完成商品支付后再进行操作", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "请完成商品支付后再进行操作");

		return;
	}

	auto productInfo = GetProductInfoByNameInDatabse(productMBCS);
	if (productInfo == nullptr)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "接收到 [显示商品 %s 介绍] 请求. 请求操作失败,商品未上架", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "商品未上架");

		return;
	}
	else
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogInfo("Http", "接收到 [显示商品 %s 介绍] 请求. 请求操作成功", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, true, "操作成功", { *productInfo });

		RakNet::BitStream bsOut;

		auto productName = nbase::win32::MBCSToUnicode(productInfo->name);
		RakNet::RakWString productOut(productName.c_str());
		bsOut.WriteCompressed(productOut);

		bsOut.Write(id);

		m_pRakServer->Signal(IDShowProductIntroduction, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
	}


}

void ProductForm::ProcessAIPurchaseProduct(const httplib::Request &request, httplib::Response &response)
{
	if (m_shoppingList.size() == 0)
	{
		nbase::ThreadManager::PostTask(kThreadUI, []() {
			LogWarning("Http", "接收到 [商品结算付款] 请求. 请求被忽略, 购物车已经为空");
		});

		SetJsonFormatHttpResponse(response, false, "购物车中没有商品, 请求付款失败");
	}
	else
	{
		// 启动付款倒计时，处理 WM_TIMER 消息
		SetPaymentCountdownTimer();

		m_shoppingStage = ShoppingStage::PayProduct;

		nbase::ThreadManager::PostTask(kThreadUI, []() {
			LogInfo("Http", "接收到 [商品结算付款] 请求");
		});

		std::string payUrl = OnDealUploadOrder();
		if (payUrl == "null")
		{
			LogInfo("Button", "获取支付链接失败。。");
			SetJsonFormatHttpResponse(response, false, "获取支付链接失败, 无法支付");
			KillPaymentCountdownTimer();

			std::string tipUTF8 = nbase::StringPrintf("speak=%s", nbase::win32::MBCSToUTF8("获取支付链接失败,无法付款"));
			ProductHttpService::GetInstance()->PostAIRequest(tipUTF8);
			m_shoppingStage = ShoppingStage::PickProduct;
		}
		else
		{
			SetJsonFormatHttpResponse(response, true, "请求付款操作成功");
			RakNet::BitStream bsOut;
			RakNet::RakWString RStrName;
			RStrName = nbase::win32::MBCSToUnicode(payUrl).c_str();
			bsOut.WriteCompressed(RStrName);
			m_pRakServer->Signal(IDProductSettlement, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
		}
	}
}

void ProductForm::ProcessAICancelPurchase(const httplib::Request &request, httplib::Response &response)
{
	// 取消支付，恢复商品在库存中的数量
	for (auto& item : m_shoppingList)
	{
		auto info = GetProductInfoByIdInDatabse(item.id);
		if (info)
		{
			info->amount += item.count;
		}
	}

	if (m_shoppingList.size() > 0)
	{
		m_shoppingStage = ShoppingStage::PickProduct;

		ProductHttpService::GetInstance()->PostAIRequest("cancelPayment");

		SetJsonFormatHttpResponse(response, true, "取消付款操作成功");

		nbase::ThreadManager::PostTask(kThreadUI, [=]() {
			LogInfo("Http", "接收到 [取消商品结算付款] 请求. 取消付款操作成功");

			KillPaymentCountdownTimer();

			m_shoppingList.clear();
			m_shoppingListBox->RemoveAll();

			m_productSearchResultList.clear();

			SignalRefreshShoppingListEvent(m_shoppingList);

			SignalRefreshSearchListEvent(m_productSearchResultList);

			RakNet::BitStream bsOut;
			m_pRakServer->Signal(IDCancelPurchase, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
		});

	}
	else
	{
		nbase::ThreadManager::PostTask(kThreadUI, []() {
			LogInfo("Http", "接收到 [取消商品结算付款] 请求. 购物车为空,取消付款操作失败");
		});

		SetJsonFormatHttpResponse(response, false, "购物车为空,取消付款操作失败");
	}
}

void ProductForm::ProcessAIAnimation(const httplib::Request &request, httplib::Response &response)
{
	std::string dataUTF8 = request.get_param_value("animation");
	if (dataUTF8.length() == 0)
		return;

	std::string dataMBCS = nbase::win32::UTF8ToMBCS(dataUTF8);
	nbase::ThreadManager::PostTask(kThreadUI, [dataMBCS]() {
		LogInfo("Http", "接收到 [播放动画%s] 请求. 操作成功", dataMBCS.c_str());
	});

	SetJsonFormatHttpResponse(response, true, "操作成功");

	std::wstring dataUTF16 = nbase::UTF8ToUTF16(dataUTF8);

	RakNet::BitStream bsOut;
	RakNet::RakWString buffer(dataUTF16.c_str());
	m_pRakServer->Signal(IDPlayAnimation, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}

void ProductForm::ProcessAIStartVoiceRecord(const httplib::Request &request, httplib::Response &response)
{
	SetJsonFormatHttpResponse(response, true, "操作成功");

	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDStartVoiceRecord, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}

void ProductForm::ProcessAIStopVoiceRecord(const httplib::Request &request, httplib::Response &response)
{
	SetJsonFormatHttpResponse(response, true, "操作成功");

	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDStopVoiceRecord, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}

void ProductForm::ProcessAISetHolidayMode(const httplib::Request &request, httplib::Response &response)
{
	std::string holidayUTF8 = request.get_param_value("holidayMode");
	if (holidayUTF8.length() == 0)
		return;

	std::string holidayMBCS = nbase::win32::UTF8ToMBCS(holidayUTF8);
	nbase::ThreadManager::PostTask(kThreadUI, [holidayMBCS]() {
		LogInfo("Http", "接收到 [切换节假日氛围 %s] 请求. 操作成功", holidayMBCS.c_str());
	});

	SetJsonFormatHttpResponse(response, true, "操作成功");

	std::wstring holidayUTF16 = nbase::UTF8ToUTF16(holidayUTF8);

	RakNet::BitStream bsOut;
	RakNet::RakWString buffer(holidayUTF16.c_str());
	bsOut.WriteCompressed(buffer);

	m_pRakServer->Signal(IDSetHolidayMode, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}

void ProductForm::ProcessAISelectSalesman(const httplib::Request &request, httplib::Response &response)
{
	std::string salesmanUTF8 = request.get_param_value("selectSalesman");
	if (salesmanUTF8.length() == 0)
		return;

	std::string salesmanMBCS = nbase::win32::UTF8ToMBCS(salesmanUTF8);
	nbase::ThreadManager::PostTask(kThreadUI, [salesmanMBCS]() {
		LogInfo("Http", "接收到 [选择导购员 %s] 请求. 操作成功", salesmanMBCS.c_str());
	});

	SetJsonFormatHttpResponse(response, true, "操作成功");

	std::wstring salesmanUTF16 = nbase::UTF8ToUTF16(salesmanUTF8);

	RakNet::BitStream bsOut;
	RakNet::RakWString buffer(salesmanUTF16.c_str());
	bsOut.WriteCompressed(buffer);

	m_pRakServer->Signal(IDSelectSalesman, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}

bool ProductForm::ProcessAISelectProduct(const httplib::Request &request, httplib::Response &response)
{
	std::string indexUTF8 = request.get_param_value("selectProduct");
	if (indexUTF8.length() == 0)
		return false;

	int length = m_productSearchResultList.size();


	int index = 0;
	nbase::StringToInt(indexUTF8, &index);

	int row = 0;
	if (request.has_param("row"))
	{
		auto value = request.get_param_value("row");
		nbase::StringToInt(value, &row);
	}

	int selectedProductIndex = CalcProductIndex(row, index);

	if (selectedProductIndex >= length || selectedProductIndex < 0)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [selectedProductIndex]() {
			LogError("Http", "接收到 [选择第 %d 项商品] 请求. 操作失败, 商品索引错误.", selectedProductIndex + 1);
		});

		SetJsonFormatHttpResponse(response, false, "操作失败,商品索引错误");

		return false;
	}

	auto selectedProduct = m_productSearchResultList[selectedProductIndex];
	int number = 1;
	if (request.has_param("number"))
	{
		auto value = request.get_param_value("number");
		nbase::StringToInt(value, &number);
	}

	if (!CheckProductStock(selectedProduct.id, number))
	{
		nbase::ThreadManager::PostTask(kThreadUI, [selectedProductIndex]() {
			LogError("Http", "接收到 [选择第 %d 项商品] 请求. 操作失败, 商品数量大于库存.", selectedProductIndex + 1);
		});

		SetJsonFormatHttpResponse(response, false, "操作失败,商品数量大于库存");

		return false;
	}

	std::vector<ProductInfo> list;
	list.push_back(selectedProduct);

	auto result = AddOrIncreaseProductNumInShoppingList(selectedProduct.id, number);
	SetJsonFormatHttpResponse(response, true, "操作成功", { *result });

	nbase::ThreadManager::PostTask(kThreadUI, [=]() {

		LogInfo("Http", "接收到 [选择第 %d 项商品] 请求. 请求成功, 商品 %s 成功添加到购物车", selectedProductIndex + 1, selectedProduct.name.c_str());

		//AddProductToShoppingListOrRefreshList(selectedProduct.id, number);
		RefreshShoppingListBox();

		SignalRefreshShoppingListEvent(m_shoppingList);

		DumpProductAmountMessage(selectedProduct.id);
	});

	return true;
}

bool ProductForm::ProcessAISetProductNumber(const httplib::Request &request, httplib::Response &response)
{
	std::string dataUTF8 = request.get_param_value("setNumber");
	if (dataUTF8.length() == 0)
		return false;

	std::string productMBCS = nbase::win32::UTF8ToMBCS(dataUTF8);

	if (m_shoppingStage == ShoppingStage::PayProduct)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "接收到[设置商品 %s 数量] 请求. 设置商品数量失败, 请完成商品支付后再进行操作", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "操作失败,请完成商品支付后再进行操作");

		return false;
	}

	std::vector<ProductInfo> list;
	GetProductInfoListByNameInShoppingList(productMBCS, list);
	if (list.size() == 0)
	{
		GetProductInfoListByDescriptionInShoppingList(productMBCS, list);
	}

	if (list.size() == 0)
	{
		std::string message = nbase::StringPrintf("接收到[设置商品 %s 数量] 请求. 购物车中找不到商品", productMBCS.c_str());
		nbase::ThreadManager::PostTask(kThreadUI, [message]() {
			LogError("Http", message.c_str());
		});

		SetJsonFormatHttpResponse(response, false, message);

		return false;
	}
	else if (list.size() > 1)
	{
		// 找到多种符合要求的商品
		std::string message = nbase::StringPrintf("接收到[设置商品 %s 数量] 请求. 购物车中找到了 %d 种商品", productMBCS.c_str(), list.size());
		nbase::ThreadManager::PostTask(kThreadUI, [message]() {
			LogInfo("Http", message.c_str());
		});

		SetJsonFormatHttpResponse(response, true, message, list);

		return true;
	}
	else
	{
		// 已经找到想要的商品
		int number = 1;
		if (request.has_param("number"))
		{
			auto value = request.get_param_value("number");
			nbase::StringToInt(value, &number);
		}

		unsigned int productID = list[0].id;
		if (list[0].count + list[0].amount < number)
		{
			nbase::ThreadManager::PostTask(kThreadUI, [productMBCS, number, productID]() {
				LogError("Http", "接收到[设置商品 %s 数量 %d] 请求. 请求失败, 商品数量大于总库存", productMBCS.c_str(), number);
			});

			SetJsonFormatHttpResponse(response, false, "请求失败,商品数量大于库存");

			return false;
		}

		auto result = SetProductNumInShoppingList(productID, number);
		SetJsonFormatHttpResponse(response, true, "操作成功", { *result });

		nbase::ThreadManager::PostTask(kThreadUI, [=]() {
			for (int i = 0; i < m_shoppingListBox->GetCount(); ++i)
			{
				auto item = dynamic_cast<ProductItem *>(m_shoppingListBox->GetItemAt(i));
				if (item->GetItemID() == productID)
				{
					item->SetProductNumber(number);
					break;
				}
			}

			LogInfo("Http", "接收到[设置商品 %s 数量 %d] 请求. 设置商品数量成功", productMBCS.c_str(), number);

			ProductForm::GetInstance()->DumpProductAmountMessage(productID);

			// 刷新客户端购物车
			SignalRefreshShoppingListEvent(m_shoppingList);
		});

		return true;
	}
}

bool ProductForm::ProcessAISetProductPref(const httplib::Request &request, httplib::Response &response)
{
	std::string dataUTF8 = request.get_param_value("setProductPref");
	std::string prefUTF8 = request.get_param_value("pref");
	if (dataUTF8.length() == 0 || prefUTF8.length() == 0)
	{
		nbase::ThreadManager::PostTask(kThreadUI, []() {
			LogWarning("Http", "接收到[设置商品偏好] 请求. 设置商品偏好失败, 参数错误");
		});

		SetJsonFormatHttpResponse(response, false, "操作失败,参数错误");
		return false;
	}

	std::string productMBCS = nbase::win32::UTF8ToMBCS(dataUTF8);
	std::string prefMBCS = nbase::win32::UTF8ToMBCS(prefUTF8);

	if (m_shoppingStage == ShoppingStage::PayProduct)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "接收到[设置商品 %s 偏好] 请求. 设置商品偏好失败, 请完成商品支付后再进行操作", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "操作失败,请完成商品支付后再进行操作");

		return false;
	}

	auto productInfo = GetProductInfoByNameInShoppingList(productMBCS);
	if (productInfo == nullptr)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogError("Http", "接收到[设置商品 %s 偏好] 请求. 请求失败, 购物车中找不到商品", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "请求失败,购物车中找不到商品");
		return false;
	}

	PRODUCT_ARRAY list;
	GetProductInfoListByNameInDatabse(productMBCS, list);
	for (auto& item : list)
	{
		if (item.keywords.find(prefMBCS) != std::string::npos)
		{
			*productInfo = item;
			m_shoppingStage = ShoppingStage::PickProduct;

			nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
				LogInfo("Http", "接收到[设置商品 %s 偏好] 请求. 请求成功", productMBCS.c_str());
			});

			SetJsonFormatHttpResponse(response, true, "操作成功", { item });

			// 刷新客户端购物车
			SignalRefreshShoppingListEvent(m_shoppingList);

			return true;
		}
	}

	SetJsonFormatHttpResponse(response, false, "操作失败,数据库中找不到相应偏好商品");
	return false;
}

bool ProductForm::ProcessAIShowPrompt(const httplib::Request &request, httplib::Response &response)
{
	std::string promptUTF8 = request.get_param_value("showPrompt");

	/*std::string promptMBCS = nbase::win32::UTF8ToMBCS(promptUTF8);
	nbase::ThreadManager::PostTask(kThreadUI, [promptMBCS]() {
	LogInfo("Http", "接收到[展示应答提示 %s ] 请求. ", promptMBCS.c_str());
	});*/

	std::string yesUTF8 = request.get_param_value("yes");
	std::string noUTF8 = request.get_param_value("no");

	RakNet::BitStream stream;

	RakNet::RakWString prompt(nbase::UTF8ToUTF16(promptUTF8).c_str());
	RakNet::RakWString yes(nbase::UTF8ToUTF16(yesUTF8).c_str());
	RakNet::RakWString no(nbase::UTF8ToUTF16(noUTF8).c_str());

	stream.WriteCompressed(prompt);
	stream.WriteCompressed(yes);
	stream.WriteCompressed(no);

	SignalMessage(IDShowPrompt, &stream, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED);

	SetJsonFormatHttpResponse(response, true, "操作成功");

	return true;
}

bool ProductForm::ProcessAILuckyDraw(const httplib::Request &request, httplib::Response &response)
{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Http", "接收到[开始抽奖] 请求. ");
	});

	std::string drawType = request.get_param_value("type");
	int type = 0;
	nbase::StringToInt(drawType, &type);

	std::vector<RakNet::RakWString> prizeOptions;
	prizeOptions.push_back(TEXT("农夫山泉"));
	prizeOptions.push_back(TEXT("谢谢参与"));

	auto size = prizeOptions.size();

	srand((unsigned)time(NULL));
	int index = rand() % size;
	RakNet::RakWString prize = prizeOptions[index];

	RakNet::BitStream stream;
	stream.Write(type);
	stream.WriteCompressed(prize);

	stream.Write(size);
	for (auto item : prizeOptions)
	{
		stream.WriteCompressed(item);
	}

	SignalMessage(IDLuckyDraw, &stream, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED);

	PRODUCT_ARRAY list;
	std::string productName = nbase::win32::UnicodeToMBCS(prize.C_String());
	GetProductInfoListByNameInDatabse(productName, list);
	if (list.size() > 0)
	{
		m_prize = list[0];
		m_prize.count = 1;
		m_prize.amount = 1;
		SetJsonFormatHttpResponse(response, true, "操作成功");
	}
	else
	{
		m_prize = ProductInfo();
		SetJsonFormatHttpResponse(response, false, "谢谢参与");
	}

	/*float timeBegin = GetTickCount();
	while (GetTickCount() - timeBegin <= LuckyDrawTimeLimit)
	{
	if (m_luckyDrawStopped)
	{
	break;
	}
	else
	{
	std::this_thread::sleep_for(std::chrono::microseconds(10));
	}
	}

	if (!m_luckyDrawStopped)
	{
	if (m_prize.id != 0)
	{
	std::string name = m_prize.name;
	nbase::ThreadManager::PostTask(kThreadUI, [name]() {
	LogInfo("Http", "10秒时间已到，自动停止抽奖。抽中奖品%s. ", name.c_str());
	});

	SetJsonFormatHttpResponse(response, true, "操作成功", { m_prize });

	RequestForShipment({ m_prize });
	}
	else
	{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
	LogInfo("Http", "10秒时间已到，自动停止抽奖。未抽中奖品, 谢谢参与. ");
	});

	SetJsonFormatHttpResponse(response, false, "谢谢参与");
	}
	}*/

	return true;
}

bool ProductForm::ProcessAILuckyDrawStop(const httplib::Request &request, httplib::Response &response)
{
	std::string drawDelay = request.get_param_value("delay");
	double delay = 0;
	nbase::StringToDouble(drawDelay, &delay);

	RakNet::BitStream stream;

	stream.Write((float)delay);

	SignalMessage(IDLuckyDrawStop, &stream, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED);

	m_luckyDrawStopped = true;

	if (m_prize.id != 0)
	{
		std::string name = m_prize.name;
		nbase::ThreadManager::PostTask(kThreadUI, [name]() {
			LogInfo("Http", "接收到[停止抽奖] 请求, 抽中奖品%s. ", name.c_str());
		});

		SetJsonFormatHttpResponse(response, true, "操作成功", { m_prize });

		RequestForShipment({ m_prize });
	}
	else
	{
		nbase::ThreadManager::PostTask(kThreadUI, []() {
			LogInfo("Http", "接收到 [停止抽奖] 请求, 未抽中奖品, 谢谢参与. ");
		});

		SetJsonFormatHttpResponse(response, false, "谢谢参与");
	}

	return true;
}

bool ProductForm::ProcessAIInquiryProductCategory(const httplib::Request &request, httplib::Response &response)
{
	std::string categoryUTF8 = request.get_param_value("category");

	std::string categoryMBCS = nbase::win32::UTF8ToMBCS(categoryUTF8);
	nbase::ThreadManager::PostTask(kThreadUI, [categoryMBCS]() {
		LogInfo("Http", "接收到[询问商品类型 %s ] 请求. ", categoryMBCS.c_str());
	});

	std::set<std::string> categoryList;
	switch (CheckCategory(categoryMBCS))
	{
	case InquiryCategory::All:
	case InquiryCategory::MainCategory:
		GetSubCategoryList(categoryMBCS, categoryList);
		break;
	case InquiryCategory::SubCategory:
		break;
	default:
		break;
	}

	SetJsonFormatHttpResponse(response, true, "操作成功");

	return true;

}

bool ProductForm::ProcessAIShowProductDetail(const httplib::Request &request, httplib::Response &response)
{
	std::string dataUTF8 = request.get_param_value("productDetail");
	if (dataUTF8.length() == 0)
		return false;

	std::wstring productUTF16 = nbase::UTF8ToUTF16(dataUTF8);

	std::string productMBCS = nbase::win32::UnicodeToMBCS(productUTF16);

	std::string numberUTF8 = request.get_param_value("number");
	int number = 0;
	nbase::StringToInt(numberUTF8, &number);
	if (number == 0)
	{
		number = 1;
	}

	if (m_shoppingStage == ShoppingStage::PayProduct)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "接收到[显示商品 %s 详情] 请求. 请求失败, 请完成商品支付后再进行操作", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "请完成商品支付后再进行操作");

		return false;
	}

	auto productInfo = GetProductInfoByNameInDatabse(productMBCS);
	if (productInfo == nullptr)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "接收到 [显示商品 %s 详情] 请求. 请求操作失败,商品未上架", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "商品未上架");

		return false;
	}
	else
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogInfo("Http", "接收到 [显示商品 %s 详情] 请求. 请求操作成功", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, true, "操作成功", { *productInfo });

		RakNet::BitStream bsOut;

		auto productName = nbase::win32::MBCSToUnicode(productInfo->name);
		RakNet::RakWString productOut(productName.c_str());
		bsOut.WriteCompressed(productOut);

		bsOut.Write(number);

		m_pRakServer->Signal(IDShowProductDetail, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
	}

	return true;
}

bool ProductForm::ProcessAIReplaceProduct(const httplib::Request &request, httplib::Response &response)
{
	std::string oldUTF8 = request.get_param_value("old");
	if (oldUTF8.length() == 0)
		return false;

	std::wstring oldUTF16 = nbase::UTF8ToUTF16(oldUTF8);
	std::string oldProductMBCS = nbase::win32::UnicodeToMBCS(oldUTF16);

	if (m_shoppingStage == ShoppingStage::PayProduct)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [oldProductMBCS]() {
			LogWarning("Http", "接收到[显示商品 %s 详情] 请求. 请求失败, 请完成商品支付后再进行操作", oldProductMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "请完成商品支付后再进行操作");

		return false;
	}

	auto oldProductInfo = GetProductInfoByNameInShoppingList(oldProductMBCS);
	if (oldProductInfo == nullptr)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [oldProductMBCS]() {
			LogWarning("Http", "接收到 [替换商品 %s ] 请求. 请求操作失败,购物车中找不到商品", oldProductMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "购物车中找不到商品");

		return false;
	}
	else
	{
		std::string newUTF8 = request.get_param_value("new");
		std::wstring newUTF16 = nbase::UTF8ToUTF16(newUTF8);
		std::string newProductMBCS = nbase::win32::UnicodeToMBCS(newUTF16);
		auto newProductInfo = GetProductInfoByNameInDatabse(newProductMBCS);
		if (newProductInfo == nullptr)
		{
			nbase::ThreadManager::PostTask(kThreadUI, [newProductMBCS]() {
				LogInfo("Http", "接收到 [替换商品 %s ] 请求. 请求操作失败,数据库中找不到替换的商品", newProductMBCS.c_str());
			});

			SetJsonFormatHttpResponse(response, false, "数据库中找不到替换的商品");
			return false;
		}

		RemoveProductFromShoppingList(oldProductInfo->id);
		AddProductToShoppingListOrRefreshList(newProductInfo->id);

		nbase::ThreadManager::PostTask(kThreadUI, [oldProductMBCS]() {
			LogInfo("Http", "接收到 [替换商品 %s ] 请求. 请求操作成功", oldProductMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, true, "操作成功", { *newProductInfo });

		RefreshShoppingListBox();
		SignalRefreshShoppingListEvent(m_shoppingList);
	}

	return true;
}

bool ProductForm::ProcessAIWakeup(const httplib::Request & request, httplib::Response & response)
{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Http", "接收到 [唤醒导购员] 消息.");
	});

	SetJsonFormatHttpResponse(response, true, "操作成功");

	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDStartGame, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}

bool ProductForm::ProcessAIEnterStorageMode(const httplib::Request& request, httplib::Response& response)
{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Http", "接收到 [进入上货模式] 消息.");
	});

	SetJsonFormatHttpResponse(response, true, "操作成功");

	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDEnterStorageMode, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}

bool ProductForm::ProcessAILaunchStorageDevice(const httplib::Request& request, httplib::Response& response)
{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Http", "接收到 [启动上货设备] 消息.");
	});

	SetJsonFormatHttpResponse(response, true, "操作成功");

	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDLaunchStorageDevice, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
	return true;
}

void ProductForm::ProcessAISayHello(const httplib::Request &request, httplib::Response &response)
{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Http", "接收到 [打招呼] 请求. 操作成功");
	});

	SetJsonFormatHttpResponse(response, true, "操作成功");

	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDSayHello, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}

bool ProductForm::ProcessAIGetGoodsListType(const httplib::Request &request, httplib::Response &response)
{
	const int buffer_size = 4096;
	char buffer[buffer_size] = { 0 };
	StaticJsonDocument<buffer_size> doc;

	doc["success"] = true;
	doc["msg"] = "操作成功";

	std::set<std::string> categorySet;
	GetMainCategoryList(categorySet);
	doc["count"] = categorySet.size();
	int i = 0;
	for (auto iter = categorySet.begin(); iter != categorySet.end(); ++iter, ++i)
	{
		doc["category"][i] = *iter;
	}

	serializeJson(doc, buffer, buffer_size);

	// 需要把多字节编码的buffer转为UTF-8编码
	std::string bufferUTF8 = nbase::win32::MBCSToUTF8(buffer);

	response.set_content(bufferUTF8, "application/json");

	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Http", "接收到AI端 [获取商品类型列表] 请求. 操作成功");
	});

	return true;
}

bool ProductForm::ProcessAIShowSearchListUI(const httplib::Request &request, httplib::Response &response)
{
	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDShowSearchListUI, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}

bool ProductForm::ProcessAIHideSearchListUI(const httplib::Request &request, httplib::Response &response)
{
	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDHideSearchListUI, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}

bool ProductForm::ProcessAITriggerIsPurchase(const httplib::Request &request, httplib::Response &response)
{
	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDIsPurchase, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}