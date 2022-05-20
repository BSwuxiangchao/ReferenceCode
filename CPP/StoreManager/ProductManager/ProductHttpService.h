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

	//���Ͷ�������̨��ȡ֧������
	httplib::Result PostOrderToBackground(PRODUCT_ARRAY list);//httplib
	std::string PostOrderToBackgroundByHttp(PRODUCT_ARRAY list);//httpclient-file
	// ��Rasa AI�˷���http����
	void PostAIRequest(std::string request);

	// ��������֧������: ɨ���ֻ��ϵĶ�ά��֧��
	httplib::Result PostPhonePaymentRequest(const std::string& path, int totalMoney);

	// �����ֻ���Httpͨ����Ϣ
	void HandlePhoneMessage(const std::string& path);

	// ����AI��������ָ��
	void HandleAICommunication(const std::string& path);

	// �����̨�����ӿ�
	void HandleBackgroundMessage(const std::string& path);

	
	void SetPaymentSuccessCallback(const std::function<void()> &callback) 
	{ 
		m_paymentCallback = callback; 
	}

	//���õ��̱��
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
	// ��ѯ��Ʒ
	void AIQueryProduct(const httplib::Request &request, httplib::Response &response);
	// ͨ������ؼ��ֲ�ѯ��Ʒ
	void AIQueryProductByKeywords(const httplib::Request &request, httplib::Response &response);
	// ��ʾAI�Ի�����
	void AIShowAISpeakContent(const httplib::Request &request, httplib::Response &response);
	// ��ʾ�˿ͶԻ�����
	void AIShowCustomerSpeakContent(const httplib::Request &request, httplib::Response &response);
	// �����Ʒ�����ﳵ
	void AIAddProduct(const httplib::Request &request, httplib::Response &response);
	// �޸���Ʒ������number >0 ������Ʒ������number < 0 ������Ʒ����
	void AIReduceProduct(const httplib::Request &request, httplib::Response &response);
	// �ӹ��ﳵ��ɾ����Ʒ
	void AIDeleteProduct(const httplib::Request &request, httplib::Response &response);
	// ��ȡ���ﳵ��Ʒ��Ϣ
	void AIGetWishlist(const httplib::Request &request, httplib::Response &response);
	// ��Ʒ���㸶��
	void AIPurchaseProduct(const httplib::Request &request, httplib::Response &response);
	// ȡ����Ʒ����
	void AICancelPurchase(const httplib::Request &request, httplib::Response &response);
	// ��ʾ��Ʒ����
	void AIIntroduceProduct(const httplib::Request &request, httplib::Response &response);
	// ���Ŷ���
	void AITriggerAnimation(const httplib::Request &request, httplib::Response &response);

	// ���չ˿���Ϣ: id, sex, age
	void AIReceiveCustomerInfo(const httplib::Request &request, httplib::Response &response);

	// ���չ˿���Ϣ: id, sex, age,regualr
	void AIReceiveRegularCustomerInfo(const httplib::Request &request, httplib::Response &response);

	// AI�˿�ʼ¼���¼�
	void AIStartVoiceRecord(const httplib::Request &request, httplib::Response &response);
	// AI��ֹͣ¼���¼�
	void AIStopVoiceRecord(const httplib::Request &request, httplib::Response &response);

	// �л���ͬ�ļ���ģʽ: ���ڣ�Ԫ�������磬���Ԫ����ʥ��������Ƚڼ���
	void AISetHolidayMode(const httplib::Request &request, httplib::Response &response);

	// ѡ��ͬ�ĵ���Ա����: ��������㡢���á�ʥ�����˵�����
	void AISelectSalesman(const httplib::Request &request, httplib::Response &response);

	// ���к�
	void AISayHello(const httplib::Request &request, httplib::Response &response);

	// ѡ����Ʒ�б��еĵ�N��
	void AISelectProduct(const httplib::Request &request, httplib::Response &response);

	// ���ù��ﳵ�е���Ʒ����
	void AISetProductNumber(const httplib::Request &request, httplib::Response &response);

	// ���ù��ﳵ�е���Ʒƫ�ã����µ�ƫ����ѡ��Ʒ
	void AISetProductPreference(const httplib::Request &request, httplib::Response &response);
	
	// ��˿�չʾ�Ի�Ӧ����ʾ��
	void AIShowPrompt(const httplib::Request &request, httplib::Response &response);
	
	void AILuckyDraw(const httplib::Request &request, httplib::Response &response);

	void AILuckyDrawStop(const httplib::Request &request, httplib::Response &response);
	
	void AIInquiryProductCategory(const httplib::Request &request, httplib::Response &response);

	void AIShowProductDetail(const httplib::Request &request, httplib::Response &response);

	void AIReplaceProduct(const httplib::Request &request, httplib::Response &response);

	// �����������ѵ���Ա��Ϣ
	void AIWakeup(const httplib::Request& request, httplib::Response& response);

	// ��������ϻ�ģʽ
	void AIEnterStorageMode(const httplib::Request &request, httplib::Response &response);

	// �����˳��ϻ�ģʽ
	void AIExitStorageMode(const httplib::Request& request, httplib::Response& response);

	// �����ϻ��豸
	void AILaunchStorageDevice(const httplib::Request& request, httplib::Response& response);

	// ��Ʒ�б�����
	void AIGetGoodsListType(const httplib::Request& request, httplib::Response& response);

private:
	std::shared_ptr<httplib::Client> m_pPhoneClient; // ���ֻ��˽���ͨѶ�� http client

	std::shared_ptr<httplib::Client> m_pAIClient; // �� Rasa AI �˽���ͨѶ�� http client

	std::shared_ptr<httplib::Server> m_pServer;

	std::map < std::string, std::function<void(const httplib::Request &, httplib::Response &)> > m_AIEventHandler; // ƥ��AI�¼��Ͷ�Ӧ�Ĵ�����

	std::function<void()> m_paymentCallback;

	std::string m_localHost;
	int m_localPort = 0;

	std::string m_urlPhonePath;
	std::string m_urlAIPath;
	
	std::string m_storeNo;//���̱��	
	CHttpClient m_HttpClient;//http����ͻ���
	CString  m_GoodsOutOrderURL;

	eventpp::EventDispatcher<std::string, void(const httplib::Request &request, httplib::Response &response)> m_AIEventDispatcher; // �ַ�AI�¼�
};

