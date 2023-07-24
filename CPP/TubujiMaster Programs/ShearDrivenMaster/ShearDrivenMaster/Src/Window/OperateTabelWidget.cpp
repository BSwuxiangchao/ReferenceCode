#include "OperateTabelWidget.h"


OperateTabelWidget::OperateTabelWidget()
{
	// 默认垂直居中，水平靠左
	m_iAlingment = (Qt::AlignVCenter | Qt::AlignLeft);
}

OperateTabelWidget::~OperateTabelWidget()
{
}

void OperateTabelWidget::InitTableWidget(QTableWidget* pTable, const QStringList& strListHeader, const unsigned char iAlingment)
{
	if (NULL == pTable)
	{
		return;
	}
	unsigned short n_col = strListHeader.size();
	pTable->setColumnCount(n_col);

	// 计算各表头的占位比
   /* unsigned short n_Total_len = 0;
	for (unsigned short i = 0; i < n_col; ++i)
	{
		n_Total_len += strList.at(i).size();
	}*/

	// 设置字体
	pTable->horizontalHeader()->setFont(QFont("Arial", 12));

	// 设置表头内容
	pTable->setHorizontalHeaderLabels(strListHeader);
	// 设置充满表宽度
	pTable->horizontalHeader()->setStretchLastSection(true);
	pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //先自适应宽度
	pTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列

	// 设置显示网格子线
	pTable->setShowGrid(true);
	// 设置垂直头不可见
	pTable->verticalHeader()->setVisible(false);
	// 设置单行选择
	pTable->setSelectionMode(QAbstractItemView::SingleSelection);
	// 设置选择行为 时每次选择一行
	pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	// 设置不可编辑
	pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	// 设置每列的宽度

	// 设置表头高度
	pTable->horizontalHeader()->setFixedHeight(30);
	// 设置选中的背景色
	pTable->setStyleSheet("selection-background-color: lightblue");
	// 设置表头背景色
	pTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background: skyblue}");

	// 对齐
	switch (iAlingment)
	{
	case 1:  // 左
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignLeft);
		break;
	case 2: // 中
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignHCenter);
		break;
	case 3: // 右
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignRight);
		break;
	default: // 左
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignLeft);
		break;
	}

	// 添加行-测试
	if (15 > pTable->rowCount())
	{
		pTable->setRowCount(15);
	}
	for (int i = 0; i < 15; i++)
	{
		QStringList strList;
		strList.clear();
		strList << QString::number(i);
		for (int j = 1; j < n_col; ++j)
		{
			strList << QString("Unit %1").arg(j);
			//pTable->setItem(i, j, new QTableWidgetItem(QString::number(j)));	
		}
		InsertTableWidgetItem(pTable, i, strList, m_iAlingment);
	}
}

void OperateTabelWidget::InitTableWidget(QTableWidget* pTable, const QStringList& strListHeader, const QVector<unsigned short>& vecColRatio, const unsigned char iAlingment)
{
	if (NULL == pTable)
	{
		return;
	}
	unsigned short n_col = strListHeader.size();
	unsigned short iTableHeaderCount = vecColRatio.size();

	if (n_col != iTableHeaderCount)
	{
		// 传入参数有误
		return;
	}
	pTable->setColumnCount(n_col);



	// 计算各表头的占位比
	unsigned short n_Total_len = 0;
	for (unsigned short i = 0; i < vecColRatio.size(); ++i)
	{
		n_Total_len += vecColRatio.at(i);
	}
	// 表格宽度
	int iTableWidth = pTable->width();

	// 设置字体
	pTable->horizontalHeader()->setFont(QFont("Arial", 12));
	// 设置表头内容
	pTable->setHorizontalHeaderLabels(strListHeader);
	
	// 设置充满表宽度
	pTable->horizontalHeader()->setStretchLastSection(true);
	pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);    //先自适应宽度
	pTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);     //然后设置要根据内容使用宽度的列

	// 设置显示网格子线
	pTable->setShowGrid(true);
	// 设置垂直头不可见
	pTable->verticalHeader()->setVisible(false);
	// 设置单行选择
	pTable->setSelectionMode(QAbstractItemView::SingleSelection);
	// 设置选择行为 时每次选择一行
	pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	// 设置不可编辑
	pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	// 设置每列的宽度

	// 设置表头高度
	pTable->horizontalHeader()->setFixedHeight(30);
	// 设置选中的背景色
	pTable->setStyleSheet("selection-background-color: lightblue");
	// 设置表头背景色
	pTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background: skyblue}");

	// 设置每列的宽度
	for (int i = 0; i < vecColRatio.size(); ++i)
	{
		int iWidth = vecColRatio.at(i) * 1.0 / n_Total_len * iTableWidth;
		pTable->setColumnWidth(i, iWidth);
	}

	// 对齐
	switch (iAlingment)
	{
	case 1:  // 左
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignLeft);
		break;
	case 2: // 中
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignHCenter);
		break;
	case 3: // 右
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignRight);
		break;
	default: // 左
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignLeft);
		break;
	}
}

void OperateTabelWidget::InitTableWidgetNoTest(QTableWidget* pTable, const QStringList& strListHeader, const unsigned char iAlingment)
{
	if (NULL == pTable)
	{
		return;
	}
	unsigned short n_col = strListHeader.size();
	pTable->setColumnCount(n_col);

	// 计算各表头的占位比
   /* unsigned short n_Total_len = 0;
	for (unsigned short i = 0; i < n_col; ++i)
	{
		n_Total_len += strList.at(i).size();
	}*/

	// 设置字体
	pTable->horizontalHeader()->setFont(QFont("Arial", 12));

	// 设置表头内容
	pTable->setHorizontalHeaderLabels(strListHeader);
	// 设置充满表宽度
	//pTable->horizontalHeader()->setStretchLastSection(true);
	//pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //先自适应宽度
	//pTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列


	// 设置显示网格子线
	pTable->setShowGrid(true);
	// 设置垂直头不可见
	pTable->verticalHeader()->setVisible(false);
	// 设置单行选择
	//pTable->setSelectionMode(QAbstractItemView::SingleSelection);
	// 设置选择行为 时每次选择一行
	//pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	// 设置不可编辑
	pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	// 设置每列的宽度

	// 设置表头高度
	pTable->horizontalHeader()->setFixedHeight(30);
	// 设置选中的背景色
	pTable->setStyleSheet("selection-background-color: lightblue");
	// 设置表头背景色
	pTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background: skyblue}");

	// 对齐
	switch (iAlingment)
	{
	case 1:  // 左
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignLeft);
		break;
	case 2: // 中
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignHCenter);
		break;
	case 3: // 右
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignRight);
		break;
	default: // 左
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignLeft);
		break;
	}
}

void OperateTabelWidget::InitTableWidgetOneSelect(QTableWidget* pTable, const QStringList& strListHeader, const unsigned char iAlingment)
{
	if (NULL == pTable)
	{
		return;
	}
	unsigned short n_col = strListHeader.size();
	pTable->setColumnCount(n_col);

	// 设置字体
	pTable->horizontalHeader()->setFont(QFont("Arial", 12));

	// 设置表头内容
	pTable->setHorizontalHeaderLabels(strListHeader);
	// 设置充满表宽度
	//pTable->horizontalHeader()->setStretchLastSection(true);
	//pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //先自适应宽度
	//pTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列


	// 设置显示网格子线
	pTable->setShowGrid(true);
	// 设置垂直头不可见
	pTable->verticalHeader()->setVisible(false);
	// 设置单行选择
	//pTable->setSelectionMode(QAbstractItemView::SingleSelection);
	// 设置选择行为 时每次选择一行
	pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	// 设置不可编辑
	pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	// 设置每列的宽度

	// 设置表头高度
	pTable->horizontalHeader()->setFixedHeight(30);
	// 设置选中的背景色
	pTable->setStyleSheet("selection-background-color: lightblue");
	// 设置表头背景色
	pTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background: skyblue}");

	// 对齐
	switch (iAlingment)
	{
	case 1:  // 左
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignLeft);
		break;
	case 2: // 中
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignHCenter);
		break;
	case 3: // 右
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignRight);
		break;
	default: // 左
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignLeft);
		break;
	}
}

void OperateTabelWidget::UpdateTabelWidget(QTableWidget* pTableWidget, const QVector<QStringList>& vecStrList)
{
	int i_vec_size = vecStrList.size();
	// 预先分配总行数，清除多余的行数
	pTableWidget->setRowCount(i_vec_size);
	if (pTableWidget == NULL)
		return;
	for (int i = 0; i < i_vec_size; ++i)
	{
		InsertTableWidgetItem(pTableWidget, i, vecStrList.at(i), m_iAlingment);
	}
}

void OperateTabelWidget::RemoveAllRow(QTableWidget* pTableWidget)
{
	if (NULL == pTableWidget)
	{
		return;
	}
	pTableWidget->setRowCount(0);
	/*int n_row = pTableWidget->rowCount();

	for (int i = n_row - 1; i >= 0; --i)
	{
		pTableWidget->removeRow(i);
	}*/
}

// 设置某一行的颜色
void OperateTabelWidget::SetWidgetRowColor(QTableWidget* pTableWidget, int iRow, const QColor& col)
{
	if (NULL == pTableWidget || 0 > iRow)
	{
		return;
	}

	int iSize = pTableWidget->rowCount();
	if (iSize <= iRow)
	{
		return;
	}
	int iCol = pTableWidget->columnCount();
	for (size_t i = 0; i < iCol; ++i)
	{
		QTableWidgetItem* pItem = pTableWidget->item(iRow, i);
		pItem->setBackground(QBrush(col));
	}
}

// 设置某一列的可编辑状态
void OperateTabelWidget::SetTableColumnEdit(QTableWidget* pTableWidget, const QVector<unsigned short>& vecEditCol)
{
	if (NULL == pTableWidget)
	{
		return;
	}
	// 如果传入的列数大于表格最大列，则默认修改最大一列状态
	int iRow = pTableWidget->rowCount();
	int iColumn = pTableWidget->columnCount();

	if (iColumn <= 0 || iRow <= 0)
	{
		// 没有可操作的
		return;
	}
	// 设置所有单元格为双击可编辑
	pTableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
	// 设置除了指定列为不可编辑
	for (int i = 0; i < iRow; ++i)
	{
		for (int j = 0; j < iColumn; ++j)
		{
			
			QTableWidgetItem* item = pTableWidget->item(i, j);
			if (NULL == item)
			{
				return;
			}
			if (FindFromVector(vecEditCol, j))
			{
				item->setFlags(item->flags() | Qt::ItemIsEditable);
			}
			else
			{
				item->setFlags(item->flags() & ~Qt::ItemIsEditable);
			}
		}
	}
}

bool OperateTabelWidget::InsertTableWidgetItem(QTableWidget* pTableWidget, unsigned int iIndex, const QStringList& strList, int iAlingment)
{
	int i_element = strList.size(); // 获取数组元素个数
	int i_col = pTableWidget->columnCount();  // 获取控件的列数
	if (0 >= i_element)
	{
		return true;
	}
	// 判断元素个数和列数是否需要扩展

	// 设置行内容
	int i_len = i_element;
	if (i_len > i_col)
	{
		i_len = i_col;
	}
	
	for (int i = 0; i < i_len; ++i)
	{
		QTableWidgetItem* item = new QTableWidgetItem(strList.at(i));
		item->setFont(QFont("Arial", 12));
		item->setTextAlignment(iAlingment);
		pTableWidget->setItem(iIndex, i, item);
	}
	return true;
}

// 查找数组中的某值是否存在
bool OperateTabelWidget::FindFromVector(const QVector<unsigned short>& vecEditCol, unsigned short iValue)
{
	bool bFind = false;
	for (int i = 0; i < vecEditCol.size(); ++i)
	{
		if (iValue == vecEditCol.at(i))
		{
			return true;
		}
	}

	return false;
}

void OperateTabelWidget::SetAlingment(unsigned short iAlingment)
{
	// 对齐
	switch (iAlingment)
	{
	case 1:  // 左
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignLeft);
		break;
	case 2: // 中
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignHCenter);
		break;
	case 3: // 右
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignRight);
		break;
	default: // 左
		m_iAlingment = (Qt::AlignVCenter | Qt::AlignLeft);
		break;
	}
}

// 设置列宽占空比
void OperateTabelWidget::SetColumnRatio(QTableWidget* pTable, const QVector<unsigned short>& vecColRatio)
{
	if (NULL == pTable || vecColRatio.isEmpty())
	{
		return;
	}
	int iTableColCount = pTable->columnCount();
	int iRatioCount = vecColRatio.size();
	if (iTableColCount != iRatioCount)
	{
		return;
	}

	unsigned short n_Total_len = 0;
	for (unsigned short i = 0; i < vecColRatio.size(); ++i)
	{
		n_Total_len += vecColRatio.at(i);
	}
	// 表格宽度
	int iTableWidth = pTable->width() - 1;  //宽度减去1，避免出现滚动条

	// 设置每列的宽度
	for (int i = 0; i < vecColRatio.size(); ++i)
	{
		int iWidth = vecColRatio.at(i) * 1.0 / n_Total_len * iTableWidth;
		pTable->setColumnWidth(i, iWidth);
	}
}