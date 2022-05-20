#pragma once

class UserConfig
{
public:
	SINGLETON_DEFINE(UserConfig);

private:
	UserConfig();
	virtual ~UserConfig();
public:
	UINT GetDefaultIcon();
	void SetDefaultIcon(UINT iconId);

private:
	UINT m_iconId;
};

