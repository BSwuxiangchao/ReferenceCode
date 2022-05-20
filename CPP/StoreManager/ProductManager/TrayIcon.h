#pragma once

class ITtrayIconDelegate
{
public:
	virtual void LeftClick(){}
	virtual void LeftDoubleClick() {}
	virtual void RightClick(){}
	virtual void RightDoubleClick(){}
};

class TrayIcon
{
public:
	SINGLETON_DEFINE(TrayIcon);

private:
	TrayIcon();
	~TrayIcon();

public:
	void Init(ITtrayIconDelegate* trayIconDelegate);
	void Destroy();

	void SetTrayIcon(HICON icon, const std::wstring& trayIconText);

	void LoadIconList(int iconResStartIndex, int count);

	void SetAnimateTray(const std::vector<int>& aniIndex, int animEscapeTime);

	void StartTrayIconAnimate();

	void StopTrayIconAnimate();

	UINT GetTaskbarCreatedMessageID() { return m_nTrayIconMessageID; }

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void ClearAnimateInfo();

	void OnTimer(UINT eventID);

	void SetTrayIconIndex(int index);

	BOOL RestoreTrayIcon();

	void ModifyTrayIcon(UINT uTrayID, DWORD dwTrayMsg, HICON hTrayIcon, const std::wstring& strTrayText);

private:
	HWND m_hWnd;
	HICON m_icon;
	std::wstring m_trayIconText;
	UINT m_nTrayIconMessageID;

	std::map<int, HICON> m_indexIconMap;
	std::vector<int> m_animIndexArray;
	int m_nAnimEscapeTime;
	int m_nAnimCurrentIndex;

	ITtrayIconDelegate *m_pTrayIconDelegate;
};

