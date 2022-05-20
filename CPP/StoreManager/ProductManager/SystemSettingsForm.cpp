#include "stdafx.h"
#include "SystemSettingsForm.h"


const std::wstring SystemSettingsForm::kClassName = L"SystemSettingsForm";

SystemSettingsForm::SystemSettingsForm()
{
}


SystemSettingsForm::~SystemSettingsForm()
{
}

std::wstring SystemSettingsForm::GetSkinFolder()
{
	return L"skin";
}

std::wstring SystemSettingsForm::GetSkinFile()
{
	return L"system_settings_form.xml";
}

std::wstring SystemSettingsForm::GetWindowClassName() const
{
	return kClassName;
}

void SystemSettingsForm::InitWindow()
{

}

LRESULT SystemSettingsForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)
	{
		this->Close();
		return 0;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}


