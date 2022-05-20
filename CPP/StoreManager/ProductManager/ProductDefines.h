#pragma once

/*
* ��Ʒ��Ϣ
*/
struct ProductInfo
{
	unsigned int id; // ��ƷID
	std::string name; // ��Ʒ����
	std::string image; // ��ƷͼƬ
	std::string description; // ��Ʒ������
	std::string mainCategory; // ��Ʒ�����
	std::string subCategory; // ��Ʒ�����
	
	std::string barcode; // ��Ʒ������
	std::string keywords; // ����������Ʒ�Ĺؼ���
	std::string specification; // ��Ʒ���
	std::string unit; // ��Ʒ��λ
	float price; // ��Ʒ����
	int amount; // ��Ʒ�������
	int count; // ��Ʒ�ڹ��ﳵ�е�����

	ProductInfo()
		:id(0),
		image(""),
		name(""),
		description(""),
		mainCategory(""),
		subCategory(""),
		specification(""),
		unit("��"),
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