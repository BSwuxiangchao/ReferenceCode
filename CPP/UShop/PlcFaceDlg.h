
// PlcFaceDlg.h : header file
//

#pragma once

#include"AddGoodsDialog.h"
#include"AllGoodsStockDialog.h"
#include"TcpServer.hpp"
#include"LookForSpecGoodDlg.h"
#include"OutGoodsDialog.h"
#include "RobotDlg.h"
#include "CameraDlg.h"
#include "afxwin.h"
#include "..\VBar\channel.h"
#include "afxcmn.h"
#include"HttpClient.h"
#include <httplib.h>

#define SERVER_CERT_FILE "./cert.pem"
#define SERVER_PRIVATE_KEY_FILE "./key.pem"

struct ConfigInfo
{
	std::string _plcIp = "";
	std::string _httpIp = "";
	std::string _centerInfoIp = "";
	std::string _storeNo = "";
	int _plcPort1 = 0;
	int _plcPort2 = 0;
	int _httpPort = 0;
	int _beatConnect = 0;
	int _logLevel = 0;
	int _centerInfoPort = 0;
};

// CPlcFaceDlg dialxog
class CPlcFaceDlg : public CDialogEx,public MyThread
{
// Construction
public:
	CPlcFaceDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CPlcFaceDlg();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLCFACE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//重绘函数
	virtual void repaint(UINT id,int last_width,int now_width,int last_height,int now_height);
	void ChangeSize(UINT id, int cx, int cy);
	void SetButtonColor();
	void ButtonColorCtl(CMFCButton& btn);
	void InitFaceLog();
	BOOL Open(LPCTSTR profile, LPCTSTR section,int& level, std::string& ip);
	void CreateHttpServe();
	void InitHttpServe();
	void DeleteOutDateLogTh();
private:
	AllGoodsStockDialog* m_goodStockDlg = nullptr;
	LookForSpecGoodDlg* m_lookForSpecGoods = nullptr;
	OutGoodsDialog* m_OutGoodsDlg = nullptr;
	AddGoodsDialog* m_AddGoodsDlg = nullptr;
	CameraDlg* m_cameraDlg = nullptr;
	RobotDlg* m_robotDlg = nullptr;
	TcpServer* m_server = nullptr;
	std::thread* m_serveTh = nullptr;
	CRect m_dlgRect;
	int m_currentBoxNum=0;              //当前货架号
	int m_BoxInNum = 0;                 //入库货架号
	ShipmentInfo m_shipmentInfo;	//订单信息
	std::vector<OnloadProductInfo> m_OnloadProductList;
	void* m_parent;                 //父指针
	std::string m_CommonPath = "";
	bool m_plcConState=false;             //plc连接状态
	int m_plcDevState = 1;               //plc设备状态
	bool m_AgentState = false;           //总控连接状态
	CListCtrl m_LogList;                 //日志列表
	CListCtrl m_WarnList;                //报警列表
	CListCtrl m_cancelWarnList;           //取消报价列表
	bool m_bStorageMode;				//是否处于上货状态
	bool m_bNotifySuccess;				//是否通知完成
	VBar			_vbar;
	char _barCode[256];
	std::string m_LastBoxID;				//上次推出的货箱编号
	std::string m_CurBoxID;				//当前推出的货箱编号
	std::string m_AiServerIp = "";      //信息中心服务ip
	int m_AiServerPort = 0;             //信息中心服务端口

	std::string m_DBFilePath = "";      //信息中心服务ip

	std::string m_storeName = "";		//店铺名称
	std::string m_stoteNo = "";			//店铺编号
	std::vector<OnloadProductInfo> m_APP_LoadInfoList;	//APP下发的入库订单
	ConfigInfo m_configInfo;
	std::vector<CompleteBox>       m_productCellInfoList;   //需要推出的货箱列表
public:
	//Tcp服务线程函数
	void Routine()override final;
	TcpServer* getServer() { return m_server; }
	inline void setCurBoxId(int id) 
	{  
		m_currentBoxNum = id;
		m_server->SetCurBoxNum(id);
	}
	inline void SetBoxInNum(int id) 
	{ 
		m_BoxInNum = id; 
		m_server->SetBinNum(id);
	}
	//出货更新数据库
	void UpdateOutGoodsInfo();
	//货箱号转货架号
	void Container2ShelfID(std::string& ShelfID,std::string ContainerID);
	//上货更新数据库
	void UpdateInGoodsInfo(std::string containerID, std::vector<CompleteBox> productCellInfoList);
	//上货成功后更新库存等具体信息到后台
	void UpdateInGoodsInfoToBg(int modeFlag);
	BOOL Checkout(ShipmentInfo& orders, std::string& msg);
	bool OutGoods();
	//发送各商品库存信息到总控   // mode：0表示全部商品，1表示上货时选择的商品，2表示出货时选择的商品
	void UpdateStockToServer(int mode = 0);
	//接收上货列表，查询分配位置信息
	bool SetOnloadInfo(std::vector<OnloadProductInfo> productList);

	inline void SetParent(void* p)
	{
		m_parent = p;
	}
	inline void* GetParentMain() { return m_parent; }
	inline void  SetStorageMode(bool bStore)
	{
		m_bStorageMode = bStore;
	}
	inline bool GetStorageMode()
	{
		return m_bStorageMode;
	}
	inline void SetNotifyState(bool success)
	{
		m_bNotifySuccess = success;
	}
	inline void SetLastBoxID(std::string LastBoxID)
	{
		m_LastBoxID = LastBoxID;
	}
	inline std::string GetLastBoxID()
	{
		return m_LastBoxID;
	}
	inline void SetCurBoxID(std::string CurBoxID)
	{
		m_CurBoxID = CurBoxID;
	}
	inline std::string GetCurBoxID()
	{
		return m_CurBoxID;
	}
	void SetPlcConnectState(bool state);

	void SetPlcStatePlcShow(const ClientStateReport* state);
	void SetRobotArmState(char* str);
	void SetRobotArmPos(char* str);
	void SetRobotArmIoState(char* str);
	void SetGoodsBoxPos(char* str);
	int WarnMsg(const char* op);
	void SetAgentState(bool state);
	int DeleteWarnMsg(const char * op);
	void ClearWarnMsg(std::string str);
	void ClearDelWarnMsg(std::string str);
	void BoxPosUpdate(char* str);
	void DoTimerTask(std::string strBoxID);
	void ShowLoadInfoForAPP(CompleteBox curBox);

	//*************************与信息中心交互BEGIN**************************************/
	std::string PostData(const std::string& postdata, std::string path);
	std::string RequestAllGoodsFromInfoCenter();                    //初始化商品列表接口
	std::string RequestStoreFromInfoCenter();                       //初始化店铺列表接口
	std::string UpdateStockToInfoCenter(std::string postData);      //更新库存到信息中心
	std::string GoodsOut(std::string postData);                     //商品订单出货接口

	//*************************与信息中心交互END**************************************/
	//--------------------------------json http-------------------------------------------------
	//初始化店铺列表接口
	bool DealStoreListFormHttp(const std::string strResponse);
	//初始化商品列表接口
	bool DealProductListFormHttp(const std::string strResponse);
	//更新库存接口
	std::string SetmodifyReserveNumtoHttp(const std::vector<CellInfoPLC> list,int modeFlag);
	bool DealModifyReserveNumFormHttp(const std::string strResponse, std::vector<CellInfoPLC>& list);
	//上.下架商品列表接口
	bool GetExhibitGoodsFormHttp(const std::string strResponse);
	//补货商品列表接口
	bool GetAddGoodsFormHttp(const std::string strResponse);
public:
	template<class ...Args>
	void NotifyMsg(const char* fmt, Args...args)
	{
		char Tbuf[2048] = { 0 };
		sprintf_s(Tbuf, fmt, args...);
		if (::IsWindow(this->GetSafeHwnd()) && (::IsWindow(GetDlgItem(IDC_PLCFACE_LOG_LIST)->GetSafeHwnd())))
		{
			int itnum = m_LogList.GetItemCount();
			if (itnum > 100)
			{
				for (int i = 0; i < 10; i++)
				{
					m_LogList.DeleteItem(0);
				}
			}
			itnum = m_LogList.GetItemCount();
			SYSTEMTIME systime;
			::GetLocalTime(&systime);
			char buf[4096];
			sprintf_s(buf, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]:%s", systime.wYear, systime.wMonth, systime.wDay,
				systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds, Tbuf);
			m_LogList.InsertItem(itnum, buf, 0);
		}
	}
	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	void Init();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	httplib::Client* m_cli = nullptr;
	httplib::Server m_svr;
	CHttpClient     m_HttpClient;
	CMFCButton m_addGoodsBtn;
	CMFCButton m_BoxInBtn;
	CMFCButton m_addProductBtn;
	CMFCButton m_decreaseProductBtn;
	CMFCButton m_outGoodsBtn;
	CMFCButton m_LookForAllBtn;
	CMFCButton m_LookForSpecBtn;
	CMFCButton m_StartBtn;
	CMFCButton m_StopBtn;
	CMFCButton m_ResetBtn;
	CMFCButton m_CameraBtn;
	CMFCButton m_JXbBtn;
	CMFCButton m_WarnResetBtn;
	CMFCButton m_AddBoxOutBtn;
	CMFCButton m_OutBoxOutBtn;
	CMFCButton m_OutBoxInBtn;
	CMFCButton m_UpdateToFaceBtn;
public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedAddgoodsButton();
	afx_msg void OnBnClickedDecreasegoodsButton();
	afx_msg void OnBnClickedLookforallButton();
	afx_msg void OnBnClickedLookforspecButton();
	afx_msg void OnBnClickedStartplcButton();
	afx_msg void OnBnClickedStopplcButton();
	afx_msg void OnBnClickedRssetButton();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedTransforinButton();
	afx_msg void OnBnClickedNewgoodsButton();
	afx_msg void OnBnClickedGoodsdecreaseButton();
	//afx_msg void OnBnClickedGoodboxinButton();
	afx_msg void OnBnClickedCameraButton();
	afx_msg void OnBnClickedRobotButton();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedWarningresetButton();
	afx_msg void OnBnClickedBoxbackButton();
	afx_msg void OnBnClickedAddboxoutButton();
	afx_msg void OnBnClickedBoxoutButton();
	afx_msg void OnBnClickedUpdatetofaceButton();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnSyncData();
	afx_msg void OnStartPlc();
	afx_msg void OnStopPlc();
	afx_msg void OnResetPlc();
	afx_msg void OnWarnReSet();
};