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

const std::wstring ProductForm::kClassName = L"商品管理";
const int ProductInfoSize = 8;

const int COUNTDOWN_TIMER_ID = 1;  // 付款倒计时timer
const int LUCKYDRAW_TIMER_ID = 2;  // 付款后的抽奖timer 
const int SHIPMENT_TIMER_ID = 3;  // 付款后的出货timer 

const int ProductNumDisplayedPerRow = 5; // 设置互动端(ue4)每一行显示多少种查询商品，用于挑选商品

//const std::string ProductDatabase = (".\\data\\unmanned_store_skus.db");
//const std::string ProductDatabase = ("\\\\192.168.6.68\\Share\\sqlite\\unmanned_store_skus.db");

//#define Demo
#define WM_AFTER_INIT (WM_USER + 101)


ProductForm::ProductForm()
	:m_bTrayIconLeftDoubleClicked(false)
{

}

ProductForm::~ProductForm()
{
	if (m_pSysSettingsForm == nullptr || !::IsWindow(m_pSysSettingsFormFlag.expired() ? nullptr : m_pSysSettingsForm->GetHWND()))
	{
		m_pSysSettingsForm.release();
	}
}

std::wstring ProductForm::GetSkinFolder()
{
	return L"skin";
}

std::wstring ProductForm::GetSkinFile()
{
	return L"main.xml";
}

std::wstring ProductForm::GetWindowClassName() const
{
	return kClassName;
}

void ProductForm::InitWindow()
{
	SetIcon(IDI_PRODUCTMANAGER);

	LoadProductDatabaseAndAddToList();

	InitProductButtonControls();

	InitSpecialButtonControls();

	InitAISpeakButtonControls();

	m_pRoot->AttachBubbledEvent(ui::kEventClick, [this](ui::EventArgs* param) { return OnClicked(param); }); 

	// 加入自定义消息，让网络初始化工作在窗口初始化完成后进行
	PostMessage(WM_AFTER_INIT);

	ProductHttpService::GetInstance()->SetPaymentSuccessCallback([this]() { OnPaymentSuccess(); });

	TrayIcon::GetInstance()->Init(this);
	TrayIcon::GetInstance()->SetTrayIcon(::LoadIconW(nbase::win32::GetCurrentModuleHandle(), MAKEINTRESOURCE(IDI_PRODUCTMANAGER)), L"StoreManager");

	AppendShoppingEventListener();

	//Test();
}



void ProductForm::InitProductButtonControls()
{
	m_btnPairs.clear();

	for (unsigned int i = 1; i <= MaxProductCount; ++i)
	{
		std::wstring btnName = nbase::StringPrintf(L"productbtn%d", i);
		ui::Button *btn = dynamic_cast<ui::Button *>(FindControl(btnName.c_str()));
		if (nullptr == btn) continue;

		if (i <= m_productDatabase.size())
		{
			auto productName = m_productDatabase[i - 1].name;
			btn->SetText(nbase::win32::MBCSToUnicode(productName));

			// 操作按钮绑定商品id
			auto id = m_productDatabase[i - 1].id;
			btn->SetDataID(nbase::UintToString16(id));
		}

		m_btnPairs.insert(make_pair(btnName, btn));
	}
}

void ProductForm::InitSpecialButtonControls()
{
	ui::Button* btnStartGame = dynamic_cast<ui::Button *>(FindControl(L"btn_start_game"));
	btnStartGame->AttachClick([this](ui::EventArgs* param) { return StartGameButtonClick(param); });

	ui::Button* btnEndGame = dynamic_cast<ui::Button *>(FindControl(L"btn_end_game"));
	btnEndGame->AttachClick([this](ui::EventArgs* param) { return EndGameButtonClick(param); });

	ui::Button* btnPlayAds = dynamic_cast<ui::Button *>(FindControl(L"btn_play_ads"));
	btnPlayAds->AttachClick([this](ui::EventArgs* param) { return PlayAdsButtonClick(param); });

	ui::Button* btnShowDrinks = dynamic_cast<ui::Button *>(FindControl(L"btn_show_drinks"));
	btnShowDrinks->AttachClick([this](ui::EventArgs* param) { return ShowDrinksButtonClick(param); });

	ui::Button* btnShowFoods = dynamic_cast<ui::Button *>(FindControl(L"btn_show_foods"));
	btnShowFoods->AttachClick([this](ui::EventArgs* param) { return ShowFoodsButtonClick(param); });

	m_shoppingListBox = dynamic_cast<ui::ListBox *>(FindControl(L"productlist"));

	m_btnSettings = dynamic_cast<ui::Button*>(FindControl(L"settings"));
	m_btnSettings->AttachClick([this](ui::EventArgs* param) { return SettingsMenuButtonClick(param); });

	m_btnSettlement = dynamic_cast<ui::Button*>(FindControl(L"btn_settlement"));
	m_btnSettlement->AttachClick([this](ui::EventArgs* param) { return SettlementButtonClick(param); });

	m_btnCancelPurchase = dynamic_cast<ui::Button*>(FindControl(L"btn_cancel_purchase"));
	m_btnCancelPurchase->AttachClick([this](ui::EventArgs* param) { return CancelPurchaseButtonClick(param); });

	m_btnDisableAI = dynamic_cast<ui::Button*>(FindControl(L"btn_disable_ai"));
	m_btnDisableAI->AttachClick([this](ui::EventArgs* param) { 
		LogInfo("Http", "向AI端发送消息 disableAI ...");
	
		ProductHttpService::GetInstance()->PostAIRequest("disableAI");

		return true; 
	});

	m_btnEnableAI = dynamic_cast<ui::Button*>(FindControl(L"btn_enable_ai"));
	m_btnEnableAI->AttachClick([this](ui::EventArgs* param) { 
		LogInfo("Http", "向AI端发送消息 enableAI ...");

		ProductHttpService::GetInstance()->PostAIRequest("enableAI"); 
		return true;
	});

	m_lblPurchaseCoundown = dynamic_cast<ui::Label*>(FindControl(L"lbl_purchase_countdown"));
	m_lblPurchaseCoundown->SetVisible(false);

	m_btnSetNormalHoliday = dynamic_cast<ui::Button*>(FindControl(L"btn_normal_holiday"));
	m_btnSetNormalHoliday->AttachClick([this](ui::EventArgs* param) { return SetNormalHolidayButtonClick(param); });

	m_btnSetNewYearHoliday = dynamic_cast<ui::Button*>(FindControl(L"btn_new_year_holiday"));
	m_btnSetNewYearHoliday->AttachClick([this](ui::EventArgs* param) { return SetNewYearButtonClick(param); });

	m_btnSetMoonFestivalHoliday = dynamic_cast<ui::Button*>(FindControl(L"btn_moon_festival_holiday"));
	m_btnSetMoonFestivalHoliday->AttachClick([this](ui::EventArgs* param) { return SetMoonFestivalButtonClick(param); });

	m_btnSelectPotatoSalesman = dynamic_cast<ui::Button*>(FindControl(L"btn_potato_salesman"));
	m_btnSelectPotatoSalesman->AttachClick([this](ui::EventArgs* param) { return SelectPotatoSalesmanButtonClick(param); });

	m_btnSelectSpinachSalesman = dynamic_cast<ui::Button*>(FindControl(L"btn_spinach_salesman"));
	m_btnSelectSpinachSalesman->AttachClick([this](ui::EventArgs* param) { return SelectSpinachSalesmanButtonClick(param); });

}

void ProductForm::InitAISpeakButtonControls()
{
	for (unsigned int i = 0;  ; ++i)
	{
		std::wstring btnName = nbase::StringPrintf(L"aispeak%d", i + 1);
		ui::Button *btn = dynamic_cast<ui::Button *>(FindControl(btnName.c_str()));
		if (nullptr == btn) break;

		std::wstring buttonText;
		AISpeakButtonConfiguration::GetInstance()->GetTextByIndex(i, buttonText);
		if (buttonText.empty())
		{
			btn->SetVisible(false);
		}
		else
		{
			btn->SetText(buttonText);
			btn->AttachClick([buttonText](ui::EventArgs* param) { 
				
				std::wstring speakUTF16;
				AISpeakButtonConfiguration::GetInstance()->GetRandomSpeakByText(buttonText, speakUTF16);
				
				std::string speakMBCS = nbase::StringPrintf("speak=%s", nbase::win32::UnicodeToMBCS(speakUTF16).c_str());

				LogInfo("Http", "向AI端发送消息 %s ...", speakMBCS.c_str());
				std::string speakUTF8 = nbase::StringPrintf("speak=%s", nbase::UTF16ToUTF8(speakUTF16).c_str());

				ProductHttpService::GetInstance()->PostAIRequest(speakUTF8);

				return true; 
			});
		}
	}
}


void ProductForm::InitNetwork()
{
	CSimpleIni configController;
	auto filePath = nbase::win32::GetCurrentModuleDirectory() + IniConfigFile;
	auto error = configController.LoadFile(filePath.c_str());
	if (error != SI_OK)
	{
		LOG(ERROR) << _T("Server") << _T("加载配置文件 ") << filePath.c_str() << _T("错误: ") << error;
		//MGR_ERROR("加载配置文件 {0} 错误：{1}", filePath.c_str(), error);
		return;
	}

	std::vector<ClientInfo> clientInfoList;
	// 获取允许连接到总控的客户端列表
	for (int i = 0; ; ++i)
	{
		TCHAR keyName[64] = { 0 };
		_stprintf_s(keyName, _T("Client%d"), i + 1);

		auto value = configController.GetValue(_T("ClientList"), keyName, _T(""));
		if (_tcsclen(value) == 0)
			break;

		RakNet::RakString clientName = RakNet::RakString::FromWideChar_S(value);
		ClientInfo info;
		info.name = clientName;

		bool existed = false;
		for (auto info : clientInfoList)
		{
			if (info.name.StrICmp(clientName) == 0)
			{
				existed = true;
				break;
			}
		}

		if (existed)
		{
			LogError("Server", "列表中配置的客户端名称 %s 重复.", clientName.C_String());
		}
		else
		{
			clientInfoList.push_back(info);
		}
	}

	SetClientInfoList(clientInfoList);

	m_pRakServer->AttachPlugin(this);

	bool success = m_pRakServer->Init("HardCorePwd", (unsigned short)SERVER_PORT);
	if (success)
	{
		LogInfo("Server", "网络初始化成功, 端口: %d", SERVER_PORT); 
	}
	else
	{
		LogError("Server", "网络初始化失败.");
	}
}


void ProductForm::InitNetworkStatusControls()
{
	m_netStatusMap.clear();
	auto clientList = GetClientInfoList();
	for (size_t i = 0; i < clientList.size(); ++i)
	{
		ui::Button* btnNetStatus = dynamic_cast<ui::Button *>(FindControl(nbase::StringPrintf(L"btn_net_node%d", i + 1)));
		if (btnNetStatus == nullptr)
			break;

		std::wstring clientName = nbase::win32::MBCSToUnicode(clientList[i].name.C_String());
		btnNetStatus->SetText(clientName);
		btnNetStatus->SetEnabled(false);
		btnNetStatus->SetVisible(true);

		m_netStatusMap[clientName] = btnNetStatus;
	}
}

void ProductForm::RegisterRPCFunction()
{
	m_pRakServer->RegisterSlot(IDUploadProductList, [](RakNet::BitStream * userData, RakNet::Packet *packet) { ProductForm::GetInstance()->ReceiveProductListFromClient(userData, packet); }, 0);

	m_pRakServer->RegisterSlot(IDUploadCustomerInfo, [](RakNet::BitStream * userData, RakNet::Packet *packet) { ProductForm::GetInstance()->ReceiveCustomerInfoFromClient(userData, packet); }, 0);
	
	m_pRakServer->RegisterSlot(IDRegularCustomer, [](RakNet::BitStream * userData, RakNet::Packet *packet) { ProductForm::GetInstance()->ReceiveRegularCustomerInfoFromClient(userData, packet); }, 0);
	
	m_pRakServer->RegisterSlot(IDResponseForShipment, [](RakNet::BitStream * userData, RakNet::Packet *packet) { ProductForm::GetInstance()->ReceiveShipmentResultFromClient(userData, packet); }, 0);
	
	m_pRakServer->RegisterSlot(IDCancelPurchase, [](RakNet::BitStream * userData, RakNet::Packet *packet) { ProductForm::GetInstance()->ProcessGameCancelPurchase(userData, packet); }, 0);

	m_pRakServer->RegisterSlot(IDRequestProductInventory, [](RakNet::BitStream * userData, RakNet::Packet *packet) { ProductForm::GetInstance()->ProcessGameRequestProductInventory(userData, packet); }, 0);
	
	m_pRakServer->RegisterSlot(IDGamePaymentSuccess, [](RakNet::BitStream* userData, RakNet::Packet* packet) { ProductForm::GetInstance()->ProcessGamePaymentSuccess(userData, packet); }, 0);
	
	m_pRakServer->RegisterSlot(IDShowGoodsBoxUI, [](RakNet::BitStream* userData, RakNet::Packet* packet) { ProductForm::GetInstance()->ProcessShowGoodsBoxUI(userData, packet); }, 0);

	m_pRakServer->RegisterSlot(IDHideGoodsBoxUI, [](RakNet::BitStream * userData, RakNet::Packet *packet) { ProductForm::GetInstance()->ProcessHideGoodsBoxUI(userData, packet); }, 0);
	
	m_pRakServer->RegisterSlot(IDShowStorageTip, [](RakNet::BitStream * userData, RakNet::Packet *packet) { ProductForm::GetInstance()->ProcessShowStorageTip(userData, packet); }, 0);
	
	m_pRakServer->RegisterSlot(IDExitStorageMode, [](RakNet::BitStream * userData, RakNet::Packet *packet) { ProductForm::GetInstance()->ProcessExitStorageMode(userData, packet); }, 0);
}

LRESULT ProductForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_AFTER_INIT)
	{
		InitNetwork();

		InitNetworkStatusControls();

		RegisterRPCFunction();
	}
	else if (uMsg == WM_KEYDOWN)
	{
		if (wParam == VK_ESCAPE)
		{
			ui::MsgboxCallback cb = [this](ui::MsgBoxRet ret) { 
				if (ret == ui::MB_YES)
				{
					PostMessage(WM_CLOSE);
				}
			};

			ui::ShowMsgBox(GetHWND(), cb, ui::MT_WARNING, L"确定要退出程序吗？", false, L"警告", false, L"确定", false, L"取消", false);
		}
		else if (wParam == 'P')
		{
			//ProductPayment::GetInstance()->PostRequest("", "");
			OnPaymentSuccess(); // 测试支付成功消息
		}
	}
	else if (uMsg == WM_TIMER)
	{
		if (wParam == COUNTDOWN_TIMER_ID)
		{
			m_lblPurchaseCoundown->SetText(nbase::StringPrintf(TEXT("付款倒计时%d秒"), m_countdown));
			m_lblPurchaseCoundown->SetVisible(true);

			--m_countdown;

			if (m_countdown == 0)
			{
				m_countdown = CountdownLimit;

				m_shoppingStage = ShoppingStage::PickProduct;

				// 取消支付，把购物车中的商品全部放回库存
				for (auto& item : m_shoppingList)
				{
					auto info = GetProductInfoByIdInDatabse(item.id);
					if (info)
					{
						info->amount += item.count;
					}
				}

				m_shoppingList.clear();

				m_shoppingListBox->RemoveAll();

				LogWarning("Cart", "支付超时, 清空购物车取消商品支付.");

				m_productSearchResultList.clear();

				SignalRefreshShoppingListEvent(m_shoppingList);

				SignalRefreshSearchListEvent(m_productSearchResultList);

				KillPaymentCountdownTimer();

			}
		}
		else if (wParam == LUCKYDRAW_TIMER_ID)
		{
			--m_luckyDrawTime;
			if (m_luckyDrawTime == 0)
			{

			}
		}
		else if (wParam == SHIPMENT_TIMER_ID)
		{
			--m_shipmentTime;
			if (m_shipmentTime == 0)
			{
				LogError("Http", "出货超时，向AI端发送 shipmentFail 出货失败消息...");
				ProductHttpService::GetInstance()->PostAIRequest("shipmentFail");
				m_shipmentStatus = ShipmentStatus::Failed;

				KillShipmentTimer();
			}
		}
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}


bool ProductForm::SettingsMenuButtonClick(ui::EventArgs* param)
{
	RECT rect = param->pSender->GetPos();
	ui::CPoint point;
	point.x = rect.left - 150;
	point.y = rect.bottom + 5;
	ClientToScreen(m_hWnd, &point);

	ui::CMenuWnd* pMenu = new ui::CMenuWnd(NULL);
	ui::STRINGorID xml(L"settings_menu.xml");
	pMenu->Init(xml, _T("xml"), point);

	ui::CMenuElementUI* system_settings = (ui::CMenuElementUI*)pMenu->FindControl(L"system_settings");
	system_settings->AttachSelect([this](ui::EventArgs* param) { return SystemSettingsMenuItemClick(param); });

	ui::CMenuElementUI* look_log = (ui::CMenuElementUI*)pMenu->FindControl(L"look_log");
	look_log->AttachSelect([this](ui::EventArgs* param) { return LookLogMenuItemClick(param); });
	
	return true;
}

bool ProductForm::SystemSettingsMenuItemClick(ui::EventArgs* param)
{
	if (m_pSysSettingsForm != nullptr && !m_pSysSettingsFormFlag.expired())
	{
		m_pSysSettingsForm->ShowWindow(true, false);
		SetForegroundWindow(m_pSysSettingsForm->GetHWND());

		return false;
	}
	else
	{
		m_pSysSettingsForm.release();
		m_pSysSettingsForm = std::make_unique<SystemSettingsForm>();
		m_pSysSettingsFormFlag = m_pSysSettingsForm->GetWeakFlag();
		m_pSysSettingsForm->Create(GetHWND(), L"系统设置", WS_OVERLAPPEDWINDOW, 0);

		m_pSysSettingsForm->CenterWindow();
		m_pSysSettingsForm->ShowWindow(true, true);

		return true;
	}
}

bool ProductForm::LookLogMenuItemClick(ui::EventArgs* param)
{
	LogForm::GetInstance()->CenterWindow();
	LogForm::GetInstance()->ShowWindow(true, true);

	SetForegroundWindow(LogForm::GetInstance()->GetHWND());

	return true;
}

bool ProductForm::SettlementButtonClick(ui::EventArgs* param)
{
	if (m_shoppingList.size() > 0)
	{
		LogInfo("Button", "商品结算...");

		ui::MsgboxCallback cb = [this](ui::MsgBoxRet ret) {
			if (ret == ui::MB_YES)
			{
				SetPaymentCountdownTimer();

				m_shoppingStage = ShoppingStage::PayProduct;
				std::string payUrl = OnDealUploadOrder();
				//测试
				//std::string payUrl = "https://store.octvisionland.com/oct-attachment/upload/store/1594714371336.png";
				if (payUrl == "null")
				{
					LogInfo("Button", "获取支付链接失败。。");
					KillPaymentCountdownTimer();

					std::string tipUTF8 = nbase::StringPrintf("speak=%s", nbase::win32::MBCSToUTF8("获取支付链接失败,无法付款"));
					ProductHttpService::GetInstance()->PostAIRequest(tipUTF8);
					m_shoppingStage = ShoppingStage::PickProduct;
				}
				else
				{
					RakNet::BitStream bsOut;
					RakNet::RakWString RStrName;
					RStrName = nbase::win32::MBCSToUnicode(payUrl).c_str();
					bsOut.WriteCompressed(RStrName);
					m_pRakServer->Signal(IDProductSettlement, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
				}
			}
		};

		ui::ShowMsgBox(GetHWND(), cb, ui::MT_INFOR, L"确定要进行商品结算吗？", false, L"提示", false, L"确定", false, L"取消", false);
	}
	else
	{
		LogWarning("Button", "请先添加商品到购物车，然后结算.");

		ui::ShowMsgBox(GetHWND(), nullptr, ui::MT_INFOR, L"请先添加商品到购物车，然后结算", false, L"提示", false, L"确定", false, L"", false);
	}

	return true;
}

bool ProductForm::CancelPurchaseButtonClick(ui::EventArgs* param)
{
	ui::MsgboxCallback cb = [this](ui::MsgBoxRet ret) {
		if (ret == ui::MB_YES)
		{
			LogWarning("Button", "取消商品支付, 清空购物车.");

			m_shoppingStage = ShoppingStage::PickProduct;

			// 取消支付，把购物车中的商品全部放回库存
			for (auto& item : m_shoppingList)
			{
				auto info = GetProductInfoByIdInDatabse(item.id);
				if (info)
				{
					info->amount += item.count;
				}
			}

			LogInfo("Http", "向AI端发送 cancelPayment取消支付消息...");
			ProductHttpService::GetInstance()->PostAIRequest("cancelPayment");

			m_shoppingList.clear();

			m_shoppingListBox->RemoveAll();

			m_productSearchResultList.clear();

			SignalRefreshShoppingListEvent(m_shoppingList);

			SignalRefreshSearchListEvent(m_productSearchResultList);

			RakNet::BitStream bsOut;
			m_pRakServer->Signal(IDCancelPurchase, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

			KillPaymentCountdownTimer();
		}
	};

	ui::ShowMsgBox(GetHWND(), cb, ui::MT_WARNING, L"放弃购买会清空购物车，确定要放弃购买吗？", false, L"警告", false, L"放弃购买", false, L"继续购买", false);

	return true;
}

bool ProductForm::StartGameButtonClick(ui::EventArgs* param)
{
	LogInfo("Button", "开始游戏.");

	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDStartGame, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	LogInfo("Http", "向AI端发送 startGame 消息...");
	ProductHttpService::GetInstance()->PostAIRequest("startGame");

	return true;
}

bool ProductForm::EndGameButtonClick(ui::EventArgs* param)
{
	LogInfo("Button", "结束游戏.");

	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDEndGame, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	LogInfo("Http", "向AI端发送 endGame 消息...");
	ProductHttpService::GetInstance()->PostAIRequest("endGame");

	return true;
}

bool ProductForm::PlayAdsButtonClick(ui::EventArgs* param)
{
	LogInfo("Button", "播放商品广告.");

	RakNet::BitStream bsOut;
	m_pRakServer->Signal(IDPlayAds, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}

bool ProductForm::ShowDrinksButtonClick(ui::EventArgs* param)
{
	LogInfo("Button", "展示所有饮料.");

	RakNet::BitStream bsOut;
	RakNet::RakWString description(TEXT("饮料"));
	bsOut.WriteCompressed(description);
	m_pRakServer->Signal(IDQueryProduct, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}

bool ProductForm::ShowFoodsButtonClick(ui::EventArgs* param)
{
	LogInfo("Button", "展示所有食品.");

	RakNet::BitStream bsOut;
	RakNet::RakWString description(TEXT("食品"));
	bsOut.WriteCompressed(description);

	m_pRakServer->Signal(IDQueryProduct, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}

bool ProductForm::SetNormalHolidayButtonClick(ui::EventArgs* param)
{
	LogInfo("Button", "设置日常背景.");

	RakNet::BitStream bsOut;
	RakNet::RakWString holiday(TEXT("日常"));
	bsOut.WriteCompressed(holiday);

	m_pRakServer->Signal(IDSetHolidayMode, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}

bool ProductForm::SetNewYearButtonClick(ui::EventArgs* param)
{
	LogInfo("Button", "设置春节背景.");

	RakNet::BitStream bsOut;
	RakNet::RakWString holiday(TEXT("春节"));
	bsOut.WriteCompressed(holiday);

	m_pRakServer->Signal(IDSetHolidayMode, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}

bool ProductForm::SetMoonFestivalButtonClick(ui::EventArgs* param)
{
	LogInfo("Button", "设置中秋背景.");

	RakNet::BitStream bsOut;
	RakNet::RakWString holiday(TEXT("中秋"));
	bsOut.WriteCompressed(holiday);

	m_pRakServer->Signal(IDSetHolidayMode, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}

bool ProductForm::SelectPotatoSalesmanButtonClick(ui::EventArgs* param)
{
	LogInfo("Button", "切换导购员土豆.");

	RakNet::BitStream bsOut;
	RakNet::RakWString holiday(TEXT("土豆"));
	bsOut.WriteCompressed(holiday);

	m_pRakServer->Signal(IDSelectSalesman, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}

bool ProductForm::SelectSpinachSalesmanButtonClick(ui::EventArgs* param)
{
	LogInfo("Button", "切换导购员菠菜.");

	RakNet::BitStream bsOut;
	RakNet::RakWString holiday(TEXT("菠菜"));
	bsOut.WriteCompressed(holiday);

	m_pRakServer->Signal(IDSelectSalesman, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	return true;
}

bool ProductForm::OnClicked(ui::EventArgs* args)
{
	auto buttonName = args->pSender->GetName();
	auto result = m_btnPairs.find(buttonName);
	if (result != m_btnPairs.end())
	{
		if (m_shoppingStage == ShoppingStage::PayProduct)
		{
			auto productName = result->second->GetText();
			LogWarning("Button", "点击按钮添加商品%s失败, 请完成商品支付后再进行操作", nbase::win32::UnicodeToMBCS(productName).c_str());
			return false;
		}

		unsigned int productID = 0;
		nbase::StringToUint(result->second->GetDataID(), &productID);
		
		if (productID == 0) return false;

		AddOrIncreaseProductNumInShoppingList(productID, 1);
		RefreshShoppingListBox();
		//AddProductToShoppingListOrRefreshList(productID);

		SignalRefreshShoppingListEvent(m_shoppingList);

		DumpProductAmountMessage(productID);

		return true;
	}
	return false;
}


ProductInfo* ProductForm::AddProductToShoppingListOrRefreshList(unsigned int productID, int number)
{
	auto productInfo = GetProductInfoByIdInDatabse(productID);
	if (!productInfo)
	{
		LogError("Cart", "通过商品ID %d 查找商品失败.", productID);
		return nullptr;
	}

	if (number > productInfo->amount)
	{
		LogWarning("Cart", "商品 %s 增加的数量%d超过库存, 采用库存数量%d.", productInfo->name.c_str(), number, productInfo->amount);
		number = productInfo->amount;
	}

	LogInfo("Cart", "添加 %s 到购物车, 单价 %g, 数量%d", (productInfo->name).c_str(), productInfo->price, number);

	// 根据商品id查找商品购物车控件列表里面是否已经存在该商品
	bool existingItem = false;
	for (int i = 0; i < m_shoppingListBox->GetCount(); ++i)
	{
		auto item = dynamic_cast<ProductItem *>(m_shoppingListBox->GetItemAt(i));
		// 商品存在，增加商品数量
		if (item->GetItemID() == productID)
		{
			existingItem = true;
			item->IncreaseProductNumber(number);

			break;
		}
	}

	// 商品不在购物车控件列表里面，创建商品控件子项并添加到列表
	if (!existingItem)
	{
		ProductItem *item = new ProductItem();
		ui::GlobalManager::FillBoxWithCache(item, L"skin/product_item.xml");
		std::wstring img = L"product.png";

		item->InitSubControls(img, nbase::win32::MBCSToUnicode(productInfo->name), productID, number);
		item->SetDataID(nbase::UintToString16(productID));
		item->SetPriceText(productInfo->price);

		item->AttachDeleteButtonClick([this](ui::EventArgs* param) { return OnItemDeleteButtonClicked(param); });
		item->AttachIncreaseButtonClick([this](ui::EventArgs* param) { return OnItemIncreaseButtonClicked(param); });
		item->AttachReduceButtonClick([this](ui::EventArgs* param) { return OnItemReduceButtonClicked(param); });

		m_shoppingListBox->Add(item);

		// 添加商品数据到商品购物数据列表中
		return AddProductToShoppingList(productID, number);
	}
	else
	{
		// 更新商品购物数据列表，增加商品数量
		return IncreaseProductNumInShoppingList(productID, number);
	}

	//SignalAddProductEvent(productID, number);

	// 刷新客户端购物车
	//SignalRefreshShoppingListEvent(m_shoppingList);
}

ProductInfo* ProductForm::AddProductToShoppingList(unsigned int productID, int number)
{
	for (auto& item : m_productDatabase)
	{
		if (item.id == productID)
		{
			if (item.amount >= number)
			{
				// 减少商品库存数量
				item.amount -= number;
				item.count = number;

				// 添加购物车
				m_shoppingList.push_back(item);

				return &item;
			}
		}
	}

	return nullptr;
}

bool ProductForm::RemoveProductFromShoppingList(unsigned int productID)
{
	for (auto iter = m_shoppingList.begin(); iter != m_shoppingList.end();  ++iter)
	{
		auto id = iter->id;
		if (id == productID)
		{
			// 恢复商品库存数量
			auto info = GetProductInfoByIdInDatabse(productID);
			info->amount += iter->count;

			m_shoppingList.erase(iter);
			return true;
		}
	}

	return false;
}

bool ProductForm::CheckProductStock(unsigned int productID, int number)
{
	for (auto& info : m_productDatabase)
	{
		if (info.id == productID)
		{
			if (info.amount >= number)
				return true;
			else
				return false;
		}
	}

	return false;
}

ProductInfo*  ProductForm::IncreaseProductNumInShoppingList(unsigned int productID, int number)
{
	for (auto& item : m_shoppingList)
	{
		if (item.id == productID)
		{
			auto info = GetProductInfoByIdInDatabse(productID);
			if (info->amount >= number)
			{
				// 减少商品库存数量
				info->amount -= number;
				
				item.amount -= number;
				item.count += number;

				return &item;
			}
		}
	}

	return nullptr;
}

ProductInfo* ProductForm::AddOrIncreaseProductNumInShoppingList(unsigned int productID, int number /*= 1*/)
{
	auto result = GetProductInfoByIdInShoppingList(productID);
	if (result)
	{
		return IncreaseProductNumInShoppingList(productID, number);
	}
	else
	{
		return AddProductToShoppingList(productID, number);
	}
}

ProductInfo*  ProductForm::ReduceProductNumInShoppingList(unsigned int productID, int number)
{
	for (auto& item : m_shoppingList)
	{
		if (item.id == productID)
		{
			auto info = GetProductInfoByIdInDatabse(productID);

			if (item.count > number)
			{
				// 增加商品数量
				info->amount += number;

				item.amount += number;
				item.count -= number;

				return &item;
			}
			else // 购物车数量小于number，从购物车列表中移除商品
			{
				// 增加商品数量
				info->amount += item.count;

				item.amount += item.count;
				item.count = 0;
				
				//m_shoppingList.erase(iter);

				return &item;
			}
		}
	}

	return nullptr;
}

ProductInfo* ProductForm::SetProductNumInShoppingList(unsigned int productID, int number)
{
	for (auto& item : m_shoppingList)
	{
		if (item.id == productID)
		{
			if (number <= 0)
			{
				//m_shoppingList.erase(iter);
				item.amount += item.count;
				item.count = 0;
				return &item;
			}

			auto info = GetProductInfoByIdInDatabse(productID);
			// 库存数量+购物车中的数量 >= 要设置的数量
			if (item.count + info->amount >= number)
			{
				// 先把购物车商品放回库存
				info->amount += item.count;
				// 减少库存数量
				info->amount -= number;

				item.amount += item.count;
				item.amount -= number;
				// 设置商品在购物车数量
				item.count = number;

				return &item;
			}
		}
	}

	return nullptr;
}

bool ProductForm::GetProductNameByIdInDatabse(unsigned int productID, std::string& outName)
{
	for (auto& info : m_productDatabase)
	{
		if (info.id == productID)
		{
			outName = info.name;
			return true;
		}
	}

	return false;
}

ProductInfo* ProductForm::GetProductInfoByIdInDatabse(unsigned int productID)
{
	for (auto& item : m_productDatabase)
	{
		if (item.id == productID)
		{
			return &item;
		}
	}

	return nullptr;
}

ProductInfo* ProductForm::GetProductInfoByNameInDatabse(const std::string&productName)
{
	for (auto& info : m_productDatabase)
	{
		if (info.name.find(productName) != std::string::npos)
		{
			return &info;
		}
	}

	return nullptr;
}

ProductInfo* ProductForm::GetProductInfoByDescriptionInDatabse(const std::string&productDescription)
{
	for (auto& info : m_productDatabase)
	{
		if (info.description.find(productDescription) != std::string::npos)
		{
			return &info;
		}
	}

	return nullptr;
}

ProductInfo* ProductForm::GetProductInfoByBarcodeInDatabse(const std::string&barcode)
{
	for (auto& info : m_productDatabase)
	{
		if (info.barcode.compare(barcode) == 0)
		{
			return &info;
		}
	}

	return nullptr;
}

void ProductForm::GetProductInfoListByNameInDatabse(const std::string&productName, std::vector<ProductInfo>& list)
{
	for (auto& info : m_productDatabase)
	{
		if (info.name.find(productName) != std::string::npos)
		{
			list.push_back(info);
		}
	}
}

void ProductForm::GetProductInfoListByDescriptionInDatabse(const std::string&productDescription, std::vector<ProductInfo>& list)
{
	for (auto& info : m_productDatabase)
	{
		if (info.description.find(productDescription) != std::string::npos)
		{
			list.push_back(info);
		}
	}
}

void ProductForm::GetProductInfoListByNameInSearchResult(const std::string&productName, std::vector<ProductInfo>& list)
{
	for (auto& info : m_productSearchResultList)
	{
		if (info.name.find(productName) != std::string::npos)
		{
			list.push_back(info);
		}
	}
}

void ProductForm::GetProductInfoListByDescriptionInSearchResult(const std::string&productDescription, std::vector<ProductInfo>& list)
{
	for (auto& info : m_productSearchResultList)
	{
		if (info.description.find(productDescription) != std::string::npos)
		{
			list.push_back(info);
		}
	}
}

int ProductForm::GetProductCountByIdInShoppingList(unsigned int productID)
{
	for (auto& item : m_shoppingList)
	{
		if (item.id == productID)
		{
			return item.count;
		}
	}

	return 0;
}

ProductInfo* ProductForm::GetProductInfoByIdInShoppingList(unsigned int productID)
{
	for (auto& item : m_shoppingList)
	{
		if (item.id == productID)
		{
			return &item;
		}
	}

	return nullptr;
}

ProductInfo* ProductForm::GetProductInfoByNameInShoppingList(const std::string&productName)
{
	for (auto& item : m_shoppingList)
	{
		if (item.name.find(productName) != std::string::npos)
		{
			return &item;
		}
	}

	return nullptr;
}

ProductInfo* ProductForm::GetProductInfoByDescriptionInShoppingList(const std::string&productDescription)
{
	for (auto& item : m_shoppingList)
	{
		if (item.description.find(productDescription) != std::string::npos)
		{
			return &item;
		}
	}

	return nullptr;
}

void ProductForm::GetProductInfoListByNameInShoppingList(const std::string&productName, std::vector<ProductInfo>& list)
{
	for (auto& item : m_shoppingList)
	{
		if (item.name.find(productName) != std::string::npos)
		{
			list.push_back(item);
		}
	}
}

void ProductForm::GetProductInfoListByDescriptionInShoppingList(const std::string&productDescription, std::vector<ProductInfo>& list)
{
	for (auto& item : m_shoppingList)
	{
		if (item.description.find(productDescription) != std::string::npos)
		{
			list.push_back(item);
		}
	}
}

void ProductForm::PopupTrayMenu(POINT point)
{
	ui::CMenuWnd *pMenu = new ui::CMenuWnd(nullptr);
	ui::STRINGorID xml(L"tray_menu.xml");
	pMenu->Init(xml, _T("xml"), point, ui::CMenuWnd::RIGHT_TOP);

	//注册回调
	ui::CMenuElementUI* quit = (ui::CMenuElementUI*)pMenu->FindControl(L"quit");
	quit->AttachSelect([this](ui::EventArgs* param) { return QuitMenuItemClick(param); });
	//显示
	pMenu->Show();
}

bool ProductForm::QuitMenuItemClick(ui::EventArgs* args)
{
	PostMessage(WM_CLOSE);

	return true;
}

void ProductForm::ReceiveProductListFromClient(RakNet::BitStream * userData, RakNet::Packet *packet)
{
	int count = 0;
	userData->Read(count);
	for (int n = 0; n < count; ++n)
	{
		RakNet::RakString barcode;
		userData->ReadCompressed(barcode);

		int amount = 0;
		userData->Read(amount);

		auto info = GetProductInfoByBarcodeInDatabse(barcode.C_String());
		if (info)
		{
			info->amount = amount;

			// 刷新商品购物列表中的库存数据
			auto shoppingInfo = GetProductInfoByIdInShoppingList(info->id);
			if (shoppingInfo)
			{
				shoppingInfo->amount = amount;
			}
		}
	}

	nbase::ThreadManager::PostTask(kThreadUI, [=]() {
		LogInfo("仓管", "接收到[上传商品库存] 请求. 共上传了 %d 种商品库存", count);
	});

	ProcessGameRequestProductInventory(userData, packet);

	//m_productDatabase.clear();
	//RakNet::RakString buffer;
	//userData->ReadCompressed(buffer);
	//StaticJsonDocument<PACKET_SIZE> doc;
	//deserializeJson(doc, buffer.C_String());
	//for (size_t i = 0; i < doc["list"].size(); i++)
	//{
	//	ProductInfo info;
	//	JsonObject jo = doc["list"][i];
	//	info.id = jo["id"].as<unsigned int>(); // id
	//	info.name = jo["na"].as<std::string>(); // name
	//	info.description = jo["de"].as<std::string>(); // description
	//	info.category = jo["ca"].as<std::string>(); // category
	//	info.image = jo["im"].as<std::string>(); // image
	//	info.price = jo["pr"].as<float>(); // price
	//	info.amount = jo["am"].as<unsigned int>(); // amount
	//	m_productDatabase.push_back(info);
	//}
	//RakNet::BitStream bsOut;
	//bsOut.Write(m_productDatabase.size() > 0 ? true : false);
	//m_pRakServer->Signal(IDProductListFeedback, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}

void ProductForm::ReceiveCustomerInfoFromClient(RakNet::BitStream * userData, RakNet::Packet *packet)
{
	RakNet::RakString userID;
	userData->ReadCompressed(userID);

	// 0为男性 1为女性
	int sex;
	userData->Read(sex);

	int age;
	userData->Read(age);

	std::string  sexString = (sex == 0 ? "男" : "女");

	nbase::ThreadManager::PostTask(kThreadUI, [=]() {
		LogInfo("仓管", "接收到 [识别的顾客信息: id %s, 性别 %s, 年龄 %d]", userID.C_String(), sexString.c_str(), age);
	});

	// 转发接收到的顾客信息到所有客户端(主要是游戏客户端); 通知AI端
	TriggerCustomerInfoAction(userID, sex, age);

	
}

void ProductForm::ReceiveRegularCustomerInfoFromClient(RakNet::BitStream * userData, RakNet::Packet *packet)
{
	RakNet::RakString userID;
	userData->ReadCompressed(userID);

	// 0为男性 1为女性
	int sex;
	userData->Read(sex);

	int age;
	userData->Read(age);

	std::string  sexString = (sex == 0 ? "男" : "女");

	int regularCustomer = 0;//是否熟客
	userData->Read(regularCustomer);

	nbase::ThreadManager::PostTask(kThreadUI, [userID, sexString, age, regularCustomer]() {
		std::string message = nbase::StringPrintf("接收到顾客信息: userID %s, 性别 %s, 年龄 %d, regularCustomer %d", userID.C_String(), sexString.c_str(), age, regularCustomer);
		LogInfo("Http", message.c_str());
	});

	TriggerCustomerInfoAction(userID, sex, age, regularCustomer);
}

void ProductForm::TriggerCustomerInfoAction(const RakNet::RakString &userID, int sex, int age)
{
	RakNet::BitStream bsOut;
	bsOut.WriteCompressed(userID);
	bsOut.Write(sex);
	bsOut.Write(age);

	// 检测到顾客已经离开，触发游戏结束事件
	if (userID.StrICmp("nobody") == 0)
	{
		nbase::ThreadManager::PostTask(kThreadUI, []() {
			LogInfo("仓管", "顾客已经离开, 触发游戏结束.");
			LogInfo("Http", "顾客已经离开, 向AI端发送 endGame 消息...");
		});

		m_pRakServer->Signal(IDEndGame, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

		ProductHttpService::GetInstance()->PostAIRequest("endGame");
	}
	else
	{

		nbase::ThreadManager::PostTask(kThreadUI, []() {
			LogInfo("仓管", "顾客光临, 触发游戏开始.");
			LogInfo("Http", "顾客光临, 向AI端发送 startGame 消息...");
		});

		m_pRakServer->Signal(IDStartGame, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
		
		RakNet::BitStream bssayHelloOut;
		m_pRakServer->Signal(IDSayHello, &bssayHelloOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		m_pRakServer->Signal(IDUploadCustomerInfo, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

		ProductHttpService::GetInstance()->PostAIRequest("startGame");

	}
}

void ProductForm::TriggerCustomerInfoAction(const RakNet::RakString &userID, int sex, int age, int regularCus)
{
	RakNet::BitStream bsOut;
	bsOut.WriteCompressed(userID);
	bsOut.Write(sex);
	bsOut.Write(age);
	bsOut.Write(regularCus);
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("仓管", "熟客光临.");
		LogInfo("Http", "熟客光临, 向AI端发送 RegularCustomer 消息...");
	});

	m_pRakServer->Signal(IDStartGame, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	RakNet::BitStream bssayHelloOut;
	m_pRakServer->Signal(IDSayHello, &bssayHelloOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
	//std::this_thread::sleep_for(std::chrono::milliseconds(50));

	//m_pRakServer->Signal(IDRegularCustomer, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	ProductHttpService::GetInstance()->PostAIRequest("RegularCustomer");
}

void ProductForm::ReceiveShipmentResultFromClient(RakNet::BitStream * userData, RakNet::Packet *packet)
{
	// 返回挑选商品阶段
	m_shoppingStage = ShoppingStage::PickProduct;

	KillShipmentTimer();

	RakNet::BitStream bssayHelloOut;
	m_pRakServer->Signal(IDPaymentSuccessEnd, &bssayHelloOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	bool success = false;
	userData->Read(success);
	if (success)
	{
		nbase::ThreadManager::PostTask(kThreadUI, []() {
			LogInfo("仓管", "商品出货成功.");
			LogInfo("Http", "向AI端发送 shipmentSuccess 出货成功消息...");
		});

		m_shipmentStatus = ShipmentStatus::Success;

		ProductHttpService::GetInstance()->PostAIRequest("shipmentSuccess");

	}
	else
	{
		unsigned int count = 0; // 购物车商品种类数目
		userData->Read(count);

		RakNet::RakString failedReason;
		userData->ReadCompressed(failedReason);

		for (unsigned int i = 0; i < count; ++i)
		{
			unsigned int id = 0; // 商品id
			userData->Read(id);

			RakNet::RakString barcode; // 商品条形码
			userData->ReadCompressed(barcode);

			unsigned int number = 0; // 商品数量
			userData->Read(number);
		}

		nbase::ThreadManager::PostTask(kThreadUI, [=]() {
			LogError("仓管", "商品出货失败: %s", failedReason.C_String());
			LogError("Http", "向AI端发送 shipmentFail 出货失败消息...");
		});

		ProductHttpService::GetInstance()->PostAIRequest("shipmentFail");

		m_shipmentStatus = ShipmentStatus::Failed;
	}
}

void ProductForm::ProcessGameCancelPurchase(RakNet::BitStream * userData, RakNet::Packet *packet)
{
	// 返回挑选商品阶段
	m_shoppingStage = ShoppingStage::PickProduct;

	nbase::ThreadManager::PostTask(kThreadUI, [=]() {

		// 取消支付，把购物车中的商品全部放回库存
		for (auto& item : m_shoppingList)
		{
			auto info = GetProductInfoByIdInDatabse(item.id);
			if (info)
			{
				info->amount += item.count;
			}
		}

		m_shoppingList.clear();
		m_shoppingListBox->RemoveAll();

		LogWarning("Cart", "支付超时, 游戏端取消商品支付.");
	});
}

void ProductForm::ProcessGameRequestProductInventory(RakNet::BitStream * userData, RakNet::Packet *packet)
{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Game", "向游戏端返回查询商品库存结果...");
	});

	RakNet::BitStream bsOut;
	auto count = m_productDatabase.size();
	bsOut.Write(count);

	for (auto& item : m_productDatabase)
	{
		RakNet::RakWString name;
		//name = nbase::win32::MBCSToUnicode(item.name).c_str();
		//前端展示商品名称+规格
		name = nbase::win32::MBCSToUnicode(item.name + item.specification).c_str();
		bsOut.WriteCompressed(name);

		RakNet::RakWString description;
		description = nbase::win32::MBCSToUnicode(item.description).c_str();
		bsOut.WriteCompressed(description);

		RakNet::RakWString category;
		category = nbase::win32::MBCSToUnicode(item.mainCategory).c_str();
		bsOut.WriteCompressed(category);

		RakNet::RakWString image;
		image = nbase::win32::MBCSToUnicode(item.image).c_str();
		bsOut.WriteCompressed(image);

		RakNet::RakWString specification;
		specification = nbase::win32::MBCSToUnicode(item.specification).c_str();
		bsOut.WriteCompressed(specification);

		RakNet::RakWString unit;
		unit = nbase::win32::MBCSToUnicode(item.unit).c_str();
		bsOut.WriteCompressed(unit);

		RakNet::RakWString keywords;
		keywords = nbase::win32::MBCSToUnicode(item.keywords).c_str();
		bsOut.WriteCompressed(keywords);

		bsOut.Write(item.price);

		bsOut.Write(item.amount);

		bsOut.Write(item.id);
	}

	SignalMessage(IDResponseProductInventory, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED);

}

void ProductForm::ProcessGamePaymentSuccess(RakNet::BitStream * userData, RakNet::Packet *packet)
{
	/*nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Game", "游戏自动支付成功. ");
	});*/
	//屏蔽游戏自动支付
	//OnPaymentSuccess();
}

void ProductForm::ProcessShowGoodsBoxUI(RakNet::BitStream* userData, RakNet::Packet* packet)
{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("上货", "显示上货界面UI. ");
	});

	int boxID = 0;
	userData->Read(boxID);
	int slotCount = 0;
	userData->Read(slotCount);
	int recommendedSlot = 0; // 建议使用的格子号
	userData->Read(recommendedSlot);

	int categoryCount = 0;
	userData->Read(categoryCount);
	for (int i = 0; i < categoryCount; ++i)
	{
		int slotIndex = 0; // 商品格子号
		userData->Read(slotIndex);

		unsigned int id = 0; // 商品id
		userData->Read(id);

		RakNet::RakString barcode; // 商品条形码
		userData->ReadCompressed(barcode);

		unsigned int number = 0; // 商品数量
		userData->Read(number);
	}


	m_pRakServer->Signal(IDShowGoodsBoxUI, userData, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}

void ProductForm::ProcessHideGoodsBoxUI(RakNet::BitStream* userData, RakNet::Packet* packet)
{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("上货", "隐藏上货界面UI. ");
	});

	m_pRakServer->Signal(IDHideGoodsBoxUI, userData, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}

void ProductForm::ProcessShowStorageTip(RakNet::BitStream* userData, RakNet::Packet* packet)
{
	RakNet::RakString tip;
	userData->ReadCompressed(tip);

	nbase::ThreadManager::PostTask(kThreadUI, [tip]() {
		LogInfo("仓管", "显示上货提示:%s ", tip.C_String());
		LogInfo("Http", "向AI端发送显示上货提示消息:%s ", tip.C_String());
	});

	std::string tipUTF8 = nbase::StringPrintf("speak=%s", nbase::win32::MBCSToUTF8(tip.C_String()).c_str());

	ProductHttpService::GetInstance()->PostAIRequest(tipUTF8);
}

void ProductForm::ProcessExitStorageMode(RakNet::BitStream* userData, RakNet::Packet* packet)
{
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("仓管", "接收到 [退出上货模式] 消息.");
		LogInfo("Http", "向AI端发送 exitStorageMode 退出上货模式消息...");
	});

	// 通知AI端已经退出上货模式
	ProductHttpService::GetInstance()->PostAIRequest("exitStorageMode");
}


void ProductForm::ProcessCustomerInfo(const httplib::Request &request, httplib::Response &response)
{
	std::string userID = nbase::win32::UTF8ToMBCS(request.get_param_value("userid"));
	std::string sexString = nbase::win32::UTF8ToMBCS(request.get_param_value("sex"));
	std::string ageString = nbase::win32::UTF8ToMBCS(request.get_param_value("age"));
	
	int sex = 0; // 0男；1女
	nbase::StringToInt(sexString, &sex);

	int age = 0;
	nbase::StringToInt(ageString, &age);

	nbase::ThreadManager::PostTask(kThreadUI, [userID, sexString, age]() {
		std::string message = nbase::StringPrintf("接收到顾客信息: userID %s, 性别 %s, 年龄 %d", userID.c_str(), sexString.c_str(), age);
		LogInfo("Http",  message.c_str());
	});

	TriggerCustomerInfoAction(userID.c_str(), sex, age);

	SetJsonFormatHttpResponse(response, true, "操作成功");
}

void ProductForm::ProcessRegularCustomerInfo(const httplib::Request &request, httplib::Response &response)
{
	std::string userID = nbase::win32::UTF8ToMBCS(request.get_param_value("userid"));
	std::string sexString = nbase::win32::UTF8ToMBCS(request.get_param_value("sex"));
	std::string ageString = nbase::win32::UTF8ToMBCS(request.get_param_value("age"));

	int sex = 0; // 0男；1女
	nbase::StringToInt(sexString, &sex);

	int age = 0;
	nbase::StringToInt(ageString, &age);

	TriggerCustomerInfoAction(userID.c_str(), sex, age, 1);
}

//bool ProductForm::ProcessAIExitStorageMode(const httplib::Request& request, httplib::Response& response)
//{
//	nbase::ThreadManager::PostTask(kThreadUI, []() {
//		LogInfo("Http", "接收到 [退出上货模式] 消息.");
//	});
//
//	SetJsonFormatHttpResponse(response, true, "操作成功");
//
//	RakNet::BitStream bsOut;
//	m_pRakServer->Signal(IDExitStorageMode, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
//
//	return true;
//}



void ProductForm::DumpProductAmountMessage(unsigned int productID)
{
	auto databaseInfo = GetProductInfoByIdInDatabse(productID);
	if (databaseInfo == nullptr)
		return;

	int buyCount = GetProductCountByIdInShoppingList(productID);

	std::string message = nbase::StringPrintf("商品 %s 当前在购物车中的数量为%d, 库存数量为%d", databaseInfo->name.c_str(), buyCount, databaseInfo->amount);

	LogInfo("Message", message.c_str());
}

std::string ProductForm::ArrayToString(const std::vector<std::string>& list, bool mbcs)
{
	std::string result;
	for (auto item : list)
	{
		if (mbcs)
		{
			auto itemMBCS = nbase::win32::UTF8ToMBCS(item);
			result += itemMBCS + ' ';
		}
		else
		{
			result += item + ' ';
		}
	}

	result = result.substr(0, result.find_last_of(' '));
	return result;
}



void ProductForm::SetJsonFormatHttpResponse(httplib::Response &response, bool success, const std::string&message, const std::vector<ProductInfo>& list)
{
	// 向AI端返回处理结果
	// buffer里面存储的内容采用多字节编码
	const int buffer_size = 4096;
	char buffer[buffer_size] = { 0 };
	StaticJsonDocument<buffer_size> doc;

	JsonObject jo = doc.createNestedObject();
	jo["success"] = success;
	jo["msg"] = message;
	JsonArray statusInfoArray = jo.createNestedArray("list");
	for (auto& item : list)
	{
		JsonObject product1 = statusInfoArray.createNestedObject();
		product1["na"] = item.name; // name
		product1["pr"] = item.price; // price
		product1["am"] = item.amount; // amount: 商品库存
		product1["co"] = item.count; // count: 购物车中的商品数量
		product1["id"] = item.id;
	}

	serializeJson(doc, buffer, buffer_size);

	// 需要把多字节编码的buffer转为UTF-8编码
	std::string bufferUTF8 = nbase::UTF16ToUTF8(nbase::win32::MBCSToUnicode(buffer));

	response.set_content(bufferUTF8, "application/json");
}

int ProductForm::CalcProductIndex(int row, int column)
{
	int selectedProductIndex = 0;
	int length = m_productSearchResultList.size();
	int rowCount = (length % ProductNumDisplayedPerRow == 0) ? length / ProductNumDisplayedPerRow : (length / ProductNumDisplayedPerRow + 1);
	if (column > 0) // 选择正数列(例如第2项)
	{
		if (row >= 1) // 选择正数行(例如第2行)
		{
			selectedProductIndex = column + (row - 1) * ProductNumDisplayedPerRow;
		}
		else if (row < 0) // 选择负数行(例如倒数第2行)
		{
			selectedProductIndex = column + (rowCount + row) * ProductNumDisplayedPerRow;
		}
		else // 不选择(row = 0)
		{
			selectedProductIndex = column;
		}

		selectedProductIndex -= 1;
	}
	else // 选择负数列(例如倒数2项)
	{
		if (row > 0) // 选择正数行(例如第2行)
		{
			selectedProductIndex = column + row * ProductNumDisplayedPerRow;
		}
		else if (row < 0) // 选择负数行(例如倒数第2行)
		{
			selectedProductIndex = column + (rowCount + row + 1) * ProductNumDisplayedPerRow;
		}
		else // 不选择(row = 0)
		{
			selectedProductIndex = column + length;
		}
			
	}

	return selectedProductIndex;
}

void ProductForm::SetPaymentCountdownTimer()
{
	SetTimer(this->m_hWnd, COUNTDOWN_TIMER_ID, 1000, nullptr);

	m_shoppingListBox->SetEnabled(false);

	m_countdown = CountdownLimit;
}

void ProductForm::KillPaymentCountdownTimer()
{
	KillTimer(this->m_hWnd, COUNTDOWN_TIMER_ID);

	m_lblPurchaseCoundown->SetVisible(false);

	m_shoppingListBox->SetEnabled(true);
	
	m_countdown = CountdownLimit;
}

void ProductForm::SetLuckyDrawTimer()
{
	SetTimer(this->m_hWnd, LUCKYDRAW_TIMER_ID, 1000, nullptr);
}

void ProductForm::KillLuckyDrawTimer()
{
	KillTimer(this->m_hWnd, LUCKYDRAW_TIMER_ID);
}

void ProductForm::SetShipmentTimer()
{
	SetTimer(this->m_hWnd, SHIPMENT_TIMER_ID, 1000, nullptr);

	m_shipmentTime = ShipmentTimeLimit;
}

void ProductForm::KillShipmentTimer()
{
	KillTimer(this->m_hWnd, SHIPMENT_TIMER_ID);

	m_shipmentTime = ShipmentTimeLimit;
}

void ProductForm::Test()
{
	//std::string s1 = "可口可乐_3";
	//auto result = ProductDatabaseManager::GetInstance()->ParseKeywordField(s1);

	//std::vector<std::string> keywords;
	//keywords.push_back(s1);
	////keywords.push_back("大瓶的_7");
	////keywords.push_back("冰冻_7");

	//std::string condition;
	//for (auto keyword : keywords)
	//{
	//	condition += keyword + " and ";
	//}

	//condition = condition.substr(0, condition.find_last_of(" and ") - 4);

	std::string s2;
}

bool ProductForm::OnItemIncreaseButtonClicked(ui::EventArgs* args)
{
	auto id = args->pSender->GetDataID();
	unsigned int productID = 0;
	nbase::StringToUint(id, &productID);

	std::string productName;
	GetProductNameByIdInDatabse(productID, productName);

	IncreaseProductNumInShoppingList(productID, 1);

	RefreshShoppingListBox();

	//SignalAddProductEvent(productID);

	// 刷新客户端购物车
	SignalRefreshShoppingListEvent(m_shoppingList);

	LogInfo("Button", "添加 %s 到购物车", productName.c_str());

	DumpProductAmountMessage(productID);
	
	return true;
}

bool ProductForm::OnItemReduceButtonClicked(ui::EventArgs* args)
{
	auto id = args->pSender->GetDataID();
	unsigned int productID = 0;
	nbase::StringToUint(id, &productID);

	std::string productName;
	GetProductNameByIdInDatabse(productID, productName);

	ReduceProductNumInShoppingList(productID);

	LogInfo("Button", "从购物车中减少商品 %s", productName.c_str());

	RefreshShoppingListBox();

	// 刷新客户端购物车
	SignalRefreshShoppingListEvent(m_shoppingList);

	DumpProductAmountMessage(productID);

	return true;
}

bool ProductForm::OnItemDeleteButtonClicked(ui::EventArgs* args)
{
	auto id = args->pSender->GetDataID();
	unsigned int productID = 0;
	nbase::StringToUint(id, &productID);

	std::string productName;
	GetProductNameByIdInDatabse(productID, productName);

	RemoveProductFromShoppingList(productID);
	
	RefreshShoppingListBox();

	//SignalRemoveProductEvent(productID);
	// 刷新客户端购物车
	SignalRefreshShoppingListEvent(m_shoppingList);

	LogInfo("Button", "从购物车中移除商品 %s.", productName.c_str());

	DumpProductAmountMessage(productID);

	return true;
}

void ProductForm::SignalAddProductEvent(unsigned int productID, int number)
{
	auto productInfo = GetProductInfoByIdInDatabse(productID);

	std::wstring nameUnicode = nbase::win32::MBCSToUnicode(productInfo->name);
	RakNet::RakWString productName(nameUnicode.c_str());
	
	RakNet::BitStream bsOut;
	bsOut.WriteCompressed(productName);
	bsOut.Write(productInfo->price);
	bsOut.Write(number);

	m_pRakServer->Signal(IDAddProductToChart, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}


void ProductForm::SignalReduceProductEvent(unsigned int productID, int number)
{
	auto productInfo = GetProductInfoByIdInDatabse(productID);

	std::wstring nameUnicode = nbase::win32::MBCSToUnicode(productInfo->name);
	RakNet::RakWString productName(nameUnicode.c_str());
	
	RakNet::BitStream bsOut;
	bsOut.WriteCompressed(productName);
	bsOut.Write(number);

	m_pRakServer->Signal(IDReduceProductFromChart, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}


void ProductForm::SignalRemoveProductEvent(unsigned int productID)
{
	std::string name;
	GetProductNameByIdInDatabse(productID, name);

	std::wstring nameUnicode = nbase::win32::MBCSToUnicode(name);
	RakNet::RakWString productName(nameUnicode.c_str());	
	
	RakNet::BitStream bsOut;
	bsOut.WriteCompressed(productName);

	m_pRakServer->Signal(IDRemoveProductFromChart, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}



void ProductForm::SignalRefreshSearchListEvent(PRODUCT_ARRAY& list)
{
	if (list.size() == 0)
		return;
	
	for (auto iter = list.begin(); iter != list.end(); ++iter)
	{
		auto info = GetProductInfoByBarcodeInDatabse(iter->barcode);
		if (info)
		{
			iter->amount = info->amount;
		}
	}

	RakNet::BitStream bsOut;
	bsOut.Write(ProductNumDisplayedPerRow);

	bsOut.Write(list.size());

	// 默认把第一种商品的类别当作整个列表的商品类别
	RakNet::RakWString listCategory;
	listCategory = nbase::win32::MBCSToUnicode(list[0].mainCategory).c_str();
	bsOut.WriteCompressed(listCategory);

	for (auto& item : list)
	{
		RakNet::RakWString name;
		//name = nbase::win32::MBCSToUnicode(item.name).c_str();
		name = nbase::win32::MBCSToUnicode(item.name + item.specification).c_str();
		bsOut.WriteCompressed(name);

		RakNet::RakWString description;
		description = nbase::win32::MBCSToUnicode(item.description).c_str();
		bsOut.WriteCompressed(description);

		RakNet::RakWString category;
		category = nbase::win32::MBCSToUnicode(item.mainCategory).c_str();
		bsOut.WriteCompressed(category);

		RakNet::RakWString image;
		image = nbase::win32::MBCSToUnicode(item.image).c_str();
		bsOut.WriteCompressed(image);

		bsOut.Write(item.price);

		bsOut.Write(item.amount);

		bsOut.Write(item.id);
	}

	SignalMessage(IDRefreshSearchList, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED);
}

void ProductForm::SignalRefreshShoppingListEvent(const PRODUCT_ARRAY& list)
{
	RakNet::BitStream bsOut;
	bsOut.Write(list.size());

	for (auto& item : list)
	{
		RakNet::RakWString name;
		//name = nbase::win32::MBCSToUnicode(item.name).c_str();
		name = nbase::win32::MBCSToUnicode(item.name + item.specification).c_str();
		bsOut.WriteCompressed(name);

		RakNet::RakWString description;
		description = nbase::win32::MBCSToUnicode(item.description).c_str();
		bsOut.WriteCompressed(description);

		RakNet::RakWString category;
		category = nbase::win32::MBCSToUnicode(item.mainCategory).c_str();
		bsOut.WriteCompressed(category);

		RakNet::RakWString image;
		image = nbase::win32::MBCSToUnicode(item.image).c_str();
		bsOut.WriteCompressed(image);

		bsOut.Write(item.price);

		bsOut.Write(item.count);

		bsOut.Write(item.id);

		bsOut.Write(item.amount);

		/*std::string strname = item.name;
		int count = item.count;
		float price = item.price;
		nbase::ThreadManager::PostTask(kThreadUI, [strname, count,price]() {
			std::string message = nbase::StringPrintf("购物车列表: 产品名称 %s, 数量 %d, 单价 %.1f", strname.c_str(), count, price);
			LogInfo("Debug", message.c_str());
		});*/
	}

	SignalMessage(IDRefreshShoppingList, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED);
}

void ProductForm::SignalStockOutEvent()
{
	RakNet::BitStream bsOut;
	SignalMessage(IDStockOut, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED);
}

#define CPPHTTPLIB_OPENSSL_SUPPORT
std::string ProductForm::OnDealUploadOrder()
{
	// 返回挑选商品阶段
	m_shoppingStage = ShoppingStage::PayProduct;
	LogInfo("Server", "上传出货订单到后台以获取支付链接");
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	std::string response = ProductHttpService::GetInstance()->PostOrderToBackgroundByHttp(m_shoppingList);
	std::wstring objUTF16 = nbase::UTF8ToUTF16(response);
	if (response == "")
	{
		LogInfo("Server", "上传出货订单到后台后未响应");
		return "null";
	}
	//auto body = nbase::win32::UTF8ToMBCS(response->body);
	StaticJsonDocument<PACKET_SIZE> doc;
	deserializeJson(doc, response.c_str());
	std::string payUrl = doc["obj"]["payUrl"];
	const std::string strMsg = nbase::win32::UTF8ToMBCS(doc["msg"]);
	LogInfo("server", "请求支付链接时后台响应MSG:%s", strMsg.c_str());

	if (payUrl == "null")
		LogError("server", "未收到支付链接");
	else
		LogInfo("server", "接收到的支付链接%s", payUrl);

	return payUrl;
#else
	auto response = ProductHttpService::GetInstance()->PostOrderToBackground(m_shoppingList);
	if (!response)
	{
		LogInfo("Server", "上传出货订单到后台后未响应");
		return "null";
	}
	std::string content = response->body;
	//auto body = nbase::win32::UTF8ToMBCS(response->body);
	StaticJsonDocument<PACKET_SIZE> doc;
	deserializeJson(doc, content.c_str());
	std::string payUrl = doc["obj"]["payUrl"];
	if (payUrl == "null")
		LogError("server", "未收到支付链接");
	else
		LogInfo("server", "接收到的支付链接%s", payUrl);

	return payUrl;
#endif // CPPHTTPLIB_OPENSSL_SUPPORT

	
}

void ProductForm::SetPaymentStatus(int state)
{
	//通知AI端支付状态
	nbase::ThreadManager::PostTask(kThreadUI, []() {
		LogInfo("Server", "订单支付.");
		LogInfo("Http", "向AI端发送  订单支付消息...");
	});

	//如果支付成功，下发订单到仓管
	if (state == 1)
	{
		OnPaymentSuccess();
	}
	else
	{
		OnPaymentFailed();
	}
}

void ProductForm::SetUploadListFromHttp(const std::vector<ProductInfo> list)
{
	//通知仓管进入上货模式，并把上货列表发到仓管
	
	//RakNet::RakWString subCategory = nbase::win32::MBCSToUnicode(item).c_str();
	
	RakNet::BitStream stream;
	stream.Write(list.size());
	for (unsigned int i = 0; i < list.size(); ++i)
	{
		RakNet::RakString nameString(list[i].name.c_str());//商品名称
		stream.WriteCompressed(nameString);

		stream.Write(list[i].id);//id

		std::string code = list[i].barcode;//条码
		RakNet::RakString codeString(code.c_str());
		stream.WriteCompressed(codeString);

		stream.Write(list[i].count);//数量
	}
	m_pRakServer->Signal(IDEnterStorageMode, &stream, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

	//发消息到AI，使进入闲聊
	ProductHttpService::GetInstance()->PostAIRequest("enterStorageMode");

}

void ProductForm::OnPaymentSuccess()
{
	KillPaymentCountdownTimer();
	
	m_shoppingStage = ShoppingStage::PickProduct;

	unsigned int count = m_shoppingList.size();
	if (count > 0)
	{
		m_shipmentStatus = ShipmentStatus::Processing;

		SetShipmentTimer();

		StdClosure closure = []() {
			LogInfo("Phone", "支付成功！通知仓管出货...");
			LogInfo("Http", "向AI端发送 paymentSuccess 支付成功消息...");
		};
		nbase::ThreadManager::PostTask(kThreadUI, ToWeakCallback(closure));
		ProductHttpService::GetInstance()->PostAIRequest("paymentSuccess");

		RakNet::BitStream bsOut;
		m_pRakServer->Signal(IDPaymentSuccess, &bsOut, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);

		RequestForShipment(m_shoppingList);

		// 商品购买完成, 清除购物清单
		m_shoppingList.clear();
		m_shoppingListBox->RemoveAll();

		m_productSearchResultList.clear();
	}
}

void ProductForm::OnPaymentFailed()
{
	KillPaymentCountdownTimer();

	m_shoppingStage = ShoppingStage::PickProduct;

	unsigned int count = m_shoppingList.size();
	if (count > 0)
	{
		m_shipmentStatus = ShipmentStatus::Waiting;

		StdClosure closure = []() {
			LogInfo("Phone", "支付失败！");
			LogInfo("Http", "向AI端发送 payedfailed 支付支付失败...");
		};
		nbase::ThreadManager::PostTask(kThreadUI, ToWeakCallback(closure));
		ProductHttpService::GetInstance()->PostAIRequest("payedfailed");
		
		// 商品购买失败, 清除购物清单
		/*m_shoppingList.clear();
		m_shoppingListBox->RemoveAll();

		m_productSearchResultList.clear();*/
	}
}

void ProductForm::RefreshShoppingListBox()
{
	// 移除列表中多余的商品项
	for (int i = 0; i < m_shoppingListBox->GetCount(); ++i)
	{
		auto item = dynamic_cast<ProductItem *>(m_shoppingListBox->GetItemAt(i));
		if (item != nullptr)
		{
			auto info = GetProductInfoByIdInShoppingList(item->GetItemID());
			if (info == nullptr)
			{
				m_shoppingListBox->Remove(item);
				break;
			}
		}
	}

	for (auto& info : m_shoppingList)
	{
		bool existingItem = false;
		int count = m_shoppingListBox->GetCount();
		for (int i = 0; i < count; ++i)
		{
			auto item = dynamic_cast<ProductItem *>(m_shoppingListBox->GetItemAt(i));
			// 商品存在
			if (item->GetItemID() == info.id)
			{
				existingItem = true;
				item->SetProductNumber(info.count);

				if (info.count == 0)
					m_shoppingListBox->Remove(item);

				break;
			}
		}

		// 商品不在购物车控件列表里面，创建商品控件子项并添加到列表
		if (!existingItem)
		{
			ProductItem *item = new ProductItem();
			ui::GlobalManager::FillBoxWithCache(item, L"skin/product_item.xml");
			std::wstring img = L"product.png";

			item->InitSubControls(img, nbase::win32::MBCSToUnicode(info.name), info.id, info.count);
			item->SetDataID(nbase::UintToString16(info.id));
			item->SetPriceText(info.price);

			item->AttachDeleteButtonClick([this](ui::EventArgs* param) { return OnItemDeleteButtonClicked(param); });
			item->AttachIncreaseButtonClick([this](ui::EventArgs* param) { return OnItemIncreaseButtonClicked(param); });
			item->AttachReduceButtonClick([this](ui::EventArgs* param) { return OnItemReduceButtonClicked(param); });

			m_shoppingListBox->Add(item);
		}
	}

	// 移除数量为0的商品
	for (auto iter = m_shoppingList.begin(); iter != m_shoppingList.end(); )
	{
		if (iter->count == 0 || iter->name.length() == 0)
			iter = m_shoppingList.erase(iter);
		else
			++iter;
	}

	// 同步库存
	for (auto &item : m_shoppingList)
	{
		auto info = GetProductInfoByBarcodeInDatabse(item.barcode);
		if (info)
		{
			info->amount = item.amount;
		}
	}
}

void ProductForm::UninitNetwork()
{
	auto clientInfoList = GetClientInfoList();
	for (auto item : clientInfoList)
	{
		if (item.address != RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			m_pRakServer->CloseConnection(item.address, true);
	}

	delete m_pRakServer;
	m_pRakServer = nullptr;
}


void ProductForm::LoadProductFileAndAddToList(const std::string& filePath)
{
	std::ifstream fileStream(filePath);
	std::string line;
	while (std::getline(fileStream, line))
	{
		std::stringstream  lineStream(line);
		std::string  cell;

		std::vector<std::string> cellArray;
		while (std::getline(lineStream, cell, (',')))
		{
			if (!cell.empty())
				cellArray.push_back(cell);
			
		}

		if (cellArray.size() != ProductInfoSize)
			break;

		ProductInfo info;
		info.id = static_cast<unsigned int>(atoi(cellArray[0].c_str()));
		info.name = cellArray[1];
		info.image = cellArray[2];
		info.description = cellArray[3];
		info.mainCategory = cellArray[4];
		info.price = static_cast<float>(atof(cellArray[5].c_str()));
		info.amount = atoi(cellArray[6].c_str());
		info.barcode = cellArray[7];

		m_productDatabase.push_back(info);
	}
}

void ProductForm::LoadProductDatabaseAndAddToList()
{
	if (!ProductDatabaseManager::GetInstance()->Init())
	{
		LogError("Database", "数据库打开失败，请检查数据库路径和配置是否正确.");
		return;
	}

	ProductDatabaseManager::GetInstance()->QueryAllProducts(m_productDatabase);
}

LRESULT ProductForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UninitNetwork();

	PostQuitMessage(0L);

	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}


void ProductForm::OnFinalMessage(HWND hWnd)
{
	TrayIcon::GetInstance()->Destroy();
}

void ProductForm::OnVerifyConnection(const RakNet::RakString &clientName, bool passed)
{
	RakNet::RakString clientAddress = GetClientAddress(clientName.C_String()).ToString(false);

	std::wstring cachedName = nbase::win32::MBCSToUnicode(clientName.C_String());
	for (auto& item : m_netStatusMap)
	{
		if (item.first.compare(cachedName) != 0)
			continue;

		if (passed)
		{
			item.second->SetEnabled(true);
		}
		else
		{
			item.second->SetEnabled(false);
		}

		break;
	}

	if (passed)
	{
		LogInfo("Server", "验证通过,客户端 %s [%s] 已连接.", clientName.C_String(), clientAddress.C_String());
	}
	else
	{
		LogError("Server", "验证失败,客户端名称或密码错误, %s [%s] 连接失败.", clientName.C_String(), clientAddress.C_String());
	}

	// 如果程序启动时打开远程商品数据库失败(远程电脑没先启动 或者 ip配置错误)，每次有新连接时尝试重新打开
	if (!ProductDatabaseManager::GetInstance()->IsDatabaseOpened())
		LoadProductDatabaseAndAddToList();
}

void ProductForm::OnClosedConnection(const RakNet::SystemAddress &systemAddress, RakNet::RakNetGUID rakNetGUID, RakNet::PI2_LostConnectionReason lostConnectionReason)
{
	RakNet::RakString clientName = GetClientName(systemAddress);
	RakNet::RakString clientAddress = systemAddress.ToString(false);

	RakServerPlugin::OnClosedConnection(systemAddress, rakNetGUID, lostConnectionReason);

	std::wstring cachedName = nbase::win32::MBCSToUnicode(clientName.C_String());
	for (auto& item : m_netStatusMap)
	{
		if (item.first.compare(cachedName) != 0)
			continue;

		item.second->SetEnabled(false);

		break;
	}

	char *reasons[] =
	{
		"closed by user",
		"disconnection notification",
		"connection lost"
	};

	LogInfo("Server", "客户端 %s [%s] 已经断开连接: %s", clientName.C_String(), clientAddress.C_String(), reasons[lostConnectionReason]);
}

void ProductForm::OnNetCommandRequest(RakNet::BitStream *bitStream)
{

}

void ProductForm::OnNetCommandResponse(RakNet::BitStream *bitStream)
{

}

void ProductForm::LeftClick()
{
	this->ActiveWindow();

	if (m_bTrayIconLeftDoubleClicked)
	{
		m_bTrayIconLeftDoubleClicked = false;
		return;
	}

	::SetForegroundWindow(m_hWnd);
	::BringWindowToTop(m_hWnd);
}

void ProductForm::LeftDoubleClick()
{
	m_bTrayIconLeftDoubleClicked = true;
}

void ProductForm::RightClick()
{
	POINT pt;
	::GetCursorPos(&pt);
	PopupTrayMenu(pt);
}

void ProductForm::GetProductListByCategoryInDatabse(const std::string &category, std::vector<ProductInfo> &productList)
{
	productList.clear();
	for (auto item : m_productDatabase)
	{
		if (item.mainCategory.find(category) != std::string::npos)
		{
			productList.push_back(item);
		}
	}
}

void ProductForm::QueryProductInDatabase(const std::string &description, std::vector<ProductInfo> &productList)
{
	productList.clear();
	for (auto item : m_productDatabase)
	{
		if (item.description.find(description) != std::string::npos)
		{
			productList.push_back(item);
		}
	}
}

void ProductForm::SignalMessage(const char *sharedIdentifier, RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability)
{
	m_pRakServer->Signal(sharedIdentifier, bitStream, priority, reliability, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}

void ProductForm::RequestForShipment(const PRODUCT_ARRAY& list)
{
	RakNet::BitStream stream;

#if defined(Demo) // 演示版本: 指定只取货农夫山泉
	stream.Write(1);
	stream.Write(1112);

	RakNet::RakString codeString("6921168511280");
	stream.WriteCompressed(codeString);
	stream.Write(1);
#else
	stream.Write(list.size());
	for (unsigned int i = 0; i < list.size(); ++i)
	{
		stream.Write(list[i].id);

		std::string code = list[i].barcode;
		RakNet::RakString codeString(code.c_str());
		stream.WriteCompressed(codeString);

		stream.Write(list[i].count);
	}
#endif

	m_pRakServer->Signal(IDRequestForShipment, &stream, PacketPriority::HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, false);
}

ProductForm::InquiryCategory ProductForm::CheckCategory(const std::string &inquiry)
{
	InquiryCategory result = InquiryCategory::Unknown;
	if (inquiry.compare("所有") == 0)
	{
		result = InquiryCategory::All;
	}
	else
	{
		for (auto item : m_productDatabase)
		{
			if (item.mainCategory.find(inquiry) != std::string::npos ||
				inquiry.find(item.mainCategory) != std::string::npos)
			{
				result = InquiryCategory::MainCategory;
				break;
			}
			else if (item.subCategory.find(inquiry) != std::string::npos ||
				inquiry.find(item.subCategory) != std::string::npos)
			{
				result = InquiryCategory::SubCategory;
				break;
			}
		}
	}

	return result;
}

ProductForm::InquiryCategory ProductForm::CheckKeywordCategory(const std::string &keyword)
{
	if (keyword.find("_0") != std::string::npos)
		return InquiryCategory::All;
	else if (keyword.find("_4") != std::string::npos)
		return InquiryCategory::MainCategory;
	else if (keyword.find("_5") != std::string::npos)
		return InquiryCategory::SubCategory;
	else
		return InquiryCategory::Unknown;

}

void ProductForm::GetMainCategoryList(std::set<std::string>& categoryList)
{
	categoryList.clear();

	for (auto item : m_productDatabase)
	{
		categoryList.insert(item.mainCategory);
	}
}

void ProductForm::GetSubCategoryList(const std::string& category, std::set<std::string>& subCategoryList)
{
	for (auto item : m_productDatabase)
	{
		if (category.compare("所有") == 0)
		{
			subCategoryList.insert(item.mainCategory);
		}
		else
		{
			if (item.mainCategory.find(category) != std::string::npos ||
				category.find(item.mainCategory) != std::string::npos)
			{
				subCategoryList.insert(item.subCategory);
			}
		}
	}

}