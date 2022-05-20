#pragma once

class LogListBox : public ui::ListBox
{
public:
	void AttachMouseRightButtonUp(const ui::EventCallback& callback) { OnEvent[ui::kEventMouseRightButtonUp] += callback; }
};


class LogForm : public ui::WindowImplBase
{
public:
	SINGLETON_DEFINE(LogForm);

private:
	LogForm();
	virtual ~LogForm();

public:
	/**
	* 一下三个接口是必须要覆写的接口，父类会调用这三个接口来构建窗口
	* GetSkinFolder		接口设置你要绘制的窗口皮肤资源路径
	* GetSkinFile			接口设置你要绘制的窗口的 xml 描述文件
	* GetWindowClassName	接口设置窗口唯一的类名称
	*/
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;

	/**
	* 收到 WM_CREATE 消息时该函数会被调用，通常做一些控件初始化的操作
	*/
	virtual void InitWindow() override;

	/**
	* 拦截并处理底层窗体消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @return LRESULT 处理结果
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
public:
	void AddLogToListBox(const std::wstring& log);

private:
	bool OnBtnClick(ui::EventArgs* args);
	bool ClearLog(ui::EventArgs* args);
	bool PopupClearMenu(ui::EventArgs* args);
	void PopupClearMenu(POINT pt);
public:
	static const std::wstring kClassName;

private:
	LogListBox* m_pLogList;
};

