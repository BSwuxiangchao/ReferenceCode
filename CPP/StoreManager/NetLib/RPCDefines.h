#ifndef RPCDEFINES_H_
#define RPCDEFINES_H_

/*
 * �����ڷ���˺Ϳͻ���ͬʱ�õ���RPC�����¼�ID 
 */

// Client->Server: ��֤���������¼�
static const char *IDVerifyConnection = "VerifyConnection";

// Server<->Client: ����Ͽ��¼�
static const char *IDClosedConnection = "ClosedConnection";

// Server->Client: �ܿض���ͻ��˷��͵Ŀ���ָ���¼�
static const char *IDNetCommandRequest = "NetCommandRequest";

// Client->Server: �ͻ������ܿض˷��صĿ���ָ��ִ�н���¼�
static const char *IDNetCommandResponse = "NetCommandResponse";

// Client->Server: �ͻ����쳣���ߴ���״̬�ϱ��¼�
static const char *IDReportClientError = "ReportClientError";

// Client->Server: PLC״̬�ϱ��¼�
static const char *IDPLCStatus = "PLCStatus";

// Client->Server: �ƿ������������ϱ��ƹ��豸�ͺ��¼������ﻹ����Z
static const char *IDReportLightType = "ReportLightType";

static const char *IDForwardCommandRequest = "ForwardCommandRequest";

static const char *IDForwardCommandResponse = "ForwardCommandResponse";

// ��Ϸ��ʼ�¼�
static const char *IDStartGame = "StartGame";
// ��Ϸ�����¼�
static const char *IDEndGame = "EndGame";

// չʾͬ����ĸ�����Ʒ�¼�
static const char *IDPlayAds = "PlayAds";

// չʾͬ����ĸ�����Ʒ�¼�
static const char *IDShowCategoryOfProducts = "ShowCategoryOfProducts";

// �����Ʒ�����ﳵ
static const char *IDAddProductToChart = "AddProductToChart";

// �ӹ��ﳵ�������Ʒ����
static const char *IDReduceProductFromChart = "ReduceProductFromChart";

// �ӹ��ﳵ�Ƴ���Ʒ
static const char *IDRemoveProductFromChart = "RemoveProductFromChart";

// ˢ�¹��ﳵ��Ʒ�б�
static const char *IDRefreshShoppingList = "RefreshShoppingList";

// ˢ�����������Ʒ�б�
static const char *IDRefreshSearchList = "RefreshSearchList";

// ���㸶��
static const char *IDProductSettlement = "ProductSettlement";

// ��������
static const char *IDCancelPurchase = "CancelPurchase";

// ֧���ɹ�
static const char *IDPaymentSuccess = "PaymentSuccess";

// ��ʾ��Ʒ����
static const char* IDShowProductIntroduction = "ProductIntroduction";

// ��ʾ��Ʒ����
static const char* IDShowProductDetail = "ProductDetail";

// ��ʾAI��˵������
static const char *IDShowAISpeakContent = "ShowAISpeakContent";

// ��ʾ�˿͵�˵������
static const char *IDShowCustomerSpeakContent = "ShowCustomerSpeakContent";

// ͨ��AI�Ի�, ��ѯ��Ʒ
static const char *IDQueryProduct = "QueryProduct";

// ��ѯ��Ʒ����: ʳƷ������...
static const char *IDInquiryProductCategory = "InquiryProductCategory";

// ������Ʒ����: ��ֿ�ͻ��˷�������
static const char *IDRequestForShipment = "RequestForShipment";

// ��Ʒ������: �ֿ�ͻ��˷��س�����
static const char *IDResponseForShipment = "ResponseForShipment";

//static const char *IDNoticeShipmentResult = "NoticeShipmentResult";

// Client->Server: �ִ����ϴ���Ʒ�б����ݵ������
static const char *IDUploadProductList = "UploadProductList";

// Server->Client: ����˷�����Ʒ�б������ϴ����
static const char *IDProductListFeedback = "ProductFeedback";

static const char *IDUploadCustomerInfo = "UploadCustomerInfo";

static const char *IDPlayAnimation = "PlayAnimation";

// AI�˿�ʼ�Թ˿�¼��
static const char *IDStartVoiceRecord = "StartVoiceRecord";

// AI�˽����Թ˿�¼��
static const char *IDStopVoiceRecord = "StopVoiceRecord";

// ���ýڼ��շ�Χ
static const char *IDSetHolidayMode = "SetHolidayMode";

// ѡ�񵼹�Ա
static const char *IDSelectSalesman = "SelectSalesman";

// ���к�
static const char *IDSayHello = "SayHello";

// ��˿�չʾ�Ի�Ӧ����ʾ��
static const char *IDShowPrompt = "ShowPrompt";

static const char *IDLuckyDraw = "LuckyDraw";

static const char *IDLuckyDrawStop = "LuckyDrawStop";

// �ͻ��������˲�ѯ��Ʒ���
static const char *IDRequestProductInventory = "RequestProductInventory";

// �������ͻ��˷�����Ʒ����ѯ���
static const char *IDResponseProductInventory = "ResponseProductInventory";

// ��Ϸ���Զ�֧������ʾ��;��
static const char* IDGamePaymentSuccess = "GamePaymentSuccess";

// �����ϻ�ģʽ
static const char *IDEnterStorageMode = "EnterStorageMode";

// �˳��ϻ�ģʽ
static const char* IDExitStorageMode = "ExitStorageMode";

// Server->Client: �����ϻ��豸
static const char* IDLaunchStorageDevice = "LaunchStorageDevice";

// Client->Server->Client: ��ʾ�����ϻ�����
static const char* IDShowGoodsBoxUI = "ShowGoodsBoxUI";

// Client->Server->Client: ���ػ����ϻ�����
static const char* IDHideGoodsBoxUI = "HideGoodsBoxUI";

// Client->Server: ��ʾ�ϻ���ʾ��Ϣ
static const char* IDShowStorageTip = "ShowStorageTip";

// Server->Client(Game): ��ʾ��Ʒ��ѯ�б�UI
static const char* IDShowSearchListUI = "ShowSearchListUI";

// Server->Client(Game): ������Ʒ��ѯ�б�UI
static const char* IDHideSearchListUI = "HideSearchListUI";

// ȱ��
static const char* IDStockOut = "StockOut";

//���
static const char* IDRegularCustomer = "RegularCustomer";

//��ʼȡ���Ķ���
static const char* IDIsPurchase = "IsPurchase";

//����ȡ���Ķ���
static const char* IDPaymentSuccessEnd = "PaymentSuccessEnd";



#endif