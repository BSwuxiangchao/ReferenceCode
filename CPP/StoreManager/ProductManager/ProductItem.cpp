#include "stdafx.h"
#include "ProductHttpService.h"
#include "ProductForm.h"
#include "ProductItem.h"

ProductItem::ProductItem()
	: m_nCurrentProductNumber(1)
{
}

ProductItem::~ProductItem()
{
}

void ProductItem::InitSubControls(const std::wstring& img, const std::wstring& title, unsigned int productID, unsigned int productNumber)
{
	// 查找 Item 下的控件
	m_pProductImg = dynamic_cast<ui::Control*>(FindSubControl(L"product_img"));
	m_pProductInfo = dynamic_cast<ui::Label*>(FindSubControl(L"label_product_info"));
	m_pProductPrice = dynamic_cast<ui::Label*>(FindSubControl(L"label_product_price"));
	m_pProductNum = dynamic_cast<ui::Label*>(FindSubControl(L"label_product_num"));

	m_pIncreaseProduct = dynamic_cast<ui::Button*>(FindSubControl(L"btn_add"));
	m_pReduceProduct = dynamic_cast<ui::Button*>(FindSubControl(L"btn_minus"));
	m_pDeleteProduct = dynamic_cast<ui::Button*>(FindSubControl(L"btn_del"));

	m_pProductImg->SetBkImage(img);
	m_pProductInfo->SetText(title);

	m_itemID = productID;

	m_nCurrentProductNumber = productNumber;
	SetNumberText();

	auto id = nbase::UintToString16(productID);
	m_pIncreaseProduct->SetDataID(id);
	m_pReduceProduct->SetDataID(id);
	m_pDeleteProduct->SetDataID(id);

	m_ItemTitle = title;

	m_pIncreaseProduct->AttachClick(nbase::Bind(&ProductItem::IncreaseButtonClicked, this, std::placeholders::_1));
	m_pReduceProduct->AttachClick(nbase::Bind(&ProductItem::ReduceButtonClicked, this, std::placeholders::_1));
	m_pDeleteProduct->AttachClick(nbase::Bind(&ProductItem::OnBtnClick, this, std::placeholders::_1));

}

void ProductItem::AttachDeleteButtonClick(const ui::EventCallback& callback)
{
	if (m_pDeleteProduct)
		m_pDeleteProduct->AttachClick(callback);
}

void ProductItem::AttachIncreaseButtonClick(const ui::EventCallback& callback)
{
	if (m_pIncreaseProduct)
		m_pIncreaseProduct->AttachClick(callback);
}

void ProductItem::AttachReduceButtonClick(const ui::EventCallback& callback)
{
	if (m_pReduceProduct)
		m_pReduceProduct->AttachClick(callback);
}

void ProductItem::SetPriceText(float price)
{
	if (m_pProductPrice)
	{
		m_pProductPrice->SetText(nbase::StringPrintf(TEXT("￥%g"), price));
	}
}

bool ProductItem::IncreaseButtonClicked(ui::EventArgs* args)
{
	IncreaseProductNumber(1);

	return true;
}

void ProductItem::IncreaseProductNumber(int number)
{
	auto info = ProductForm::GetInstance()->GetProductInfoByIdInDatabse(m_itemID);
	if (info->amount > 0)
		m_nCurrentProductNumber += number;

	SetNumberText();
}

void ProductItem::SetNumberText()
{
	auto numText = nbase::UintToString16(m_nCurrentProductNumber);
	m_pProductNum->SetText(numText);
}

bool ProductItem::ReduceButtonClicked(ui::EventArgs* args)
{
	ReduceProductNumber(1);

	return true;
}

void ProductItem::ReduceProductNumber(int number)
{
	/*if (m_nCurrentProductNumber - number <= 0)
	{
		ui::ListBox* parent = dynamic_cast<ui::ListBox*>(this->GetParent());
		parent->Remove(this);

		return;
	}*/

	m_nCurrentProductNumber -= number;
	
	SetNumberText();
} 

void ProductItem::SetProductNumber(int number)
{
	if (number > 0)
	{
		m_nCurrentProductNumber = number;
		SetNumberText();
	}
	/*else
	{
		ui::ListBox* parent = dynamic_cast<ui::ListBox*>(this->GetParent());
		parent->Remove(this);
	}*/
}

bool ProductItem::OnBtnClick(ui::EventArgs* args)
{
	auto name = args->pSender->GetName(); 
	if (name == L"btn_del")
	{
		ui::ListBox* parent = dynamic_cast<ui::ListBox*>(this->GetParent());
		return parent->Remove(this);
	}
	
	return true;
}

