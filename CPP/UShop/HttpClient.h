#pragma once

#include <afxinet.h>
#include <string>
using namespace std;

#define  IE_AGENT  _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)")

// 操作成功
#define SUCCESS        0
// 操作失败
#define FAILURE        1
// 操作超时 www.it165.net
#define OUTTIME        2

class CHttpClient
{
public:
	CHttpClient(LPCTSTR strAgent = IE_AGENT);
	virtual ~CHttpClient(void);

	int HttpGet(LPCTSTR strUrl, LPCTSTR strPostData, string &strResponse);
	int HttpPost(LPCTSTR strUrl, LPCTSTR strPostData, string &strResponse);

	static BOOL UploadFile(LPCTSTR strURL, LPCTSTR strLocalFileName, LPTSTR errorMsgBuffer = NULL);
	static BOOL DownloadFile(LPCTSTR strFileURLInServer, LPCTSTR strFileLocalFullPath, LPTSTR errorMsgBuffer = NULL);
	inline BOOL IsConnected()
	{
		if (m_pSession&&m_pConnection)
			return true;
		else
			return false;
	}
private:
	int ExecuteRequest(LPCTSTR strMethod, LPCTSTR strUrl, LPCTSTR strPostData, string &strResponse);
	void Clear();

private:
	CInternetSession *m_pSession;
	CHttpConnection *m_pConnection;
	CHttpFile *m_pFile;
};