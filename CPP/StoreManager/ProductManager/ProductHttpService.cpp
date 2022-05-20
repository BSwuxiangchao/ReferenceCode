#include "stdafx.h"
#include <set>
#include "../Utility/ArduinoJson.h"
#include "ProductForm.h"
#include "ProductDefines.h"
#include "ProductDatabaseManager.h"
#include "ProductHttpService.h"

#define CA_CERT_FILE "./client.crt"
#define CPPHTTPLIB_OPENSSL_SUPPORT

ProductHttpService::ProductHttpService()
{

}


ProductHttpService::~ProductHttpService()
{
	Uninit();
}

bool ProductHttpService::InitCommonService(const std::string& localHost, int localPort)
{
	m_localHost = localHost;
	m_localPort = localPort;

	m_pServer = std::make_shared<httplib::Server>();

	auto httpThread = std::thread([this]() { AutostartService(m_localHost, m_localPort); });
	httpThread.detach();

	return true;
}

bool ProductHttpService::InitPhoneService(const std::string& phoneHost, int phonePort, const std::string&phonePath)
{
	m_urlPhonePath = phonePath;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	m_GoodsOutOrderURL = "";
	m_GoodsOutOrderURL.Format(_T("%s/octv-unmanned-store/third/store/"), phoneHost);
	
#else
	m_pPhoneClient = std::make_shared<httplib::Client>(phoneHost, phonePort);

#endif
	LogInfo("Http", "初始化Http Client: %s,端口 %d, 与手机端通讯", phoneHost.c_str(), phonePort);

	return true;
}

bool ProductHttpService::InitAIService(const std::string& aiHost, int aiPort, const std::string&aiPath)
{
	m_urlAIPath = aiPath;

	m_pAIClient = std::make_shared<httplib::Client>(aiHost, aiPort);
	
	RegisterAIEventHandlerFuction();

	LogInfo("Http", "初始化Http Client: %s,端口 %d, 与AI端通讯", aiHost.c_str(), aiPort);

	return true;
}

void ProductHttpService::Uninit()
{
	if (m_pServer)
		m_pServer->stop();

	if (m_pPhoneClient)
		m_pPhoneClient->stop();

	if (m_pAIClient)
		m_pAIClient->stop();
}

bool ProductHttpService::AutostartService(const std::string& localHost, int localPort)
{
	std::vector<std::string> addressList;
	nbase::GetIpAddressList(addressList);
	if (addressList.size() == 0)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [&localHost]() { LogError("Http", "错误:本机未设置IP,请检查网络设置."); });

		return false;
	}

	bool addressMatch = false;
	for (auto address : addressList)
	{
		if (address.compare(localHost) == 0)
		{
			addressMatch = true;
			break;
		}
	}

	if (addressMatch)
	{
		nbase::ThreadManager::PostTask(kThreadUI, [&localHost, localPort]() { LogInfo("Http", "初始化Http Server: 在本地IP %s,端口 %d 监听事件", localHost.c_str(), localPort); });
		
		return m_pServer->listen(localHost.c_str(), localPort);
	}
	else
	{
		nbase::ThreadManager::PostTask(kThreadUI, [&localHost]() { LogError("Http", "错误:本机未找到与配置文件中匹配的IP:%s", localHost.c_str()); });

		return false;
	}
}

void ProductHttpService::RegisterAIEventHandlerFuction()
{
	m_AIEventHandler.clear();

	m_AIEventHandler.insert(std::make_pair(("aiSpeak"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIShowAISpeakContent(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("customerSpeak"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIShowCustomerSpeakContent(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("queryProduct"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIQueryProduct(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("addProduct"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIAddProduct(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("reduceProduct"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIReduceProduct(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("deleteProduct"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIDeleteProduct(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("getWishlist"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIGetWishlist(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("purchaseProduct"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIPurchaseProduct(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("cancelPurchase"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AICancelPurchase(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("introduceProduct"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIIntroduceProduct(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("customerInfo"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIReceiveCustomerInfo(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("regularCustomer"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIReceiveRegularCustomerInfo(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("animation"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AITriggerAnimation(request, response); }));
	
	m_AIEventHandler.insert(std::make_pair(("startRecord"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIStartVoiceRecord(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("stopRecord"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIStopVoiceRecord(request, response); }));
	
	m_AIEventHandler.insert(std::make_pair("sayHello", [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AISayHello(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("holidayMode"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AISetHolidayMode(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("selectSalesman"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AISelectSalesman(request, response); }));
	
	m_AIEventHandler.insert(std::make_pair(("selectProduct"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AISelectProduct(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("keyword1"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIQueryProductByKeywords(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("setNumber"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AISetProductNumber(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("setProductPref"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AISetProductPreference(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("showPrompt"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIShowPrompt(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("luckyDraw"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AILuckyDraw(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("luckyDrawStop"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AILuckyDrawStop(request, response); }));
	
	m_AIEventHandler.insert(std::make_pair(("inquiryProductCategory"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIInquiryProductCategory(request, response); }));

	m_AIEventHandler.insert(std::make_pair(("productDetail"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIShowProductDetail(request, response); }));
	
	m_AIEventHandler.insert(std::make_pair(("replaceProduct"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIReplaceProduct(request, response); }));
	
	m_AIEventHandler.insert(std::make_pair(("wakeup"), [](const httplib::Request& request, httplib::Response& response) { ProductHttpService::GetInstance()->AIWakeup(request, response); }));
	
	m_AIEventHandler.insert(std::make_pair(("enterStorageMode"), [](const httplib::Request& request, httplib::Response& response) { ProductHttpService::GetInstance()->AIEnterStorageMode(request, response); }));
	m_AIEventHandler.insert(std::make_pair(("exitStorageMode"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIExitStorageMode(request, response); }));
	
	m_AIEventHandler.insert(std::make_pair(("launchStorageDevice"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AILaunchStorageDevice(request, response); }));
	
	m_AIEventHandler.insert(std::make_pair(("getGoodsListType"), [](const httplib::Request  &request, httplib::Response &response) { ProductHttpService::GetInstance()->AIGetGoodsListType(request, response); }));
}


httplib::Result ProductHttpService::PostPhonePaymentRequest(const std::string& path, int totalMoney)
{
	m_pPhoneClient->Post("/post", "text", "text/plain");

	httplib::Params params{
		{"totalMoney", nbase::StringPrintf("%d", totalMoney)}
	};

	return m_pPhoneClient->Post(path.c_str(), params);
}

void ProductHttpService::PostRequest(const std::string& url, const std::string& request)
{
	std::shared_ptr<httplib::Client>  client = std::make_shared<httplib::Client>("192.168.3.177", 6020);
	httplib::Headers headers{
		{ "Content-Type", "text/plain" }
	};

	httplib::Params params{
		{ "", "" }
	};

	//client->set_proxy("test.octvisionland.com", 443);
	//auto response = client->Get("/octv-unmanned-store/a/store/octStoreGoodsType/getOctStoreGoodsTypeList");
	//auto response = client->Post("/octv-unmanned-store/a/store/octStoreGoodsType/getOctStoreGoodsTypeList", "1", "application/json");
	auto response = client->Post("/octv-unmanned-store/a/store/octStoreGoodsType/getOctStoreGoodsTypeList", headers, params);
	auto body = nbase::win32::UTF8ToMBCS(response->body);

}

httplib::Result ProductHttpService::PostOrderToBackground(PRODUCT_ARRAY list)
{
	//m_pPhoneClient->Post("/post", "text", "text/plain");

	char buffer[PACKET_SIZE] = { 0 };
	StaticJsonDocument<PACKET_SIZE> doc;
	doc["storeNo"] = m_storeNo;
	JsonArray statusInfoArray = doc.createNestedArray("orderList");
	for (auto& item : list)
	{
		JsonObject cellJS = statusInfoArray.createNestedObject();
		cellJS["goodsNo"] = item.id; //商品编号
		cellJS["goodsBarCodes"] = item.barcode; // 商品条形码
		cellJS["goodCounts"] = item.count;//商品库存
	}
	serializeJson(doc, buffer, PACKET_SIZE);

	// 需要把多字节编码的buffer转为UTF-8编码
	LogInfo("server","出货订单:%s", buffer);
	std::string bufferUTF8 = nbase::UTF16ToUTF8(nbase::win32::MBCSToUnicode(buffer));

	return m_pPhoneClient->Post("/octv-unmanned-store/third/store/goodsOutOrder", bufferUTF8, "application/json");//"application/json"
}

std::string ProductHttpService::PostOrderToBackgroundByHttp(PRODUCT_ARRAY list)
{
	char buffer[PACKET_SIZE] = { 0 };
	StaticJsonDocument<PACKET_SIZE> doc;
	doc["storeNo"] = m_storeNo;
	JsonArray statusInfoArray = doc.createNestedArray("orderList");
	for (auto& item : list)
	{
		JsonObject cellJS = statusInfoArray.createNestedObject();
		cellJS["goodsNo"] = item.id; //商品编号
		cellJS["goodsBarCodes"] = item.barcode; // 商品条形码
		cellJS["goodCounts"] = item.count;//商品库存
		
		std::string strname = item.name;
		int count = item.count;
		float price = item.price;
		nbase::ThreadManager::PostTask(kThreadUI, [strname, count, price]() {
			std::string message = nbase::StringPrintf("购物车列表: 产品名称 %s, 数量 %d, 单价 %.1f", strname.c_str(), count, price);
			LogInfo("Debug", message.c_str());
		});
	}
	serializeJson(doc, buffer, PACKET_SIZE);

	// 需要把多字节编码的buffer转为UTF-8编码
	LogInfo("server", "收到出货订单");
	std::string bufferUTF8 = nbase::UTF16ToUTF8(nbase::win32::MBCSToUnicode(buffer));

	std::wstring strResponse; 
	//CString url = m_GoodsOutOrderURL + "goodsOutOrder";
	wstring url = L"https://store.octvisionland.com//octv-unmanned-store/third/store/goodsOutOrder"; 
	//string buffer="{\"storeNo\":\"ZT_10001\"}";
	
	//std::string bufferUTF8 = nbase::UTF16ToUTF8(buffer);
	//CA2T szr(bufferUTF8.c_str());
	//CString theString = (LPCTSTR)szr;//(LPCTSTR)为强制类型转换
	m_HttpClient.HttpPost(url.c_str(), buffer,strResponse);

	//LPCTSTR stt ("\{\"storeNo\":\"ZT_10001\"\}");

	return nbase::UTF16ToUTF8(strResponse);
}

void ProductHttpService::PostAIRequest(std::string request)
{
	httplib::Headers headers{
		{ "Content-Type", "text/plain" }
	};

	httplib::Params params{
		{ request, "1" }
	};
	auto response = m_pAIClient->Post(m_urlAIPath.c_str(), request, "text/plain");
	//auto body = nbase::win32::UTF8ToMBCS(response->body);
	
}

void ProductHttpService::HandlePhoneMessage(const std::string& path)
{
	m_pServer->Get("/", [=](const httplib::Request &request, httplib::Response &response) {
		response.set_content("Hello world!\n", "text/plain");
	});

	// 获取商品类型列表
	m_pServer->Get("/octv-unmanned-store/a/store/octStoreGoodsType/getOctStoreGoodsTypeList", [=](const httplib::Request &request, httplib::Response &response) {
		std::vector<ProductInfo> productlist;
		ProductDatabaseManager::GetInstance()->QueryAllProducts(productlist);

		std::set<std::string> categroySet;
		for (auto item : productlist)
		{
			categroySet.insert(item.mainCategory);
		}
		
		const int buffer_size = 4096;
		char buffer[buffer_size] = { 0 };
		StaticJsonDocument<buffer_size> doc;

		doc["success"] = true;
		doc["msg"] = "操作成功";

		JsonArray statusInfoArray = doc.createNestedArray("obj");
		for (auto item : categroySet)
		{
			JsonObject product = statusInfoArray.createNestedObject();
			product["id"] = "";
			product["typeName"] = item;
		}

		doc["code"] = "ERR_00000";

		serializeJson(doc, buffer, buffer_size);

		// 需要把多字节编码的buffer转为UTF-8编码
		std::string bufferUTF8 = nbase::UTF16ToUTF8(nbase::win32::MBCSToUnicode(buffer));

		response.set_content(bufferUTF8, "application/json");

		nbase::ThreadManager::PostTask(kThreadUI, []() {
			LogInfo("Pad", "接收到pad [获取商品类型列表] 请求. 操作成功");
		});

	});

	// 获取商品列表
	m_pServer->Get("/octv-unmanned-store/a/store/octStoreProduct/getProductList", [=](const httplib::Request &request, httplib::Response &response) {

		auto categoryUTF8 = request.get_param_value("goodsType");
		if (categoryUTF8.length() == 0) return;

		std::string category = nbase::win32::UTF8ToMBCS(categoryUTF8);

		std::vector<ProductInfo> list;
		ProductDatabaseManager::GetInstance()->QueryAllProducts(list);

		std::vector<ProductInfo> result;
		for (auto item : list)
		{
			if (item.mainCategory == category)
			{
				result.push_back(item);
			}
		}

		const int buffer_size = 4096 * 2;
		char buffer[buffer_size] = { 0 };
		StaticJsonDocument<buffer_size> doc;

		doc["success"] = true;
		doc["msg"] = "操作成功";
		
		JsonArray statusInfoArray = doc.createNestedArray("obj");
		for (auto item : result)
		{
			JsonObject product1 = statusInfoArray.createNestedObject();
			product1["id"] = item.id;
			product1["goodsName"] = item.name; // name
			product1["surfaceImage"] = ""; 
			product1["salesVolume"] = item.amount; // 商品库存
			product1["goodsRetailPrice"] = item.price; // price

			product1["goodsSize"] = "100*100*100"; // amount: 商品库存
			product1["goodsBarCodes"] = item.barcode;
			product1["productId"] = item.id; 
			auto surfaceImage = nbase::StringPrintf("http://%s/%s", m_localHost.c_str(), item.image.c_str());
			product1["surfaceImage"] = surfaceImage;
		}

		doc["code"] = "ERR_00000";

		serializeJson(doc, buffer, buffer_size);

		// 需要把多字节编码的buffer转为UTF-8编码
		std::string bufferUTF8 = nbase::UTF16ToUTF8(nbase::win32::MBCSToUnicode(buffer));

		response.set_content(bufferUTF8, "application/json");

		nbase::ThreadManager::PostTask(kThreadUI, [category]() {
			LogInfo("Pad", "接收到pad [获取 %s 列表] 请求. 操作成功", category.c_str());
		});
	});

	// 获取商品详情
	m_pServer->Get("/octv-unmanned-store/a/store/octStoreProduct/getProductDetail", [=](const httplib::Request &request, httplib::Response &response) {
		auto idUTF8 = request.get_param_value("id");
		if (idUTF8.length() == 0) return;

		unsigned int id = 0;
		nbase::StringToUint(idUTF8, &id); 
		std::vector<ProductInfo> list;
		ProductDatabaseManager::GetInstance()->QueryAllProducts(list);

		ProductInfo info;
		for (auto item : list)
		{
			if (item.id == id)
			{
				info = item;
				break;
			}
		}

		const int buffer_size = 4096;
		char buffer[buffer_size] = { 0 };
		StaticJsonDocument<buffer_size> doc;

		doc["success"] = true;
		doc["msg"] = "操作成功";

		JsonObject jobject = doc.createNestedObject("obj");
		jobject["id"] = info.id;

		jobject["goodsImageList"][0] = nbase::StringPrintf("http://%s/%s", m_localHost.c_str(), info.image.c_str());
		jobject["goodsName"] = info.name;
		jobject["goodsUnit"] = "瓶";
		jobject["reserve"] = info.amount;
		jobject["goodsRetailPrice"] = info.price;
		jobject["goodsRetailPrice"] = info.price;
		jobject["surfaceImage"] = "";
		jobject["productId"] = info.id;
		jobject["goodsBarCodes"] = info.barcode;
		jobject["surfaceImage"] = nbase::StringPrintf("http://%s/%s", m_localHost.c_str(), info.image.c_str());

		doc["code"] = "ERR_00000";

		serializeJson(doc, buffer, buffer_size);

		// 需要把多字节编码的buffer转为UTF-8编码
		std::string bufferUTF8 = nbase::UTF16ToUTF8(nbase::win32::MBCSToUnicode(buffer));

		response.set_content(bufferUTF8, "application/json");

		nbase::ThreadManager::PostTask(kThreadUI, [id]() {
			LogInfo("Pad", "接收到pad [获取商品 %d 详情] 请求. 操作成功", id);
		});
	});

	m_pServer->Post("/octv-unmanned-store/a/store/octStoreOrder/addPadOrder", [this](const httplib::Request & req, httplib::Response &response) {
		auto content = req.body;
		StaticJsonDocument<PACKET_SIZE> doc;
		deserializeJson(doc, content.c_str());

		std::vector<ProductInfo> list;

		JsonArray array = doc.as<JsonArray>();
		for (size_t i = 0; i < array.size(); i++)
		{
			std::string barcode = array[i]["goodsBarCodes"];
			auto number = array[i]["goodsNumber"].as<unsigned int>();
			auto id = array[i]["productId"].as<unsigned int>();
			
			auto info = ProductForm::GetInstance()->GetProductInfoByBarcodeInDatabse(barcode);
			if (info != nullptr)
			{
				list.push_back(*info);
			}
		}

		ProductForm::GetInstance()->RequestForShipment(list);

		char buffer[PACKET_SIZE] = { 0 };
		StaticJsonDocument<PACKET_SIZE> docSent;

		docSent["success"] = true;
		docSent["msg"] = "操作成功";
		
		JsonObject jobject = docSent.createNestedObject("obj");
		jobject["id"] = "111111111";

		docSent["code"] = "ERR_00000";

		serializeJson(docSent, buffer, PACKET_SIZE);

		// 需要把多字节编码的buffer转为UTF-8编码
		std::string bufferUTF8 = nbase::UTF16ToUTF8(nbase::win32::MBCSToUnicode(buffer));

		response.set_content(bufferUTF8, "application/json");

		ProductForm::GetInstance()->SetShipmentStatus(2);

		nbase::ThreadManager::PostTask(kThreadUI, []() {
			LogInfo("Pad", "接收到pad [提交商品订单] 请求. 操作成功");
		});
	});

	// 查询pad订单状态
	m_pServer->Post("/octv-unmanned-store/a/store/octStoreOrder/getPadOrderState", [this](const httplib::Request & req, httplib::Response &response) {

		char buffer[PACKET_SIZE] = { 0 };
		StaticJsonDocument<PACKET_SIZE> doc;

		doc["success"] = true;
		doc["msg"] = "操作成功";

		JsonObject jobject = doc.createNestedObject("obj");
		auto status = ProductForm::GetInstance()->GetShipmentStatus();
		if (status == ProductForm::ShipmentStatus::Processing)
		{
			jobject["msg"] = "正在挑取您的商品，请稍等";
		}
		else if (status == ProductForm::ShipmentStatus::Waiting)
		{
			jobject["msg"] = "正在挑取您的商品，请稍等";
		}
		else if (status == ProductForm::ShipmentStatus::Success)
		{
			jobject["msg"] = "出货成功";
		}
		else if(status == ProductForm::ShipmentStatus::Failed)
		{
			jobject["msg"] = "出货失败";
		}

		jobject["orderState"] = (int)status;
		
		doc["code"] = "ERR_00000";

		serializeJson(doc, buffer, PACKET_SIZE);

		// 需要把多字节编码的buffer转为UTF-8编码
		std::string bufferUTF8 = nbase::UTF16ToUTF8(nbase::win32::MBCSToUnicode(buffer));

		response.set_content(bufferUTF8, "application/json");
	});

	// 设置pad订单状态
	m_pServer->Get("/octv-unmanned-store/a/store/octStoreOrder/setPadOrderState", [this](const httplib::Request & req, httplib::Response &response) {
		auto stateUTF8 = req.get_param_value("state");
		if (stateUTF8.length() == 0) return;

		unsigned int state = 0;
		nbase::StringToUint(stateUTF8, &state);

		ProductForm::GetInstance()->SetShipmentStatus(state);

		char buffer[PACKET_SIZE] = { 0 };
		StaticJsonDocument<PACKET_SIZE> doc;

		doc["success"] = true;
		doc["msg"] = "操作成功";

		doc["code"] = "ERR_00000";

		serializeJson(doc, buffer, PACKET_SIZE);

		// 需要把多字节编码的buffer转为UTF-8编码
		std::string bufferUTF8 = nbase::UTF16ToUTF8(nbase::win32::MBCSToUnicode(buffer));

		response.set_content(bufferUTF8, "application/json");
	});

	// 手机扫码支付成功
	m_pServer->Get(path.c_str(), [this](const httplib::Request & req, httplib::Response &response) {
		if (req.has_param("paid"))
		{
			auto val = req.get_param_value("paid");
			response.set_content("商品支付成功!\n", "application/json");
			
			if (m_paymentCallback)
			{
				m_paymentCallback();
			}
		}
	});
}

void ProductHttpService::HandleAICommunication(const std::string& path)
{
	m_pServer->Get(path.c_str(), [this](const httplib::Request &request, httplib::Response &response) {
		for (auto item : m_AIEventHandler)
		{
			if (request.has_param(item.first.c_str()))
			{
				item.second(request, response);
			}
		}
	});

	m_pServer->Post(path.c_str(), [this](const httplib::Request &request, httplib::Response &response) {
		for (auto item : m_AIEventHandler)
		{
			if (request.has_param(item.first.c_str()))
			{
				item.second(request, response);
			}
		}
	});
}

void ProductHttpService::HandleBackgroundMessage(const std::string& path)
{
	//接收上货列表
	m_pServer->Post("/addGoods", [this](const httplib::Request & req, httplib::Response &response) {
		auto content = req.body;
		LogInfo("上货请求body:", nbase::win32::UTF8ToMBCS(req.body).c_str());
		StaticJsonDocument<PACKET_SIZE*10> doc;
		std::wstring objUTF16 = nbase::UTF8ToUTF16(content.c_str());
		std::string objMBCS = nbase::win32::UnicodeToMBCS(objUTF16);
		deserializeJson(doc, objMBCS);

		std::vector<ProductInfo> list;
		JsonArray array = doc.as<JsonArray>();
		for (size_t i = 0; i < array.size(); i++)
		{
			ProductInfo product;
			product.name = array[i]["goodsName"].as<std::string>();
			product.id = array[i]["goodsNo"].as<int>();
			product.barcode = array[i]["goodsBarCodes"].as<std::string>();
			product.count = array[i]["goodCounts"].as<int>();
			list.push_back(product);
		}
		//设置上货订单并通知程序进入上货模式
		ProductForm::GetInstance()->SetUploadListFromHttp(list);

		char buffer[PACKET_SIZE] = { 0 };
		StaticJsonDocument<PACKET_SIZE> docRes;
		docRes["success"] = true;
		docRes["msg"] = "操作成功";
		serializeJson(docRes, buffer, PACKET_SIZE);

		// 需要把多字节编码的buffer转为UTF-8编码
		std::string bufferUTF8 = nbase::UTF16ToUTF8(nbase::win32::MBCSToUnicode(buffer));

		response.set_content(bufferUTF8, "application/json");
	});


	// 收到订单支付状态
	m_pServer->Post("/wxPayResult", [this](const httplib::Request & req, httplib::Response &response) {
		auto content = req.body;
		LogInfo("订单支付结果:", nbase::win32::UTF8ToMBCS(req.body).c_str());
		StaticJsonDocument<PACKET_SIZE> doc;
		std::wstring objUTF16 = nbase::UTF8ToUTF16(content.c_str());
		std::string objMBCS = nbase::win32::UnicodeToMBCS(objUTF16);
		deserializeJson(doc, objMBCS);

		unsigned int state = doc["payResult"].as<int>();;

		//设置支付状态
		ProductForm::GetInstance()->SetPaymentStatus(state);

		char buffer[PACKET_SIZE] = { 0 };
		StaticJsonDocument<PACKET_SIZE> docRes;

		docRes["success"] = true;
		docRes["msg"] = "操作成功";
		serializeJson(docRes, buffer, PACKET_SIZE);

		// 需要把多字节编码的buffer转为UTF-8编码
		std::string bufferUTF8 = nbase::UTF16ToUTF8(nbase::win32::MBCSToUnicode(buffer));

		response.set_content(bufferUTF8, "application/json");
	});
}

void ProductHttpService::AppendShoppingEventListener(std::string eventType, std::function<void(const httplib::Request  &request, httplib::Response &response)> callback)
{
	m_AIEventDispatcher.appendListener((eventType), callback);
}

void ProductHttpService::AIQueryProduct(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("queryProduct"), request, response);
}

void ProductHttpService::AIQueryProductByKeywords(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("keyword1"), request, response);
}

void ProductHttpService::AIShowAISpeakContent(const httplib::Request  &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("aiSpeak"), request, response);
}

void ProductHttpService::AIShowCustomerSpeakContent(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("customerSpeak"), request, response);
}

void ProductHttpService::AIAddProduct(const httplib::Request  &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("addProduct"), request, response);
}

void ProductHttpService::AIReduceProduct(const httplib::Request  &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("reduceProduct"), request, response);
}

void ProductHttpService::AIDeleteProduct(const httplib::Request  &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("deleteProduct"), request, response);
}

void ProductHttpService::AIGetWishlist(const httplib::Request  &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("getWishlist"), request, response);
}

void ProductHttpService::AIPurchaseProduct(const httplib::Request  &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("purchaseProduct"), request, response);
}

void ProductHttpService::AICancelPurchase(const httplib::Request  &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("cancelPurchase"), request, response);
}

void ProductHttpService::AIIntroduceProduct(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("introduceProduct"), request, response);
}

void ProductHttpService::AITriggerAnimation(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("animation"), request, response);
}

void ProductHttpService::AIReceiveCustomerInfo(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("customerInfo"), request, response);
}

void ProductHttpService::AIReceiveRegularCustomerInfo(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("regularCustomer"), request, response);
}

void ProductHttpService::AIStartVoiceRecord(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("startRecord"), request, response);
}

void ProductHttpService::AIStopVoiceRecord(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("stopRecord"), request, response);
}

void ProductHttpService::AISetHolidayMode(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("holidayMode"), request, response);
}

void ProductHttpService::AISelectSalesman(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("selectSalesman"), request, response);
}

void ProductHttpService::AISelectProduct(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("selectProduct"), request, response);
}

void ProductHttpService::AISetProductNumber(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("setNumber"), request, response);
}

void ProductHttpService::AISetProductPreference(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("setProductPref"), request, response);
}

void ProductHttpService::AIShowPrompt(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("showPrompt"), request, response);
}

void ProductHttpService::AILuckyDraw(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("luckyDraw"), request, response);
}

void ProductHttpService::AILuckyDrawStop(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("luckyDrawStop"), request, response);
}

void ProductHttpService::AIInquiryProductCategory(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("inquiryProductCategory"), request, response);
}

void ProductHttpService::AIShowProductDetail(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("productDetail"), request, response);
}

void ProductHttpService::AIReplaceProduct(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch(("replaceProduct"), request, response);
}

void ProductHttpService::AIWakeup(const httplib::Request & request, httplib::Response & response)
{
	m_AIEventDispatcher.dispatch("wakeup", request, response);
}

void ProductHttpService::AIEnterStorageMode(const httplib::Request& request, httplib::Response& response)
{
	m_AIEventDispatcher.dispatch("enterStorageMode", request, response);
}

void ProductHttpService::AIExitStorageMode(const httplib::Request& request, httplib::Response& response)
{
	m_AIEventDispatcher.dispatch("exitStorageMode", request, response);
}

void ProductHttpService::AILaunchStorageDevice(const httplib::Request& request, httplib::Response& response)
{
	m_AIEventDispatcher.dispatch("launchStorageDevice", request, response);
}

void ProductHttpService::AIGetGoodsListType(const httplib::Request& request, httplib::Response& response)
{
	m_AIEventDispatcher.dispatch("getGoodsListType", request, response);
}

void ProductHttpService::AISayHello(const httplib::Request &request, httplib::Response &response)
{
	m_AIEventDispatcher.dispatch("sayHello", request, response);
}



