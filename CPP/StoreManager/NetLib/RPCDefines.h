#ifndef RPCDEFINES_H_
#define RPCDEFINES_H_

/*
 * 定义在服务端和客户端同时用到的RPC调用事件ID 
 */

// Client->Server: 验证网络连接事件
static const char *IDVerifyConnection = "VerifyConnection";

// Server<->Client: 网络断开事件
static const char *IDClosedConnection = "ClosedConnection";

// Server->Client: 总控端向客户端发送的控制指令事件
static const char *IDNetCommandRequest = "NetCommandRequest";

// Client->Server: 客户端向总控端返回的控制指令执行结果事件
static const char *IDNetCommandResponse = "NetCommandResponse";

// Client->Server: 客户端异常或者错误状态上报事件
static const char *IDReportClientError = "ReportClientError";

// Client->Server: PLC状态上报事件
static const char *IDPLCStatus = "PLCStatus";

// Client->Server: 灯控软件向服务器上报灯光设备型号事件：马田还是万璟
static const char *IDReportLightType = "ReportLightType";

static const char *IDForwardCommandRequest = "ForwardCommandRequest";

static const char *IDForwardCommandResponse = "ForwardCommandResponse";

// 游戏开始事件
static const char *IDStartGame = "StartGame";
// 游戏结束事件
static const char *IDEndGame = "EndGame";

// 展示同种类的各种商品事件
static const char *IDPlayAds = "PlayAds";

// 展示同种类的各种商品事件
static const char *IDShowCategoryOfProducts = "ShowCategoryOfProducts";

// 添加商品到购物车
static const char *IDAddProductToChart = "AddProductToChart";

// 从购物车里减少商品数量
static const char *IDReduceProductFromChart = "ReduceProductFromChart";

// 从购物车移除商品
static const char *IDRemoveProductFromChart = "RemoveProductFromChart";

// 刷新购物车商品列表
static const char *IDRefreshShoppingList = "RefreshShoppingList";

// 刷新搜索结果商品列表
static const char *IDRefreshSearchList = "RefreshSearchList";

// 结算付款
static const char *IDProductSettlement = "ProductSettlement";

// 放弃购买
static const char *IDCancelPurchase = "CancelPurchase";

// 支付成功
static const char *IDPaymentSuccess = "PaymentSuccess";

// 显示商品介绍
static const char* IDShowProductIntroduction = "ProductIntroduction";

// 显示商品详情
static const char* IDShowProductDetail = "ProductDetail";

// 显示AI的说话内容
static const char *IDShowAISpeakContent = "ShowAISpeakContent";

// 显示顾客的说话内容
static const char *IDShowCustomerSpeakContent = "ShowCustomerSpeakContent";

// 通过AI对话, 查询商品
static const char *IDQueryProduct = "QueryProduct";

// 查询商品类型: 食品、饮料...
static const char *IDInquiryProductCategory = "InquiryProductCategory";

// 请求商品出库: 向仓库客户端发送请求
static const char *IDRequestForShipment = "RequestForShipment";

// 商品出库结果: 仓库客户端返回出库结果
static const char *IDResponseForShipment = "ResponseForShipment";

//static const char *IDNoticeShipmentResult = "NoticeShipmentResult";

// Client->Server: 仓储端上传商品列表数据到服务端
static const char *IDUploadProductList = "UploadProductList";

// Server->Client: 服务端反馈商品列表数据上传结果
static const char *IDProductListFeedback = "ProductFeedback";

static const char *IDUploadCustomerInfo = "UploadCustomerInfo";

static const char *IDPlayAnimation = "PlayAnimation";

// AI端开始对顾客录音
static const char *IDStartVoiceRecord = "StartVoiceRecord";

// AI端结束对顾客录音
static const char *IDStopVoiceRecord = "StopVoiceRecord";

// 设置节假日氛围
static const char *IDSetHolidayMode = "SetHolidayMode";

// 选择导购员
static const char *IDSelectSalesman = "SelectSalesman";

// 打招呼
static const char *IDSayHello = "SayHello";

// 向顾客展示对话应答提示语
static const char *IDShowPrompt = "ShowPrompt";

static const char *IDLuckyDraw = "LuckyDraw";

static const char *IDLuckyDrawStop = "LuckyDrawStop";

// 客户端向服务端查询商品库存
static const char *IDRequestProductInventory = "RequestProductInventory";

// 服务端向客户端返回商品库存查询结果
static const char *IDResponseProductInventory = "ResponseProductInventory";

// 游戏端自动支付（演示用途）
static const char* IDGamePaymentSuccess = "GamePaymentSuccess";

// 进入上货模式
static const char *IDEnterStorageMode = "EnterStorageMode";

// 退出上货模式
static const char* IDExitStorageMode = "ExitStorageMode";

// Server->Client: 启动上货设备
static const char* IDLaunchStorageDevice = "LaunchStorageDevice";

// Client->Server->Client: 显示货箱上货界面
static const char* IDShowGoodsBoxUI = "ShowGoodsBoxUI";

// Client->Server->Client: 隐藏货箱上货界面
static const char* IDHideGoodsBoxUI = "HideGoodsBoxUI";

// Client->Server: 显示上货提示信息
static const char* IDShowStorageTip = "ShowStorageTip";

// Server->Client(Game): 显示商品查询列表UI
static const char* IDShowSearchListUI = "ShowSearchListUI";

// Server->Client(Game): 隐藏商品查询列表UI
static const char* IDHideSearchListUI = "HideSearchListUI";

// 缺货
static const char* IDStockOut = "StockOut";

//熟客
static const char* IDRegularCustomer = "RegularCustomer";

//开始取货的动作
static const char* IDIsPurchase = "IsPurchase";

//结束取货的动作
static const char* IDPaymentSuccessEnd = "PaymentSuccessEnd";



#endif