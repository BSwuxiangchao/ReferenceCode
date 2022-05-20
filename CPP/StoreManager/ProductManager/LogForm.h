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
	* һ�������ӿ��Ǳ���Ҫ��д�Ľӿڣ����������������ӿ�����������
	* GetSkinFolder		�ӿ�������Ҫ���ƵĴ���Ƥ����Դ·��
	* GetSkinFile			�ӿ�������Ҫ���ƵĴ��ڵ� xml �����ļ�
	* GetWindowClassName	�ӿ����ô���Ψһ��������
	*/
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;

	/**
	* �յ� WM_CREATE ��Ϣʱ�ú����ᱻ���ã�ͨ����һЩ�ؼ���ʼ���Ĳ���
	*/
	virtual void InitWindow() override;

	/**
	* ���ز�����ײ㴰����Ϣ
	* @param[in] uMsg ��Ϣ����
	* @param[in] wParam ���Ӳ���
	* @param[in] lParam ���Ӳ���
	* @return LRESULT ������
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

