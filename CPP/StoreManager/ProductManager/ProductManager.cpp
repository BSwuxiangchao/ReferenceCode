// ProductManager.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "ProductForm.h"
#include "PaymentThread.h"
#include "MiscThread.h"
#include "ProductManager.h"
#include "ProductHttpService.h"
#include "LogForm.h"
#include "ProductDatabaseManager.h"

INITIALIZE_EASYLOGGINGPP

const char* EasyLoggingConfigFile = "./logging.conf";
Clog theLog(EasyLoggingConfigFile);

void AddLogToListView(const char* strMsg, ...)
{
	char strBuffer[1024];

	va_list args;
	va_start(args, strMsg);
	_vsnprintf_s(strBuffer, 1024, 1023, strMsg, args);
	va_end(args);

	std::wstring log;
	nbase::win32::MBCSToUnicode(strBuffer, log);
	LogForm::GetInstance()->AddLogToListBox(log);
	
}



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	MainThread thread;
	thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop);

    return 0;
}


void MainThread::Init()
{
	std::wstring themeDir = nbase::win32::GetCurrentModuleDirectory();
	ui::GlobalManager::Startup(themeDir + L"resources\\", ui::CreateControlCallback(), false, L"themes");

	nbase::ThreadManager::RegisterThread(kThreadUI);

	// 确保程序只运行一个单例
	if (!nbase::win32::SingletonRun(L"SelfServiceStoreManager"))
	{
		ui::MsgboxCallback cb = [this](ui::MsgBoxRet ret) {
			if (ret == ui::MB_YES)
			{
				exit(1);
			}
		};

		ui::ShowMsgBox(nullptr, cb, ui::MT_WARNING, L"程序已经在运行", false, L"警告", false, L"确定", false, L"", false);

		return;
	}

	BeforeMessageLoop();

	LogForm::GetInstance()->Create(/*this->GetHWND()*/nullptr, L"查看日志", WS_OVERLAPPEDWINDOW, 0);
	LogForm::GetInstance()->CenterWindow();
	LogForm::GetInstance()->ShowWindow(false, false);

	InitHttpService();

	ProductForm::GetInstance()->Create(NULL, ProductForm::kClassName.c_str(), WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
	ProductForm::GetInstance()->CenterWindow();
	ProductForm::GetInstance()->ShowWindow();
	
}

void MainThread::Cleanup()
{
	ui::GlobalManager::Shutdown();

	AfterMesssageLoop();
	SetThreadWasQuitProperly(true);
	nbase::ThreadManager::UnregisterThread();

}

void MainThread::BeforeMessageLoop()
{
	m_pPaymentThread = std::make_unique<PaymentThread>(kThreadPayment, "Payment thread");
	m_pPaymentThread->Start();

	m_pMiscThread = std::make_unique<MiscThread>(KThreadMisc, "Misc thread");
	m_pMiscThread->Start();
}

void MainThread::AfterMesssageLoop()
{
	m_pPaymentThread->Stop();
	m_pPaymentThread.reset(nullptr);

	m_pMiscThread->Stop();
	m_pMiscThread.reset(nullptr);

}

//将wstring转换成string  
string wstring2string(wstring wstr)
{
	string result;
	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	//宽字节编码转换成多字节编码  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}

void MainThread::InitHttpService()
{
	CSimpleIni configController;
	auto filePath = nbase::win32::GetCurrentModuleDirectory() + IniConfigFile;
	auto error = configController.LoadFile(filePath.c_str());
	if (error != SI_OK)
	{
		LogError("Http", "初始化Http服务失败, 加载配置文件 %s 错误: %d", filePath.c_str(), error);
		return;
	}

	auto phoneHost = configController.GetValue(_T("Http"), _T("RemoteHost"), _T(""));
	auto phonePort = configController.GetLongValue(_T("Http"), _T("RemotePort"));
	auto urlPhonePath = configController.GetValue(_T("Http"), _T("UrlPayPath"), _T(""));
	auto urlAIPath = configController.GetValue(_T("Http"), _T("UrlAIPath"), _T(""));

	auto localHost = configController.GetValue(_T("Http"), _T("LocalHost"));
	auto localPort = configController.GetLongValue(_T("Http"), _T("LocalPort"));
	
	auto aiHost = configController.GetValue(_T("Http"), _T("AIHost"));
	auto aiPort = configController.GetLongValue(_T("Http"), _T("AIPort"));

	auto databasePath = configController.GetValue(_T("Database"), _T("Path"));

	auto storeNo = configController.GetValue(_T("Store"), _T("storeNo"));

	ProductHttpService::GetInstance()->InitCommonService(nbase::win32::UnicodeToMBCS(localHost), localPort);
	ProductHttpService::GetInstance()->InitPhoneService(nbase::win32::UnicodeToMBCS(phoneHost), phonePort, nbase::win32::UnicodeToMBCS(urlPhonePath));
	ProductHttpService::GetInstance()->InitAIService(nbase::win32::UnicodeToMBCS(aiHost), aiPort, nbase::win32::UnicodeToMBCS(urlAIPath));

	ProductHttpService::GetInstance()->HandlePhoneMessage(nbase::win32::UnicodeToMBCS(urlPhonePath));
	
	ProductHttpService::GetInstance()->HandleAICommunication(nbase::win32::UnicodeToMBCS(urlAIPath));

	ProductHttpService::GetInstance()->HandleBackgroundMessage(nbase::win32::UnicodeToMBCS(urlPhonePath));

	ProductHttpService::GetInstance()->SetStoreNo(nbase::win32::UnicodeToMBCS(storeNo));

	ProductDatabaseManager::GetInstance()->SetDatabasePath(nbase::win32::UnicodeToMBCS(databasePath));

	//测试
	/*using PRODUCT_ARRAY = std::vector<ProductInfo>;
	PRODUCT_ARRAY list;
	ProductInfo p1,p2;
	p1.id = 1101;
	p1.barcode = "fheiofw";
	p1.amount = 2;
	list.push_back(p1);
	p2.id = 1102;
	p2.barcode = "fheiofw22";
	p2.amount = 1;
	list.push_back(p2);
	auto response = ProductHttpService::GetInstance()->PostOrderToBackground(list);
	std::string content = response->body;
	int b;*/
}
