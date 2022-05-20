
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
	//�ػ溯��
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
	int m_currentBoxNum=0;              //��ǰ���ܺ�
	int m_BoxInNum = 0;                 //�����ܺ�
	ShipmentInfo m_shipmentInfo;	//������Ϣ
	std::vector<OnloadProductInfo> m_OnloadProductList;
	void* m_parent;                 //��ָ��
	std::string m_CommonPath = "";
	bool m_plcConState=false;             //plc����״̬
	int m_plcDevState = 1;               //plc�豸״̬
	bool m_AgentState = false;           //�ܿ�����״̬
	CListCtrl m_LogList;                 //��־�б�
	CListCtrl m_WarnList;                //�����б�
	CListCtrl m_cancelWarnList;           //ȡ�������б�
	bool m_bStorageMode;				//�Ƿ����ϻ�״̬
	bool m_bNotifySuccess;				//�Ƿ�֪ͨ���
	VBar			_vbar;
	char _barCode[256];
	std::string m_LastBoxID;				//�ϴ��Ƴ��Ļ�����
	std::string m_CurBoxID;				//��ǰ�Ƴ��Ļ�����
	std::string m_AiServerIp = "";      //��Ϣ���ķ���ip
	int m_AiServerPort = 0;             //��Ϣ���ķ���˿�

	std::string m_DBFilePath = "";      //��Ϣ���ķ���ip

	std::string m_storeName = "";		//��������
	std::string m_stoteNo = "";			//���̱��
	std::vector<OnloadProductInfo> m_APP_LoadInfoList;	//APP�·�����ⶩ��
	ConfigInfo m_configInfo;
	std::vector<CompleteBox>       m_productCellInfoList;   //��Ҫ�Ƴ��Ļ����б�
public:
	//Tcp�����̺߳���
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
	//�����������ݿ�
	void UpdateOutGoodsInfo();
	//�����ת���ܺ�
	void Container2ShelfID(std::string& ShelfID,std::string ContainerID);
	//�ϻ��������ݿ�
	void UpdateInGoodsInfo(std::string containerID, std::vector<CompleteBox> productCellInfoList);
	//�ϻ��ɹ�����¿��Ⱦ�����Ϣ����̨
	void UpdateInGoodsInfoToBg(int modeFlag);
	BOOL Checkout(ShipmentInfo& orders, std::string& msg);
	bool OutGoods();
	//���͸���Ʒ�����Ϣ���ܿ�   // mode��0��ʾȫ����Ʒ��1��ʾ�ϻ�ʱѡ�����Ʒ��2��ʾ����ʱѡ�����Ʒ
	void UpdateStockToServer(int mode = 0);
	//�����ϻ��б���ѯ����λ����Ϣ
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

	//*************************����Ϣ���Ľ���BEGIN**************************************/
	std::string PostData(const std::string& postdata, std::string path);
	std::string RequestAllGoodsFromInfoCenter();                    //��ʼ����Ʒ�б�ӿ�
	std::string RequestStoreFromInfoCenter();                       //��ʼ�������б�ӿ�
	std::string UpdateStockToInfoCenter(std::string postData);      //���¿�浽��Ϣ����
	std::string GoodsOut(std::string postData);                     //��Ʒ���������ӿ�

	//*************************����Ϣ���Ľ���END**************************************/
	//--------------------------------json http-------------------------------------------------
	//��ʼ�������б�ӿ�
	bool DealStoreListFormHttp(const std::string strResponse);
	//��ʼ����Ʒ�б�ӿ�
	bool DealProductListFormHttp(const std::string strResponse);
	//���¿��ӿ�
	std::string SetmodifyReserveNumtoHttp(const std::vector<CellInfoPLC> list,int modeFlag);
	bool DealModifyReserveNumFormHttp(const std::string strResponse, std::vector<CellInfoPLC>& list);
	//��.�¼���Ʒ�б�ӿ�
	bool GetExhibitGoodsFormHttp(const std::string strResponse);
	//������Ʒ�б�ӿ�
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