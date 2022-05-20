
// PlcFaceDlg.cpp : implementation file
//
#pragma once
#include "stdafx.h"
#include "Wingdi.h"
#include "PlcFace.h"
#include "PlcFaceDlg.h"
#include "afxdialogex.h"
#include"NewGooddAddDlg.h"
#include "PlcDB.h"
#include"sLog.h"
#include"WarningDlg.h"
#include"BoxShelfDlg.h"
#include <nlohmann\json.hpp>
#include<boost/algorithm/string/join.hpp>
#include"ToolHelp.h"
using jsonF = nlohmann::json;
#include "../Utility/ArduinoJson.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//#define DEMO
#ifndef _HTTPLIB_
//#define _HTTPLIB_
#endif //_HTTPLIB_
bool g_isRun = true;

// CAboutDlg dialog used for App About


std::string dump_headers(const httplib::Headers& headers) {
	std::string s;
	char buf[BUFSIZ];

	for (auto it = headers.begin(); it != headers.end(); ++it) {
		const auto& x = *it;
		snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
		s += buf;
	}

	return s;
}

std::string log(const httplib::Request& req, const httplib::Response& res) {
	std::string s;
	char buf[BUFSIZ];

	s += "================================\n";

	snprintf(buf, sizeof(buf), "%s %s %s", req.method.c_str(),
		req.version.c_str(), req.path.c_str());
	s += buf;

	std::string query;
	for (auto it = req.params.begin(); it != req.params.end(); ++it) {
		const auto& x = *it;
		snprintf(buf, sizeof(buf), "%c%s=%s",
			(it == req.params.begin()) ? '?' : '&', x.first.c_str(),
			x.second.c_str());
		query += buf;
	}
	snprintf(buf, sizeof(buf), "%s\n", query.c_str());
	s += buf;
	s += dump_headers(req.headers);
	if (!req.body.empty()) { s += req.body; }
	s += "\n";
	s += "--------------------------------\n";
	snprintf(buf, sizeof(buf), "%d %s\n", res.status, res.version.c_str());
	s += buf;
	s += dump_headers(res.headers);
	s += "\n";
	if (!res.body.empty()) { s += res.body; }
	s += "\n";
	return s;
}
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPlcFaceDlg dialog



CPlcFaceDlg::CPlcFaceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PLCFACE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//CFileLogger::CreateInstance(_T("PlcFace"));
	m_dlgRect.SetRect(0, 0, 0, 0);
	m_LastBoxID = "";
}
CPlcFaceDlg::~CPlcFaceDlg()
{
	KillTimer(1);
	KillTimer(2);
	g_isRun = false;
	UShop::PLCDB::Instance().Uninitialize();
	if (m_cameraDlg != nullptr)
	{
		delete m_cameraDlg;
	}
	if (m_robotDlg != nullptr)
	{
		delete m_robotDlg;
	}
	if (m_goodStockDlg != nullptr)
	{
		delete m_goodStockDlg;
		m_goodStockDlg = nullptr;
	}
	if (m_AddGoodsDlg != nullptr)
	{
		delete m_AddGoodsDlg;
		m_AddGoodsDlg = nullptr;
	}
	if (m_lookForSpecGoods != nullptr)
	{
		delete m_lookForSpecGoods;
		m_lookForSpecGoods = nullptr;
	}
	if (m_serveTh)
	{
		Join(m_serveTh);
	}
	if (m_server != nullptr)
	{
		delete m_server;
		m_server = nullptr;
	}
	if (m_cli != nullptr)
	{
		delete m_cli;
		m_cli = nullptr;
	}
	m_svr.stop();
}
void CPlcFaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ADDGOODS_BUTTON, m_addGoodsBtn);
	DDX_Control(pDX, IDC_TRANSFORIN_BUTTON, m_BoxInBtn);
	DDX_Control(pDX, IDC_NEWGOODS_BUTTON, m_addProductBtn);
	DDX_Control(pDX, IDC_GOODSDECREASE_BUTTON, m_decreaseProductBtn);
	DDX_Control(pDX, IDC_DECREASEGOODS_BUTTON, m_outGoodsBtn);
	DDX_Control(pDX, IDC_LOOKFORALL_BUTTON, m_LookForAllBtn);
	DDX_Control(pDX, IDC_LOOKFORSPEC_BUTTON, m_LookForSpecBtn);
	DDX_Control(pDX, IDC_STARTPLC_BUTTON, m_StartBtn);
	DDX_Control(pDX, IDC_STOPPLC_BUTTON, m_StopBtn);
	DDX_Control(pDX, IDC_RSSET_BUTTON, m_ResetBtn);
	DDX_Control(pDX, IDC_CAMERA_BUTTON, m_CameraBtn);
	DDX_Control(pDX, IDC_ROBOT_BUTTON, m_JXbBtn);
	DDX_Control(pDX, IDC_WARNINGRESET_BUTTON, m_WarnResetBtn);
	DDX_Control(pDX, IDC_PLCFACE_LOG_LIST, m_LogList);
	DDX_Control(pDX, IDC_PLCWARNING_LIST, m_WarnList);
	DDX_Control(pDX, IDC_CANCELPLCWARNING_LIST, m_cancelWarnList);
	DDX_Control(pDX, IDC_ADDBOXOUT_BUTTON, m_AddBoxOutBtn);
	DDX_Control(pDX, IDC_BOXOUT_BUTTON,    m_OutBoxOutBtn);
	DDX_Control(pDX, IDC_BOXBACK_BUTTON,   m_OutBoxInBtn);
	DDX_Control(pDX, IDC_UPDATETOFACE_BUTTON, m_UpdateToFaceBtn);
}

void CPlcFaceDlg::repaint(UINT id, int last_width, int now_width, int last_height, int now_height)
{
	CRect rect;
	CWnd *wnd = nullptr;
	wnd = GetDlgItem(id);
	if (wnd == nullptr)
	{
		//MessageBox(_T("相应控件不存在"));
		return;
	}
	wnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	double prop_x = now_width / (double)last_width;
	double prop_y = now_height / (double)last_height;
	rect.left = (long)(rect.left*prop_x + 0.5);
	rect.right= (long)(rect.right*prop_x + 0.5);
	rect.top= (long)(rect.top*prop_y + 0.5);
	rect.bottom= (long)(rect.bottom*prop_y + 0.5);
	wnd->MoveWindow(&rect);
}


BEGIN_MESSAGE_MAP(CPlcFaceDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ADDGOODS_BUTTON, &CPlcFaceDlg::OnBnClickedAddgoodsButton)
	ON_BN_CLICKED(IDC_DECREASEGOODS_BUTTON, &CPlcFaceDlg::OnBnClickedDecreasegoodsButton)
	ON_BN_CLICKED(IDC_LOOKFORALL_BUTTON, &CPlcFaceDlg::OnBnClickedLookforallButton)
	ON_BN_CLICKED(IDC_LOOKFORSPEC_BUTTON, &CPlcFaceDlg::OnBnClickedLookforspecButton)
	ON_BN_CLICKED(IDC_STARTPLC_BUTTON, &CPlcFaceDlg::OnBnClickedStartplcButton)
	ON_BN_CLICKED(IDC_STOPPLC_BUTTON, &CPlcFaceDlg::OnBnClickedStopplcButton)
	ON_BN_CLICKED(IDC_RSSET_BUTTON, &CPlcFaceDlg::OnBnClickedRssetButton)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_TRANSFORIN_BUTTON, &CPlcFaceDlg::OnBnClickedTransforinButton)
	ON_BN_CLICKED(IDC_NEWGOODS_BUTTON, &CPlcFaceDlg::OnBnClickedNewgoodsButton)
	ON_BN_CLICKED(IDC_GOODSDECREASE_BUTTON, &CPlcFaceDlg::OnBnClickedGoodsdecreaseButton)
	ON_BN_CLICKED(IDC_CAMERA_BUTTON, &CPlcFaceDlg::OnBnClickedCameraButton)
	ON_BN_CLICKED(IDC_ROBOT_BUTTON, &CPlcFaceDlg::OnBnClickedRobotButton)
	ON_WM_DESTROY(/*IDD_PLCFACE_DIALOG, &CPlcFaceDlg::OnDestroy*/)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_WARNINGRESET_BUTTON, &CPlcFaceDlg::OnBnClickedWarningresetButton)
	ON_BN_CLICKED(IDC_BOXBACK_BUTTON, &CPlcFaceDlg::OnBnClickedBoxbackButton)
	ON_BN_CLICKED(IDC_ADDBOXOUT_BUTTON, &CPlcFaceDlg::OnBnClickedAddboxoutButton)
	ON_BN_CLICKED(IDC_BOXOUT_BUTTON, &CPlcFaceDlg::OnBnClickedBoxoutButton)
	ON_BN_CLICKED(IDC_UPDATETOFACE_BUTTON, &CPlcFaceDlg::OnBnClickedUpdatetofaceButton)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_32771, &CPlcFaceDlg::OnSyncData)
	ON_COMMAND(ID_32772, &CPlcFaceDlg::OnStartPlc)
	ON_COMMAND(ID_32773, &CPlcFaceDlg::OnStopPlc)
	ON_COMMAND(ID_32774, &CPlcFaceDlg::OnResetPlc)
	ON_COMMAND(ID_32775, &CPlcFaceDlg::OnWarnReSet)
END_MESSAGE_MAP()


// CPlcFaceDlg message handlers

BOOL CPlcFaceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	Init();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPlcFaceDlg::UpdateStockToServer(int mode)
{
	//发送各商品库存信息到总控
	std::vector<StroreInfo> storeList;
	SZL_INFO("商品库存查询开始");
	NotifyMsg("商品库存查询开始");
	if (mode == 0)
	{
		UShop::PLCDB::Instance().QueryAllStoreInfo(storeList);
	}
	else if (mode == 1)
	{
		for (auto outPutProduct : m_shipmentInfo.outputProductInfoList)
		{
			StroreInfo store;
			BoxForProductPLC BoxForProduc;
			UShop::PLCDB::Instance().QueryStoreInfoOfProduct(outPutProduct.productID, BoxForProduc);
			store.barcode = BoxForProduc.productInfo.barcode;
			store.name = BoxForProduc.productInfo.name;
			store.amount = BoxForProduc.iTotalStock;
			storeList.push_back(store);
		}
	}
	else
	{
		for (auto onloadProduct : m_OnloadProductList)
		{
			StroreInfo store;
			BoxForProductPLC BoxForProduc;
			UShop::PLCDB::Instance().QueryStoreInfoOfProduct(onloadProduct.productID, BoxForProduc);
			store.barcode = BoxForProduc.productInfo.barcode;
			store.name = BoxForProduc.productInfo.name;
			store.amount = BoxForProduc.iTotalStock;
			storeList.push_back(store);
		}
	}
	SZL_INFO("商品库存查询结束");
	NotifyMsg("商品库存查询结束");
	CPlcFaceApp *app = (CPlcFaceApp *)m_parent;
	app->SendAmountToServey(storeList);
}

bool CPlcFaceDlg::SetOnloadInfo(std::vector<OnloadProductInfo> productList)
{
	m_productCellInfoList.clear();
	m_OnloadProductList.clear();
	m_OnloadProductList = productList;
	std::vector<CompleteBox>       productCellInfoList;   //需要推出的货箱列表
	std::vector<std::string>_containerIDlist;
	UShop::PLCDB::Instance().GetEnableContainerList(productList, productCellInfoList, _containerIDlist);
	if (productCellInfoList.size() <= 0)
	{
		SZL_DEBUG("查詢到可以上货的货箱列表为空,上货失败");
		NotifyMsg("查詢到可以上货的货箱列表为空，上货失败");
		return false;
	}
	SZL_DEBUG("查詢到可以上货的货箱列表size为 {0}", productCellInfoList.size());
	NotifyMsg("查詢到可以上货的货箱列表size为 %d", (int)productCellInfoList.size());
	for (auto itP : productCellInfoList)
	{
		SZL_DEBUG("上货的货箱号为 {0}", itP.container.containerID);
		for (auto itC : itP.vecCellInfo)
		{
			SZL_DEBUG("上货的格子号为 {0}", itC.cellNum);
		}
	}
	m_productCellInfoList = productCellInfoList;
	auto itB = productCellInfoList.begin();
	std::string cId = itB->container.containerID;
	std::string shelfID = itB->container.shelfID;
	/*ContainerInfoPLC cPlc;
	UShop::PLCDB::Instance().QueryContainerInfoByID(itB->container.containerID, cPlc);*/
	if (shelfID.empty())
	{
		SZL_ERROR("未找到对应货架号");
		NotifyMsg("未找到对应货架号");
		return false;
	}
	getServer()->AddGoodsBoxOut(std::stoi(shelfID));
	//设置当前推出的货架号
	setCurBoxId(std::stoi(shelfID));
	//更新数据库
	UpdateInGoodsInfo(cId, productCellInfoList);
	ShowLoadInfoForAPP(*itB);
	productCellInfoList.erase(itB);
	getServer()->SetCellInfoList(productCellInfoList);

	//test
	//UpdateInGoodsInfoToBg();
	return true;
}

void CPlcFaceDlg::Routine()
{
	SZL_INFO("开启Tcp服务线程####");
	m_server=new TcpServer;
	m_server->IntiSocket();
	std::string configFile = m_CommonPath + R"(config\PLCconfig.ini)";
	m_server->Open(configFile.c_str(),"plcFace");
	m_server->Bind();
	m_server->Listen(5);
	m_server->SetPlcDlg(this);
	m_server->SetIsStartUp(true);
	while (1)
	{
		MyThread::Lock();
		if (!g_isRun)
		{
			MyThread::UnLock();
			break;
		}
		if (m_server != nullptr)
		{
			m_server->OnRun();
		}
		MyThread::UnLock();
		MyThread::SleepMillSec(100);
	}
}

void TranslateCompleteBoxToCellInfo(const std::vector<CompleteBox> ContainerList, std::vector<CellInfoPLC> &list)
{
	for (auto box : ContainerList)
	{
		std::string type = box.container.containerType;
		for (auto cell : box.vecCellInfo)
		{
			cell.containerType = type;
			list.push_back(cell);
		}
	}
}

void CPlcFaceDlg::UpdateOutGoodsInfo()
{
	if (m_OutGoodsDlg != nullptr)
		m_OutGoodsDlg->updateInfoForOutputProduct();
	else
	{
		std::vector<CompleteBox> ContainerList;
		UShop::PLCDB::Instance().QueryPositionInfoByShipmentInfo(m_shipmentInfo, ContainerList);
		UShop::PLCDB::Instance().UpdateInfoForOutputProduct(m_shipmentInfo, ContainerList);//使用正式出货前查到的地址信息
		//上传库存到后台
		std::vector<CellInfoPLC> list;
		TranslateCompleteBoxToCellInfo(ContainerList, list);
		std::string postData = SetmodifyReserveNumtoHttp(list,1);
		UpdateStockToInfoCenter(postData);
	}
	CPlcFaceApp *app = (CPlcFaceApp *)m_parent;
	app->SendShipmentResult(true, m_shipmentInfo.outputProductInfoList, "出货成功");
}
void CPlcFaceDlg::UpdateInGoodsInfo(std::string containerID, std::vector<CompleteBox> productCellInfoList)
{
	UShop::PLCDB::Instance().UpdateOrInsertPosInfo(containerID, productCellInfoList);
	
}

void CPlcFaceDlg::UpdateInGoodsInfoToBg(int modeFlag)
{
	//上传库存到后台
	//m_productCellInfoList.clear();

	std::vector<CellInfoPLC> list;
	TranslateCompleteBoxToCellInfo(m_productCellInfoList, list);
	std::string postData = SetmodifyReserveNumtoHttp(list, modeFlag);
	UpdateStockToInfoCenter(postData);
}

BOOL CPlcFaceDlg::Checkout(ShipmentInfo& orders, std::string& msg)
{
	msg = "";
	/*ShipmentInfo curShipment;
	curShipment.orderID = orders.orderID;
	//根据产品barcode转换产品ID（两张表的barcode一样，ID不一样），
	for (size_t n = 0; n < orders.outputProductInfoList.size(); n++)
	{
		ProductInfoPLC goods;
		if (UShop::PLCDB::Instance().QueryProductInfoByBarcode(orders.outputProductInfoList[n].barcode.c_str(), goods))
		{
			OutputProductInfo gs;
			gs.outputQuantity = orders.outputProductInfoList[n].outputQuantity;
			gs.productID = goods.id;
			gs.barcode = goods.barcode;
			curShipment.outputProductInfoList.push_back(gs);
			SZL_INFO("转换后的商品id:{0}，商品条码:{1},商品数量{2}", gs.productID, gs.barcode, gs.outputQuantity);
		}
		else
		{
			SZL_WARN("产品条码:{0},对应的商品在数据库中未找到", orders.outputProductInfoList[n].barcode);
		}
	}
	if (orders.outputProductInfoList.size() != curShipment.outputProductInfoList.size())
	{
		msg = "商品未上架";
		return false;
	}*/
	

	/***********************TODO*******************************/
#if defined(DEMO)
	;
#else 
	bool _isCheckingOut = true;
	for (auto product : orders.outputProductInfoList)
	{
		BoxForProductPLC BoxForProduct1;
		UShop::PLCDB::Instance().QueryStoreInfoOfProduct(product.productID, BoxForProduct1);
		BoxForProductPLC BoxForProduct;
		UShop::PLCDB::Instance().QueryStoreInfoOfProduct(product.productID, BoxForProduct);//语音下订单时根据商品ID查询商品具体信息失败，原因未明
		SZL_ERROR("出货时商品 {0} 库存为 {1},应出货数量 {}", product.productID, BoxForProduct.iTotalStock, product.outputQuantity);
		NotifyMsg("出货时商品 %d 库存为 %d,应出货数量 %d", product.productID, BoxForProduct.iTotalStock, product.outputQuantity);
		if (BoxForProduct.iTotalStock < product.outputQuantity)
		{
			_isCheckingOut = false;
			break;
		}
	}
	if (!_isCheckingOut)
	{
		msg = "库存不足";
		SZL_ERROR("出货时库存不足");
		NotifyMsg("出货时库存不足");
		return false;
	}
#endif
	m_shipmentInfo.Clear();
	m_shipmentInfo = orders;
	OutGoods();
	return true;
}

void CPlcFaceDlg::Container2ShelfID(std::string& ShelfID, std::string ContainerID)
{
	ContainerInfoPLC cPlc;
	UShop::PLCDB::Instance().QueryContainerInfoByID(ContainerID, cPlc);
	ShelfID = cPlc.shelfID;
}

bool CPlcFaceDlg::OutGoods()
{
	std::vector<CompleteBox> positionInfoList; //出货位置
#if defined(DEMO)
	CompleteBox temp1;
	temp1.container.containerType = 1;
	temp1.container.containerID = "10";
	CellInfoPLC  cell1,cell2;
	cell1.cellNum = 1;
	cell1.barcode = "6901285991240";
	cell1.outputGoodsNum = 1;
	temp1.vecCellInfo.push_back(cell1);

	auto it = m_shipmentInfo.outputProductInfoList.begin();
	if (it != m_shipmentInfo.outputProductInfoList.end())
	{
		if (it->outputQuantity > 1)
		{
			SZL_INFO("待取商品数量大于1，暂取1，2 号格子");
			cell2.cellNum = 2;
			cell2.barcode = "6901285991240";
			cell2.outputGoodsNum = 1;
			temp1.vecCellInfo.push_back(cell2);
		}
	}
	positionInfoList.push_back(temp1);
#else
	UShop::PLCDB::Instance().QueryPositionInfoByShipmentInfo(m_shipmentInfo, positionInfoList);
#endif
	SZL_INFO("出货货箱数量:{0}", positionInfoList.size());
	for (auto &it : positionInfoList)
	{
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "货箱：%s，要取的格子数量%d",it.container.containerID.c_str(), (int)it.vecCellInfo.size());
		NotifyMsg(tBuf);
		SZL_INFO("货箱：{0}，要取的格子数量{1}", it.container.containerID, it.vecCellInfo.size());
		for (auto &itcell : it.vecCellInfo)
		{
			memset(tBuf, 0X00, sizeof(tBuf));
			sprintf_s(tBuf, "格子号%d:,对应的出货数量：%d",itcell.cellNum, itcell.outputGoodsNum);
			NotifyMsg(tBuf);
			SZL_INFO("格子号:{0},对应的出货数量：{1}", itcell.cellNum, itcell.outputGoodsNum);
		}
	}
	if (positionInfoList.size() <= 0)
	{
		SZL_ERROR("订单商品出货数量大于商品总库存");
		NotifyMsg("订单商品出货数量大于商品总库存");
		return false;
	}
	auto itB = positionInfoList.begin();

	std::string shelfID;
	Container2ShelfID(shelfID,itB->container.containerID);
	if (shelfID.empty())
	{
		SZL_INFO("********数据库货架号未配置**********");
		return false;
	}
	getServer()->goodsBoxOut(std::stoi(shelfID));
	SetBoxInNum(std::stoi(shelfID));
	getServer()->SetOutBoxInfo(positionInfoList);
	m_server->SetOutGoodsState(false);
	return true;
}

void CPlcFaceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPlcFaceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		/*CPaintDC dc(this);
		CRect rect;
		GetClientRect(&rect);
		dc.FillSolidRect(rect, 250|(250<<8)|(250<<16));
		dc.SetTextColor(250|250|250);*/
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPlcFaceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPlcFaceDlg::OnDestroy()
{
	std::thread t1([&]() {
		ROBOT_MODULE::RobotAgent::GetInstance()->cancelAllSlot();
		ROBOT_MODULE::RobotAgent::GetInstance()->deinit();
	});
	t1.detach();//异步释放，节约等待时间
	UShop::GoodsLocationAdapter::getInstance()->stop();
	UShop::GoodsLocationAdapter::getInstance()->Deinit();
}

void CPlcFaceDlg::OnBnClickedAddgoodsButton()
{
	// TODO: Add your control notification handler code here
	//测试
	/*CPlcFaceApp *app = (CPlcFaceApp *)m_parent;
	app->UploadRegularCustomerInfo("123", 0, 23, 1);
	return;*/
	//1.上货先输入货物名称
	if (m_AddGoodsDlg == nullptr)
	{

		m_AddGoodsDlg = new AddGoodsDialog(this);
		if (m_AddGoodsDlg == nullptr)
		{
			SZL_ERROR("初始化上货对话框失败");
			MessageBox(_T("初始化上货对话框失败"));
			return;
		}
		m_AddGoodsDlg->Create(IDD_ADDGOODS_DIALOG, this);
	}
	m_AddGoodsDlg->ClearVector();
	m_AddGoodsDlg->AddComVarags();
	//显示非模态对话框
	m_AddGoodsDlg->ShowWindow(SW_SHOW);
}


void CPlcFaceDlg::OnBnClickedDecreasegoodsButton()
{
	// TODO: Add your control notification handler code here
	//OutGoodsDialog dlg;
	if (m_OutGoodsDlg == nullptr)
	{
		m_OutGoodsDlg = new OutGoodsDialog(this);
		if (m_OutGoodsDlg == nullptr)
		{
			SZL_ERROR("初始化出货对话框失败");
			MessageBox(_T("初始化出货对话框失败"));
			return;
		}
		m_OutGoodsDlg->Create(IDD_CHECKOUT_DIALOG, this);
	}
	m_OutGoodsDlg->AddComVarags();

	//显示非模态对话框
	m_OutGoodsDlg->ShowWindow(SW_SHOW);
}


void CPlcFaceDlg::OnBnClickedLookforallButton()
{
	// TODO: Add your control notification handler code here
	//1.查看所有商品库存信息
	if (m_goodStockDlg == nullptr)
	{
		m_goodStockDlg = new AllGoodsStockDialog(this);
		m_goodStockDlg->Create(IDD_AllGOODSTOCK_DIALOG, this);
	}
	//显示非模态对话框
	m_goodStockDlg->AddComVarags();
	m_goodStockDlg->ShowWindow(SW_SHOW);
}
void CPlcFaceDlg::OnBnClickedLookforspecButton()
{
	// TODO: Add your control notification handler code here
	if (m_lookForSpecGoods == nullptr)
	{
		m_lookForSpecGoods = new LookForSpecGoodDlg(this);
		m_lookForSpecGoods->Create(IDD_LOOKFORSPEC_DIALOG, this);
	}
	m_lookForSpecGoods->AddComVarags();
	m_lookForSpecGoods->ShowWindow(SW_SHOW);
}


void CPlcFaceDlg::OnBnClickedStartplcButton()
{
	// TODO: Add your control notification handler code here
	m_server->startPlc();
	m_server->SetArmMachineOptState(true);
	NotifyMsg("已设置机器臂为可以抓取状态");
}


void CPlcFaceDlg::OnBnClickedStopplcButton()
{
	// TODO: Add your control notification handler code here
	m_server->stopPlc();
	m_server->SetArmMachineOptState(false);
	NotifyMsg("已设置机器臂为不可抓取状态");
}

void CPlcFaceDlg::OnBnClickedRssetButton()
{
	// TODO: Add your control notification handler code here
	m_server->resetPos();
}

void CPlcFaceDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CRect recT(0, 0, 0, 0);
	if (m_dlgRect == recT)
	{

	}
	else
	{
		if (cx == 0 && cy == 0)
		{
			return;
		}
		CRect rectCh;
		GetClientRect(&rectCh);
		//重绘控件
		repaint(IDC_ADDGOODS_BUTTON,      m_dlgRect.Width(),rectCh.Width(),m_dlgRect.Height(),rectCh.Height());
		repaint(IDC_TRANSFORIN_BUTTON,    m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_LOOKFORALL_BUTTON,    m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_DECREASEGOODS_BUTTON, m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_LOOKFORSPEC_BUTTON,   m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_STARTPLC_BUTTON,      m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_STOPPLC_BUTTON,       m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_RSSET_BUTTON,         m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_NEWGOODS_BUTTON,      m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_GOODSDECREASE_BUTTON, m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_CAMERA_BUTTON,        m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_ROBOT_BUTTON,         m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_PLCSTATE_STATIC,      m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_PLCSTATE_MFCBUTTON,   m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_ADDGOODSLC_STATIC,    m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_GOODSMANAGER_STATIC,  m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_OUTGOODSLC_STATIC,    m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_GOODSCHECK_STATIC,    m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_PLCCTR_STATIC,		  m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_ZUJIAN_STATIC,		  m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_CONNECTSATE_STATIC,   m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_PLCDECSTATE_STATIC,   m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_PLC_LOG_MFCBUTTON,    m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_ANGENTSTATE_STATIC,   m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_WARNINGRESET_BUTTON,  m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_ADDBOXOUT_BUTTON,     m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_BOXOUT_BUTTON,        m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_BOXBACK_BUTTON,       m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
		repaint(IDC_UPDATETOFACE_BUTTON,  m_dlgRect.Width(), rectCh.Width(), m_dlgRect.Height(), rectCh.Height());
	}
	//保存对话框大小
	GetClientRect(&m_dlgRect);
	//更新窗口
	Invalidate();
}


void CPlcFaceDlg::OnBnClickedTransforinButton()
{
	// TODO: Add your control notification handler code here
	PlcDevState state = m_server->GetDevState();
	if (state._abnormalState == 2 || state._devState == 2)
	{
		SZL_ERROR("設備狀態異常或忙碌");
		NotifyMsg("設備狀態異常或忙碌");
		return;
	}
	BoxShelfDlg dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
		int boxNum = dlg.m_boxNum;
		if (boxNum < 0 || boxNum>36)
		{
			NotifyMsg("货箱编号错误");
			return;
		}
		m_server->SetIsBtnBox(true);
		m_server->AddGoodsBoxIn(boxNum);
	}
}
void CPlcFaceDlg::ChangeSize(UINT id, int cx, int cy)
{
	CWnd *pWnd=nullptr;
	pWnd = GetDlgItem(id);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.left = rect.left*cx / m_dlgRect.Width();
		rect.right = rect.right*cx / m_dlgRect.Width();
		rect.top = rect.top*cy / m_dlgRect.Height();
		rect.bottom = rect.bottom*cy / m_dlgRect.Height();
		pWnd->MoveWindow(rect);
	}
}

void CPlcFaceDlg::SetButtonColor()
{
	ButtonColorCtl(m_addGoodsBtn);
	ButtonColorCtl(m_BoxInBtn);
	ButtonColorCtl(m_addProductBtn);
	ButtonColorCtl(m_decreaseProductBtn);
	ButtonColorCtl(m_outGoodsBtn);
	ButtonColorCtl(m_LookForAllBtn);
	ButtonColorCtl(m_LookForSpecBtn);
	ButtonColorCtl(m_StartBtn);
	ButtonColorCtl(m_StopBtn);
	ButtonColorCtl(m_ResetBtn);
	ButtonColorCtl(m_CameraBtn);
	ButtonColorCtl(m_JXbBtn);
	ButtonColorCtl(m_WarnResetBtn);
	ButtonColorCtl(m_AddBoxOutBtn);
	ButtonColorCtl(m_OutBoxOutBtn);
	ButtonColorCtl(m_OutBoxInBtn);
	ButtonColorCtl(m_UpdateToFaceBtn);
	ClientStateReport state;
	state._abnormalState = 2;
	SetPlcStatePlcShow(&state);
}

void CPlcFaceDlg::ButtonColorCtl(CMFCButton& btn)
{
	btn.SetFaceColor( 48| (128 << 8) | (207 << 16));
	btn.SetTextColor(250|(250<<8)|(250<<16));
	btn.m_bDontUseWinXPTheme = TRUE;
	btn.m_bTransparent = FALSE;
	btn.m_bDrawFocus = FALSE;
	CMFCButton* pbtn = (CMFCButton*)GetDlgItem(IDC_PLC_LOG_MFCBUTTON);
	if (pbtn)
	{
		pbtn->SetFaceColor(48 | (128 << 8) | (207 << 16));
		pbtn->SetTextColor(250 | (250 << 8) | (250 << 16));
		pbtn->m_bDontUseWinXPTheme = TRUE;
		pbtn->m_bTransparent = FALSE;
		pbtn->m_bDrawFocus = FALSE;
	}
}

void CPlcFaceDlg::InitFaceLog()
{
	m_LogList.InsertColumn(0, _T("日志"), LVCFMT_LEFT, 1000, 0);
	m_WarnList.InsertColumn(0, _T("PLC报警消息"), LVCFMT_LEFT, 500, 0);
	m_cancelWarnList.InsertColumn(0, _T("PLC报警消息消除"), LVCFMT_LEFT, 500, 0);
}

void CPlcFaceDlg::OnBnClickedNewgoodsButton()
{
	// TODO: Add your control notification handler code here
	NewGooddAddDlg dlg(this, 0);
	dlg.DoModal();
}


void CPlcFaceDlg::OnBnClickedGoodsdecreaseButton()
{
	// TODO: Add your control notification handler code here
	NewGooddAddDlg dlg(this, 1);
	dlg.DoModal();
}


void CPlcFaceDlg::OnBnClickedCameraButton()
{
	// TODO: Add your control notification handler code here
	if (m_cameraDlg == nullptr)
	{
		m_cameraDlg = new CameraDlg(this);
		m_cameraDlg->Create(IDD_3DCAMERA_DIALOG, this);
	}
	m_cameraDlg->ShowWindow(SW_SHOW);
}


void CPlcFaceDlg::OnBnClickedRobotButton()
{
	// TODO: Add your control notification handler code here
// 	RobotDlg dlg;
// 	dlg.DoModal();
	if (m_robotDlg == nullptr)
	{
		m_robotDlg = new RobotDlg(this);
		m_robotDlg->Create(IDD_ROBOT_DIALOG, this);
	}
	m_robotDlg->ShowWindow(SW_SHOW);
}
void CPlcFaceDlg::SetPlcConnectState(bool state)
{
	m_plcConState = state;
	CWnd* wnd = GetDlgItem(IDC_PLCSTATE_STATIC);
	if (wnd)
	{
		CString str;
		str.Format("plc连接:%d", state);
		wnd->SetWindowText(str);
		Invalidate();
	}
}
void CPlcFaceDlg::SetPlcStatePlcShow(const ClientStateReport* state)
{
	bool needFlush = false;
	if (m_plcDevState != state->_abnormalState)
	{
		needFlush =true;
	}
	CString tip = "";
	CWnd* wnd = GetDlgItem(IDC_PLCDECSTATE_STATIC);
	if (state->_abnormalState == 1)
	{
		m_plcDevState = state->_abnormalState;
		char str[] = "plc状态:正常";
		tip.Format("%s", str);
	}
	if (state->_abnormalState == 2)
	{
		m_plcDevState = state->_abnormalState;
		char str[] = "plc状态:异常";
		tip.Format("%s", str);
	}
	wnd->SetWindowText(tip);
	if (needFlush)
	{
		Invalidate();
	}
}
HBRUSH CPlcFaceDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
	CBrush newBrush;
	CRect rect;
	// TODO:  Change any attributes of the DC here
	if ( nCtlColor == CTLCOLOR_STATIC)
	{
		if (   ((pWnd->GetDlgCtrlID() == IDC_PLCSTATE_STATIC)&&m_plcConState)\
			|| ((pWnd->GetDlgCtrlID() == IDC_PLCDECSTATE_STATIC) && m_plcDevState==1)\
			|| ((pWnd->GetDlgCtrlID() == IDC_ANGENTSTATE_STATIC)&& m_AgentState)
		   )
		{
			pWnd->GetClientRect(&rect);
			newBrush.CreateSolidBrush(0 | (255 << 8) | (128 << 16));
			pDC->SelectObject(&newBrush);
			pDC->FillRect(rect, &newBrush);

			//pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(0);
			pDC->SetBkColor(0 | (255 << 8) | (128 << 16));
			
		}
		if ( ((pWnd->GetDlgCtrlID() == IDC_PLCSTATE_STATIC) && !m_plcConState) \
			|| ((pWnd->GetDlgCtrlID() == IDC_PLCDECSTATE_STATIC) && m_plcDevState ==2)\
			|| ((pWnd->GetDlgCtrlID() == IDC_ANGENTSTATE_STATIC) && !m_AgentState)
		   )
		{
			pWnd->GetClientRect(&rect);
			newBrush.CreateSolidBrush(255 | (0 << 8) | (0 << 16));
			pDC->SelectObject(&newBrush);
			pDC->FillRect(rect, &newBrush);

			//pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(0);
			pDC->SetBkColor(255 | (0 << 8) | (0 << 16));
		}
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_ROBOT_STATE_MAIN
			|| pWnd->GetDlgCtrlID() == IDC_STATIC_ROBOT_POS_MAIN2
			|| pWnd->GetDlgCtrlID() == IDC_STATIC_IO_STATE_MAIN
			|| pWnd->GetDlgCtrlID() == IDC_GOODSBOX_POS_STATIC
			)
		{
			CFont font;
			font.CreatePointFont(110, _T("宋体"));
			pDC->SelectObject(&font);
		}
		hbr = (HBRUSH)newBrush;
	}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
void CPlcFaceDlg::SetRobotArmState(char* str)
{
	CString tip = "";
	char tBuf[4096] = { 0 };
	sprintf_s(tBuf, "机械臂状态:%s", str);
	tip.Format("%s", tBuf);
	CWnd* wnd = GetDlgItem(IDC_STATIC_ROBOT_STATE_MAIN);
	wnd->SetWindowText(tip);
}
void CPlcFaceDlg::SetRobotArmPos(char* str)
{
	CString tip = "";
	char tBuf[4096] = { 0 };
	sprintf_s(tBuf, "机械臂位置(mm,o):%s", str);
	tip.Format("%s", tBuf);
	CWnd* wnd = GetDlgItem(IDC_STATIC_ROBOT_POS_MAIN2);
	wnd->SetWindowText(tip);
}
void CPlcFaceDlg::SetRobotArmIoState(char* str)
{
	CString tip = "";
	char tBuf[4096] = { 0 };
	sprintf_s(tBuf, "机械臂IO状态:%s", str);
	tip.Format("%s", tBuf);
	CWnd* wnd = GetDlgItem(IDC_STATIC_IO_STATE_MAIN);
	wnd->SetWindowText(tip);
}
void CPlcFaceDlg::SetGoodsBoxPos(char* str)
{
	CString tip = "";
	char tBuf[4096] = { 0 };
	sprintf_s(tBuf, "货箱位置:%s", str);
	tip.Format("%s", tBuf);
	CWnd* wnd = GetDlgItem(IDC_GOODSBOX_POS_STATIC);
	wnd->SetWindowText(tip);
}

int CPlcFaceDlg::WarnMsg(const char * op)
{
	if (::IsWindow(this->GetSafeHwnd()) && (::IsWindow(GetDlgItem(IDC_PLCWARNING_LIST)->GetSafeHwnd())))
	{
		/*int itnum = m_WarnList.GetItemCount();
		if (itnum > 50)
		{
			for (int i = 0; i < 10; i++)
			{
				m_WarnList.DeleteItem(0);
			}
		}*/
		int itnum = m_WarnList.GetItemCount();
		SYSTEMTIME systime;
		::GetLocalTime(&systime);
		char buf[4096] = {0};
		sprintf_s(buf, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]:%s", systime.wYear, systime.wMonth, systime.wDay,
			systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds, op);
		m_WarnList.InsertItem(itnum, buf, 0);
		itnum = m_WarnList.GetItemCount();
		return itnum;
	}
	return 0;
}

bool IsRightCellForGoods()
{
	bool isRight = false;
	return isRight;
}

void CPlcFaceDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case 1:
		if (/*m_bStorageMode*/0)
		{
			if (!m_bNotifySuccess)
			{
				char buf[256];
				if (_vbar.Scan(buf))
				{
					bool bSecondPushBox = true;
					CPlcFaceApp* app = (CPlcFaceApp*)m_parent;
					PlcDevState st = getServer()->GetDevState();
					if (st._abnormalState == 2 || st._devState == 2)
					{
						SZL_ERROR("设备异常或货箱正在推出,货箱到位后才能扫码");
						//语音提示
						app->OnSendTipToServer("设备异常或货箱正在推出,货箱到位后才能扫码");
						return;
					}
					SZL_INFO("当前扫到的码为：{0}", buf);
					SetNotifyState(true);
					strcpy(_barCode, buf);
					ProductInfoPLC goodsInfo;
					UShop::PLCDB::Instance().QueryProductInfoByBarcode(_barCode, goodsInfo);
					if (goodsInfo.id == 0)
					{
						SZL_ERROR("未找到对应商品ID");
						return;
					}


					std::vector<std::string> containerIDList;
					UShop::PLCDB::Instance().GetOnloadSuggestBoxIDByBarcode(_barCode, containerIDList);
					if (containerIDList.size() == 0)
					{
						//发消息到总控提示 不能上货，库存已满
						app->OnSendTipToServer("当前商品库存已满，无法继续上货，请扫描别的商品");
						SZL_ERROR("当前商品库存已满1，无法继续上货，请扫描别的商品");
						SetNotifyState(false);
						return;
					}
					if (m_LastBoxID != "")
					{
						ContainerInfoPLC containerInfo;
						UShop::PLCDB::Instance().QueryContainerInfoByID(m_LastBoxID, containerInfo);
						//利用商品信息里面存放格子类型判断是否适合存放在当前货箱
						if (IsRightCellForGoods())
						{
							app->OnSendTipToServer("当前商品不适合放在当前货箱，无法继续上货，请扫描别的商品");
							SZL_ERROR("当前商品不适合放在当前货箱，无法继续上货，请扫描别的商品");
							SetNotifyState(false);
							return;
						}
					}
					//如果查出的上货箱号和上次的箱号不一致，需要把上次的箱子推进去，然后再推出当前查出的箱子
					auto itC = containerIDList.begin();
					std::string shelfID;
					Container2ShelfID(shelfID, *itC);
					if (shelfID == "")
					{
						SZL_ERROR("未找到对应货架号");
						SetNotifyState(false);
						return;
					}
					m_CurBoxID = *itC;
					if (m_LastBoxID == "")
					{
						m_LastBoxID = *itC;
						getServer()->AddGoodsBoxOut(std::stoi(shelfID));
					}
					else
					{
						for (auto it : containerIDList)
						{
							if (it == m_LastBoxID)
							{
								bSecondPushBox = false;
								break;
							}
						}
						if (bSecondPushBox)
						{
							getServer()->SetAutoPushBoxState(true);
							//发消息到总控提示上货人员把上次推出的货箱放回然后下发“收回货箱”语音指令
							app->OnSendTipToServer("当前商品需要切换货箱上号,如果不想切换货箱请继续扫描不同种类的商品，否则需要收回当前货箱");
							return;
						}
						else
						{
							m_CurBoxID = m_LastBoxID;
						}
					}
					DoTimerTask(m_CurBoxID);
				}
			}
		};
		break;
	case 2:
		DealStoreListFormHttp(RequestStoreFromInfoCenter());
		DealProductListFormHttp(RequestAllGoodsFromInfoCenter());
		break;
	default:
		break;
	}
	__super::OnTimer(nIDEvent);
}

void CPlcFaceDlg::DoTimerTask(std::string CurBoxID)
{
	//CompleteBox boxInfo; 
	//char storageTip[100];
	//int suggestCellNum = 0;
	//CPlcFaceApp *app = (CPlcFaceApp *)m_parent;

	//UShop::PLCDB::Instance().GetOnloadSuggestCellNumByBoxID(CurBoxID, suggestCellNum);
	////更新库存
	//UShop::PLCDB::Instance().UpdateLoadInfoSingleTime(CurBoxID, suggestCellNum);
	////通知显示信息
	//UShop::PLCDB::Instance().GetOnloadShowInfo(CurBoxID, suggestCellNum, boxInfo);
	//app->NotifyShowGoodsBoxUI(boxInfo, suggestCellNum);

	//if (boxInfo.vecCellInfo.size() == 0)
	//{
	//	SetNotifyState(false);
	//	app->OnSendTipToServer("当前商品库存已满，无法继续上货，请扫描别的商品");
	//	SZL_ERROR("当前商品库存已满2，无法继续上货，请扫描别的商品");
	//	return;
	//}
	//sprintf_s(storageTip, "将商品放到 %d 号货箱第 %d 格", std::stoi(CurBoxID), suggestCellNum);
	//app->OnSendTipToServer(storageTip);

	//for (auto itC : boxInfo.vecCellInfo)
	//{
	//	if (itC.cellNum == suggestCellNum)
	//	{
	//		if (itC.cellStock == 0)
	//			SZL_INFO("上货时查到的数据库码：{0}", "");
	//		else
	//			SZL_INFO("上货时查到的数据库码：{0}", itC.barcode);
	//		/*if (strcmp(m_lastOutProductBarcode, _barCode))
	//		strcpy(m_lastOutProductBarcode, _barCode);*/
	//	}
	//}
	//SetNotifyState(false);
}

void CPlcFaceDlg::ShowLoadInfoForAPP(CompleteBox curBox)
{
	CPlcFaceApp *app = (CPlcFaceApp *)m_parent;
	char storageTip[100];
	//int suggestCellNum = 0;
	std::string suggestCellNum = "";
	for (auto &it : curBox.vecCellInfo)
	{
		if (!suggestCellNum.empty())
			suggestCellNum.append(",");
		suggestCellNum.append(std::to_string(it.cellNum));
	}
	app->NotifyShowGoodsBoxUI(curBox,suggestCellNum);
	sprintf_s(storageTip, "当前推出 %s 号货箱,请按提示放入商品", curBox.container.containerID);
	app->OnSendTipToServer(storageTip);
}

void CPlcFaceDlg::SetAgentState(bool state)
{
	CString tip = "";
	CWnd* wnd = GetDlgItem(IDC_ANGENTSTATE_STATIC);
	if (state)
	{
		m_AgentState = true;
	}
	else
	{
		m_AgentState = false;
	}
	Invalidate();
}
BOOL CPlcFaceDlg::Open(LPCTSTR profile, LPCTSTR section, int& level,std::string& ip)
{
	CProfile file;
	file.SetProfile(profile);

	ip= file.GetString("plcFace", _T("IP"), _T("127.0.0.1"));
	level = file.GetLong("plcFace", _T("LogLevel"), _T("9000"));

	m_AiServerIp= file.GetString("InfoCenter", _T("IP"), _T("192.168.64.22"));
	m_AiServerPort= file.GetLong("InfoCenter", _T("PORT"), _T("9080"));
	m_DBFilePath = file.GetString("Database", _T("Path"), _T("config\\ushop.db"));
	char buf[1024] = { 0 };
	m_configInfo._logLevel = level;
	m_configInfo._httpIp = file.GetString("plcFace", _T("HttpIp"), _T("127.0.0.1"));
	m_configInfo._httpPort = file.GetLong("plcFace", _T("HttpPort"), _T("9080"));
	m_configInfo._centerInfoIp = m_AiServerIp;
	m_configInfo._plcPort1 = file.GetLong("plcFace", _T("Port"), _T("9000"));
	m_configInfo._plcPort2 = file.GetLong("plcFace", _T("Port2"), _T("9001"));
	m_configInfo._plcIp = file.GetString("plcFace", _T("IP"), _T("127.0.0.1"));
	m_configInfo._beatConnect = file.GetLong("plcFace", _T("BeatConnect"), _T("5000"));
	m_configInfo._storeNo= file.GetString("plcFace", _T("StoreNo"), _T(""));

	sprintf_s(buf, "配置文件读取到的日志级别:%d,plcip:%s,信息中心服务后台IP:%s,端口:%d,本地http服务ip:%s,端口:%d,店铺编号:%s", level, ip.c_str(), m_AiServerIp.c_str(), m_AiServerPort, m_configInfo._httpIp.c_str(), m_configInfo._httpPort, m_configInfo._storeNo.c_str());
	NotifyMsg(buf);
	m_cli = new httplib::Client(m_AiServerIp.c_str());
	//m_cli = new httplib::Client("localhost", 8080);
	return TRUE;
}
int CPlcFaceDlg::DeleteWarnMsg(const char * op)
{
	if (::IsWindow(this->GetSafeHwnd()) && (::IsWindow(GetDlgItem(IDC_CANCELPLCWARNING_LIST)->GetSafeHwnd())))
	{
		int itnum = m_cancelWarnList.GetItemCount();
		SYSTEMTIME systime;
		::GetLocalTime(&systime);
		char buf[4096] = {0};
		sprintf_s(buf, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]:%s", systime.wYear, systime.wMonth, systime.wDay,
			systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds, op);
		m_cancelWarnList.InsertItem(itnum, buf, 0);
		itnum = m_cancelWarnList.GetItemCount();
		return itnum;
	}
	return 0;
}
void CPlcFaceDlg::ClearWarnMsg(std::string str)
{
	int itnum = m_WarnList.GetItemCount();
	CString strC;
	for (int i = 0; i <itnum; i++)
	{
		strC = "";
		strC=m_WarnList.GetItemText(i,0);
		char tCh[256] = { 0 };
		strcpy(tCh, strC.GetBuffer(strC.GetLength()));
		std::string TeStr = tCh;
		if (TeStr.empty())
			continue;
		if ( (TeStr.substr(26, TeStr.size()-26)) == str)
		{
			m_WarnList.DeleteItem(i);
		}
	}
}
void CPlcFaceDlg::ClearDelWarnMsg(std::string str)
{
	int itnum = m_cancelWarnList.GetItemCount();
	CString strC;
	for (int i = 0; i <itnum; i++)
	{
		strC = "";
		strC = m_cancelWarnList.GetItemText(i, 0);
		char tCh[256] = { 0 };
		strcpy(tCh, strC.GetBuffer(strC.GetLength()));
		std::string TeStr = tCh;
		if (TeStr.empty())
			continue;
		if ((TeStr.substr(26, TeStr.size() - 26)) == str)
		{
			m_cancelWarnList.DeleteItem(i);
		}
	}
}
void CPlcFaceDlg::BoxPosUpdate(char* str)
{
	CWnd* wnd = GetDlgItem(IDC_GOODSBOX_POS_STATIC);
	if (wnd)
	{
		CString cstr = "";
		cstr.Format("%s", str);
		wnd->SetWindowText(cstr);
	}
}


void CPlcFaceDlg::OnBnClickedWarningresetButton()
{
	// TODO: Add your control notification handler code here
	m_server->PlcWarnReset();
}


void CPlcFaceDlg::OnBnClickedBoxbackButton()
{
	// TODO: Add your control notification handler code here
	PlcDevState state = m_server->GetDevState();
	if (state._abnormalState == 2 || state._devState == 2)
	{
		SZL_ERROR("設備狀態異常或忙碌");
		NotifyMsg("設備狀態異常或忙碌");
		return;
	}
	BoxShelfDlg dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
		int boxNum = dlg.m_boxNum;
		if (boxNum < 0 || boxNum>36)
		{
			NotifyMsg("货箱编号错误");
			return;
		}
		m_server->SetIsBtnBox(true);
		m_server->goodsBoxIn(boxNum);
	}
}


void CPlcFaceDlg::OnBnClickedAddboxoutButton()
{
	// TODO: Add your control notification handler code here
	PlcDevState state = m_server->GetDevState();
	if (state._abnormalState == 2 || state._devState == 2)
	{
		SZL_ERROR("設備狀態異常或忙碌");
		NotifyMsg("設備狀態異常或忙碌");
		return;
	}
	BoxShelfDlg dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
		int boxNum = dlg.m_boxNum;
		if (boxNum < 0 || boxNum>36)
		{
			NotifyMsg("货箱编号错误");
			return;
		}
		m_server->SetIsBtnBox(true);
		m_server->AddGoodsBoxOut(boxNum);
	}
}


void CPlcFaceDlg::OnBnClickedBoxoutButton()
{
	// TODO: Add your control notification handler code here
	PlcDevState state = m_server->GetDevState();
	if (state._abnormalState == 2 || state._devState == 2)
	{
		SZL_ERROR("設備狀態異常或忙碌");
		NotifyMsg("設備狀態異常或忙碌");
		return;
	}
	BoxShelfDlg dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
		int boxNum = dlg.m_boxNum;
		if (boxNum < 0 || boxNum>36)
		{
			NotifyMsg("货箱编号错误");
			return;
		}
		m_server->SetIsBtnBox(true);
		m_server->goodsBoxOut(boxNum);
	}
}


void CPlcFaceDlg::OnBnClickedUpdatetofaceButton()
{
	// TODO: Add your control notification handler code here
	UpdateStockToServer();
}


void CPlcFaceDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	CMenu menu;
	menu.LoadMenu(IDR_PLCFACE_MENU);
	CMenu * pMenu;
	pMenu = menu.GetSubMenu(0);

	pMenu->EnableMenuItem(ID_32771, MF_BYCOMMAND | MF_ENABLED);
	pMenu->EnableMenuItem(ID_32772, MF_BYCOMMAND | MF_ENABLED);
	pMenu->EnableMenuItem(ID_32773, MF_BYCOMMAND | MF_ENABLED);
	pMenu->EnableMenuItem(ID_32774, MF_BYCOMMAND | MF_ENABLED);
	pMenu->EnableMenuItem(ID_32775, MF_BYCOMMAND | MF_ENABLED);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	pMenu->Detach();
	menu.DestroyMenu();
}
void CPlcFaceDlg::OnSyncData()
{
	OnBnClickedUpdatetofaceButton();
}
void CPlcFaceDlg::OnStartPlc()
{
	OnBnClickedStartplcButton();
}
void CPlcFaceDlg::OnStopPlc()
{
	OnBnClickedStopplcButton();
}
void CPlcFaceDlg::OnResetPlc()
{
	OnBnClickedRssetButton();
}
void CPlcFaceDlg::OnWarnReSet()
{
	OnBnClickedWarningresetButton();
}
std::string CPlcFaceDlg::PostData(const std::string& postdata, std::string path)
{
	std::string strResponse = "";
	std::string data = postdata;
	
#ifdef _HTTPLIB_
	std::string pathT = (std::string)("/octv-unmanned-store/third/store/") + path;
	SZL_DEBUG("req->body:{0},协议地址:{1}", data,pathT);
	NotifyMsg("req->body:%s,协议地址:%s", data.c_str(), pathT.c_str());
	std::string Tdata = nbase::win32::MBCSToUTF8(data);
	auto res = m_cli->Post(pathT.c_str(), Tdata, "application/json");
	if (res)
	{
		NotifyMsg("HTTP回复状态码:%d", res->status);
		SZL_DEBUG("HTTP回复状态码:{0},res->body:{1}", res->status, nbase::win32::UTF8ToMBCS(res->body));
		if (res->status == 200)
		{
			//NotifyMsg("回复body:%s",(nbase::win32::UTF8ToMBCS(res->body).c_str()));
			strResponse = res->body;
		}
	}
	else
	{
		SZL_ERROR("http请求异常或超时,协议类型{0}", pathT);
		NotifyMsg(((std::string)("http请求异常或超时,协议类型:")+ pathT).c_str());
	}
#else
	CString url;
	url.Format("%s/octv-unmanned-store/third/store/%s", m_AiServerIp.c_str(), path.c_str());
	NotifyMsg("HTTP协议地址%s", url.GetBuffer(url.GetLength()));
	SZL_DEBUG("HTTP协议地址{0}", url.GetBuffer(url.GetLength()));
	//SZL_DEBUG();
	m_HttpClient.HttpPost(url, data.c_str(), strResponse);
	SZL_DEBUG("HTTP回复,res->body:{0}", strResponse);
	return nbase::win32::MBCSToUTF8(strResponse);
#endif // _HTTPLIB_
	return strResponse;
}
std::string  CPlcFaceDlg::UpdateStockToInfoCenter(std::string postData)
{
	if (postData == "")
	{
		SZL_ERROR("更新到后台的库存信息有误");
		NotifyMsg("更新到后台的库存信息有误");
		return "";
	}
	SZL_DEBUG("更新库存到信息中心");
	SZL_DEBUG("库存数据 {0}", postData);
	NotifyMsg("更新库存到信息中心");
	return PostData(postData, "modifyReserveNum");
}
std::string CPlcFaceDlg::GoodsOut(std::string postData)
{
	SZL_DEBUG("出货订单到信息中心");
	NotifyMsg("出货订单到信息中心");
	return PostData(postData, "goodsOut");
}
std::string CPlcFaceDlg::RequestAllGoodsFromInfoCenter()
{
	SZL_DEBUG("初始化商品列表");
	NotifyMsg("初始化商品列表");
	std::string postStr="";
	postStr.append("{\"storeNo\":\"").append(m_configInfo._storeNo).append("\"}");
	return PostData(postStr, "productList");
}
std::string CPlcFaceDlg::RequestStoreFromInfoCenter()
{
	SZL_DEBUG("初始化店铺列表");
	NotifyMsg("初始化店铺列表");
	std::string postStr = "";
	postStr.append("{\"storeNo\":\"").append(m_configInfo._storeNo).append("\"}");
	return PostData(postStr, "storeList");
}
void CPlcFaceDlg::CreateHttpServe()
{
	if (!m_svr.is_valid()) {
		SZL_ERROR("httpserver has an error...");
		NotifyMsg("httpserver has an error...");
		return;
	}
	//boxIn
	m_svr.Get("/boxIn", [this](const httplib::Request& req, httplib::Response& res) {
		std::string body = req.body;
		NotifyMsg("收到补货货箱入库请求，body:%s", (nbase::win32::UTF8ToMBCS(body)).c_str());
		SZL_DEBUG("收到补货货箱入库请求，body:{0}", nbase::win32::UTF8ToMBCS(body));
		m_server->AddGoodsBoxIn(m_server->GetCurBoxNum());
		std::string repStr = R"({"success":"true","msg":"操作成功"})";
		res.set_content(nbase::win32::MBCSToUTF8(repStr), "application/json");
		});
	
	m_svr.Post("/boxIn", [this](const httplib::Request& req, httplib::Response& res) {
		std::string body = req.body;
		NotifyMsg("收到补货货箱入库请求，body:%s", (nbase::win32::UTF8ToMBCS(body)).c_str());
		SZL_DEBUG("收到补货货箱入库请求，body:{0}", nbase::win32::UTF8ToMBCS(body));
		m_server->AddGoodsBoxIn(m_server->GetCurBoxNum());
		std::string repStr = R"({"success":"true","msg":"操作成功"})";
		res.set_content(nbase::win32::MBCSToUTF8(repStr), "application/json");
		});

	//上下架商品
	m_svr.Post("/OnOffCommodity", [this](const httplib::Request& req, httplib::Response& res) {
		std::string body = req.body;
		NotifyMsg("收到上下架商品请求，body:%s",(nbase::win32::UTF8ToMBCS(body)).c_str());
		SZL_DEBUG("收到上下架商品请求，body:{0}", nbase::win32::UTF8ToMBCS(body));
		GetExhibitGoodsFormHttp(body);
		std::string repStr = R"({"success":"true","msg":"操作成功"})";
		res.set_content(nbase::win32::MBCSToUTF8(repStr), "application/json");
		});

	m_svr.set_error_handler([](const httplib::Request& req, httplib::Response& res) {
		const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
		char buf[BUFSIZ];
		snprintf(buf, sizeof(buf), fmt, res.status);
		res.set_content(buf, "text/html");
		});

	m_svr.set_logger([this](const httplib::Request& req, const httplib::Response& res) {
		std::string logstr = log(req, res);
		SZL_DEBUG("{0}", logstr.c_str());
		NotifyMsg(logstr.c_str());
		});
}
void CPlcFaceDlg::Init()
{
	InitFaceLog();
	//初始化日志
	char _path[256] = { 0 };
	::GetModuleFileName(nullptr, _T(_path), sizeof(_path));
	_path[strrchr(_path, '\\') - _path + 1] = 0;
	m_CommonPath = _path;
	/*SYSTEMTIME systime;
	::GetLocalTime(&systime);*/
	//每天00:00创建日志
	sprintf_s(_path, "%s%s", _path, R"(\Logs\PlcFace.log)");
	std::string configFile = m_CommonPath + R"(config\PLCconfig.ini)";
	int LogLevel = 0;
	std::string ip = "";
	Open(configFile.c_str(), "plcFace", LogLevel, ip);
	InitDayLog(_path, LogLevel);

	//开启服务线程
	m_serveTh = CreateThreadS();
	//初始化数据库\\192.168.50.114\share\data\ushop.db
	std::string DBFile = m_DBFilePath;
	DBFile = nbase::win32::MBCSToUTF8(DBFile);
	UShop::PLCDB::Instance().Initialize(DBFile.c_str());
	UShop::PLCDB::Instance().SetScancodeState(false);
	//测试
	//UpdateStockToServer();//测试

	ROBOT_MODULE::RobotAgent::GetInstance()->run();
	UShop::GoodsLocationAdapter::getInstance()->start();//启动画面并实时识别
	SetButtonColor();
	CreateHttpServe();
	InitHttpServe();
	DealStoreListFormHttp(RequestStoreFromInfoCenter());
	DealProductListFormHttp(RequestAllGoodsFromInfoCenter());

	//测试
	//UpdateStockToInfoCenter("");
	_vbar.Open();
	SetTimer(1, 30, NULL);
	SetTimer(2, 1000 * 60 * 60, NULL);        //一小时拉取一次后台数据
	DeleteOutDateLogTh();
}
void CPlcFaceDlg::InitHttpServe()
{
	std::thread th([&] {
		while (1)
		{
			//5秒检测一次,断开重连
			MyThread::Lock();
			if (!g_isRun)
			{
				MyThread::UnLock();
				break;
			}
			if (!m_svr.is_running())
			{
				std::thread th(
					[&]() {m_svr.listen(m_configInfo._httpIp.c_str(), m_configInfo._httpPort); });
				th.detach();
			}
			MyThread::UnLock();
			Sleep(5000);
		}
		});

	th.detach();
}
void CPlcFaceDlg::DeleteOutDateLogTh()
{
	std::thread th([this]() {
		while (1)
		{
			MyThread::Lock();
			if (!g_isRun)
			{
				MyThread::UnLock();
				break;
			}
			time_t _30Days = 2592000;
			std::vector<struct _finddata_t> files;
			ToolHelp::GetFiles(m_CommonPath + R"(\Logs\*.log)", files);
			time_t nowT = time(nullptr);
			for (auto& it : files)
			{
				//大于30天的日志文件删除
				if ((nowT - it.time_create) > _30Days)
				{
					DeleteFile(it.name);
				}
			}
			MyThread::UnLock();
			//一天检测一次
			Sleep(1000 * 60 * 60 * 24);
		}
		}
	);
	th.detach();
}