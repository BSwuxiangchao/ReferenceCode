#include "CoaterDataWidget.h"
#include "BgiMpsEvent.h"
#include "BgiMasterApp.h"

CoaterDataWidget::CoaterDataWidget(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	InitControls();
}

CoaterDataWidget::~CoaterDataWidget()
{}

void CoaterDataWidget::InitControls()
{
    for (int i = 0; i < ui.TableData->rowCount(); ++i)
    {
        for (int j = 0; j < ui.TableData->columnCount(); ++j)
        {
            QTableWidgetItem* item = new QTableWidgetItem("");
            item->setTextAlignment(Qt::AlignCenter);
            if (j % 2 == 0)
            {
                item->setBackground(QBrush(QColor(230, 230, 230)));

                int k = (j == 4 ? 7300 : (7000 + j * 100));
                item->setText("DM" + QString::number(k + i));
            }
            else
            {
                item->setText("0");  //默认值为0
            }

            ui.TableData->setItem(i, j, item);
        }
    }
}

void CoaterDataWidget::showEvent(QShowEvent* event)
{
    //打开开始查询命令区数据
    BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Coater");
    pEvent->SetAction("ReadCmdData");
    pEvent->AddParams("Param1", true);
    BgiMasterApp* app = BgiMasterApp::Instance();
    QApplication::postEvent(app, pEvent);
}

void CoaterDataWidget::closeEvent(QCloseEvent* event)
{
    //关闭结束查询命令区数据
    BgiMpsEvent* pEvent = new BgiMpsEvent("Driver_Event_Coater");
    pEvent->SetAction("ReadCmdData");
    pEvent->AddParams("Param1", false);
    BgiMasterApp* app = BgiMasterApp::Instance();
    QApplication::postEvent(app, pEvent);
}

void CoaterDataWidget::SlotNewData(PLCDataType type, const QVector<int>& data)
{
    int col = 1;
    switch (type)
    {
    case ALARM_TYPE:
        col = 1;
        break;
    case STATUS_TYPE:
        col = 3;
        break;
    case COMMAND_TYPE:
        col = 5;
        break;
    default:
        return;
    }

    for (int i = 0; i < data.count(); ++i)
    {
        if (i >= ui.TableData->rowCount())
        {
            break;
        }

        ui.TableData->item(i, col)->setText(QString::number(data.at(i)));
    }
}