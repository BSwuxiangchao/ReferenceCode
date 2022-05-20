#include "stdafx.h"
#include <shellapi.h>
#include "TrayIcon.h"

const DWORD TrayIconAnimateID = 0x0001;

#define TRAYICON_ID 1
#define WM_TRAYICON_NOTIFY (WM_USER + 1)

static bool bInit = false;

TrayIcon::TrayIcon()
	:m_hWnd(nullptr)
	,m_animIndexArray()
	,m_indexIconMap()
	,m_nAnimEscapeTime(0)
	,m_nAnimCurrentIndex(0)
	,m_pTrayIconDelegate(nullptr)
	,m_icon(nullptr)
	,m_trayIconText(L"")
	,m_nTrayIconMessageID(0)
{
}


TrayIcon::~TrayIcon()
{
}

void TrayIcon::Init(ITtrayIconDelegate* trayIconDelegate)
{
	m_pTrayIconDelegate = trayIconDelegate;

	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = GetModuleHandleW(NULL);
	wndclass.hIcon = NULL;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = L"TrayIconClassName";
	::RegisterClass(&wndclass);

	m_hWnd = CreateWindowExW(NULL, L"TrayIconClassName", L"TrayIconWindowName", WS_POPUP | WS_CHILD,
		0, 0, 0, 0, NULL, NULL, GetModuleHandleW(NULL), NULL);

	//注册explorer崩溃后重新刷新trayicon
	m_nTrayIconMessageID = ::RegisterWindowMessage(L"TaskbarCreated");

	assert(m_hWnd);
}

void TrayIcon::Destroy()
{
	NOTIFYICONDATAW iconData = { 0 };
	iconData.cbSize = sizeof(NOTIFYICONDATA);
	iconData.uFlags = 0;
	iconData.hWnd = m_hWnd;
	iconData.uID = TRAYICON_ID;

	::Shell_NotifyIcon(NIM_DELETE, &iconData);

}

void TrayIcon::SetTrayIcon(HICON icon, const std::wstring& trayIconText)
{
	assert(icon);
	m_icon = icon;
	m_trayIconText = trayIconText;

	ModifyTrayIcon(TRAYICON_ID, WM_TRAYICON_NOTIFY, icon, trayIconText);
}

void TrayIcon::LoadIconList(int iconResStartIndex, int count)
{
	for (int i = 0; i < count; ++i)
	{
		HICON icon = (HICON)::LoadImage(nullptr,
			MAKEINTRESOURCE(iconResStartIndex + i),
			IMAGE_ICON,
			::GetSystemMetrics(SM_CXSMICON),
			::GetSystemMetrics(SM_CYSMICON),
			LR_DEFAULTCOLOR | LR_SHARED
		);

		assert(icon);

		m_indexIconMap[iconResStartIndex + i] = icon;
	}
}

void TrayIcon::SetAnimateTray(const std::vector<int>& aniIndex, int animEscapeTime)
{
	ClearAnimateInfo();

	m_animIndexArray = aniIndex;

	m_nAnimEscapeTime = animEscapeTime;
}

void TrayIcon::StartTrayIconAnimate()
{
	assert(m_indexIconMap.size());

	m_nAnimCurrentIndex = 0;

	KillTimer(m_hWnd, TrayIconAnimateID);

	SetTimer(m_hWnd, TrayIconAnimateID, m_nAnimEscapeTime, nullptr);
}

void TrayIcon::StopTrayIconAnimate()
{
	KillTimer(m_hWnd, TrayIconAnimateID);
}

LRESULT CALLBACK TrayIcon::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 1;
	switch (WM_TRAYICON_NOTIFY)
	{
	case WM_TRAYICON_NOTIFY:
	{
		if (TRAYICON_ID != wParam)
			break;

		if (WM_LBUTTONUP == lParam)
		{
			TrayIcon::GetInstance()->m_pTrayIconDelegate->LeftClick();
		}
		else if (WM_LBUTTONDBLCLK == lParam)
		{
			TrayIcon::GetInstance()->m_pTrayIconDelegate->LeftDoubleClick();
		}
		else if (WM_RBUTTONUP == lParam)
		{
			TrayIcon::GetInstance()->m_pTrayIconDelegate->RightClick();
		}
		else if (WM_RBUTTONDBLCLK == lParam)
		{
			TrayIcon::GetInstance()->m_pTrayIconDelegate->RightDoubleClick();
		}

		break;
	}
	case WM_TIMER:
	{
		TrayIcon::GetInstance()->OnTimer((UINT)wParam);
		break;
	}

	default:
	{
		if (uMsg != 0 && uMsg == TrayIcon::GetInstance()->GetTaskbarCreatedMessageID())
		{
			TrayIcon::GetInstance()->RestoreTrayIcon();

		}
		result = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}
		
	}

	return result;
}

void TrayIcon::ClearAnimateInfo()
{

}

void TrayIcon::OnTimer(UINT eventID)
{
	switch (eventID)
	{
	case TrayIconAnimateID:
	{
		if (0 != m_animIndexArray.size())
		{
			m_nAnimCurrentIndex %= m_animIndexArray.size();
			SetTrayIconIndex(m_animIndexArray[m_nAnimCurrentIndex++]);
		}
		else
		{
			StopTrayIconAnimate();
		}
		break;
	}
	default:
	{
		assert(FALSE);
		break;
	}
		
	}
}

void TrayIcon::SetTrayIconIndex(int index)
{
	auto hIcon = m_indexIconMap[index];
	assert(hIcon);

	SetTrayIcon(hIcon, L"");
	
}

BOOL TrayIcon::RestoreTrayIcon()
{
	if (!IsWindow(m_hWnd))
		return FALSE;

	bInit = false;

	SetTrayIcon(m_icon, m_trayIconText);

	return TRUE;
}

void TrayIcon::ModifyTrayIcon(UINT uTrayID, DWORD dwTrayMsg, HICON hTrayIcon, const std::wstring& strTrayText)
{
	NOTIFYICONDATAW iconData = { 0 };
	iconData.cbSize = sizeof(NOTIFYICONDATA);
	iconData.hWnd = m_hWnd;
	iconData.uID = uTrayID;
	iconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	iconData.uCallbackMessage = dwTrayMsg;
	iconData.hIcon = hTrayIcon;

#ifdef NDEBUG
	wcscpy_s(iconData.szTip, 128, strTrayText.c_str());
#else
	std::wstring debugText = strTrayText + L"[DEBUG]";
	wcscpy_s(iconData.szTip, 128, debugText.c_str());
#endif // NDEBUG

	if (!bInit)
	{
		bInit = true;

		BOOL result = ::Shell_NotifyIconW(NIM_ADD, &iconData);
		if (!result)
		{
			result = ::Shell_NotifyIconW(NIM_ADD, &iconData);
		}
	}
	else
	{
		BOOL result = ::Shell_NotifyIcon(NIM_MODIFY, &iconData);
		assert(result);
	}
}
