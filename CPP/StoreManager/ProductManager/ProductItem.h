#pragma once


class ProductItem : public ui::ListContainerElement
{
public:
	ProductItem();
	~ProductItem();

	// 提供外部调用来初始化 item 数据
	void InitSubControls(const std::wstring& img, const std::wstring& title, unsigned int productID, unsigned int productNumber);

	void AttachDeleteButtonClick(const ui::EventCallback& callback);
	void AttachIncreaseButtonClick(const ui::EventCallback& callback);
	void AttachReduceButtonClick(const ui::EventCallback& callback);

	std::wstring GetTitle() { return m_ItemTitle; }

	unsigned int GetItemID() { return m_itemID; } // 等于productID

	void SetPriceText(float price);

	void IncreaseProductNumber(int number);

	void ReduceProductNumber(int number);

	void SetProductNumber(int number);

private:
	bool OnBtnClick(ui::EventArgs* args);

	bool IncreaseButtonClicked(ui::EventArgs* args);
	bool ReduceButtonClicked(ui::EventArgs* args);

	void SetNumberText();
private:
	ui::ListBox* m_pProductList = nullptr;

	ui::Control* m_pProductImg = nullptr;
	ui::Label* m_pProductInfo = nullptr;
	ui::Label* m_pProductPrice = nullptr;
	ui::Label* m_pProductNum = nullptr;
	ui::Button* m_pReduceProduct = nullptr;
	ui::Button* m_pIncreaseProduct = nullptr;
	ui::Button* m_pDeleteProduct = nullptr;

	std::wstring m_ItemTitle;
	int m_nCurrentProductNumber = 0;
	unsigned int m_itemID = 0;
};

