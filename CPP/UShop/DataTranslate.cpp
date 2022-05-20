#pragma once
#include "stdafx.h"
#include "PlcFace.h"
#include "PlcDB.h"
#include"sLog.h"
#include "PlcFaceDlg.h"
#include "../Utility/ArduinoJson.h"

const int PACK_SIZE = 4096 * 20;

bool CPlcFaceDlg::DealStoreListFormHttp(const std::string strResponse)
{
	//std::string objUTF8 = request.get_param_value("obj");

	std::string objUTF8 = strResponse;
	if (objUTF8.length() == 0)
		return false;
	char tBuf[1024] = { 0 };
	/*int leng = objUTF8.length();
	const int buffer_size = goodsListUTF8.length() + 128;*/

	std::wstring objUTF16 = nbase::UTF8ToUTF16(objUTF8);
	std::string objMBCS = nbase::win32::UnicodeToMBCS(objUTF16);
	StaticJsonDocument<PACK_SIZE> doc;
	deserializeJson(doc, objMBCS);
	if (!doc["success"].as<bool>() || doc["msg"].as<std::string>() != "操作成功")
		return false;
	//m_storeName = doc["obj"]["storeName"].as<std::string>();
	//m_stoteNo = doc["obj"]["storeNo"].as<std::string>();

	//解析格子信息
	std::vector<CellAttribu> cellList;
	JsonArray arrayCell = doc["obj"]["cellList"].as<JsonArray>();
	sprintf_s(tBuf, "从后台获取到 %d 种格子", arrayCell.size());
	NotifyMsg(tBuf);
	SZL_INFO("从后台获取到 {0} 种格子", arrayCell.size());
	int icccc = arrayCell.size();
	for (size_t i = 0; i < arrayCell.size(); i++)
	{
		CellAttribu cell;
		cell.gridNo = arrayCell[i]["gridNo"].as<std::string>();
		cell.containerType = arrayCell[i]["boxType"].as<std::string>();
		cell.cellSize.Lenth = arrayCell[i]["gridLong"].as<float>();
		cell.cellSize.Width = arrayCell[i]["gridWidth"].as<float>();
		cell.cellSize.Higth = arrayCell[i]["gridHigh"].as<float>();
		cellList.push_back(cell);
	}
	//更新格子信息到本地数据库
	UShop::PLCDB::Instance().UpdateCellInfo(cellList);

	//解析货箱信息
	std::vector<ContainerInfoPLC> boxList;
	JsonArray arrayBox = doc["obj"]["boxList"].as<JsonArray>();
	SZL_INFO("从后台获取到 {0} 个货箱", arrayBox.size());
	for (size_t i = 0; i < arrayBox.size(); i++)
	{
		ContainerInfoPLC tempBox;
		tempBox.containerID = arrayBox[i]["boxNo"].as<std::string>();
		tempBox.containerType = arrayBox[i]["boxType"].as<std::string>();
		JsonArray arrayCell = arrayBox[i]["gridList"].as<JsonArray>();
		if (arrayBox[i]["boxType"].as<std::string>() == "3x3")
		{
			/*if (arrayCell.size() != 9)
			{
				SZL_ERROR("从后台获取到的货箱数据不正确");
				return false;
			}*/
		}
		else
		{
			/*if (arrayCell.size() != 6)
			{
				SZL_ERROR("从后台获取到的货箱数据不正确");
				return false;
			}*/
		}
		SZL_INFO("货箱{0}有{1}个格子", tempBox.containerID,arrayCell.size());
		for (size_t j = 0; j < arrayCell.size(); j++)
		{
			CellAttribu cell;
			cell.gridNo = arrayCell[j]["gridNo"].as<std::string>();
			tempBox.vecCellType.push_back(cell);
		}
		boxList.push_back(tempBox);
	}

	//更新货箱信息到数据库
	UShop::PLCDB::Instance().UpdateContainerInfo(boxList);
	return true;
}

bool stringToShipSize(const std::string strSize, ShipSize &descShipSize)
{
	std::vector<std::string> vecSize = UShop::PLCDB::Instance().splitStr(strSize, "*");
	if (vecSize.size() != 3)
		return false;
	descShipSize.Lenth = atof(vecSize[0].c_str());
	descShipSize.Width = atof(vecSize[1].c_str());
	descShipSize.Higth = atof(vecSize[2].c_str());
	return true;
}

bool CPlcFaceDlg::DealProductListFormHttp(const std::string strResponse)
{
	if (strResponse.length() == 0)
		return false;
	/*int leng = objUTF8.length();
	const int buffer_size = goodsListUTF8.length() + 128;*/

	StaticJsonDocument<PACK_SIZE> doc;
	char tBuf[1024] = { 0 };
	std::wstring objUTF16 = nbase::UTF8ToUTF16(strResponse);
	std::string objMBCS = nbase::win32::UnicodeToMBCS(objUTF16);
	deserializeJson(doc, objMBCS);

	if (!doc["success"].as<bool>() || doc["msg"].as<std::string>() != "操作成功")
		return false;
	
	std::vector<ProductInfoPLC> proList;
	JsonArray array = doc["obj"].as<JsonArray>();
	for (size_t i = 0; i < array.size(); i++)
	{
		ProductInfoPLC pro;
		pro.id = array[i]["goodsNo"].as<int>();
		pro.keywords = array[i]["keyWords"].as<std::string>();
		pro.barcode = array[i]["goodsBarCodes"].as<std::string>();
		pro.unit = array[i]["goodsUnit"].as<std::string>();
		pro.name = array[i]["goodsName"].as<std::string>();
		pro.brand = array[i]["brand"].as<std::string>();
		pro.mainCategory = array[i]["mainclass"].as<std::string>();
		pro.subCategory = array[i]["subclass"].as<std::string>();
		pro.specification = array[i]["specification"].as<std::string>();
		pro.description = array[i]["description"].as<std::string>();
		pro.image = array[i]["image"].as<std::string>();
		pro.price = array[i]["price"].as<float>();

		//解析尺寸
		pro.length = array[i]["goodsLong"].as<float>();
		pro.width = array[i]["goodsWidth"].as<float>();
		pro.higth = array[i]["goodsHigh"].as<float>();
		pro.cellType = array[i]["cellType"].as<std::string>();
		//pro.validityDate = array[i]["validityDate"].as<std::string>();
		proList.push_back(pro);
	}
	//更新到本地数据库
	UShop::PLCDB::Instance().UpdateProductInfo(proList);
	return true;
}

// 上传库存
std::string CPlcFaceDlg::SetmodifyReserveNumtoHttp(const std::vector<CellInfoPLC> list,int modeFlag)
{
	// 向AI端返回处理结果
	//buffer里面存储的内容采用多字节编码
	/*const int buffer_size = list.size()*128;*/
	if (modeFlag != 0 && modeFlag != 1) //0 上货，1 出货
	{
		SZL_ERROR("后台的库存数据上传方式有误");
		return "";
	}
	char buffer[PACK_SIZE] = { 0 };
	StaticJsonDocument<PACK_SIZE> doc;

	doc["storeNo"] = m_configInfo._storeNo;
	doc["modeFlag"] = modeFlag;
	JsonArray statusInfoArray = doc.createNestedArray("orderList");
	for (auto& item : list)
	{
		JsonObject cellJS = statusInfoArray.createNestedObject();
		cellJS["boxNo"] = item.containerID; //商品所在货箱号
		if (item.containerType=="3x3")
		{
			cellJS["gridNo"] = item.cellNum; // 商品所在格子号
		}
		else
		{
			cellJS["gridNo"] = item.cellNum+9; // 商品所在格子号
		}
		cellJS["goodsNo"] = item.productID; //商品编号
		if (modeFlag == 0)
		{
			cellJS["reserve"] = item.cellStock+item.cellLoadQ;//商品库存
		}
		else
		{
			int iReserve = item.cellStock - item.outputGoodsNum;
			if (iReserve == 0)
				cellJS["goodsNo"] = 0; //商品编号
			else if (iReserve < 0)
				return "";
			cellJS["reserve"] = iReserve;//商品库存
		}
	}

	serializeJson(doc, buffer, PACK_SIZE);

	// 需要把多字节编码的buffer转为UTF-8编码
	//std::string bufferUTF8 = nbase::UTF16ToUTF8(nbase::win32::MBCSToUnicode(buffer));

	//response.set_content(bufferUTF8, "application/json");
	return buffer;
}

// 保留，下载库存信息的接口解析
bool CPlcFaceDlg::DealModifyReserveNumFormHttp(const std::string strResponse, std::vector<CellInfoPLC>& list)
{
	std::string goodsListUTF8 = strResponse;
	if (goodsListUTF8.length() == 0)
		return false;

	/*const int buffer_size = goodsListUTF8.length()+128;*/
	std::wstring goodListUTF16 = nbase::UTF8ToUTF16(goodsListUTF8);
	std::string newProductMBCS = nbase::win32::UnicodeToMBCS(goodListUTF16);
	StaticJsonDocument<PACK_SIZE> doc;
	deserializeJson(doc, newProductMBCS);
	JsonArray array = doc["pList"].as<JsonArray>();
	for (size_t i = 0; i < array.size(); i++)
	{
		CellInfoPLC info;
		info.containerID = array[i]["_boxID"].as<std::string>(); // id
		info.cellNum = array[i]["_cellID"].as<unsigned int>(); // cellid
		info.barcode = array[i]["_barcode"].as<std::string>(); // _barcode
		info.cellStock = array[i]["_cellStock"].as<unsigned int>(); // _cellStock
		list.push_back(info);
	}
	return true;
}

bool CPlcFaceDlg::GetExhibitGoodsFormHttp(const std::string strResponse)
{
	if (strResponse.length() == 0)
		return false;
	/*const int buffer_size = goodsListUTF8.length() + 128;*/

	StaticJsonDocument<PACK_SIZE> doc;
	std::wstring goodListUTF16 = nbase::UTF8ToUTF16(strResponse);
	std::string newProductMBCS = nbase::win32::UnicodeToMBCS(goodListUTF16);
	deserializeJson(doc, newProductMBCS);

	std::vector<ProductInfoPLC> proList;
	JsonArray array = doc["obj"].as<JsonArray>();
	for (size_t i = 0; i < array.size(); i++)
	{
		ProductInfoPLC pro;
		pro.name = array[i]["goodsName"].as<std::string>();
		pro.mainCategory = array[i]["typeName"].as<std::string>();
		pro.sort = array[i]["sort"].as<int>();
		pro.id = array[i]["goodsNo"].as<int>();
		pro.barcode = array[i]["goodsBarCodes"].as<std::string>();
		pro.unit = array[i]["goodsUnit"].as<std::string>();
		pro.keywords = array[i]["keyWords"].as<std::string>();

		//stringToShipSize(array[i]["goodsSize"].as<std::string>(), pro.goodsSize);//解析尺寸

		//pro.validityDate = jo["validityDate"].as<std::string>();
		proList.push_back(pro);
	}
	//更新到本地数据库??

	return true;
}


bool CPlcFaceDlg::GetAddGoodsFormHttp(const std::string strResponse)
{
	m_APP_LoadInfoList.clear();
	std::string objUTF8 = strResponse;
	if (objUTF8.length() == 0)
		return false;
	/*const int buffer_size = goodsListUTF8.length() + 128;*/

	std::wstring objUTF16 = nbase::UTF8ToUTF16(objUTF8);
	std::string objMBCS = nbase::win32::UnicodeToMBCS(objUTF16);
	StaticJsonDocument<PACK_SIZE> doc;
	deserializeJson(doc, objMBCS);
	JsonArray array = doc["obj"].as<JsonArray>();
	for (size_t i = 0; i < array.size(); i++)
	{
		OnloadProductInfo loadInfo;
		loadInfo.productName = array[i]["goodsName"].as<std::string>();
		loadInfo.productID = array[i]["goodsNo"].as<int>();
		loadInfo.barcode = array[i]["goodsBarCodes"].as<std::string>();
		loadInfo.loadQuantity = array[i]["GoodCounts"].as<int>();
		m_APP_LoadInfoList.push_back(loadInfo);
	}

	return true;
}