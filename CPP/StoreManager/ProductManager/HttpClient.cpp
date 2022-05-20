// HttpClient.cpp
#include "stdafx.h"
#include "HttpClient.h"
#define  BUFFER_SIZE       1024

#define  NORMAL_CONNECT             INTERNET_FLAG_KEEP_CONNECTION
#define  SECURE_CONNECT                NORMAL_CONNECT | INTERNET_FLAG_SECURE
#define  NORMAL_REQUEST             INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE 
#define  SECURE_REQUEST             NORMAL_REQUEST | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID

CHttpClient::CHttpClient(LPCTSTR strAgent)
{
	m_pSession = new CInternetSession(strAgent);
	m_pConnection = NULL;
	m_pFile = NULL;
}


CHttpClient::~CHttpClient(void)
{
	Clear();
	if (NULL != m_pSession)
	{
		m_pSession->Close();
		delete m_pSession;
		m_pSession = NULL;
	}
}

void CHttpClient::Clear()
{
	if (NULL != m_pFile)
	{
		m_pFile->Close();
		delete m_pFile;
		m_pFile = NULL;
	}

	if (NULL != m_pConnection)
	{
		m_pConnection->Close();
		delete m_pConnection;
		m_pConnection = NULL;
	}
}

int CHttpClient::ExecuteRequest(LPCTSTR strMethod, LPCTSTR strUrl, const char* strPostData, wstring &strResponse)
{
	CString strServer;
	CString strObject;
	DWORD dwServiceType;
	INTERNET_PORT nPort;
	strResponse = L"";

	AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort);

	if (AFX_INET_SERVICE_HTTP != dwServiceType && AFX_INET_SERVICE_HTTPS != dwServiceType)
	{
		return FAILURE;
	}

	try
	{
		m_pConnection = m_pSession->GetHttpConnection(strServer,
			dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_CONNECT : SECURE_CONNECT,
			nPort);
		m_pFile = m_pConnection->OpenRequest(strMethod, strObject,
			NULL, 1, NULL, NULL,
			(dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_REQUEST : SECURE_REQUEST));

		//DWORD dwFlags;
		//m_pFile->QueryOption(INTERNET_OPTION_SECURITY_FLAGS, dwFlags);
		//dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
		//set web server option
		//m_pFile->SetOption(INTERNET_OPTION_SECURITY_FLAGS, dwFlags);

		//m_pFile->AddRequestHeaders("Accept: *,*/*");
		/*m_pFile->AddRequestHeaders(L"Accept-Language: zh-cn");*/
		m_pFile->AddRequestHeaders(L"Content-Type: application/json");
		//m_pFile->AddRequestHeaders("Accept-Encoding: gzip, deflate");

		m_pFile->SendRequest(NULL, 0, (LPVOID)strPostData, strPostData == NULL ? 0 : strlen(strPostData));

		char szChars[BUFFER_SIZE + 1] = { 0 };
		string strRawResponse = "";
		UINT nReaded = 0;
		while ((nReaded = m_pFile->Read((void*)szChars, BUFFER_SIZE)) > 0)
		{
			szChars[nReaded] = '\0';
			strRawResponse += szChars;
			memset(szChars, 0, BUFFER_SIZE + 1);
		}

		int unicodeLen = MultiByteToWideChar(CP_UTF8, 0, strRawResponse.c_str(), -1, NULL, 0);
		WCHAR *pUnicode = new WCHAR[unicodeLen + 1];
		memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));

		MultiByteToWideChar(CP_UTF8, 0, strRawResponse.c_str(), -1, pUnicode, unicodeLen);
		CString cs(pUnicode);
		delete[]pUnicode;
		pUnicode = NULL;

		strResponse = cs;

		Clear();
	}
	catch (CInternetException* e)
	{
		Clear();
		DWORD dwErrorCode = e->m_dwError;
		e->Delete();

		DWORD dwError = GetLastError();

		//PRINT_LOG("dwError = %d", dwError, 0);

		if (ERROR_INTERNET_TIMEOUT == dwErrorCode)
		{
			return OUTTIME;
		}
		else
		{
			return FAILURE;
		}
	}
	return SUCCESS;
}

int CHttpClient::HttpGet(LPCTSTR strUrl, const char* strPostData, wstring &strResponse)
{
	return ExecuteRequest(L"GET", strUrl, strPostData, strResponse);
}

int CHttpClient::HttpPost(LPCTSTR strUrl, const char* strPostData, wstring &strResponse)
{
	return ExecuteRequest(L"POST", strUrl, strPostData, strResponse);
}

BOOL CHttpClient::DownloadFile(LPCTSTR strFileURLInServer, LPCTSTR strFileLocalFullPath, LPTSTR errorMsgBuffer/* = NULL*/)
{
	ASSERT(strFileURLInServer != NULL);
	ASSERT(strFileLocalFullPath != NULL);

	BOOL bResult = FALSE;

	CInternetSession session(IE_AGENT);
	CHttpConnection* pHttpConnection = NULL;
	CHttpFile* pHttpFile = NULL;
	CString strServer, strObject;
	INTERNET_PORT wPort;

	DWORD dwType;
	const int nTimeOut = 2000;
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeOut); //重试之间的等待延时
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1); //重试次数
	char* pszBuffer = NULL;

	try
	{
		AfxParseURL(strFileURLInServer, dwType, strServer, strObject, wPort);
		pHttpConnection = session.GetHttpConnection(strServer, wPort);
		
		pHttpFile = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject);
		if (pHttpFile->SendRequest() == FALSE)
		{
			session.Close();
			return FALSE;
		}

		DWORD dwStateCode;

		pHttpFile->QueryInfoStatusCode(dwStateCode);
		if (dwStateCode == HTTP_STATUS_OK)
		{
			HANDLE hFile = CreateFile(strFileLocalFullPath, GENERIC_WRITE,
				FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
				NULL); //创建本地文件

			if (hFile != INVALID_HANDLE_VALUE)
			{
				char szInfoBuffer[1000]; //返回消息
				DWORD dwFileSize = 0; //文件长度
				DWORD dwInfoBufferSize = sizeof(szInfoBuffer);
				BOOL bResult = FALSE;
				bResult = pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH,
					(void*)szInfoBuffer, &dwInfoBufferSize, NULL);

				dwFileSize = atoi(szInfoBuffer);
				const int BUFFER_LENGTH = 1024 * 10;
				pszBuffer = new char[BUFFER_LENGTH]; //读取文件的缓冲
				DWORD dwWrite, dwTotalWrite;
				dwWrite = dwTotalWrite = 0;
				UINT nRead = pHttpFile->Read(pszBuffer, BUFFER_LENGTH); //读取服务器上数据

				while (nRead > 0)
				{
					WriteFile(hFile, pszBuffer, nRead, &dwWrite, NULL); //写到本地文件
					dwTotalWrite += dwWrite;
					nRead = pHttpFile->Read(pszBuffer, BUFFER_LENGTH);
				}

				delete[]pszBuffer;
				pszBuffer = NULL;
				CloseHandle(hFile);

				bResult = TRUE;
			}
		}
	}
	catch (CInternetException * pEx)
	{
		if (errorMsgBuffer != NULL)
			pEx->GetErrorMessage(errorMsgBuffer, 128);
	}
	catch (CFileException& fe)
	{
		if (errorMsgBuffer != NULL)
		{
			_stprintf_s(errorMsgBuffer, 128, _T("FileException occur! %d"), fe.m_lOsError);
		}
	}
	catch (...)
	{
		if (errorMsgBuffer != NULL)
		{
			DWORD dwError = GetLastError();
			_stprintf_s(errorMsgBuffer, 128, _T("Unknow Exception occur!\r\n%d"), dwError);
		}
	}
	delete[]pszBuffer;
	pszBuffer = NULL;
	if (pHttpFile != NULL)
	{
		pHttpFile->Close();
		delete pHttpFile;
		pHttpFile = NULL;
	}
	if (pHttpConnection != NULL)
	{
		pHttpConnection->Close();
		delete pHttpConnection;
		pHttpConnection = NULL;
	}
	session.Close();
	return bResult;
}

//2、上传文件
BOOL CHttpClient::UploadFile(LPCTSTR strURL, LPCTSTR strLocalFileName, LPTSTR errorMsgBuffer)
{
	ASSERT(strURL != NULL && strLocalFileName != NULL);

	BOOL bResult = FALSE;
	DWORD dwType = 0;
	CString strServer;
	CString strObject;
	INTERNET_PORT wPort = 0;
	DWORD dwFileLength = 0;
	char * pFileBuff = NULL;

	CHttpConnection * pHC = NULL;
	CHttpFile * pHF = NULL;
	CInternetSession cis(IE_AGENT);

	bResult = AfxParseURL(strURL, dwType, strServer, strObject, wPort);
	if (!bResult)
		return FALSE;
	CFile file;
	try
	{
		if (!file.Open(strLocalFileName, CFile::shareDenyNone | CFile::modeRead))
			return FALSE;

		dwFileLength = file.GetLength();
		if (dwFileLength <= 0)
			return FALSE;

		pFileBuff = new char[dwFileLength];
		memset(pFileBuff, 0, sizeof(char) * dwFileLength);
		file.Read(pFileBuff, dwFileLength);

		const int nTimeOut = 5000;
		cis.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeOut); //联接超时设置
		cis.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1); //重试1次
		pHC = cis.GetHttpConnection(strServer, wPort); //取得一个Http联接

		pHF = pHC->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject);
		if (!pHF->SendRequest(NULL, 0, pFileBuff, dwFileLength))
		{
			delete[]pFileBuff;
			pFileBuff = NULL;
			pHF->Close();
			pHC->Close();
			cis.Close();
			return FALSE;
		}
		DWORD dwStateCode = 0;
		pHF->QueryInfoStatusCode(dwStateCode);

		if (dwStateCode == HTTP_STATUS_OK)
			bResult = TRUE;
	}

	catch (CInternetException * pEx)
	{
		if (errorMsgBuffer != NULL)
			pEx->GetErrorMessage(errorMsgBuffer, 128);
	}
	catch (CFileException& fe)
	{
		if (errorMsgBuffer != NULL)
		{
			_stprintf_s(errorMsgBuffer, 128, _T("FileException occur! %d"), fe.m_lOsError);
		}
	}
	catch (...)
	{
		if (errorMsgBuffer != NULL)
		{
			DWORD dwError = GetLastError();
			_stprintf_s(errorMsgBuffer, 128, _T("Unknow Exception occur!\r\n%d"), dwError);
		}
	}

	delete[]pFileBuff;
	pFileBuff = NULL;
	
	file.Close();
	pHF->Close();
	pHC->Close();
	cis.Close();

	return bResult;
}