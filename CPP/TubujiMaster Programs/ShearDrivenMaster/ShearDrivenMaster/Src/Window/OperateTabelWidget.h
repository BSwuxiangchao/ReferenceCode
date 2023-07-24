/*
* @简    述：操作QTabelWidget控件
* @详    细：提供对QTableWidget的初始化，插入，删除，查找，标记特殊显示等功能；
* @继    承：无
* @头 文 件：OperateTabelWidget.h
* @实现文件：OperateTabelWidget.cpp
* @上一版本：1.0
* @作    者：xiechengwang
* @创建时间：2022/3/10 10:04
* @当前版本：1.0
* @修改时间：无
*
*/

// 使用宏定义保护头文件不被重复包含
#pragma once
// 包含系统文件
#include <QString>
#include <QTableWidget>
#include <QVector>
#include <QHeaderView>

// 包含项目文件

// 包含局部头文件

// 放置前置引用

class OperateTabelWidget
{
public:
	// 这里放置类的生命期控制函数，例如构造，析构函数，拷贝构造函数
	OperateTabelWidget();
	~OperateTabelWidget();

	// 这里放置类的运算操作符函数，=，==等

	// 这里放置类的操作
	
/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：初始化表格控件，每一列的名称占位宽度为列名称宽度的占比；
 * 时    间：2021/11/10 16:03
 * 入    参：strList: 表格头名称
 * 入    参：vecColRatio：每列的占空比；
 * 入    参：iAlingment:对齐方式，1-左对齐，2-中间对齐，3-右对齐，其他值默认是左对齐
 * 出    参：pTable：表格控件
 * 返    回：无
 ************************************* end-XCW *****************************************/
	void InitTableWidget(QTableWidget* pTable, const QStringList& strListHeader, const unsigned char iAlingment);
	void InitTableWidget(QTableWidget* pTable, const QStringList& strListHeader, const QVector<unsigned short>& vecColRatio, const unsigned char iAlingment);
	// 初始化表格控件，没有测试信息
	void InitTableWidgetNoTest(QTableWidget* pTable, const QStringList& strListHeader, const unsigned char iAlingment);
	//选择一行
	void InitTableWidgetOneSelect(QTableWidget* pTable, const QStringList& strListHeader, const unsigned char iAlingment);
	// 更新表格控件
	void UpdateTabelWidget(QTableWidget* pTableWidget, const QVector<QStringList>& vecStrList);
	// 删除所有行
	void RemoveAllRow(QTableWidget* pTableWidget);

	// 设置某一行的颜色
	void SetWidgetRowColor(QTableWidget* pTableWidget, int iRow, const QColor& col);

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：设置某些列为可编辑状态，其他的为不可编辑
 * 时    间：2022/7/21 18:45
 * 入    参：pTableWidget：表格指针
 * 入    参：vecEditCol：需要设置可编辑列的下标值（从0开始），其余的为不可编辑
 * 出    参：无
 * 返    回：无
 ************************************* end-XCW *****************************************/
	void SetTableColumnEdit(QTableWidget* pTableWidget, const QVector<unsigned short>& vecEditCol);

	// 这里放置属性的存取
/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：单元格对齐方式
 * 时    间：2022/7/21 10:02
 * 入    参：iAlingment:对齐方式，1-左对齐，2-中间对齐，3-右对齐，其他值默认是左对齐
 * 出    参：无
 * 返    回：无
 ************************************* end-XCW *****************************************/
	 void SetAlingment(unsigned short iAlingment);

	 // 设置列宽占空比
	 void SetColumnRatio(QTableWidget* pTable, const QVector<unsigned short>& vecColRatio);

	// 这里放置类的状态查询

protected:
	// 在表格控件中添加一条记录
	bool InsertTableWidgetItem(QTableWidget* pTableWidget, unsigned int iIndex, const QStringList& strList, int iAlingment);

	// 查找数组中的某值是否存在
	bool FindFromVector(const QVector<unsigned short>& vecEditCol, unsigned short iValue);

	// 单元格对齐方式
	unsigned short m_iAlingment;

private:
};

