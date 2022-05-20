#include "stdafx.h"
#include "LogItem.h"
#include "LogForm.h"

const std::wstring LogForm::kClassName = L"LogForm";


LogForm::LogForm()
{
}

LogForm::~LogForm()
{
}

std::wstring LogForm::GetSkinFolder()
{
	return L"skin";
}

std::wstring LogForm::GetSkinFile()
{
	return L"log_form.xml";
}

std::wstring LogForm::GetWindowClassName() const
{
	return kClassName;
}

void LogForm::InitWindow()
{
	//m_pLogList = dynamic_cast<LogListBox *>(FindControl(L"loglist"));
	m_pLogList = (LogListBox *)(FindControl(L"loglist"));
	if (m_pLogList)
	{
		m_pLogList->AttachMouseRightButtonUp(nbase::Bind(&LogForm::OnBtnClick, this, std::placeholders::_1));
	}

	ui::Button* pHideBtn = dynamic_cast<ui::Button*>(FindControl(L"hidebtn"));
	if (pHideBtn)
	{
		pHideBtn->AttachClick(nbase::Bind(&LogForm::OnBtnClick, this, std::placeholders::_1));
	}
}

LRESULT LogForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)
	{
		this->ShowWindow(false, false);
		return 0;
	}
	else if (uMsg == WM_RBUTTONUP)
	{
		int xPos = LOWORD(lParam);
		int yPos = HIWORD(lParam);

		POINT pt;
		pt.x = xPos;
		pt.y = yPos;
		ClientToScreen(m_hWnd, &pt);

		PopupClearMenu(pt);
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT LogForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ShowWindow(false, false);
	return 0;
}

void LogForm::AddLogToListBox(const std::wstring& log)
{
	LogItem *item = new LogItem();
	ui::GlobalManager::FillBoxWithCache(item, L"skin/log_item.xml");
	item->InitSubControlWithLogText(log);

	m_pLogList->Add(item);
	m_pLogList->EndDown();
}

bool LogForm::OnBtnClick(ui::EventArgs* args)
{
	auto name = args->pSender->GetName();
	if (name == L"hidebtn")
	{
		ShowWindow(false, false);
	}

	return true;
}

bool LogForm::ClearLog(ui::EventArgs* args)
{
	if (m_pLogList)
	{
		m_pLogList->RemoveAll();
	}

	return true;
}

void LogForm::PopupClearMenu(POINT pt)
{
	ui::CMenuWnd* pMenu = new ui::CMenuWnd(nullptr);
	ui::STRINGorID xml(L"clear_log_menu.xml");

	pMenu->Init(xml, _T("xml"), pt);

	ui::CMenuElementUI *pClearLog =  dynamic_cast<ui::CMenuElementUI*>(pMenu->FindControl(L"clear_log"));
	if (pClearLog)
	{
		pClearLog->AttachSelect(nbase::Bind(&LogForm::ClearLog, this, std::placeholders::_1));
	}
}

bool LogForm::PopupClearMenu(ui::EventArgs* args)
{
	return true;
}

