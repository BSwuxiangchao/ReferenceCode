#pragma once
#include"ProductDefines.h"
#include"HttpClient.h"

class ProductHttpService
{
public:
	SINGLETON_DEFINE(ProductHttpService);
	using PRODUCT_ARRAY = std::vector<ProductInfo>;

private:
	ProductHttpService();
	~ProductHttpService();
public:
	bool InitCommonService(const std::string& localHost, int localPort);
	bool InitPhoneService(const std::string& phoneHost, int phonePort, const std::string& phonePath);
	bool InitAIService(const std::string& aiHost, int aiPort, const std::string& aiPath);

	void Uninit();

	void PostRequest(const std::string& url, const std::string& request);

	//发送订单到后台获取支付链接
	httplib::Result PostOrderToBackground(PRODUCT_ARRAY list);//httplib
	std::string PostOrderToBackgroundByHttp(PRODUCT_ARRAY list);//httpclient-file
	// 向Rasa AI端发送http请求
	void PostAIRequest(std::string request);

	// 主动发送支付请求: 扫描手机上的二维码支付
	httplib::Result PostPhonePaymentRequest(const std::string& path, int totalMoney);

	// 处理手机端Http通信消息
	void HandlePhoneMessage(const std::string& path);

	// 处理AI语音交互指令
	void HandleAICommunication(const std::string& path);

	// 处理后台交互接口
	void HandleBackgroundMessage(const std::string& path);

	
	void SetPaymentSuccessCallback(const std::function<void()> &callback) 
	{ 
		m_paymentCallback = callback; 
	}

	//设置店铺编号
	inline void SetStoreNo(const std::string storeNO)
	{
		m_storeNo = storeNO;
	}
private:
	bool AutostartService(const std::string& localHost, int localPort);

	void RegisterAIEventHandlerFuction();

public:
	void AppendShoppingEventListener(std::string eventType, std::function<void(const httplib::Request &request, httplib::Response &response)> callback);

private:
	// 查询商品
	void AIQueryProduct(const httplib::Request &request, httplib::Response &response);
	// 通过多个关键字查询商品
	void AIQueryProductByKeywords(const httplib::Request &request, httplib::Response &response);
	// 显示AI对话内容
	void AIShowAISpeakContent(const httplib::Request &request, httplib::Response &response);
	// 显示顾客对话内容
	void AIShowCustomerSpeakContent(const httplib::Request &request, httplib::Response &response);
	// 添加商品到购物车
	void AIAddProduct(const httplib::Request &request, httplib::Response &response);
	// 修改商品数量：number >0 增加商品数量；number < 0 减少商品数量
	void AIReduceProduct(const httplib::Request &request, httplib::Response &response);
	// 从购物车中删除商品
	void AIDeleteProduct(const httplib::Request &request, httplib::Response &response);
	// 获取购物车商品信息
	void AIGetWishlist(const httplib::Request &request, httplib::Response &response);
	// 商品结算付款
	void AIPurchaseProduct(const httplib::Request &request, httplib::Response &response);
	// 取消商品结算
	void AICancelPurchase(const httplib::Request &request, httplib::Response &response);
	// 显示商品介绍
	void AIIntroduceProduct(const httplib::Request &request, httplib::Response &response);
	// 播放动画
	void AITriggerAnimation(const httplib::Request &request, httplib::Response &response);

	// 接收顾客信息: id, sex, age
	void AIReceiveCustomerInfo(const httplib::Request &request, httplib::Response &response);

	// 接收顾客信息: id, sex, age,regualr
	void AIReceiveRegularCustomerInfo(const httplib::Request &request, httplib::Response &response);

	// AI端开始录音事件
	void AIStartVoiceRecord(const httplib::Request &request, httplib::Response &response);
	// AI端停止录音事件
	void AIStopVoiceRecord(const httplib::Request &request, httplib::Response &response);

	// 切换不同的假日模式: 春节，元宵，端午，中秋，元旦，圣诞，国庆等节假日
	void AISetHolidayMode(const httplib::Request &request, httplib::Response &response);

	// 选择不同的导购员形象: 恐龙、姐姐、妹妹、圣诞老人等形象
	void AISelectSalesman(const httplib::Request &request, httplib::Response &response);

	// 打招呼
	void AISayHello(const httplib::Request &request, httplib::Response &response);

	// 选择商品列表中的第N项
	void AISelectProduct(const httplib::Request &request, httplib::Response &response);

	// 设置购物车中的商品数量
	void AISetProductNumber(const httplib::Request &request, httplib::Response &response);

	// 设置购物车中的商品偏好，按新的偏好挑选商品
	void AISetProductPreference(const httplib::Request &request, httplib::Response &response);
	
	// 向顾客展示对话应答提示语
	void AIShowPrompt(const httplib::Request &request, httplib::Response &response);
	
	void AILuckyDraw(const httplib::Request &request, httplib::Response &response);

	void AILuckyDrawStop(const httplib::Request &request, httplib::Response &response);
	
	void AIInquiryProductCategory(const httplib::Request &request, httplib::Response &response);

	void AIShowProductDetail(const httplib::Request &request, httplib::Response &response);

	void AIReplaceProduct(const httplib::Request &request, httplib::Response &response);

	// 处理语音唤醒导购员消息
	void AIWakeup(const httplib::Request& request, httplib::Response& response);

	// 处理进入上货模式
	void AIEnterStorageMode(const httplib::Request &request, httplib::Response &response);

	// 处理退出上货模式
	void AIExitStorageMode(const httplib::Request& request, httplib::Response& response);

	// 启动上货设备
	void AILaunchStorageDevice(const httplib::Request& request, httplib::Response& response);

	// 商品列表种类
	void AIGetGoodsListType(const httplib::Request& request, httplib::Response& response);

private:
	std::shared_ptr<httplib::Client> m_pPhoneClient; // 与手机端进行通讯的 http client

	std::shared_ptr<httplib::Client> m_pAIClient; // 与 Rasa AI 端进行通讯的 http client

	std::shared_ptr<httplib::Server> m_pServer;

	std::map < std::string, std::function<void(const httplib::Request &, httplib::Response &)> > m_AIEventHandler; // 匹配AI事件和对应的处理函数

	std::function<void()> m_paymentCallback;

	std::string m_localHost;
	int m_localPort = 0;

	std::string m_urlPhonePath;
	std::string m_urlAIPath;
	
	std::string m_storeNo;//店铺编号	
	CHttpClient m_HttpClient;//http请求客户端
	CString  m_GoodsOutOrderURL;

	eventpp::EventDispatcher<std::string, void(const httplib::Request &request, httplib::Response &response)> m_AIEventDispatcher; // 分发AI事件
};

