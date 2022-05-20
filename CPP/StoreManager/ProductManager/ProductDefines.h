#pragma once

/*
* 商品信息
*/
struct ProductInfo
{
	unsigned int id; // 商品ID
	std::string name; // 商品名称
	std::string image; // 商品图片
	std::string description; // 商品简单描述
	std::string mainCategory; // 商品主类别
	std::string subCategory; // 商品子类别
	
	std::string barcode; // 商品条形码
	std::string keywords; // 用于搜索商品的关键词
	std::string specification; // 商品规格
	std::string unit; // 商品单位
	float price; // 商品单价
	int amount; // 商品库存总量
	int count; // 商品在购物车中的数量

	ProductInfo()
		:id(0),
		image(""),
		name(""),
		description(""),
		mainCategory(""),
		subCategory(""),
		specification(""),
		unit("个"),
		price(.0f),
		amount(1),
		count(0),
		barcode("")
	{

	}

	ProductInfo(const ProductInfo& info)
	{
		id = info.id;
		name = info.name;
		image = info.image;
		description = info.description;
		mainCategory = info.mainCategory;
		subCategory = info.subCategory;
		price = info.price;
		amount = info.amount;
		count = info.count;
		barcode = info.barcode;
		keywords = info.keywords;
		specification = info.specification;
		unit = info.unit;
	}
};