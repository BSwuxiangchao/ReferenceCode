#include "ImagerFovWidget.h"
#include "OpeStyleSheet.h"
#include <QMessageBox.h>
#include "ImagerMgrWidget.h"
#include "BgiLogger.h"

ImagerFovWidget::ImagerFovWidget(ImagerMgrWidget* parent)
	: QMainWindow((QWidget*)parent)
	, m_pImagerMgrWidget(parent)
{
	ui.setupUi(this);

	m_SaveColor = QBrush(Qt::darkGreen);
	m_NoSaveColor = QBrush(Qt::white);

	InitControls();
	SetStyleSheet();
}

ImagerFovWidget::~ImagerFovWidget()
{
	ui.tableWidgetFov->clear();
	ui.tableWidgetFov->deleteLater();
}

void ImagerFovWidget::InitControls()
{
	QObject::connect(ui.ButtonAdd, SIGNAL(clicked()), this, SLOT(ClickAddFovs()));
	QObject::connect(ui.ButtonDel, SIGNAL(clicked()), this, SLOT(ClickDelFovs()));
	QObject::connect(ui.ButtonDelAll, SIGNAL(clicked()), this, SLOT(ClickDelAllFovs()));
	QObject::connect(ui.ButtonSave, SIGNAL(clicked()), this, SLOT(ClickSaveFovs()));

	int rowNum = 90;
	int colNum = 90;
	ui.tableWidgetFov->setRowCount(rowNum);
	ui.tableWidgetFov->setColumnCount(colNum);
	for (int i = 0; i < rowNum; ++i)
	{
		for (int j = 0; j < colNum; ++j)
		{
			QTableWidgetItem* item = new QTableWidgetItem("");
			item->setBackground(m_NoSaveColor);
			ui.tableWidgetFov->setItem(i, j, item);
		}
	}
	//ui.tableWidgetFov->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color:black;}");
	ui.tableWidgetFov->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.tableWidgetFov->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

	//右键菜单
	ui.tableWidgetFov->setContextMenuPolicy(Qt::CustomContextMenu);
	m_pRightMenu = QSharedPointer<QMenu>(new QMenu(ui.tableWidgetFov));
	QAction* actionAdd = new QAction("增加", this);
	m_pRightMenu->addAction(actionAdd);
	QAction* actionDel = new QAction("删除", this);
	m_pRightMenu->addAction(actionDel);
	connect(ui.tableWidgetFov, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowRightMenu(QPoint)));
	connect(actionAdd, SIGNAL(triggered(bool)), this, SLOT(SlotAddFov(bool)));
	connect(actionDel, SIGNAL(triggered(bool)), this, SLOT(SlotDelFov(bool)));
}

void ImagerFovWidget::SetStyleSheet()
{
	OpeStyleSheet styleSheet;
	QString strStyle;
	bool bRet = styleSheet.ReadQPushbuttonStyleSheet(strStyle, 18, 1);
	if (bRet)
	{
		ui.centralWidget->setStyleSheet(strStyle);
		//ui.LayoutParams->setStyleSheet(strStyle);
		//ui.LayoutButtons->setStyleSheet(strStyle);
	}
}

void ImagerFovWidget::showEvent(QShowEvent* event)
{
	ui.tableWidgetFov->setCurrentItem(0);
}

int ImagerFovWidget::ReplaceFovs(const QList<QPoint>& points)
{
	int count = 0;
	for (int i = 0; i < ui.tableWidgetFov->rowCount(); ++i)
	{
		for (int j = 0; j < ui.tableWidgetFov->columnCount(); ++j)
		{
			QTableWidgetItem* item = ui.tableWidgetFov->item(i, j);
			if (points.contains(QPoint(i + 1, j + 1)))
			{
				item->setBackground(m_SaveColor);
				++count;
			}
			else
			{
				item->setBackground(m_NoSaveColor);
			}
		}
	}

	RefreshFovCount(count);

	QString str = "ReplaceFovs get count: " + QString::number(count);
	BGI_MESSAGE(str.toStdString().c_str());

	return count;
}

void ImagerFovWidget::AddFovs(const QList<QPoint>& points)
{
	for (int i = 0; i < ui.tableWidgetFov->rowCount(); ++i)
	{
		for (int j = 0; j < ui.tableWidgetFov->columnCount(); ++j)
		{
			QTableWidgetItem* item = ui.tableWidgetFov->item(i, j);
			if (points.contains(QPoint(i + 1, j + 1)))
			{
				item->setBackground(m_SaveColor);
			}
		}
	}

	RefreshFovCount();
}

void ImagerFovWidget::DelFovs(const QList<QPoint>& points)
{
	for (int i = 0; i < ui.tableWidgetFov->rowCount(); ++i)
	{
		for (int j = 0; j < ui.tableWidgetFov->columnCount(); ++j)
		{
			QTableWidgetItem* item = ui.tableWidgetFov->item(i, j);
			if (points.contains(QPoint(i + 1, j + 1)))
			{
				item->setBackground(m_NoSaveColor);
			}
		}
	}

	RefreshFovCount();
}

void ImagerFovWidget::ClearFovs()
{
	for (int i = 0; i < ui.tableWidgetFov->rowCount(); ++i)
	{
		for (int j = 0; j < ui.tableWidgetFov->columnCount(); ++j)
		{
			QTableWidgetItem* item = ui.tableWidgetFov->item(i, j);
			item->setBackground(m_NoSaveColor);
		}
	}

	RefreshFovCount(0);
}

void ImagerFovWidget::RefreshFovCount()
{
	try
	{
		int count = 0;
		for (int i = 0; i < ui.tableWidgetFov->rowCount(); ++i)
		{
			for (int j = 0; j < ui.tableWidgetFov->columnCount(); ++j)
			{
				QTableWidgetItem* item = ui.tableWidgetFov->item(i, j);
				if (item->background() == m_SaveColor)
				{
					++count;
				}
			}
		}

		ui.labelFovCount->setText(QString::number(count));
		ui.tableWidgetFov->setCurrentItem(0);
	}
	catch (...)
	{

	}
}

void ImagerFovWidget::RefreshFovCount(int count)
{
	try
	{
		ui.labelFovCount->setText(QString::number(count));
		ui.tableWidgetFov->setCurrentItem(0);
	}
	catch (...)
	{

	}
}

QList<QPoint> ImagerFovWidget::GetFovPoints()
{
	QList<QPoint> fovs;

	int startRow = ui.lineStartRow->text().toInt();
	int endRow = ui.lineEndRow->text().toInt();
	int startCol = ui.lineStartCol->text().toInt();
	int endCol = ui.lineEndCol->text().toInt();
	if (startRow <= 0 || endRow <= 0 || startCol <= 0 || endCol <= 0 || startRow>endRow || startCol > endCol)
	{
		QMessageBox::critical(this, "错误", "请设置正确的行列值！");
		return fovs;
	}

	for (int i = startRow; i <= endRow; ++i)
	{
		for (int j = startCol; j <= endCol; ++j)
		{
			fovs.append(QPoint(i, j));
		}
	}

	return fovs;
}

int ImagerFovWidget::SetSaveFovString(QString fovString)
{
	QList<QPoint> fovs;
	auto items = fovString.split(';');
	for (int i = 0; i < items.size(); ++i)
	{
		auto point = items[i].split(',');
		if (point.size() == 2)
		{
			int x = point[0].toInt();
			int y = point[1].toInt();
			fovs.append(QPoint(x, y));
		}
	}

	return ReplaceFovs(fovs);
}

void ImagerFovWidget::ClickAddFovs()
{
	BGI_MESSAGE("ClickAddFovs");
	QList<QPoint> fovs = GetFovPoints();
	if (!fovs.isEmpty())
	{
		AddFovs(fovs);
	}
}

void ImagerFovWidget::ClickDelFovs()
{
	BGI_MESSAGE("ClickDelFovs");
	QList<QPoint> fovs = GetFovPoints();
	if (!fovs.isEmpty())
	{
		DelFovs(fovs);
	}
}

void ImagerFovWidget::ClickDelAllFovs()
{
	BGI_MESSAGE("ClickDelAllFovs");
	ClearFovs();
	ClearRowColText();
}

void ImagerFovWidget::ClickSaveFovs()
{
	QList<QPoint> fovs;
	for (int i = 0; i < ui.tableWidgetFov->rowCount(); ++i)
	{
		for (int j = 0; j < ui.tableWidgetFov->columnCount(); ++j)
		{
			QTableWidgetItem* item = ui.tableWidgetFov->item(i, j);
			if (item->background() == m_SaveColor)
			{
				fovs.append(QPoint(item->row() + 1, item->column() + 1));
			}
		}
	}

	QString fovString = "";
	foreach (QPoint point, fovs)
	{
		fovString += QString::number(point.x()) + "," + QString::number(point.y()) + ";";
	}

	m_pImagerMgrWidget->SetSaveFovString(fovString, fovs.size());

	QString str = "SaveFovs count: " + QString::number(fovs.size());
	BGI_MESSAGE(str.toStdString().c_str());

	this->close();
}

void ImagerFovWidget::ShowRightMenu(QPoint point)
{
	m_pRightMenu->exec(QCursor::pos());
}

void ImagerFovWidget::SlotAddFov(bool checked)
{
	foreach (auto item, ui.tableWidgetFov->selectedItems())
	{
		item->setBackground(m_SaveColor);
	}

	RefreshFovCount();
	ClearRowColText();
}

void ImagerFovWidget::SlotDelFov(bool checked)
{
	foreach(auto item, ui.tableWidgetFov->selectedItems())
	{
		item->setBackground(m_NoSaveColor);
	}

	RefreshFovCount();
	ClearRowColText();
}

void ImagerFovWidget::ClearRowColText()
{
	ui.lineStartRow->setText("");
	ui.lineEndRow->setText("");
	ui.lineStartCol->setText("");
	ui.lineEndCol->setText("");
}
