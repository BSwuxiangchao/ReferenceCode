/*
 * ProductForm�������̫���߼����Ѳ�����AI��֮��Ľ����߼���ֵ�����ļ�
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
	// ������ʾAI�Ի�����
	ProductHttpService::GetInstance()->AppendShoppingEventListener("aiSpeak", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIMachineSpeak(request, response);
	});

	// ������ʾAI�Ի�����
	ProductHttpService::GetInstance()->AppendShoppingEventListener("customerSpeak", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAICustomerSpeak(request, response);
	});

	// �����ѯ��Ʒ(����Ʒ���ݿ��������)
	ProductHttpService::GetInstance()->AppendShoppingEventListener("queryProduct", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIQueryProduct(request, response);
	});

	// ��������ؼ��ֲ�ѯ��Ʒ(����Ʒ���ݿ��������)
	ProductHttpService::GetInstance()->AppendShoppingEventListener("keyword1", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIQueryProductByKeywords(request, response);
	});

	// ���������Ʒ(����Ʒ���ݿ��������)
	ProductHttpService::GetInstance()->AppendShoppingEventListener("addProduct", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIAddProduct(request, response);
	});

	// ���������Ʒ����(�ӹ��ﳵ�������)
	ProductHttpService::GetInstance()->AppendShoppingEventListener("reduceProduct", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIReduceProduct(request, response);
	});

	// ����ɾ����Ʒ(�ӹ��ﳵ�������)
	ProductHttpService::GetInstance()->AppendShoppingEventListener("deleteProduct", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIDeleteProduct(request, response);
	});

	// ������Ʒ��������
	ProductHttpService::GetInstance()->AppendShoppingEventListener("purchaseProduct", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIPurchaseProduct(request, response);
	});

	// ����ȡ����������
	ProductHttpService::GetInstance()->AppendShoppingEventListener("cancelPurchase", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAICancelPurchase(request, response);
	});

	// ����ȡ����������
	ProductHttpService::GetInstance()->AppendShoppingEventListener("animation", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIAnimation(request, response);
	});

	// ��ʾ��Ʒ����
	ProductHttpService::GetInstance()->AppendShoppingEventListener("introduceProduct", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIIntroduceProduct(request, response);
	});

	// ���չ˿���Ϣ
	ProductHttpService::GetInstance()->AppendShoppingEventListener("customerInfo", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessCustomerInfo(request, response);
	});

	// ������͹˿���Ϣ
	ProductHttpService::GetInstance()->AppendShoppingEventListener("regularCustomer", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessRegularCustomerInfo(request, response);
	});

	// ���տ�ʼ¼����Ϣ
	ProductHttpService::GetInstance()->AppendShoppingEventListener("startRecord", [this](const httplib::Request &request, httplib::Response &response) {
		ProcessAIStartVoiceRecord(request, response);
	});

	// ����ֹͣ¼����Ϣ
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
	// httpЭ�����UTF-8����
	std::string contentUTF8 = request.get_param_value("aiSpeak");
	if (contentUTF8.length() == 0)
		return;

	std::wstring content = nbase::UTF8ToUTF16(contentUTF8);

	std::string contentANSI = nbase::win32::UnicodeToMBCS(content);

	/*nbase::ThreadManager::PostTask(kThreadUI, [content, contentANSI]() {
	LogInfo("Http", "AI˵��: %s", contentANSI.c_str());
	});*/
	
	SetJsonFormatHttpResponse(response, true, "����ɹ�");
	if (contentANSI.find("��������")!=std::string::npos)
	{
		ProcessAITriggerIsPurchase(request, response);
		nbase::ThreadManager::PostTask(kThreadUI, [content, contentANSI]() {
			LogInfo("Http", "AI˵��: %s", contentANSI.c_str());
		});
	}
	RakNet::BitStream bsOut;
	RakNet::RakWString speakContent(content.c_str());
	bsOut.WriteCompressed(speakContent);

	// ����Ϸ�ͻ��˷�����ʾ�Ի�������Ϣ
	SignalMessage(IDShowAISpeakContent, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED);
}

void ProductForm::ProcessAICustomerSpeak(const httplib::Request &request, httplib::Response &response)
{
	// httpЭ�����UTF-8����
	std::string contentUTF8 = request.get_param_value("customerSpeak");
	if (contentUTF8.length() == 0)
		return;

	std::wstring content = nbase::UTF8ToUTF16(contentUTF8);

	std::string contentANSI = nbase::win32::UnicodeToMBCS(content);

	/*nbase::ThreadManager::PostTask(kThreadUI, [content, contentANSI]() {
	LogInfo("Http", "�˿�˵��: %s", contentANSI.c_str());
	});*/

	SetJsonFormatHttpResponse(response, true, "����ɹ�");

	RakNet::BitStream bsOut;
	RakNet::RakWString speakContent(content.c_str());
	bsOut.WriteCompressed(speakContent);

	// ����Ϸ�ͻ��˷�����ʾ�Ի�������Ϣ
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
			LogWarning("Http", "���յ�[��ѯ��Ʒ %s] ����.��ѯʧ��, �������Ʒ֧�����ٽ��в���", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "�������Ʒ֧�����ٽ��в���");

		return;
	}

	std::vector<ProductInfo> productList;
	QueryProductInDatabase(productMBCS, productList);
	if (productList.size() == 0)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "���յ� [��ѯ��Ʒ %s] ����. ��ѯʧ��, ��Ʒδ�ϼ�", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "��Ʒδ�ϼ�");

		SignalStockOutEvent();

		return;
	}
	else
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogInfo("Http", "���յ� [��ѯ��Ʒ %s] ����. ��ѯ�����ɹ�", productMBCS.c_str());
		});
	}

	SetJsonFormatHttpResponse(response, true, "��ѯ�����ɹ�", productList);

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
			LogWarning("Http", "���յ�[���ؼ��� %s ��ѯ��Ʒ] ����.��ѯʧ��, �������Ʒ֧�����ٽ��в���", keywordsString.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "�������Ʒ֧�����ٽ��в���");

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
			RakNet::RakWString category(TEXT("��Ʒ"));
			//category = nbase::win32::MBCSToUnicode(prefixMBCS).c_str();
			bs.WriteCompressed(category);

			bs.Write(categoryList.size());
			for (auto item : categoryList)
			{
				RakNet::RakWString subCategory = nbase::win32::MBCSToUnicode(item).c_str();
				bs.WriteCompressed(subCategory);
			}

			SignalMessage(IDInquiryProductCategory, &bs, PacketPriority::HIGH_PRIORITY, RELIABLE);

			SetJsonFormatHttpResponse(response, true, "�����ɹ�");

			return;
		}
		}
	}

	std::vector<ProductInfo> list;

	if (ProductDatabaseManager::GetInstance()->CheckRequeryCondition(keywordsList))
	{
		// ֱ�Ӵ����ݿ��в�ѯ
		ProductDatabaseManager::GetInstance()->QueryProductFromDatabase(keywordsList, list, false);
	}
	else
	{
		// ���֮ǰ�Ĳ�ѯ�б�ǿ�
		if (m_productSearchResultList.size() > 0)
		{
			// �����ݿ�View�����ѯ��Ʒ
			ProductDatabaseManager::GetInstance()->QueryProductFromDatabase(keywordsList, list, true);

			//View���Ҳ�����Ʒ
			if (list.size() == 0)
			{
				// �����ݿ��в���
				ProductDatabaseManager::GetInstance()->QueryProductFromDatabase(keywordsList, list, false);
			}
		}
		else // ���֮ǰ�Ĳ�ѯ�б�Ϊ�գ������ݿ������ѯ��Ʒ
		{
			ProductDatabaseManager::GetInstance()->QueryProductFromDatabase(keywordsList, list, false);
		}
	}

	auto count = list.size();
	if (count == 0) // ��Ʒδ�ϼ�
	{
		nbase::ThreadManager::PostTask(kThreadUI, [keywordsString]() {
			LogWarning("Http", "���յ� [���ؼ��� %s ��ѯ��Ʒ] ����. ��ѯʧ��, ��Ʒδ�ϼ�", keywordsString.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "��Ʒδ�ϼ�");

		SignalStockOutEvent();

		return;
	}

	nbase::ThreadManager::PostTask(kThreadUI, [keywordsString, count]() {
		LogInfo("Http", "���յ� [���ؼ��� %s ��ѯ��Ʒ] ����. ��ѯ�����ɹ�, ����ѯ�� %d ����Ʒ", keywordsString.c_str(), count);
	});

	// ����Ƿ���Ҫ����Search List
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
		// ������Ʒ��ѯ����б�
		m_productSearchResultList.clear();
		m_productSearchResultList.assign(list.begin(), list.end());

		// �滻��ƷDatabase View
		ProductDatabaseManager::GetInstance()->ReplaceProductSearchListDatabaseView(keywordsList);

		SignalRefreshSearchListEvent(list);
	}

	SetJsonFormatHttpResponse(response, true, "��ѯ�����ɹ�", list);
}

void ProductForm::ProcessAIAddProduct(const httplib::Request &request, httplib::Response &response)
{
	// ���AIͬʱ��Ӷ�����Ʒʱ��������Ϸ�˴�������������
	double elapsedTime = (std::clock() - m_lastAIAddProductTime) / (double)CLOCKS_PER_SEC;
	if (elapsedTime <= 0.15)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
	}
	m_lastAIAddProductTime = std::clock();

	std::string productUTF8 = request.get_param_value("addProduct");
	if (productUTF8.length() == 0)
		return;

	std::string productMBCS = nbase::win32::UTF8ToMBCS(productUTF8); // product�ַ��� ��������Ʒ���ơ���Ʒ���ࡢ��Ʒ����

	if (m_shoppingStage == ShoppingStage::PayProduct)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "���յ�[�����Ʒ %s] ����.��Ʒ���ʧ��, �������Ʒ֧�����ٽ��в���", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "�������Ʒ֧�����ٽ��в���");

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
		std::string message = nbase::StringPrintf("���յ�[�����Ʒ %s] ����.��Ʒ���ʧ��, ��Ʒδ�ϼ�", productMBCS.c_str());
		nbase::ThreadManager::PostTask(kThreadUI, [message]() {
			LogWarning("Http", message.c_str());
		});

		SignalStockOutEvent();

		SetJsonFormatHttpResponse(response, false, "��Ʒδ�ϼ�");

		return;
	}
	else if (count > 1) // �ҵ��˶�����Ʒ
	{

		std::string message = nbase::StringPrintf("���յ�[�����Ʒ %s] ����. ���ҵ� %d ����Ʒ", productMBCS.c_str(), count);
		nbase::ThreadManager::PostTask(kThreadUI, [message]() {
			LogWarning("Http", message.c_str());
		});

		SetJsonFormatHttpResponse(response, true, "�ҵ�������Ʒ", list);

		return;
	}
	else // �ҵ�ָ����һ����Ʒ
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
				LogWarning("Http", "���յ� [�����Ʒ %s, ���� 0] ����; ����Ĭ�ϸ�Ϊ1", productMBCS.c_str());
			});
		}

		if (!CheckProductStock(id, number))
		{
			nbase::ThreadManager::PostTask(kThreadUI, [productMBCS, number]() {
				LogError("Http", "���յ� [�����Ʒ %s, ���� %d] ����. ��Ʒ���ʧ��, ��Ʒ�����Ѿ������������", productMBCS.c_str(), number);
			});

			SignalStockOutEvent();

			SetJsonFormatHttpResponse(response, false, "��Ʒ���������Ѿ������������");

			return;
		}

		auto result = AddOrIncreaseProductNumInShoppingList(id, number);
		SetJsonFormatHttpResponse(response, true, "��Ʒ�Ѿ��ɹ���ӵ����ﳵ", { *result });

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
			LogWarning("Http", "���յ�[�޸���Ʒ %s] ����. ��Ʒ�޸�ʧ��, �������Ʒ֧�����ٽ��в���", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "�������Ʒ֧�����ٽ��в���");
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
		std::string message = nbase::StringPrintf("���յ�[�޸���Ʒ %s] ����. ���ﳵ���Ҳ�����Ʒ", productMBCS.c_str());
		nbase::ThreadManager::PostTask(kThreadUI, [message]() {
			LogWarning("Http", message.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "���ﳵ���Ҳ�����Ʒ");

		return;
	}
	else if (count > 1)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [=]() {
			LogInfo("Http", "���յ�[�޸���Ʒ %s] ����. ���ҵ� %d ����Ʒ", productMBCS.c_str(), count);
		});

		SetJsonFormatHttpResponse(response, true, "�ҵ�������Ʒ", list);
		return;
	}
	else
	{
		unsigned int productID = list[0].id;

		int number = 1; // ��ȡ����number�����и�������ʾ������Ʒ����������ʾ������Ʒ����
		if (request.has_param("number"))
		{
			auto value = request.get_param_value("number");
			nbase::StringToInt(value, &number);
		}

		if (number == 0)
		{
			SetJsonFormatHttpResponse(response, false, "����ʧ��", list);
			nbase::ThreadManager::PostTask(kThreadUI, [productMBCS, number, productID]() {
				LogWarning("Http", "���յ� [�޸���Ʒ %s, ���� %d] ����. �޸���Ʒ����0ʧ��", productMBCS.c_str(), number);
			});
			return;
		}

		// number > 0 ������Ʒ����
		if (number > 0)
		{
			auto result = IncreaseProductNumInShoppingList(productID, number);
			if (result != nullptr)
			{
				ProductInfo info = *result;
				PRODUCT_ARRAY array;
				array.push_back(info);

				SetJsonFormatHttpResponse(response, true, "�����ɹ�", array);
			}
			else
			{
				SetJsonFormatHttpResponse(response, true, "�����ɹ�", list);
			}
		}
		else
		{
			number = -number;

			auto result = ReduceProductNumInShoppingList(productID, number);
			SetJsonFormatHttpResponse(response, true, "�����ɹ�", { *result });

		}

		// ˢ�¿ͻ��˹��ﳵ
		SignalRefreshShoppingListEvent(m_shoppingList);

		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS, number, productID]() {
			LogInfo("Http", "���յ� [�޸���Ʒ %s, ���� %d] ����. �޸���Ʒ�����ɹ�", productMBCS.c_str(), number);

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
			LogWarning("Http", "���յ�[�Ƴ���Ʒ %s] ����. ��Ʒ�Ƴ�ʧ��, �������Ʒ֧�����ٽ��в���", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "�������Ʒ֧�����ٽ��в���");
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
		std::string message = nbase::StringPrintf("���յ� [�Ƴ���Ʒ %s] ����. ��Ʒ�Ƴ�ʧ��, ���ﳵ���Ҳ�����Ʒ", productMBCS.c_str());
		nbase::ThreadManager::PostTask(kThreadUI, [message]() {
			LogWarning("Http", message.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "���ﳵ���Ҳ�����Ʒ");

		return;
	}
	else if (count > 1)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [=]() {
			LogInfo("Http", "���յ�[�Ƴ���Ʒ %s] ����. ���ҵ� %d ����Ʒ", productMBCS.c_str(), count);
		});

		SetJsonFormatHttpResponse(response, true, "�ҵ�������Ʒ", list);
		return;
	}
	else
	{
		SetJsonFormatHttpResponse(response, true, "�����ɹ�", list);

		nbase::ThreadManager::PostTask(kThreadUI, [=]() {
			LogInfo("Http", "���յ� [�Ƴ���Ʒ %s] ����. �����ɹ�", productMBCS.c_str());

			unsigned int productID = list[0].id;
			RemoveProductFromShoppingList(productID);

			// ˢ�¿ͻ��˹��ﳵ
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
			LogWarning("Http", "���յ�[��ʾ��Ʒ %s ����] ����. ����ʧ��, �������Ʒ֧�����ٽ��в���", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "�������Ʒ֧�����ٽ��в���");

		return;
	}

	auto productInfo = GetProductInfoByNameInDatabse(productMBCS);
	if (productInfo == nullptr)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "���յ� [��ʾ��Ʒ %s ����] ����. �������ʧ��,��Ʒδ�ϼ�", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "��Ʒδ�ϼ�");

		return;
	}
	else
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogInfo("Http", "���յ� [��ʾ��Ʒ %s ����] ����. ��������ɹ�", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, true, "�����ɹ�", { *productInfo });

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
			LogWarning("Http", "���յ� [��Ʒ���㸶��] ����. ���󱻺���, ���ﳵ�Ѿ�Ϊ��");
		});

		SetJsonFormatHttpResponse(response, false, "���ﳵ��û����Ʒ, ���󸶿�ʧ��");
	}
	else
	{
		// ���������ʱ������ WM_TIMER ��Ϣ
		SetPaymentCountdownTimer();

		m_shoppingStage = ShoppingStage::PayProduct;

		nbase::ThreadManager::PostTask(kThreadUI, []() {
			LogInfo("Http", "���յ� [��Ʒ���㸶��] ����");
		});

		std::string payUrl = OnDealUploadOrder();
		if (payUrl == "null")
		{
			LogInfo("Button", "��ȡ֧������ʧ�ܡ���");
			SetJsonFormatHttpResponse(response, false, "��ȡ֧������ʧ��, �޷�֧��");
			KillPaymentCountdownTimer();

			std::string tipUTF8 = nbase::StringPrintf("speak=%s", nbase::win32::MBCSToUTF8("��ȡ֧������ʧ��,�޷�����"));
			ProductHttpService::GetInstance()->PostAIRequest(tipUTF8);
			m_shoppingStage = ShoppingStage::PickProduct;
		}
		else
		{
			SetJsonFormatHttpResponse(response, true, "���󸶿�����ɹ�");
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
	// ȡ��֧�����ָ���Ʒ�ڿ���е�����
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

		SetJsonFormatHttpResponse(response, true, "ȡ����������ɹ�");

		nbase::ThreadManager::PostTask(kThreadUI, [=]() {
			LogInfo("Http", "���յ� [ȡ����Ʒ���㸶��] ����. ȡ����������ɹ�");

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
			LogInfo("Http", "���յ� [ȡ����Ʒ���㸶��] ����. ���ﳵΪ��,ȡ���������ʧ��");
		});

		SetJsonFormatHttpResponse(response, false, "���ﳵΪ��,ȡ���������ʧ��");
	}
}

void ProductForm::ProcessAIAnimation(const httplib::Request &request, httplib::Response &response)
{
	std::string dataUTF8 = request.get_param_value("animation");
	if (dataUTF8.length() == 0)
		return;

	std::string dataMBCS = nbase::win32::UTF8ToMBCS(dataUTF8);
	nbase::ThreadManager::PostTask(kThreadUI, [dataMBCS]() {
		LogInfo("Http", "���յ� [���Ŷ���%s] ����. �����ɹ�", dataMBCS.c_str());
	});

	SetJsonFormatHttpResponse(response, true, "�����ɹ�");

	std::wstring dataUTF16 = nbase::UTF8ToUTF16(dataUTF8);

	RakNet::BitStream bsOut;
	RakNet::RakWString buffer(dataUTF16.c_str());
	m_pRakServer->Signal(IDPlayAnimation, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}

void ProductForm::ProcessAIStartVoiceRecord(const httplib::Request &request, httplib::Response &response)
{
	SetJsonFormatHttpResponse(response, true, "�����ɹ�");

	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDStartVoiceRecord, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}

void ProductForm::ProcessAIStopVoiceRecord(const httplib::Request &request, httplib::Response &response)
{
	SetJsonFormatHttpResponse(response, true, "�����ɹ�");

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
		LogInfo("Http", "���յ� [�л��ڼ��շ�Χ %s] ����. �����ɹ�", holidayMBCS.c_str());
	});

	SetJsonFormatHttpResponse(response, true, "�����ɹ�");

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
		LogInfo("Http", "���յ� [ѡ�񵼹�Ա %s] ����. �����ɹ�", salesmanMBCS.c_str());
	});

	SetJsonFormatHttpResponse(response, true, "�����ɹ�");

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
			LogError("Http", "���յ� [ѡ��� %d ����Ʒ] ����. ����ʧ��, ��Ʒ��������.", selectedProductIndex + 1);
		});

		SetJsonFormatHttpResponse(response, false, "����ʧ��,��Ʒ��������");

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
			LogError("Http", "���յ� [ѡ��� %d ����Ʒ] ����. ����ʧ��, ��Ʒ�������ڿ��.", selectedProductIndex + 1);
		});

		SetJsonFormatHttpResponse(response, false, "����ʧ��,��Ʒ�������ڿ��");

		return false;
	}

	std::vector<ProductInfo> list;
	list.push_back(selectedProduct);

	auto result = AddOrIncreaseProductNumInShoppingList(selectedProduct.id, number);
	SetJsonFormatHttpResponse(response, true, "�����ɹ�", { *result });

	nbase::ThreadManager::PostTask(kThreadUI, [=]() {

		LogInfo("Http", "���յ� [ѡ��� %d ����Ʒ] ����. ����ɹ�, ��Ʒ %s �ɹ���ӵ����ﳵ", selectedProductIndex + 1, selectedProduct.name.c_str());

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
			LogWarning("Http", "���յ�[������Ʒ %s ����] ����. ������Ʒ����ʧ��, �������Ʒ֧�����ٽ��в���", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "����ʧ��,�������Ʒ֧�����ٽ��в���");

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
		std::string message = nbase::StringPrintf("���յ�[������Ʒ %s ����] ����. ���ﳵ���Ҳ�����Ʒ", productMBCS.c_str());
		nbase::ThreadManager::PostTask(kThreadUI, [message]() {
			LogError("Http", message.c_str());
		});

		SetJsonFormatHttpResponse(response, false, message);

		return false;
	}
	else if (list.size() > 1)
	{
		// �ҵ����ַ���Ҫ�����Ʒ
		std::string message = nbase::StringPrintf("���յ�[������Ʒ %s ����] ����. ���ﳵ���ҵ��� %d ����Ʒ", productMBCS.c_str(), list.size());
		nbase::ThreadManager::PostTask(kThreadUI, [message]() {
			LogInfo("Http", message.c_str());
		});

		SetJsonFormatHttpResponse(response, true, message, list);

		return true;
	}
	else
	{
		// �Ѿ��ҵ���Ҫ����Ʒ
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
				LogError("Http", "���յ�[������Ʒ %s ���� %d] ����. ����ʧ��, ��Ʒ���������ܿ��", productMBCS.c_str(), number);
			});

			SetJsonFormatHttpResponse(response, false, "����ʧ��,��Ʒ�������ڿ��");

			return false;
		}

		auto result = SetProductNumInShoppingList(productID, number);
		SetJsonFormatHttpResponse(response, true, "�����ɹ�", { *result });

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

			LogInfo("Http", "���յ�[������Ʒ %s ���� %d] ����. ������Ʒ�����ɹ�", productMBCS.c_str(), number);

			ProductForm::GetInstance()->DumpProductAmountMessage(productID);

			// ˢ�¿ͻ��˹��ﳵ
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
			LogWarning("Http", "���յ�[������Ʒƫ��] ����. ������Ʒƫ��ʧ��, ��������");
		});

		SetJsonFormatHttpResponse(response, false, "����ʧ��,��������");
		return false;
	}

	std::string productMBCS = nbase::win32::UTF8ToMBCS(dataUTF8);
	std::string prefMBCS = nbase::win32::UTF8ToMBCS(prefUTF8);

	if (m_shoppingStage == ShoppingStage::PayProduct)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "���յ�[������Ʒ %s ƫ��] ����. ������Ʒƫ��ʧ��, �������Ʒ֧�����ٽ��в���", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "����ʧ��,�������Ʒ֧�����ٽ��в���");

		return false;
	}

	auto productInfo = GetProductInfoByNameInShoppingList(productMBCS);
	if (productInfo == nullptr)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogError("Http", "���յ�[������Ʒ %s ƫ��] ����. ����ʧ��, ���ﳵ���Ҳ�����Ʒ", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "����ʧ��,���ﳵ���Ҳ�����Ʒ");
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
				LogInfo("Http", "���յ�[������Ʒ %s ƫ��] ����. ����ɹ�", productMBCS.c_str());
			});

			SetJsonFormatHttpResponse(response, true, "�����ɹ�", { item });

			// ˢ�¿ͻ��˹��ﳵ
			SignalRefreshShoppingListEvent(m_shoppingList);

			return true;
		}
	}

	SetJsonFormatHttpResponse(response, false, "����ʧ��,���ݿ����Ҳ�����Ӧƫ����Ʒ");
	return false;
}

bool ProductForm::ProcessAIShowPrompt(const httplib::Request &request, httplib::Response &response)
{
	std::string promptUTF8 = request.get_param_value("showPrompt");

	/*std::string promptMBCS = nbase::win32::UTF8ToMBCS(promptUTF8);
	nbase::ThreadManager::PostTask(kThreadUI, [promptMBCS]() {
	LogInfo("Http", "���յ�[չʾӦ����ʾ %s ] ����. ", promptMBCS.c_str());
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

	SetJsonFormatHttpResponse(response, true, "�����ɹ�");

	return true;
}

bool ProductForm::ProcessAILuckyDraw(const httplib::Request &request, httplib::Response &response)
{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Http", "���յ�[��ʼ�齱] ����. ");
	});

	std::string drawType = request.get_param_value("type");
	int type = 0;
	nbase::StringToInt(drawType, &type);

	std::vector<RakNet::RakWString> prizeOptions;
	prizeOptions.push_back(TEXT("ũ��ɽȪ"));
	prizeOptions.push_back(TEXT("лл����"));

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
		SetJsonFormatHttpResponse(response, true, "�����ɹ�");
	}
	else
	{
		m_prize = ProductInfo();
		SetJsonFormatHttpResponse(response, false, "лл����");
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
	LogInfo("Http", "10��ʱ���ѵ����Զ�ֹͣ�齱�����н�Ʒ%s. ", name.c_str());
	});

	SetJsonFormatHttpResponse(response, true, "�����ɹ�", { m_prize });

	RequestForShipment({ m_prize });
	}
	else
	{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
	LogInfo("Http", "10��ʱ���ѵ����Զ�ֹͣ�齱��δ���н�Ʒ, лл����. ");
	});

	SetJsonFormatHttpResponse(response, false, "лл����");
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
			LogInfo("Http", "���յ�[ֹͣ�齱] ����, ���н�Ʒ%s. ", name.c_str());
		});

		SetJsonFormatHttpResponse(response, true, "�����ɹ�", { m_prize });

		RequestForShipment({ m_prize });
	}
	else
	{
		nbase::ThreadManager::PostTask(kThreadUI, []() {
			LogInfo("Http", "���յ� [ֹͣ�齱] ����, δ���н�Ʒ, лл����. ");
		});

		SetJsonFormatHttpResponse(response, false, "лл����");
	}

	return true;
}

bool ProductForm::ProcessAIInquiryProductCategory(const httplib::Request &request, httplib::Response &response)
{
	std::string categoryUTF8 = request.get_param_value("category");

	std::string categoryMBCS = nbase::win32::UTF8ToMBCS(categoryUTF8);
	nbase::ThreadManager::PostTask(kThreadUI, [categoryMBCS]() {
		LogInfo("Http", "���յ�[ѯ����Ʒ���� %s ] ����. ", categoryMBCS.c_str());
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

	SetJsonFormatHttpResponse(response, true, "�����ɹ�");

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
			LogWarning("Http", "���յ�[��ʾ��Ʒ %s ����] ����. ����ʧ��, �������Ʒ֧�����ٽ��в���", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "�������Ʒ֧�����ٽ��в���");

		return false;
	}

	auto productInfo = GetProductInfoByNameInDatabse(productMBCS);
	if (productInfo == nullptr)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogWarning("Http", "���յ� [��ʾ��Ʒ %s ����] ����. �������ʧ��,��Ʒδ�ϼ�", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "��Ʒδ�ϼ�");

		return false;
	}
	else
	{
		nbase::ThreadManager::PostTask(kThreadUI, [productMBCS]() {
			LogInfo("Http", "���յ� [��ʾ��Ʒ %s ����] ����. ��������ɹ�", productMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, true, "�����ɹ�", { *productInfo });

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
			LogWarning("Http", "���յ�[��ʾ��Ʒ %s ����] ����. ����ʧ��, �������Ʒ֧�����ٽ��в���", oldProductMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "�������Ʒ֧�����ٽ��в���");

		return false;
	}

	auto oldProductInfo = GetProductInfoByNameInShoppingList(oldProductMBCS);
	if (oldProductInfo == nullptr)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [oldProductMBCS]() {
			LogWarning("Http", "���յ� [�滻��Ʒ %s ] ����. �������ʧ��,���ﳵ���Ҳ�����Ʒ", oldProductMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, false, "���ﳵ���Ҳ�����Ʒ");

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
				LogInfo("Http", "���յ� [�滻��Ʒ %s ] ����. �������ʧ��,���ݿ����Ҳ����滻����Ʒ", newProductMBCS.c_str());
			});

			SetJsonFormatHttpResponse(response, false, "���ݿ����Ҳ����滻����Ʒ");
			return false;
		}

		RemoveProductFromShoppingList(oldProductInfo->id);
		AddProductToShoppingListOrRefreshList(newProductInfo->id);

		nbase::ThreadManager::PostTask(kThreadUI, [oldProductMBCS]() {
			LogInfo("Http", "���յ� [�滻��Ʒ %s ] ����. ��������ɹ�", oldProductMBCS.c_str());
		});

		SetJsonFormatHttpResponse(response, true, "�����ɹ�", { *newProductInfo });

		RefreshShoppingListBox();
		SignalRefreshShoppingListEvent(m_shoppingList);
	}

	return true;
}

bool ProductForm::ProcessAIWakeup(const httplib::Request & request, httplib::Response & response)
{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Http", "���յ� [���ѵ���Ա] ��Ϣ.");
	});

	SetJsonFormatHttpResponse(response, true, "�����ɹ�");

	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDStartGame, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}

bool ProductForm::ProcessAIEnterStorageMode(const httplib::Request& request, httplib::Response& response)
{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Http", "���յ� [�����ϻ�ģʽ] ��Ϣ.");
	});

	SetJsonFormatHttpResponse(response, true, "�����ɹ�");

	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDEnterStorageMode, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}

bool ProductForm::ProcessAILaunchStorageDevice(const httplib::Request& request, httplib::Response& response)
{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Http", "���յ� [�����ϻ��豸] ��Ϣ.");
	});

	SetJsonFormatHttpResponse(response, true, "�����ɹ�");

	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDLaunchStorageDevice, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
	return true;
}

void ProductForm::ProcessAISayHello(const httplib::Request &request, httplib::Response &response)
{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Http", "���յ� [���к�] ����. �����ɹ�");
	});

	SetJsonFormatHttpResponse(response, true, "�����ɹ�");

	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDSayHello, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}

bool ProductForm::ProcessAIGetGoodsListType(const httplib::Request &request, httplib::Response &response)
{
	const int buffer_size = 4096;
	char buffer[buffer_size] = { 0 };
	StaticJsonDocument<buffer_size> doc;

	doc["success"] = true;
	doc["msg"] = "�����ɹ�";

	std::set<std::string> categorySet;
	GetMainCategoryList(categorySet);
	doc["count"] = categorySet.size();
	int i = 0;
	for (auto iter = categorySet.begin(); iter != categorySet.end(); ++iter, ++i)
	{
		doc["category"][i] = *iter;
	}

	serializeJson(doc, buffer, buffer_size);

	// ��Ҫ�Ѷ��ֽڱ����bufferתΪUTF-8����
	std::string bufferUTF8 = nbase::win32::MBCSToUTF8(buffer);

	response.set_content(bufferUTF8, "application/json");

	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Http", "���յ�AI�� [��ȡ��Ʒ�����б�] ����. �����ɹ�");
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