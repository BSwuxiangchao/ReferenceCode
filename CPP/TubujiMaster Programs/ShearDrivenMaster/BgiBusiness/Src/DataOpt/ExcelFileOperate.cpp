#include "ExcelFileOperate.h"
#include <QAxObject>
#include <QDir>
#include "TransformCoding.h"

// 读文件
bool ExcelFileOperate::ReadExcelFile(const QString& pathName, const QStringList& Header, QVector<QStringList>& vecContent, const int& iType, QString& strReadInfo)
{
	QAxObject* excel = new QAxObject();

	if (2 == iType)
	{
		// wps
		excel->setControl("ket.Application");  //连接Excel控件(wps)
	}
	else
	{
		// office
		excel->setControl("Excel.Application");//连接Excel控件(office)  
	}
	if (NULL == excel)
	{
		strReadInfo = StringConversionEncoding("读取表格，连接Excel软件失败！");
		return false;
	}
	
	excel->dynamicCall("SetVisible (bool Visible)", "false");//不显示窗体  
	if (NULL == excel)
	{
		strReadInfo = StringConversionEncoding("读取表格，设置不显示窗体失败！");
		return false;
	}

	excel->setProperty("DisplayAlerts", false);//不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
	if (NULL == excel)
	{
		strReadInfo = StringConversionEncoding("读取表格，设置“文件已修改，是否保存”的提示失败！");
		return false;
	}

	QAxObject* workbooks = excel->querySubObject("WorkBooks");
	if (workbooks == NULL)
	{
		strReadInfo = StringConversionEncoding("读取表格，获取WorkBooks失败！");
		return false;
	}
		
	QAxObject* workbook = workbooks->querySubObject("Open (const QString &)", pathName);//获取当前工作簿
	if (workbook == NULL)
	{
		strReadInfo = StringConversionEncoding("读取表格，获取当前工作簿失败！");
		return false;
	}
	QAxObject* worksheet = workbook->querySubObject("WorkSheets(int)", 1);//工作表1，即sheet1 
	if (worksheet == NULL)
	{
		strReadInfo = StringConversionEncoding("读取表格，获取工作表1，即sheet1失败！");
		return false;
	}
	//获取表中的数据到QVariant中
	QAxObject* usedRange = worksheet->querySubObject("UsedRange");
	if (usedRange == NULL)
	{
		strReadInfo = StringConversionEncoding("读取表格，获取UsedRange失败！");
		return false;
	}
		
	QVariant var = usedRange->dynamicCall("value");
	QVariantList varRowContents = var.toList();
	const int rowCount = varRowContents.size();
	QVariantList row_list;
	int column = 0;

	if (rowCount < 2)
	{
		//未有任何一条有效数据
		strReadInfo = StringConversionEncoding("选择的表格无有效数据！");
		return false;
	}
	//非合成序列表
	row_list = varRowContents[0].toList();

	int i_col = row_list.size();
	if (i_col != Header.size())
	{
		strReadInfo = StringConversionEncoding("选择表格的表头列数和标准的表头列数不相等！");
		return false;
	}
	// 判断表头是否一致
	for (int i = 0; i < i_col; ++i)
	{
		if (row_list[i].toString() != Header.at(i))
		{
			strReadInfo = QString("%1,%2<->%3").arg(StringConversionEncoding("选择表格的表头和标准的表头不相同: ")).arg(row_list[i].toString()).arg(Header.at(i));
			return false;
		}
	}
	
	// 清空数组列表
	vecContent.clear();
	QStringList strList;
	//第0行为字段名称
	for (int i = 1; i < rowCount; ++i)
	{
		strList.clear();
		row_list = varRowContents[i].toList();
		column = row_list.size();
		for (size_t j = 0; j < column; j++)
		{
			strList.append(row_list.at(j).toString());
		}
		vecContent.push_back(strList);
	}

	//删除，防止程序关闭后有excel进程残留
	excel->dynamicCall("Quit(void)");
	delete excel;

	strReadInfo = StringConversionEncoding("读取成功");

	return true;
}
// 写文件
bool ExcelFileOperate::WriteExcelFile(const QString& pathName, const QStringList& Header, const QVector<QStringList>& vecContent, const int& iType = 1)
{
	QAxObject* excel = new QAxObject();
	if (2 == iType)
	{
		// wps
		excel->setControl("ket.Application");  //连接Excel控件(wps)
	}
	else
	{
		// office
		excel->setControl("Excel.Application");//连接Excel控件(office)  
	}
	if (NULL == excel)
	{
		return false;
	}

	excel->dynamicCall("SetVisible (bool Visible)", "false");//不显示窗体  
	if (NULL == excel)
	{
		return false;
	}

	excel->setProperty("DisplayAlerts", false);//不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
	if (NULL == excel)
	{
		return false;
	}

	QAxObject* workbooks = excel->querySubObject("WorkBooks");
	if (workbooks == NULL)
		return false;
	//workbooks->dynamicCall(booksName.toStdString().c_str());//新建一个工作簿
	workbooks->dynamicCall("Add");//新建一个工作簿
	QAxObject* workbook = excel->querySubObject("ActiveWorkBook");//获取当前工作簿
	if (NULL == workbook)
	{
		return false;
	}
	QAxObject* worksheets = workbook->querySubObject("Sheets");//获取工作表集合
	if (NULL == worksheets)
	{
		return false;
	}

	QAxObject* worksheet = worksheets->querySubObject("Item(int)", 1);//获取工作表集合的工作表1，即sheet1
	if (NULL == worksheet)
	{
		return false;
	}

	QAxObject* Range = NULL;

	for (size_t i = 0; i < Header.size(); i++)
	{
		Range = worksheet->querySubObject("Cells(int,int)", 1, i+1);
		if (NULL == Range)
		{
			return false;
		}
		Range->dynamicCall("SetValue(const QString &)", Header.at(i));
	}

	int index = 2;
	for (size_t i = 0; i < vecContent.size(); i++)
	{
		Range = worksheet->querySubObject("Cells(int,int)", index, 1);
		Range->dynamicCall("SetValue(const QString &)", index - 1);

		for (size_t j = 0; j < vecContent.at(i).size(); j++)
		{
			Range = worksheet->querySubObject("Cells(int,int)", index, j + 2);
			Range->dynamicCall("SetValue(const QString &)", vecContent.at(i).at(j));

		}
		index++;
	}

	//path = "D:\\GigaFab\\Build\\Output\\x64\\Debug\\ChipId11.xlsx";
	workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(pathName));
	workbook->dynamicCall("Close()");//关闭工作簿
	excel->dynamicCall("Quit()");//关闭excel
	delete excel;
	excel = NULL;


	return true;
}
