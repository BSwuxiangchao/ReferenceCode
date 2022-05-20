#pragma once

#include <set>
#include "NetLib/RakServer.h"
#include "NetLib/RakServerPlugin.h"
#include "TrayIcon.h"
#include "ProductDefines.h"


class ProductForm : public ui::WindowImplBase, public RakServerPlugin, public ITtrayIconDelegate
{
public:
	SINGLETON_DEFINE(ProductForm);

	using PRODUCT_ARRAY = std::vector<ProductInfo>;

	// 购物清单: unsigned int 商品ID(productID), ProductInfo 商品信息, int 数量
	//using SHOPPING_ARRAY = std::vector<std::tuple<unsigned int, ProductInfo, int> >;

	// 购物阶段
	enum class ShoppingStage
	{
		// 挑选商品阶段
		PickProduct,
		// 支付阶段
		PayProduct,
	};

	// 商品出货状态
	enum class ShipmentStatus
	{
		// 等待出货
		Waiting,
		// 已支付, 出货中
		Processing = 2,
		// 出货成功
		Success = 5,
		// 出货失败
		Failed = 6,
	};

	// 商品查询条件分类
	enum class InquiryCategory
	{
		// 所有商品
		All,
		// 商品大类
		MainCategory,
		// 商品子类
		SubCategory,
		Unknown,
	};

private:
	ProductForm();
	virtual ~ProductForm();

public:
	/**
	* 一下三个接口是必须要覆写的接口，父类会调用这三个接口来构建窗口
	* GetSkinFolder		接口设置你要绘制的窗口皮肤资源路径
	* GetSkinFile			接口设置你要绘制的窗口的 xml 描述文件
	* GetWindowClassName	接口设置窗口唯一的类名称
	*/
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;

	/**
	* 收到 WM_CREATE 消息时该函数会被调用，通常做一些控件初始化的操作
	*/
	virtual void InitWindow() override;

	/**
	* 拦截并处理底层窗体消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @return LRESULT 处理结果
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	/**
	* 收到 WM_CLOSE 消息时该函数会被调用
	*/
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void OnFinalMessage(HWND hWnd) override;

	static const std::wstring kClassName;

public:
	virtual void OnVerifyConnection(const RakNet::RakString &clientName, bool passed) override;
	virtual void OnClosedConnection(const RakNet::SystemAddress &systemAddress, RakNet::RakNetGUID rakNetGUID, RakNet::PI2_LostConnectionReason lostConnectionReason) override;

	// 处理客户端(主要是C#客户端, C++客户端采用RPC调用)发送过来的控制指令
	virtual void OnNetCommandRequest(RakNet::BitStream *bitStream) override;
	virtual void OnNetCommandResponse(RakNet::BitStream *bitStream) override;

public:
	/**
	* 实现ITrayIconDelegate接口函数
	* @return void	无返回值
	*/
	virtual void LeftClick() override;
	virtual void LeftDoubleClick() override;
	virtual void RightClick() override;

public:
	void GetProductListByCategoryInDatabse(const std::string &category, std::vector<ProductInfo> &productList);
	bool GetProductNameByIdInDatabse(unsigned int productID, std::string& outName); // 从商品数据库中查找
	
	ProductInfo* GetProductInfoByIdInDatabse(unsigned int productID); // 从商品数据库中查找
	ProductInfo* GetProductInfoByNameInDatabse(const std::string&productName); // 从商品数据库中查找
	ProductInfo* GetProductInfoByDescriptionInDatabse(const std::string&productDescription); // 从商品数据库中查找
	ProductInfo* GetProductInfoByBarcodeInDatabse(const std::string&barcode); // 从商品数据库中查找

	void GetProductInfoListByNameInDatabse(const std::string&productName, std::vector<ProductInfo>& list); // 从商品数据库中查找
	void GetProductInfoListByDescriptionInDatabse(const std::string&productDescription, std::vector<ProductInfo>& list); // 从商品数据库中查找

	void GetProductInfoListByNameInSearchResult(const std::string&productName, std::vector<ProductInfo>& list); // 从商品搜索结果列表中查找
	void GetProductInfoListByDescriptionInSearchResult(const std::string&productDescription, std::vector<ProductInfo>& list); // 从商品搜索结果列表中查找

	int GetProductCountByIdInShoppingList(unsigned int productID); // 获取购物车中的商品购买数量
	ProductInfo* GetProductInfoByIdInShoppingList(unsigned int productID); // 从购物车商品列表中查找
	ProductInfo* GetProductInfoByNameInShoppingList(const std::string&productName); // 从购物车商品列表中查找
	ProductInfo* GetProductInfoByDescriptionInShoppingList(const std::string&productDescription); // 从购物车商品列表中查找

	void GetProductInfoListByNameInShoppingList(const std::string&productName, std::vector<ProductInfo>& list);
	void GetProductInfoListByDescriptionInShoppingList(const std::string&productDescription, std::vector<ProductInfo>& list);

	// 通过商品信息查询商品，商品信息可以是种类、名称、品牌等
	void QueryProductInDatabase(const std::string &description, std::vector<ProductInfo> &productList); 

	void SignalMessage(const char *sharedIdentifier, RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability);

	void RequestForShipment(const PRODUCT_ARRAY& list);

	ShipmentStatus GetShipmentStatus()
	{
		return m_shipmentStatus;
	}

	void SetShipmentStatus(int status)
	{
		m_shipmentStatus = (ShipmentStatus) status;
	}


	// 从后台获取到支付状态，下发订单到仓管
	void SetPaymentStatus(int state);

	// 从后台获取上货列表，AI进入上货模式
	void SetUploadListFromHttp(const std::vector<ProductInfo> list);


private:
	void InitNetwork();
	void UninitNetwork();

	// 注册RPC调用函数
	void RegisterRPCFunction();

	// 注册Http的商品事件处理函数
	void AppendShoppingEventListener();
	/**
	* 初始化各类食品饮料按钮
	*/
	void InitProductButtonControls();

	/**
	* 初始化特殊控制按钮
	*/
	void InitSpecialButtonControls();

	/**
	* 初始化AI说话控制按钮
	*/
	void InitAISpeakButtonControls();

	/**
	* 初始化网络状态显示按钮
	*/
	void InitNetworkStatusControls();

	bool OnClicked(ui::EventArgs* args);

	ProductInfo* AddProductToShoppingListOrRefreshList(unsigned int productID, int number = 1);



	// 向游戏客户端发送添加商品事件
	void SignalAddProductEvent(unsigned int productID, int number = 1);

	// 向游戏客户端发送减少商品事件
	void SignalReduceProductEvent(unsigned int productID, int number = 1);

	// 向游戏客户端发送移除商品事件
	void SignalRemoveProductEvent(unsigned int productID);

	// 向游戏客户端发送刷新搜索商品结果列表事件
	void SignalRefreshSearchListEvent(PRODUCT_ARRAY& list);

	// 向游戏客户端发送刷新购物车列表事件
	void SignalRefreshShoppingListEvent(const PRODUCT_ARRAY& list);

	// 向游戏客户端发送缺货事件
	void SignalStockOutEvent();

	// 点击购物车商品列表项"+"按钮，增加商品数量
	bool OnItemIncreaseButtonClicked(ui::EventArgs* args);

	// 点击购物车商品列表项"-"按钮，减少商品数量
	bool OnItemReduceButtonClicked(ui::EventArgs* args);

	// 点击购物车商品列表项删除按钮，删除商品
	bool OnItemDeleteButtonClicked(ui::EventArgs* args);

	// 获得上货列表后上传后台返回支付链接并发给游戏端
	std::string OnDealUploadOrder();

	// 当手机端支付成功后, 向仓库货架客户端发送商品出货请求
	void OnPaymentSuccess();

	void OnPaymentFailed();

	// 购物车商品发生变化时，刷新购物车
	void RefreshShoppingListBox();

	// 加载商品列表配置文件并添加到商品vector列表：需要处理多字节转Unicode编码
	void LoadProductFileAndAddToList(const std::string& filePath);

	void LoadProductDatabaseAndAddToList();

	bool SettingsMenuButtonClick(ui::EventArgs* param);
	bool SystemSettingsMenuItemClick(ui::EventArgs* param);
	bool LookLogMenuItemClick(ui::EventArgs* param); 

	bool SettlementButtonClick(ui::EventArgs* param);
	bool CancelPurchaseButtonClick(ui::EventArgs* param);

	bool StartGameButtonClick(ui::EventArgs* param); 
	bool EndGameButtonClick(ui::EventArgs* param);
	bool PlayAdsButtonClick(ui::EventArgs* param);
	bool ShowDrinksButtonClick(ui::EventArgs* param);
	bool ShowFoodsButtonClick(ui::EventArgs* param);
	bool SetNormalHolidayButtonClick(ui::EventArgs* param); // 选择ue4正常背景
	bool SetNewYearButtonClick(ui::EventArgs* param); // 选择ue4春节背景
	bool SetMoonFestivalButtonClick(ui::EventArgs* param); // 选择ue4中秋背景
	bool SelectPotatoSalesmanButtonClick(ui::EventArgs* param); // 选择土豆导购员
	bool SelectSpinachSalesmanButtonClick(ui::EventArgs* param); // 选择菠菜导购员

	ProductInfo* AddProductToShoppingList(unsigned int productID, int number = 1);

	ProductInfo* IncreaseProductNumInShoppingList(unsigned int productID, int number = 1);

	ProductInfo* AddOrIncreaseProductNumInShoppingList(unsigned int productID, int number = 1);
	
	// 减少购物车中的商品数量：返回当前商品
	ProductInfo* ReduceProductNumInShoppingList(unsigned int productID, int number = 1);
	
	// 直接设置商品在购物车中的数量: 返回当前商品
	ProductInfo* SetProductNumInShoppingList(unsigned int productID, int number);

	bool RemoveProductFromShoppingList(unsigned int productID);

	// 检查商品库存数量是否大于等于给定的数量number
	bool CheckProductStock(unsigned int productID, int number);

	// 右键系统托盘图标，弹出操作菜单
	void PopupTrayMenu(POINT point);

	// 点击退出
	bool QuitMenuItemClick(ui::EventArgs* args);

	// 接收客户端上传的商品数据列表(如果不从本地读取商品配置信息的话)
	void ReceiveProductListFromClient(RakNet::BitStream * userData, RakNet::Packet *packet);

	// 接收客户端上传的顾客信息(id，性别，年龄), 然后转发到游戏端
	void ReceiveCustomerInfoFromClient(RakNet::BitStream * userData, RakNet::Packet *packet);
	void ReceiveRegularCustomerInfoFromClient(RakNet::BitStream * userData, RakNet::Packet *packet);
	void TriggerCustomerInfoAction(const RakNet::RakString &userID, int sex, int age);
	void TriggerCustomerInfoAction(const RakNet::RakString &userID, int sex, int age,int regularCus);

	// 接收从客户端返回的商品出货结果
	void ReceiveShipmentResultFromClient(RakNet::BitStream * userData, RakNet::Packet *packet);

	// 处理游戏客户端取消支付消息：清空购物车
	void ProcessGameCancelPurchase(RakNet::BitStream * userData, RakNet::Packet *packet);

	// 处理游戏客户端查询商品库存
	void ProcessGameRequestProductInventory(RakNet::BitStream * userData, RakNet::Packet *packet);

	// 处理游戏客户端自动支付（演示版本）
	void ProcessGamePaymentSuccess(RakNet::BitStream * userData, RakNet::Packet *packet);

	// 处理仓管端请求,显示上货程序界面
	void ProcessShowGoodsBoxUI(RakNet::BitStream* userData, RakNet::Packet* packet);
	// 处理仓管端请求,隐藏上货程序界面
	void ProcessHideGoodsBoxUI(RakNet::BitStream* userData, RakNet::Packet* packet);
	// 处理仓管端请求,显示上货操作提示
	void ProcessShowStorageTip(RakNet::BitStream* userData, RakNet::Packet* packet);

	void ProcessExitStorageMode(RakNet::BitStream* userData, RakNet::Packet* packet);

	// 处理AI对话内容
	void ProcessAIMachineSpeak(const httplib::Request &request, httplib::Response &response);

	// 处理顾客对话内容
	void ProcessAICustomerSpeak(const httplib::Request &request, httplib::Response &response);

	// 处理查询商品请求
	void ProcessAIQueryProduct(const httplib::Request &request, httplib::Response &response);

	void ProcessAIQueryProductByKeywords(const httplib::Request &request, httplib::Response &response);

	void ProcessAIAddProduct(const httplib::Request &request, httplib::Response &response);

	// 修改商品数量：number >0 增加商品数量；number < 0 减少商品数量
	void ProcessAIReduceProduct(const httplib::Request &request, httplib::Response &response);

	void ProcessAIDeleteProduct(const httplib::Request &request, httplib::Response &response);
	void ProcessAIIntroduceProduct(const httplib::Request &request, httplib::Response &response);
	void ProcessAIPurchaseProduct(const httplib::Request &request, httplib::Response &response);
	void ProcessAICancelPurchase(const httplib::Request &request, httplib::Response &response);
	void ProcessAIAnimation(const httplib::Request &request, httplib::Response &response);

	// 使用HTTP协议，处理从摄像头程序传过来的顾客信息: 用户id, 性别, 年龄
	void ProcessCustomerInfo(const httplib::Request &request, httplib::Response &response);

	// 使用HTTP协议，处理从摄像头程序传过来的顾客信息: 用户id, 性别, 年龄,熟客
	void ProcessRegularCustomerInfo(const httplib::Request &request, httplib::Response &response);

	//使用HTTP协议，处理识别到的熟客
	void ProcessAIStartVoiceRecord(const httplib::Request &request, httplib::Response &response);
	void ProcessAIStopVoiceRecord(const httplib::Request &request, httplib::Response &response);

	void ProcessAISetHolidayMode(const httplib::Request &request, httplib::Response &response);
	void ProcessAISelectSalesman(const httplib::Request &request, httplib::Response &response);

	void ProcessAISayHello(const httplib::Request &request, httplib::Response &response);

	bool ProcessAISelectProduct(const httplib::Request &request, httplib::Response &response);
	bool ProcessAISetProductNumber(const httplib::Request &request, httplib::Response &response);

	bool ProcessAISetProductPref(const httplib::Request &request, httplib::Response &response);

	bool ProcessAIShowPrompt(const httplib::Request &request, httplib::Response &response);

	bool ProcessAILuckyDraw(const httplib::Request &request, httplib::Response &response);
	bool ProcessAILuckyDrawStop(const httplib::Request &request, httplib::Response &response);

	bool ProcessAIInquiryProductCategory(const httplib::Request &request, httplib::Response &response); 

	bool ProcessAIShowProductDetail(const httplib::Request &request, httplib::Response &response);

	bool ProcessAIReplaceProduct(const httplib::Request &request, httplib::Response &response);

	bool ProcessAIWakeup(const httplib::Request& request, httplib::Response& response);

	bool ProcessAIEnterStorageMode(const httplib::Request &request, httplib::Response &response);

	//bool ProcessAIExitStorageMode(const httplib::Request& request, httplib::Response& response);
	bool ProcessAILaunchStorageDevice(const httplib::Request& request, httplib::Response& response);

	bool ProcessAIGetGoodsListType(const httplib::Request &request, httplib::Response &response);

	bool ProcessAIShowSearchListUI(const httplib::Request &request, httplib::Response &response);
	bool ProcessAIHideSearchListUI(const httplib::Request &request, httplib::Response &response);
	//触发是否买单动作
	bool ProcessAITriggerIsPurchase(const httplib::Request &request, httplib::Response &response);

	void DumpProductAmountMessage(unsigned int productID);

	std::string ArrayToString(const std::vector<std::string>& list, bool mbcs = true);

	void SetJsonFormatHttpResponse(httplib::Response &response, bool success, const std::string&message, const std::vector<ProductInfo>& list = std::vector<ProductInfo>());

	// 根据第几行第几列(个)计算商品搜索列表中的索引
	int CalcProductIndex(int row, int column);

	// 设置付款倒计时
	void SetPaymentCountdownTimer();
	void KillPaymentCountdownTimer();

	// 设置抽奖倒计时
	void SetLuckyDrawTimer();
	void KillLuckyDrawTimer();

	// 设置出货倒计时
	void SetShipmentTimer();
	void KillShipmentTimer();

	InquiryCategory CheckCategory(const std::string &inquiry);
	InquiryCategory CheckKeywordCategory(const std::string &keyword);

	// 获取商品种类列表
	void GetMainCategoryList(std::set<std::string>& categoryList);

	// 获取商品种类列表
	void GetSubCategoryList(const std::string& category, std::set<std::string>& subCategoryList);

	void Test();

private:

	ShoppingStage m_shoppingStage = ShoppingStage::PickProduct;

	RakServer *m_pRakServer = new RakServer();

	bool m_bTrayIconLeftDoubleClicked;

	const unsigned int MaxProductCount = 20;
	std::map<std::wstring, ui::Button*> m_btnPairs;
	std::map<std::wstring, ui::Button*> m_netStatusMap;
	ui::ListBox* m_shoppingListBox = nullptr;
	ui::Button* m_btnSettings = nullptr;
	ui::Button* m_btnSettlement = nullptr;
	ui::Button* m_btnCancelPurchase = nullptr;
	ui::Button* m_btnEnableAI = nullptr;
	ui::Button* m_btnDisableAI = nullptr;
	ui::Button* m_btnSetNormalHoliday = nullptr;
	ui::Button* m_btnSetNewYearHoliday = nullptr;
	ui::Button* m_btnSetMoonFestivalHoliday = nullptr;
	ui::Button* m_btnSelectPotatoSalesman = nullptr;
	ui::Button* m_btnSelectSpinachSalesman = nullptr;

	std::vector<ProductInfo> m_productDatabase; // 所有商品信息列表

	std::vector<ProductInfo> m_productSearchResultList; // 查询的商品信息结果列表
	std::vector<ProductInfo> m_productLastSearchResultList; // 上一次查询的商品信息结果列表

	std::unique_ptr<class SystemSettingsForm> m_pSysSettingsForm;
	std::weak_ptr<nbase::WeakFlag> m_pSysSettingsFormFlag;

	PRODUCT_ARRAY m_shoppingList;

	std::clock_t m_lastAIAddProductTime = std::clock();

	RakNet::RakString m_currentCustomerUserID;

	ui::Label* m_lblPurchaseCoundown = nullptr;
	const int CountdownLimit = 90; // 付款倒计时限制为90秒
	int m_countdown = CountdownLimit;

	ShipmentStatus m_shipmentStatus = ShipmentStatus::Waiting;

	ProductInfo m_prize; // 抽奖奖品
	const int LuckyDrawTimeLimit = 10; // 10秒
	int m_luckyDrawTime = LuckyDrawTimeLimit;
	bool m_luckyDrawStopped = false;

	const int ShipmentTimeLimit = 420; // 420秒
	int m_shipmentTime = ShipmentTimeLimit;
};