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

	// �����嵥: unsigned int ��ƷID(productID), ProductInfo ��Ʒ��Ϣ, int ����
	//using SHOPPING_ARRAY = std::vector<std::tuple<unsigned int, ProductInfo, int> >;

	// ����׶�
	enum class ShoppingStage
	{
		// ��ѡ��Ʒ�׶�
		PickProduct,
		// ֧���׶�
		PayProduct,
	};

	// ��Ʒ����״̬
	enum class ShipmentStatus
	{
		// �ȴ�����
		Waiting,
		// ��֧��, ������
		Processing = 2,
		// �����ɹ�
		Success = 5,
		// ����ʧ��
		Failed = 6,
	};

	// ��Ʒ��ѯ��������
	enum class InquiryCategory
	{
		// ������Ʒ
		All,
		// ��Ʒ����
		MainCategory,
		// ��Ʒ����
		SubCategory,
		Unknown,
	};

private:
	ProductForm();
	virtual ~ProductForm();

public:
	/**
	* һ�������ӿ��Ǳ���Ҫ��д�Ľӿڣ����������������ӿ�����������
	* GetSkinFolder		�ӿ�������Ҫ���ƵĴ���Ƥ����Դ·��
	* GetSkinFile			�ӿ�������Ҫ���ƵĴ��ڵ� xml �����ļ�
	* GetWindowClassName	�ӿ����ô���Ψһ��������
	*/
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;

	/**
	* �յ� WM_CREATE ��Ϣʱ�ú����ᱻ���ã�ͨ����һЩ�ؼ���ʼ���Ĳ���
	*/
	virtual void InitWindow() override;

	/**
	* ���ز�����ײ㴰����Ϣ
	* @param[in] uMsg ��Ϣ����
	* @param[in] wParam ���Ӳ���
	* @param[in] lParam ���Ӳ���
	* @return LRESULT ������
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	/**
	* �յ� WM_CLOSE ��Ϣʱ�ú����ᱻ����
	*/
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void OnFinalMessage(HWND hWnd) override;

	static const std::wstring kClassName;

public:
	virtual void OnVerifyConnection(const RakNet::RakString &clientName, bool passed) override;
	virtual void OnClosedConnection(const RakNet::SystemAddress &systemAddress, RakNet::RakNetGUID rakNetGUID, RakNet::PI2_LostConnectionReason lostConnectionReason) override;

	// ����ͻ���(��Ҫ��C#�ͻ���, C++�ͻ��˲���RPC����)���͹����Ŀ���ָ��
	virtual void OnNetCommandRequest(RakNet::BitStream *bitStream) override;
	virtual void OnNetCommandResponse(RakNet::BitStream *bitStream) override;

public:
	/**
	* ʵ��ITrayIconDelegate�ӿں���
	* @return void	�޷���ֵ
	*/
	virtual void LeftClick() override;
	virtual void LeftDoubleClick() override;
	virtual void RightClick() override;

public:
	void GetProductListByCategoryInDatabse(const std::string &category, std::vector<ProductInfo> &productList);
	bool GetProductNameByIdInDatabse(unsigned int productID, std::string& outName); // ����Ʒ���ݿ��в���
	
	ProductInfo* GetProductInfoByIdInDatabse(unsigned int productID); // ����Ʒ���ݿ��в���
	ProductInfo* GetProductInfoByNameInDatabse(const std::string&productName); // ����Ʒ���ݿ��в���
	ProductInfo* GetProductInfoByDescriptionInDatabse(const std::string&productDescription); // ����Ʒ���ݿ��в���
	ProductInfo* GetProductInfoByBarcodeInDatabse(const std::string&barcode); // ����Ʒ���ݿ��в���

	void GetProductInfoListByNameInDatabse(const std::string&productName, std::vector<ProductInfo>& list); // ����Ʒ���ݿ��в���
	void GetProductInfoListByDescriptionInDatabse(const std::string&productDescription, std::vector<ProductInfo>& list); // ����Ʒ���ݿ��в���

	void GetProductInfoListByNameInSearchResult(const std::string&productName, std::vector<ProductInfo>& list); // ����Ʒ��������б��в���
	void GetProductInfoListByDescriptionInSearchResult(const std::string&productDescription, std::vector<ProductInfo>& list); // ����Ʒ��������б��в���

	int GetProductCountByIdInShoppingList(unsigned int productID); // ��ȡ���ﳵ�е���Ʒ��������
	ProductInfo* GetProductInfoByIdInShoppingList(unsigned int productID); // �ӹ��ﳵ��Ʒ�б��в���
	ProductInfo* GetProductInfoByNameInShoppingList(const std::string&productName); // �ӹ��ﳵ��Ʒ�б��в���
	ProductInfo* GetProductInfoByDescriptionInShoppingList(const std::string&productDescription); // �ӹ��ﳵ��Ʒ�б��в���

	void GetProductInfoListByNameInShoppingList(const std::string&productName, std::vector<ProductInfo>& list);
	void GetProductInfoListByDescriptionInShoppingList(const std::string&productDescription, std::vector<ProductInfo>& list);

	// ͨ����Ʒ��Ϣ��ѯ��Ʒ����Ʒ��Ϣ���������ࡢ���ơ�Ʒ�Ƶ�
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


	// �Ӻ�̨��ȡ��֧��״̬���·��������ֹ�
	void SetPaymentStatus(int state);

	// �Ӻ�̨��ȡ�ϻ��б�AI�����ϻ�ģʽ
	void SetUploadListFromHttp(const std::vector<ProductInfo> list);


private:
	void InitNetwork();
	void UninitNetwork();

	// ע��RPC���ú���
	void RegisterRPCFunction();

	// ע��Http����Ʒ�¼�������
	void AppendShoppingEventListener();
	/**
	* ��ʼ������ʳƷ���ϰ�ť
	*/
	void InitProductButtonControls();

	/**
	* ��ʼ��������ư�ť
	*/
	void InitSpecialButtonControls();

	/**
	* ��ʼ��AI˵�����ư�ť
	*/
	void InitAISpeakButtonControls();

	/**
	* ��ʼ������״̬��ʾ��ť
	*/
	void InitNetworkStatusControls();

	bool OnClicked(ui::EventArgs* args);

	ProductInfo* AddProductToShoppingListOrRefreshList(unsigned int productID, int number = 1);



	// ����Ϸ�ͻ��˷��������Ʒ�¼�
	void SignalAddProductEvent(unsigned int productID, int number = 1);

	// ����Ϸ�ͻ��˷��ͼ�����Ʒ�¼�
	void SignalReduceProductEvent(unsigned int productID, int number = 1);

	// ����Ϸ�ͻ��˷����Ƴ���Ʒ�¼�
	void SignalRemoveProductEvent(unsigned int productID);

	// ����Ϸ�ͻ��˷���ˢ��������Ʒ����б��¼�
	void SignalRefreshSearchListEvent(PRODUCT_ARRAY& list);

	// ����Ϸ�ͻ��˷���ˢ�¹��ﳵ�б��¼�
	void SignalRefreshShoppingListEvent(const PRODUCT_ARRAY& list);

	// ����Ϸ�ͻ��˷���ȱ���¼�
	void SignalStockOutEvent();

	// ������ﳵ��Ʒ�б���"+"��ť��������Ʒ����
	bool OnItemIncreaseButtonClicked(ui::EventArgs* args);

	// ������ﳵ��Ʒ�б���"-"��ť��������Ʒ����
	bool OnItemReduceButtonClicked(ui::EventArgs* args);

	// ������ﳵ��Ʒ�б���ɾ����ť��ɾ����Ʒ
	bool OnItemDeleteButtonClicked(ui::EventArgs* args);

	// ����ϻ��б���ϴ���̨����֧�����Ӳ�������Ϸ��
	std::string OnDealUploadOrder();

	// ���ֻ���֧���ɹ���, ��ֿ���ܿͻ��˷�����Ʒ��������
	void OnPaymentSuccess();

	void OnPaymentFailed();

	// ���ﳵ��Ʒ�����仯ʱ��ˢ�¹��ﳵ
	void RefreshShoppingListBox();

	// ������Ʒ�б������ļ�����ӵ���Ʒvector�б���Ҫ������ֽ�תUnicode����
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
	bool SetNormalHolidayButtonClick(ui::EventArgs* param); // ѡ��ue4��������
	bool SetNewYearButtonClick(ui::EventArgs* param); // ѡ��ue4���ڱ���
	bool SetMoonFestivalButtonClick(ui::EventArgs* param); // ѡ��ue4���ﱳ��
	bool SelectPotatoSalesmanButtonClick(ui::EventArgs* param); // ѡ����������Ա
	bool SelectSpinachSalesmanButtonClick(ui::EventArgs* param); // ѡ�񲤲˵���Ա

	ProductInfo* AddProductToShoppingList(unsigned int productID, int number = 1);

	ProductInfo* IncreaseProductNumInShoppingList(unsigned int productID, int number = 1);

	ProductInfo* AddOrIncreaseProductNumInShoppingList(unsigned int productID, int number = 1);
	
	// ���ٹ��ﳵ�е���Ʒ���������ص�ǰ��Ʒ
	ProductInfo* ReduceProductNumInShoppingList(unsigned int productID, int number = 1);
	
	// ֱ��������Ʒ�ڹ��ﳵ�е�����: ���ص�ǰ��Ʒ
	ProductInfo* SetProductNumInShoppingList(unsigned int productID, int number);

	bool RemoveProductFromShoppingList(unsigned int productID);

	// �����Ʒ��������Ƿ���ڵ��ڸ���������number
	bool CheckProductStock(unsigned int productID, int number);

	// �Ҽ�ϵͳ����ͼ�꣬���������˵�
	void PopupTrayMenu(POINT point);

	// ����˳�
	bool QuitMenuItemClick(ui::EventArgs* args);

	// ���տͻ����ϴ�����Ʒ�����б�(������ӱ��ض�ȡ��Ʒ������Ϣ�Ļ�)
	void ReceiveProductListFromClient(RakNet::BitStream * userData, RakNet::Packet *packet);

	// ���տͻ����ϴ��Ĺ˿���Ϣ(id���Ա�����), Ȼ��ת������Ϸ��
	void ReceiveCustomerInfoFromClient(RakNet::BitStream * userData, RakNet::Packet *packet);
	void ReceiveRegularCustomerInfoFromClient(RakNet::BitStream * userData, RakNet::Packet *packet);
	void TriggerCustomerInfoAction(const RakNet::RakString &userID, int sex, int age);
	void TriggerCustomerInfoAction(const RakNet::RakString &userID, int sex, int age,int regularCus);

	// ���մӿͻ��˷��ص���Ʒ�������
	void ReceiveShipmentResultFromClient(RakNet::BitStream * userData, RakNet::Packet *packet);

	// ������Ϸ�ͻ���ȡ��֧����Ϣ����չ��ﳵ
	void ProcessGameCancelPurchase(RakNet::BitStream * userData, RakNet::Packet *packet);

	// ������Ϸ�ͻ��˲�ѯ��Ʒ���
	void ProcessGameRequestProductInventory(RakNet::BitStream * userData, RakNet::Packet *packet);

	// ������Ϸ�ͻ����Զ�֧������ʾ�汾��
	void ProcessGamePaymentSuccess(RakNet::BitStream * userData, RakNet::Packet *packet);

	// ����ֹܶ�����,��ʾ�ϻ��������
	void ProcessShowGoodsBoxUI(RakNet::BitStream* userData, RakNet::Packet* packet);
	// ����ֹܶ�����,�����ϻ��������
	void ProcessHideGoodsBoxUI(RakNet::BitStream* userData, RakNet::Packet* packet);
	// ����ֹܶ�����,��ʾ�ϻ�������ʾ
	void ProcessShowStorageTip(RakNet::BitStream* userData, RakNet::Packet* packet);

	void ProcessExitStorageMode(RakNet::BitStream* userData, RakNet::Packet* packet);

	// ����AI�Ի�����
	void ProcessAIMachineSpeak(const httplib::Request &request, httplib::Response &response);

	// ����˿ͶԻ�����
	void ProcessAICustomerSpeak(const httplib::Request &request, httplib::Response &response);

	// �����ѯ��Ʒ����
	void ProcessAIQueryProduct(const httplib::Request &request, httplib::Response &response);

	void ProcessAIQueryProductByKeywords(const httplib::Request &request, httplib::Response &response);

	void ProcessAIAddProduct(const httplib::Request &request, httplib::Response &response);

	// �޸���Ʒ������number >0 ������Ʒ������number < 0 ������Ʒ����
	void ProcessAIReduceProduct(const httplib::Request &request, httplib::Response &response);

	void ProcessAIDeleteProduct(const httplib::Request &request, httplib::Response &response);
	void ProcessAIIntroduceProduct(const httplib::Request &request, httplib::Response &response);
	void ProcessAIPurchaseProduct(const httplib::Request &request, httplib::Response &response);
	void ProcessAICancelPurchase(const httplib::Request &request, httplib::Response &response);
	void ProcessAIAnimation(const httplib::Request &request, httplib::Response &response);

	// ʹ��HTTPЭ�飬���������ͷ���򴫹����Ĺ˿���Ϣ: �û�id, �Ա�, ����
	void ProcessCustomerInfo(const httplib::Request &request, httplib::Response &response);

	// ʹ��HTTPЭ�飬���������ͷ���򴫹����Ĺ˿���Ϣ: �û�id, �Ա�, ����,���
	void ProcessRegularCustomerInfo(const httplib::Request &request, httplib::Response &response);

	//ʹ��HTTPЭ�飬����ʶ�𵽵����
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
	//�����Ƿ��򵥶���
	bool ProcessAITriggerIsPurchase(const httplib::Request &request, httplib::Response &response);

	void DumpProductAmountMessage(unsigned int productID);

	std::string ArrayToString(const std::vector<std::string>& list, bool mbcs = true);

	void SetJsonFormatHttpResponse(httplib::Response &response, bool success, const std::string&message, const std::vector<ProductInfo>& list = std::vector<ProductInfo>());

	// ���ݵڼ��еڼ���(��)������Ʒ�����б��е�����
	int CalcProductIndex(int row, int column);

	// ���ø����ʱ
	void SetPaymentCountdownTimer();
	void KillPaymentCountdownTimer();

	// ���ó齱����ʱ
	void SetLuckyDrawTimer();
	void KillLuckyDrawTimer();

	// ���ó�������ʱ
	void SetShipmentTimer();
	void KillShipmentTimer();

	InquiryCategory CheckCategory(const std::string &inquiry);
	InquiryCategory CheckKeywordCategory(const std::string &keyword);

	// ��ȡ��Ʒ�����б�
	void GetMainCategoryList(std::set<std::string>& categoryList);

	// ��ȡ��Ʒ�����б�
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

	std::vector<ProductInfo> m_productDatabase; // ������Ʒ��Ϣ�б�

	std::vector<ProductInfo> m_productSearchResultList; // ��ѯ����Ʒ��Ϣ����б�
	std::vector<ProductInfo> m_productLastSearchResultList; // ��һ�β�ѯ����Ʒ��Ϣ����б�

	std::unique_ptr<class SystemSettingsForm> m_pSysSettingsForm;
	std::weak_ptr<nbase::WeakFlag> m_pSysSettingsFormFlag;

	PRODUCT_ARRAY m_shoppingList;

	std::clock_t m_lastAIAddProductTime = std::clock();

	RakNet::RakString m_currentCustomerUserID;

	ui::Label* m_lblPurchaseCoundown = nullptr;
	const int CountdownLimit = 90; // �����ʱ����Ϊ90��
	int m_countdown = CountdownLimit;

	ShipmentStatus m_shipmentStatus = ShipmentStatus::Waiting;

	ProductInfo m_prize; // �齱��Ʒ
	const int LuckyDrawTimeLimit = 10; // 10��
	int m_luckyDrawTime = LuckyDrawTimeLimit;
	bool m_luckyDrawStopped = false;

	const int ShipmentTimeLimit = 420; // 420��
	int m_shipmentTime = ShipmentTimeLimit;
};